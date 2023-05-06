#pragma once
#ifdef __DAWN__
#include "../../Interface/IGraphicsAPI.h"
#include <webgpu.h>
#include <wgpu.h>
#include <vector>

namespace api
{
	class CWebGPUAPI : public IGraphicsAPI
	{
		//
		const std::string m_ShaderExtension;

		// Instance
#ifndef __EMSCRIPTEN__
		WGPUInstance m_Instance;
#endif
		// WIndow Surface
		WGPUSurface m_Surface;

		// Device
#ifndef __EMSCRIPTEN__
		WGPUAdapter  m_Adapter; // 物理デバイス
#endif
		WGPUDevice   m_Device; // 論理デバイス

		// Queue, Command
		WGPUQueue m_Queue;
		WGPUCommandEncoder m_Encoder;
		WGPUCommandBuffer m_CommandBuffer;

		// SwapChain
		WGPUTextureView m_NextTexture;
		WGPUSwapChain m_SwapChain;
		WGPUTextureFormat m_SwapChainFormat;

		// RenderPass
		WGPURenderPassEncoder m_RenderPass;

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
		bool CreateSwapChain();
	public:
		CWebGPUAPI();
		virtual ~CWebGPUAPI();

#ifdef __EMSCRIPTEN__
		bool Initialize() override;
#else
		bool InitializeWithGLFW(GLFWwindow* pWindow) override;
#endif // __EMSCRIPTEN__

		void Release();

		std::shared_ptr<vertex::IVertex> CreateVertex() override;
		std::shared_ptr<renderer::IRenderer> CreateRenderer() override;

		bool BeginRender(ERenderPassType RenderPassType) override;
		bool EndRender() override;
		bool IsWaitting() override;

		const std::string& GetShaderExtension() const override;

		//
		WGPUDevice GetLogicalDevice() const;
		WGPUQueue GetQueue() const;
		WGPUTextureFormat GetSwapChainFormat() const;
		WGPURenderPassEncoder GetRenderPass() const;
	};
}
#endif