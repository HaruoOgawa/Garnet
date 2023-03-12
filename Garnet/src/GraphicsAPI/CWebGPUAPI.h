#pragma once
#ifdef __DAWN__
#include "../Interface/IGraphicsAPI.h"
#include <webgpu.h>

namespace api
{
	class CWebGPUAPI : public IGraphicsAPI
	{
		WGPUInstance m_Instance;
		WGPUAdapter  m_Adapter;
	private:
		// WebGPU メインロジック ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateInstance();
		bool CreatePhysicalDevice();

	public:
		CWebGPUAPI();
		virtual ~CWebGPUAPI();

		bool Initialize() override;
		void Release();

		std::shared_ptr<renderer::IRenderer> CreateRenderer() override;

		bool BeginRender() override;
		bool EndRender() override;
		bool IsWaitting() override;
	};
}
#endif