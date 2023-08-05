#pragma once
#ifdef USE_WEBGPU
#include "../../Interface/IGraphicsAPI.h"
#include <vector>
#include <map>

#include <webgpu.h>

namespace api
{
	class CWebGPURenderPass;

	class CWebGPUAPI : public IGraphicsAPI
	{
		//
		const std::string m_VertexShaderExtension;
		const std::string m_FragmentShaderExtension;
		const std::string m_GeometryShaderExtension;
		const std::string m_HullShaderExtension;
		const std::string m_DomainShaderExtension;
		const std::string m_ComputeShaderExtension;

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
		WGPUSwapChain m_SwapChain;
		WGPUTextureFormat m_SwapChainFormat;

		// DepthTexture
		WGPUTexture		m_SwapChainDepthTexture;
		WGPUTextureView m_SwapChainDepthTextureView;

		// RenderPass
		std::map<std::string, std::shared_ptr<graphics::IRenderPass>> m_OffScreenRenderPassMap;
		WGPURenderPassEncoder m_SwapChainRenderPass;

		WGPURenderPassEncoder m_CurrentRenderPass;
		CWebGPURenderPass* m_pWebGPURenderPass;

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

		bool BeginRenderPass();
		bool EndRenderPass();

	public:
		CWebGPUAPI(int Width, int Height);
		virtual ~CWebGPUAPI();

#ifdef __EMSCRIPTEN__
		virtual bool Initialize() override;
#else
		virtual bool InitializeWithGLFW(GLFWwindow* pWindow) override;
#endif // __EMSCRIPTEN__

		void Release();

		virtual bool CreateRenderPass(const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor, int Width = -1, int Height = -1) override;
		virtual std::shared_ptr<renderer::IRenderer> CreateRenderer(const std::string& PassName) override;
		virtual std::shared_ptr<graphics::CMaterial> CreateMaterial() override;
		virtual std::shared_ptr<graphics::CTexture> CreateTexture(bool UseMipMap = false) override;

		virtual bool Resize(int Width, int Height) override;

		virtual bool PrepareRender() override;
		virtual bool BeginRender(const std::string& PassName = "") override;
		virtual bool EndRender() override;
		virtual bool SubmitRender() override;

		virtual const std::string& GetVertexShaderExtension() const override;
		virtual const std::string& GetFragmentShaderExtension() const override;
		virtual const std::string& GetGeometryShaderExtension() const override;
		virtual const std::string& GetHullShaderExtension() const override;
		virtual const std::string& GetDomainShaderExtension() const override;
		virtual const std::string& GetComputeShaderExtension() const override;

		virtual int GetWidth() const override;
		virtual int GetHeight() const override;
		
		virtual const std::map<std::string, std::shared_ptr<graphics::IRenderPass>>& GetOffScreenRenderPassMap() const override;

		WGPUCommandEncoder GetEncoder() { return m_Encoder; }
		WGPUCommandBuffer GetCommandBuffer() { return m_CommandBuffer; }

		WGPUTextureView GetSwapChainDepthTextureView() { return m_SwapChainDepthTextureView; }

		// SwapChain
		WGPUSwapChain GetSwapChain() { return m_SwapChain; }
		WGPUTextureFormat GetSwapChainFormat() const;

		//
		WGPUDevice GetLogicalDevice() const;
		WGPUQueue GetQueue() const;
		WGPURenderPassEncoder GetCurrentRenderPass() const;

		// 縮小も同時に行い、元のテクスチャを別のテクスチャにコピー
		//bool Blit
	};
}
#endif