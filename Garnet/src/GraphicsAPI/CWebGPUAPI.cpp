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

	bool CWebGPUAPI::Initialize()
	{
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
}