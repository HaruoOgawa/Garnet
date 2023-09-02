#ifdef USE_WEBGPU
#include "CWebGPUAPI.h"
#include "CWebGPURenderPass.h"
#include "CWebGPURenderer.h"
#include "CWebGPUMaterial.h"
#include "CWebGPUTexture.h"
#include "CWebGPUGPGPUHandler.h"
#include "../../Debug/Message/Console.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
// emscripten_webgpu_get_deviceの使用に必要なインクルード
#include <emscripten/html5_webgpu.h>
#else
#include <glfw3webgpu.h>
#endif // !__EMSCRIPTEN__

#include <cassert>

namespace api
{
	CWebGPUAPI::CWebGPUAPI(int Width, int Height):
		m_VertexShaderExtension("_vert.wgsl"),
		m_FragmentShaderExtension("_frag.wgsl"),
		m_GeometryShaderExtension("_geom.wgsl"),
		m_HullShaderExtension("_tesc.wgsl"),
		m_DomainShaderExtension("_tese.wgsl"),
		m_ComputeShaderExtension("_comp.wgsl"),
		m_Width(Width),
		m_Height(Height),
#ifndef __EMSCRIPTEN__
		m_Instance(nullptr),
		m_Adapter(nullptr),
#endif
		m_Surface(nullptr),
		m_Device(nullptr),
		m_Queue(nullptr),
		m_Encoder(nullptr),
		m_CommandBuffer(nullptr),
		m_SwapChain(nullptr),
		m_SwapChainFormat(WGPUTextureFormat_Undefined),
		m_SwapChainDepthTexture(nullptr),
		m_SwapChainDepthTextureView(nullptr),
		m_SwapChainRenderPass(nullptr),
		m_CurrentRenderPass(nullptr),
		m_pWebGPURenderPass(nullptr)
	{
	}

	CWebGPUAPI::~CWebGPUAPI()
	{

	}

	// IGraphicsAPI //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __EMSCRIPTEN__
	bool CWebGPUAPI::Initialize()
#else
	bool CWebGPUAPI::InitializeWithGLFW(GLFWwindow* pWindow)
#endif // __EMSCRIPTEN__
	{
		if (!CreateInstance()) return false; // インスタンスを生成
#ifdef __EMSCRIPTEN__
		if (!CreateSurface()) return false; // ウィンドウサーフェイスを生成
#else
		if (!CreateSurface(pWindow)) return false; // ウィンドウサーフェイスを生成
#endif // __EMSCRIPTEN__
		if (!CreatePhysicalDevice()) return false; // 物理デバイス(アダプター)を生成
		if (!CreateLogicalDevice()) return false; // 論理デバイスを生成
		if (!CreateQueue()) return false; // キューを生成
		if (!CreateSwapChain()) return false; // スワップチェーンを生成
		if (!CreateDepthTexture()) return false; // デプステクスチャを生成

		return true;
	}

	void CWebGPUAPI::Release()
	{
		// オフスクリーンレンダリング用のフレームバッファを解放
		m_OffScreenRenderPassMap.clear();
	}

	bool CWebGPUAPI::CreateRenderPass(const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor, int Width, int Height)
	{
		std::shared_ptr<CWebGPURenderPass> RenderPass = std::make_shared<CWebGPURenderPass>(this, PassName, RenderPassFormat, InitColor);
		
		if (Width != -1 && Height != -1)
		{
			if (!RenderPass->Create(Width, Height)) return false;
		}
		else
		{
			if (!RenderPass->Create(m_Width, m_Height)) return false;
		}

		m_OffScreenRenderPassMap.insert({ PassName, RenderPass });

		return true;
	}

	std::shared_ptr<renderer::IRenderer> CWebGPUAPI::CreateRenderer(const std::string& PassName)
	{
		auto Renderer = std::make_shared<renderer::CWebGPURenderer>(this, PassName);

		return Renderer;
	}

