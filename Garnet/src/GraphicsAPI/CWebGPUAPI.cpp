#ifdef __DAWN__
#include "CWebGPUAPI.h"
#include "CWebGPURenderer.h"
#include "../Debug/Message/Console.h"
#include <cassert>

namespace api
{
	CWebGPUAPI::CWebGPUAPI():
		m_Instance(nullptr),
		m_Adapter(nullptr)
	{

	}

	CWebGPUAPI::~CWebGPUAPI()
	{

	}

	// IGraphicsAPI //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool CWebGPUAPI::Initialize()
	{
		if (!CreateInstance()) return false; // インスタンスを生成
		if (!CreatePhysicalDevice()) return false; // 物理デバイス(アダプター)を生成

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

	bool CWebGPUAPI::CreatePhysicalDevice()
	{
		// アダプターの生成オプション
		WGPURequestAdapterOptions adapterOpts = {};

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
			Console::Log("Cound not get WebGPU Adapter\n");
			return false;
		}

		return true;
	}
}
#endif