#ifdef __DAWN__
#include "CWebGPUAPI.h"
#include "CWebGPURenderer.h"
#include "../Debug/Message/Console.h"

#ifndef __EMSCRIPTEN__
#include <glfw3webgpu.h>
#endif // !__EMSCRIPTEN__

#include <cassert>

namespace api
{
	CWebGPUAPI::CWebGPUAPI():
		m_Instance(nullptr),
		m_Surface(nullptr),
		m_Adapter(nullptr)
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
		return true;
	}

	bool CWebGPUAPI::EndRender()
	{
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
		m_Instance = wgpuCreateInstance(&desc);

		if (!m_Instance)
		{
			Console::Log("[Error] Failed to create Instance\n");
			return false;
		}

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
		//m_Device = emscripten_webgpu_get_device();
		
		//
		WGPUSurfaceDescriptorFromCanvasHTMLSelector canvDesc = {};
		canvDesc.chain.sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector;
		canvDesc.selector = "canvas";

		//
		WGPUSurfaceDescriptor surfDesc = {};
		surfDesc.nextInChain = reinterpret_cast<WGPUChainedStruct*>(&surfDesc); // 拡張機能

		//
		m_Surface = wgpuInstanceCreateSurface(m_Instance, &surfDesc);
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

		return true;
	}

	bool CWebGPUAPI::CreateLogicalDevice()
	{
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

		return true;
	}

	bool CWebGPUAPI::CreateQueue()
	{
		// キューファミリを生成
		m_Queue = wgpuDeviceGetQueue(m_Device);

		//
		auto onQueueWorkDone = [](WGPUQueueWorkDoneStatus status, void*)
		{
			Console::Log("Queued work finished with status: %s\n", status);
		};
		//wgpuQueueOnSubmittedWorkDone(m_Queue, onQueueWorkDone, nullptr); // なんか未解決になる

		// コマンドエンコーダーを生成
		// (コマンドバッファの生成に必要なもの)
		WGPUCommandEncoderDescriptor encoderDesc = {};
		encoderDesc.nextInChain = nullptr;
		encoderDesc.label = "Command Encoder";
		WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(m_Device, &encoderDesc);

		// コマンドバッファを生成
		WGPUCommandBufferDescriptor cmdBufferDesc = {};
		cmdBufferDesc.nextInChain = nullptr;
		cmdBufferDesc.label = "Command Buffer";
		m_CommandBuffer = wgpuCommandEncoderFinish(encoder, &cmdBufferDesc);

		// [テストコード] キューの送信
		wgpuQueueSubmit(m_Queue, 1, &m_CommandBuffer);

		return true;
	}
}
#endif