	std::shared_ptr<graphics::CMaterial> CWebGPUAPI::CreateMaterial(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
	{
		auto Material = std::make_shared<api::CWebGPUMaterial>(this, createInfo);

		return Material;
	}

	std::shared_ptr<graphics::CTexture> CWebGPUAPI::CreateTexture(bool UseMipMap)
	{
		auto Texture = std::make_shared<api::CWebGPUTexture>(this, UseMipMap);

		return Texture;
	}

#ifdef USE_GPGPU
	std::shared_ptr<api::IGPGPUHandler> CWebGPUAPI::CreateGPGPUHandler(const std::shared_ptr<graphics::CMaterial>& ComputeMaterial)
	{
		auto GPGPUHandler = std::make_shared<api::CWebGPUGPGPUHandler>(this, ComputeMaterial);

		return GPGPUHandler;
	}
#endif // USE_GPGPU

	bool CWebGPUAPI::Resize(int Width, int Height)
	{
		m_Width = Width;
		m_Height = Height;

		if (!CreateSwapChain()) return false;
		if (!CreateDepthTexture()) return false;

		return true;
	}

	bool CWebGPUAPI::PrepareRender()
	{
		// コマンドエンコーダーを生成
		// (コマンドバッファの生成に必要なもの)
		WGPUCommandEncoderDescriptor encoderDesc = {};
		encoderDesc.nextInChain = nullptr;
		encoderDesc.label = "Command Encoder";
		m_Encoder = wgpuDeviceCreateCommandEncoder(m_Device, &encoderDesc);
		if (!m_Encoder)
		{
			Console::Log("Failed to Create Encorder\n");
			return false;
		}

		return true;
	}

	bool CWebGPUAPI::BeginRender(const std::string& PassName)
	{
		// レンダーパスを切り替える
		const auto& OffScreenRenderPass = m_OffScreenRenderPassMap.find(PassName);
		if (OffScreenRenderPass != m_OffScreenRenderPassMap.end())
		{
			m_pWebGPURenderPass = static_cast<CWebGPURenderPass*>(OffScreenRenderPass->second.get());

			if (!m_pWebGPURenderPass->BeginRenderPass()) return false;

			m_CurrentRenderPass = m_pWebGPURenderPass->GetRenderPass();
		}
		else
		{
			m_pWebGPURenderPass = nullptr;

			if (!BeginRenderPass()) return false;

			m_CurrentRenderPass = m_SwapChainRenderPass;
		}

		return true;
	}

	bool CWebGPUAPI::EndRender()
	{
		// 記録終了
		if (m_CurrentRenderPass != m_SwapChainRenderPass)
		{
			// オフスクリーンレンダーパス
			if (m_pWebGPURenderPass)
			{
				if (!m_pWebGPURenderPass->EndRenderPass()) return false;

				m_pWebGPURenderPass = nullptr;
			}
		}
		else
		{
			// デフォルトレンダーパス
			if (!EndRenderPass()) return false;
		}

		return true;
	}

	bool CWebGPUAPI::SubmitRender()
	{
		// コマンドバッファを生成
		WGPUCommandBufferDescriptor cmdBufferDesc = {};
		cmdBufferDesc.nextInChain = nullptr;
		cmdBufferDesc.label = "Command Buffer";
		m_CommandBuffer = wgpuCommandEncoderFinish(m_Encoder, &cmdBufferDesc);
		if (!m_CommandBuffer)
		{
			Console::Log("Failed to Create CommandBuffer\n");
			return false;
		}

		// コマンドの実行
		wgpuQueueSubmit(m_Queue, 1, &m_CommandBuffer);

		// スワップチェーンに描画結果を送る
		wgpuSwapChainPresent(m_SwapChain);

		return true;
	}

	const std::string& CWebGPUAPI::GetVertexShaderExtension() const
	{
		return m_VertexShaderExtension;
	}

	const std::string& CWebGPUAPI::GetFragmentShaderExtension() const
	{
		return m_FragmentShaderExtension;
	}

	const std::string& CWebGPUAPI::GetGeometryShaderExtension() const
	{
		return m_GeometryShaderExtension;
	}

	const std::string& CWebGPUAPI::GetHullShaderExtension() const
	{
		return m_HullShaderExtension;
	}

	const std::string& CWebGPUAPI::GetDomainShaderExtension() const
	{
		return m_DomainShaderExtension;
	}

	const std::string& CWebGPUAPI::GetComputeShaderExtension() const
	{
		return m_ComputeShaderExtension;
	}

	int CWebGPUAPI::GetWidth() const
	{
		return m_Width;
	}

	int CWebGPUAPI::GetHeight() const
	{
		return m_Width;
	}

	const std::map<std::string, std::shared_ptr<graphics::IRenderPass>>& CWebGPUAPI::GetOffScreenRenderPassMap() const
	{
		return m_OffScreenRenderPassMap;
	}

	//
	WGPUDevice CWebGPUAPI::GetLogicalDevice() const
	{
		return m_Device;
	}

	WGPUQueue CWebGPUAPI::GetQueue() const
	{
		return m_Queue;
	}

	WGPUTextureFormat CWebGPUAPI::GetSwapChainFormat() const
	{
		return m_SwapChainFormat;
	}

	WGPURenderPassEncoder CWebGPUAPI::GetCurrentRenderPass() const
	{
		return m_CurrentRenderPass;
	}

	// WebGPU メインロジック ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CWebGPUAPI::CreateInstance()
	{
		// インスタンスの設定
		WGPUInstanceDescriptor desc = {};
		desc.nextInChain = nullptr; // 拡張機を設定用のフィールド

		// インスタンスを生成
#ifndef __EMSCRIPTEN__
		// Emscriptenの場合はInstanceを必要としない
		m_Instance = wgpuCreateInstance(&desc);

		if (!m_Instance)
		{
			Console::Log("[Error] Failed to create Instance\n");
			return false;
		}
#endif
		return true;
	}

#ifdef __EMSCRIPTEN__
	bool CWebGPUAPI::CreateSurface()
#else
	bool CWebGPUAPI::CreateSurface(GLFWwindow* pWindow)
#endif // __EMSCRIPTEN__
	{
#ifdef __EMSCRIPTEN__
		//
		WGPUSurfaceDescriptorFromCanvasHTMLSelector canvDesc = {};
		canvDesc.chain.sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector;
		canvDesc.selector = "canvas";

		//
		WGPUSurfaceDescriptor surfDesc = {};
		surfDesc.nextInChain = reinterpret_cast<WGPUChainedStruct*>(&canvDesc); // 拡張機

		//
		m_Surface = wgpuInstanceCreateSurface(nullptr, &surfDesc); // Emscriptenの場合はInstanceを必要としない
#else
		m_Surface = glfwGetWGPUSurface(m_Instance, pWindow);
#endif // __EMSCRIPTEN__

		//
		if (!m_Surface)
		{
			Console::Log("[Error] Faliled to create Window Surface\n");
			return false;
		}

		return true;
	}

	bool CWebGPUAPI::CreatePhysicalDevice()
	{
#ifndef __EMSCRIPTEN__
		// アダプターの生成オプション
		WGPURequestAdapterOptions adapterOpts = {};
		adapterOpts.nextInChain = nullptr; // 拡張機
		adapterOpts.compatibleSurface = m_Surface; // ウィンドウサーフェイスを渡す

		// アダプターを取得するためのローカル構造体を定義
		struct UserData
		{
			WGPUAdapter adapter = nullptr;
			bool requestEnded = false;
		};
		UserData userData;

		// アダプターのリクエスト関数に渡すコールバックを作成
		auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const* message, void* pUserData) {
			UserData& userData = *reinterpret_cast<UserData*>(pUserData);
			if (status == WGPURequestAdapterStatus_Success)
			{
				userData.adapter = adapter;
			}
			userData.requestEnded = true;
		};

		// アダプターをリクエスト
		wgpuInstanceRequestAdapter(
			m_Instance,
			&adapterOpts,
			onAdapterRequestEnded,
			(void*)&userData
		);

		//
		m_Adapter = userData.adapter;
		if (!m_Adapter)
		{
			Console::Log("[Error] Cound not get WebGPU Adapter\n");
			return false;
		}

		// 物理デバイスの持つ機を問い合わせる
		std::vector<WGPUFeatureName> features;
		std::size_t featureCount = wgpuAdapterEnumerateFeatures(m_Adapter, nullptr);
		features.resize(featureCount);
		wgpuAdapterEnumerateFeatures(m_Adapter, &features[0]);
#endif
		return true;
	}

