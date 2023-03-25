#pragma once
#ifdef __DAWN__
#include "../Interface/IGraphicsAPI.h"
#include <webgpu.h>
#include <vector>

namespace api
{
	class CWebGPUAPI : public IGraphicsAPI
	{
		// Instance
		WGPUInstance m_Instance;
		
		// WIndow Surface
		WGPUSurface m_Surface;

		// Device
		WGPUAdapter  m_Adapter;
		WGPUDevice   m_Device;

		// Queue
		WGPUQueue m_Queue;

		// Command
		WGPUCommandBuffer m_CommandBuffer;
	private:
		// WebGPU メインロジック ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool CreateInstance();
#ifdef __EMSCRIPTEN__
		bool CreateSurface();
#else
		bool CreateSurface(GLFWwindow* pWindow);
#endif // __EMSCRIPTEN__
		bool CreatePhysicalDevice();
		bool CreateLogicalDevice();
		bool CreateQueue();
	public:
		CWebGPUAPI();
		virtual ~CWebGPUAPI();

#ifdef __EMSCRIPTEN__
		bool Initialize() override;
#else
		bool InitializeWithGLFW(GLFWwindow* pWindow) override;
#endif // __EMSCRIPTEN__

		void Release();

		std::shared_ptr<renderer::IRenderer> CreateRenderer() override;

		bool BeginRender() override;
		bool EndRender() override;
		bool IsWaitting() override;
	};
}
#endif