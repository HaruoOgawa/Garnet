#pragma once
#ifdef __DAWN__
#include "../../Interface/IGraphicsAPI.h"
#include <webgpu.h>
#include <wgpu.h>
#include <vector>
#include <map>

namespace api
{
	class CWebGPURenderPass;

	class CWebGPUAPI : public IGraphicsAPI
	{
		//
		const std::string m_ShaderExtension;
		int m_Width;
		int m_Height;

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

		// DepthTexture
		WGPUTexture		m_DepthTexture;
		WGPUTextureView m_DepthTextureView;

		// RenderPass
		std::map<std::string, std::shared_ptr<graphics::IRenderPass>> m_OffScreenRenderPassMap;
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
		bool CreateDepthTexture();
	public:
		CWebGPUAPI(int Width, int Height);
		virtual ~CWebGPUAPI();

#ifdef __EMSCRIPTEN__
		virtual bool Initialize() override;
#else
		virtual bool InitializeWithGLFW(GLFWwindow* pWindow) override;
#endif // __EMSCRIPTEN__

		void Release();

		virtual bool CreateRenderPass(const std::string& PassName, int Width, int Height, ERenderPassFormat RenderPassFormat) override;
		virtual std::shared_ptr<renderer::IRenderer> CreateRenderer(const std::string& PassName) override;
		virtual std::shared_ptr<graphics::CMaterial> CreateMaterial() override;
		virtual std::shared_ptr<graphics::CTexture> CreateTexture(bool UseMipMap = false) override;

		virtual bool Resize(int Width, int Height) override;

		virtual bool BeginRender(const std::string& PassName = "") override;
		virtual bool EndRender() override;

		virtual const std::string& GetShaderExtension() const override;

		virtual const std::map<std::string, std::shared_ptr<graphics::IRenderPass>>& GetOffScreenRenderPassMap() const override;

		//
		WGPUDevice GetLogicalDevice() const;
		WGPUQueue GetQueue() const;
		WGPUTextureFormat GetSwapChainFormat() const;
		WGPURenderPassEncoder GetRenderPass() const;

		// 縮小も同時に行い、元のテクスチャを別のテクスチャにコピー
		//bool Blit
	};
}
#endif