	bool CWebGPUAPI::CreateLogicalDevice()
	{
#ifndef __EMSCRIPTEN__
		// デバイスの取得オプション
		WGPUDeviceDescriptor descriptor = {};
		descriptor.nextInChain = nullptr; // 拡張機
		descriptor.label = "Garnet Device"; // デバイスを判別するためのラベル
		descriptor.requiredFeaturesCount = 0; // 使用することを指定するデバイスの特徴の数
		descriptor.requiredLimits = nullptr; // ???
		descriptor.defaultQueue.nextInChain = nullptr; // デフォルトコマンドキューの拡張機
		descriptor.defaultQueue.label = "Default Queue"; // デフォルトコマンドキューの判別用ラベル

		// 論理デバイスの制限の設定
		WGPUSupportedLimits supportedLimits{};
		wgpuAdapterGetLimits(m_Adapter, &supportedLimits);

		WGPURequiredLimits requiredLimits{};
		requiredLimits.nextInChain = nullptr;
		requiredLimits.limits = supportedLimits.limits;
		requiredLimits.limits.maxTextureDimension2D = 8192;

		descriptor.requiredLimits = &requiredLimits;

		// 論理デバイスを取得する
		struct UserData
		{
			WGPUDevice device = nullptr;
			bool	   requestEnded = false;
		};

		UserData userData;

		auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, char const* message, void* pUserData)
		{
			UserData& userData = *reinterpret_cast<UserData*>(pUserData);
			if (status == WGPURequestDeviceStatus_Success)
			{
				userData.device = device;
			}

			userData.requestEnded = true;
		};

