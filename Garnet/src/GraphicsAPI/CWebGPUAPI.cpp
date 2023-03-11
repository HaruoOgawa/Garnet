#ifdef __DAWN__
#include "CWebGPUAPI.h"
#include "CWebGPURenderer.h"

namespace api
{
	CWebGPUAPI::CWebGPUAPI()
	{

	}

	CWebGPUAPI::~CWebGPUAPI()
	{

	}

	// IGraphicsAPI //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool CWebGPUAPI::Initialize()
	{
		if (!CreateInstance()) return false; // インスタンスを生成

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
	bool  CWebGPUAPI::CreateInstance()
	{
		// インスタンスの設定
		WGPUInstanceDescriptor desc = {};
		desc.nextInChain = nullptr; // ???

		// インスタンスを生成
		WGPUInstance instance = wgpuCreateInstance(&desc);

		if (!instance)
		{
			return false;
		}

		return true;
	}
}
#endif