#ifdef __DAWN__
#include "CWebGPUAPI.h"
#include "CWebGPURenderer.h"
#include "../Debug/Message/Console.h"

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
	CWebGPUAPI::CWebGPUAPI()
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

		return true;
	}

	void CWebGPUAPI::Release()
	{
	}

	std::shared_ptr<renderer::IRenderer> CWebGPUAPI::CreateRenderer()
	{
		const auto Renderer = std::make_shared<renderer::CWebGPURenderer>();

		return Renderer;
	}

	bool CWebGPUAPI::BeginRender()
	{
		// スワップチェーンから次の待機中テクスチャを取得
		m_NextTexture = wgpuSwapChainGetCurrentTextureView(m_SwapChain);
		if (!m_NextTexture)
		{
			Console::Log("Not Exist nextTexture\n");
			return false;
		}
		
		// レンダーパスの設定
		WGPURenderPassColorAttachment renderPassColorAttachment = {};
		renderPassColorAttachment.view = m_NextTexture; // レンダリングの描画先テクスチャを指定
		renderPassColorAttachment.resolveTarget = nullptr; // マルチサンプリングの設定
		renderPassColorAttachment.loadOp = WGPULoadOp_Clear; // レンダー パスを実行する前にビューで実行するロード操作を示します。例えばクリア値に初期化するだったり
		renderPassColorAttachment.storeOp = WGPUStoreOp_Store; // レンダリング実行後の操作
		renderPassColorAttachment.clearValue = WGPUColor{ 0.9f, 0.1f, 0.2f, 1.0f }; // 初期カラー

		WGPURenderPassDescriptor renderPassDesc = {};
		renderPassDesc.colorAttachmentCount = 1; 
		renderPassDesc.colorAttachments = &renderPassColorAttachment; // レンダーパスのカラーフォーマットを指定
		renderPassDesc.depthStencilAttachment = nullptr; // デプスステンシルバッファ
		renderPassDesc.timestampWriteCount = 0;
		renderPassDesc.timestampWrites = nullptr; // レンダリングの同期用のオブジェクト領域
		renderPassDesc.nextInChain = nullptr; // 拡張機能

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

		// レンダーパス開始
		m_RenderPass = wgpuCommandEncoderBeginRenderPass(m_Encoder, &renderPassDesc);

		return true;
	}

	bool CWebGPUAPI::EndRender()
	{
		// レンダーパス終了
		wgpuRenderPassEncoderEnd(m_RenderPass);

		//
#ifdef __EMSCRIPTEN__
		//wgpuTextureViewDrop(m_NextTexture);
#endif // __EMSCRIPTEN__

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
		// スワップチェーンに
		wgpuSwapChainPresent(m_SwapChain);

		return true;
	}

	bool CWebGPUAPI::IsWaitting()
	{
		return false;
	}

	// WebGPU メインロジック ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CWebGPUAPI::CreateInstance()
	{
		// インスタンスの設定
		WGPUInstanceDescriptor desc = {};
		desc.nextInChain = nullptr; // 拡張機能を設定用のフィールド

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
		surfDesc.nextInChain = reinterpret_cast<WGPUChainedStruct*>(&canvDesc); // 拡張機能

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
		adapterOpts.nextInChain = nullptr; // 拡張機能
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

		// 物理デバイスの持つ機能を問い合わせる
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
		descriptor.nextInChain = nullptr; // 拡張機能
		descriptor.label = "Garnet Device"; // デバイスを判別するためのラベル
		descriptor.requiredFeaturesCount = 0; // 使用することを指定するデバイスの特徴の数
		descriptor.requiredLimits = nullptr; // ???
		descriptor.defaultQueue.nextInChain = nullptr; // デフォルトコマンドキューの拡張機能
		descriptor.defaultQueue.label = "Default Queue"; // デフォルトコマンドキューの判別用ラベル

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
			Console::Log("Uncaptured device error: %d\n", type);
			if (message) Console::Log("message: %s\n", message);
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
		swapChainDesc.width = 800;
		swapChainDesc.height = 600;

#ifdef __EMSCRIPTEN__
		WGPUTextureFormat swapChainFormat = WGPUTextureFormat_BGRA8Unorm;
#else
		WGPUTextureFormat swapChainFormat = wgpuSurfaceGetPreferredFormat(m_Surface, m_Adapter);
#endif // __EMSCRIPTEN__
		swapChainDesc.format = swapChainFormat;
		swapChainDesc.usage = WGPUTextureUsage_RenderAttachment; // レンダーパスのターゲットとして使用することを宣言
		swapChainDesc.presentMode = WGPUPresentMode_Fifo; // 各フレームで待機中のキューからどのようにテクスチャを表示するかを指定する https://eliemichel.github.io/LearnWebGPU/getting-started/first-color.html

		//
		Console::Log("[START] wgpuDeviceCreateSwapChain\n");
		m_SwapChain = wgpuDeviceCreateSwapChain(m_Device, m_Surface, &swapChainDesc);
		Console::Log("[END] wgpuDeviceCreateSwapChain\n");
		return true;
	}
}
#endif