		wgpuAdapterRequestDevice(
			m_Adapter,
			&descriptor,
			onDeviceRequestEnded,
			(void*)&userData
		);

		m_Device = userData.device;
		if (!m_Device)
		{
			Console::Log("Could not get WebGPU Adapter\n");
			return false;
		}

		// デバイスエラーをハンドリングするためのコールバックを登録しておく
		auto onDeviceError = [](WGPUErrorType type, char const* message, void*)
		{
			std::string TypeStr = "";

			switch (type)
			{
			case WGPUErrorType_NoError:
				TypeStr = "WGPUErrorType_NoError";
				break;
			case WGPUErrorType_Validation:
				TypeStr = "WGPUErrorType_Validation";
				break;
			case WGPUErrorType_OutOfMemory:
				TypeStr = "WGPUErrorType_OutOfMemory";
				break;
			case WGPUErrorType_Unknown:
				TypeStr = "WGPUErrorType_Unknown";
				break;
			case WGPUErrorType_DeviceLost:
				TypeStr = "WGPUErrorType_DeviceLost";
				break;
			case WGPUErrorType_Force32:
				TypeStr = "WGPUErrorType_Force32";
				break;
			default:
				break;
			}
			Console::Log("[Error Occured] Uncaptured device error: %s\n", TypeStr.c_str());
			if (message) Console::Log("[Error Message] %s\n", message);
		};

		wgpuDeviceSetUncapturedErrorCallback(m_Device, onDeviceError, nullptr);

#else
		// Emscriptenの場合は論理デバイスは物理デバイス経由ではなくEmscripten APIから直接もらう
		// なのでAdapter Objectも必要ない
		m_Device = emscripten_webgpu_get_device();
#endif
		return true;
	}

	bool CWebGPUAPI::CreateQueue()
	{
		// キューファミリを生成
		m_Queue = wgpuDeviceGetQueue(m_Device);
		if (!m_Queue)
		{
			Console::Log("Failed to create Queue\n");
			return false;
		}

		//
		auto onQueueWorkDone = [](WGPUQueueWorkDoneStatus status, void*)
		{
			Console::Log("Queued work finished with status: %s\n", status);
		};
		//wgpuQueueOnSubmittedWorkDone(m_Queue, onQueueWorkDone, nullptr); // なんか未解決になる

		return true;
	}

	bool CWebGPUAPI::CreateSwapChain()
	{
		//
		WGPUSwapChainDescriptor swapChainDesc = {};
		swapChainDesc.width = static_cast<uint32_t>(m_Width);
		swapChainDesc.height = static_cast<uint32_t>(m_Height);

		// スワップチェーンの色空間を指定 ///////////////////////////////////////////////
		m_SwapChainFormat = WGPUTextureFormat_BGRA8Unorm;

		swapChainDesc.format = m_SwapChainFormat;
		swapChainDesc.usage = WGPUTextureUsage_RenderAttachment; // レンダーパスのターゲットとして使用することを宣言
		swapChainDesc.presentMode = WGPUPresentMode_Fifo; // 各フレームで待機中のキューからどのようにテクスチャを示するかを指定する https://eliemichel.github.io/LearnWebGPU/getting-started/first-color.html

		//
		m_SwapChain = wgpuDeviceCreateSwapChain(m_Device, m_Surface, &swapChainDesc);
		return true;
	}

	bool CWebGPUAPI::CreateDepthTexture()
	{
		if (m_SwapChainDepthTexture)
		{
			wgpuTextureDestroy(m_SwapChainDepthTexture);
			m_SwapChainDepthTexture = nullptr;
		}

		WGPUTextureFormat depthTextureFormat = WGPUTextureFormat_Depth24Plus;

		// Textureを生成
		WGPUTextureDescriptor depthTextureDesc{};
		depthTextureDesc.nextInChain = nullptr;
		depthTextureDesc.dimension = WGPUTextureDimension_2D;
		depthTextureDesc.format = depthTextureFormat;
		depthTextureDesc.mipLevelCount = 1;
		depthTextureDesc.sampleCount = 1;
		depthTextureDesc.size = { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height), 1 };
		depthTextureDesc.usage = WGPUTextureUsage_RenderAttachment;
		depthTextureDesc.viewFormatCount = 1;
		depthTextureDesc.viewFormats = &depthTextureFormat;
		m_SwapChainDepthTexture = wgpuDeviceCreateTexture(m_Device, &depthTextureDesc);

		// TextureViewを生成
		WGPUTextureViewDescriptor depthTextureViewDesc{};
		depthTextureViewDesc.nextInChain = nullptr;
		depthTextureViewDesc.aspect = WGPUTextureAspect_DepthOnly;
		depthTextureViewDesc.baseArrayLayer = 0;
		depthTextureViewDesc.arrayLayerCount = 1;
		depthTextureViewDesc.baseMipLevel = 0;
		depthTextureViewDesc.mipLevelCount = 1;
		depthTextureViewDesc.dimension = WGPUTextureViewDimension_2D;
		depthTextureViewDesc.format = depthTextureFormat;

		m_SwapChainDepthTextureView = wgpuTextureCreateView(m_SwapChainDepthTexture, &depthTextureViewDesc);

		return true;
	}

	bool CWebGPUAPI::BeginRenderPass()
	{
		// スワップチェーンから次の待機中テクスチャを取得
		WGPUTextureView NextTexture = wgpuSwapChainGetCurrentTextureView(m_SwapChain);
		if (!NextTexture)
		{
			Console::Log("Not Exist nextTexture\n");
			return false;
		}

		// レンダーパスの設定
		WGPURenderPassColorAttachment renderPassColorAttachment = {};
		renderPassColorAttachment.nextInChain = nullptr;
		//renderPassColorAttachment.depthSlice = 0;
		renderPassColorAttachment.view = NextTexture; // レンダリングの描画先テクスチャを指定
		renderPassColorAttachment.resolveTarget = nullptr; // マルチサンプリングの設定
		renderPassColorAttachment.loadOp = WGPULoadOp_Clear; // レンダー パスを実行する前にビューで実行するロード操作を示します。例えばクリア値に初期化するだったり
		renderPassColorAttachment.storeOp = WGPUStoreOp_Store; // レンダリング実行後の操作
		renderPassColorAttachment.clearValue = WGPUColor{ 0.0f, 0.0f, 0.0f, 1.0f }; // 初期カラー

		// デプスステンシルバッファの設定
		WGPURenderPassDepthStencilAttachment depthStencilAttachment;
		depthStencilAttachment.view = m_SwapChainDepthTextureView; // デプステクスチャ
		depthStencilAttachment.depthClearValue = 1.0f; // デプスの初期値
		depthStencilAttachment.depthLoadOp = WGPULoadOp_Clear; // 処理開始時(ロード)にどうするか。ここでは全てクリアする
		depthStencilAttachment.depthStoreOp = WGPUStoreOp_Store; // デプスデータの保存処理(ストア)の時どうするか。普通に保存する
		depthStencilAttachment.depthReadOnly = false;

		depthStencilAttachment.stencilClearValue = 0;
#ifndef __EMSCRIPTEN__
		// このパラメーターはWebブラウザ側のWebGPUでは非推奨とのこと. https://github.com/emscripten-core/emscripten/issues/16471
		depthStencilAttachment.stencilLoadOp = WGPULoadOp_Clear;
		depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Store;
		depthStencilAttachment.stencilReadOnly = true;
#endif // !__EMSCRIPTEN__

		//
		WGPURenderPassDescriptor renderPassDesc = {};
		renderPassDesc.colorAttachmentCount = 1;
		renderPassDesc.colorAttachments = &renderPassColorAttachment; // レンダーパスのカラーフォーマットを指定
		renderPassDesc.depthStencilAttachment = &depthStencilAttachment; // デプスステンシルバッファ
		renderPassDesc.timestampWriteCount = 0;
		renderPassDesc.timestampWrites = nullptr; // レンダリングの同期用のオブジェクト領域
		renderPassDesc.nextInChain = nullptr; // 拡張機

		// レンダーパス開始
		m_SwapChainRenderPass = wgpuCommandEncoderBeginRenderPass(m_Encoder, &renderPassDesc);

		return true;
	}

	bool CWebGPUAPI::EndRenderPass()
	{
		// レンダーパス終了
		wgpuRenderPassEncoderEnd(m_SwapChainRenderPass);

		return true;
	}
}
#endif