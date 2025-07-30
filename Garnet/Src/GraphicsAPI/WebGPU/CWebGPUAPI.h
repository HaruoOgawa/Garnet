#pragma once
#ifdef USE_WEBGPU
#include "../CGraphicsAPI.h"
#include <vector>
#include <map>
#include <memory>

#ifdef __EMSCRIPTEN__
#include <dawn/webgpu.h>
#else
#ifdef USE_WebGPUNative
#include <webgpu.h>
#else
#include <dawn/webgpu.h>
#endif // USE_WebGPUNative
#endif

namespace api
{
	class CWebGPURenderPass;

	class CWebGPUAPI : public CGraphicsAPI
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
		WGPUInstance m_Instance;

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

		virtual bool Initialize(window::IWindowAPI* pWindowAPI) override;

		virtual void Release() override;

		virtual bool CreateRenderPass(const std::string& PassName, ERenderPassFormat RenderPassFormat, int Width = -1, int Height = -1,
			const graphics::SRenderPassState& PassState = graphics::SRenderPassState(1)) override;
		virtual std::shared_ptr<graphics::CVertexBuffer> CreateVertexBuffer() override;
		virtual std::shared_ptr<graphics::CIndexBuffer> CreateIndexBuffer() override;
		virtual std::shared_ptr<graphics::IRenderer> CreateRenderer() override;
		virtual std::shared_ptr<graphics::CMaterial> CreateMaterial(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, graphics::ECullMode CullMode) override;
		virtual std::shared_ptr<graphics::CTexture> CreateTexture(bool UseMipMap = false, graphics::STextureSamplerParam SamplerParam = {}) override;
#ifdef USE_GPGPU
		virtual std::shared_ptr<api::IGPGPUHandler> CreateGPGPUHandler(const std::shared_ptr<graphics::CMaterial>& ComputeMaterial) override;
#endif // USE_GPGPU

		virtual bool Resize(int Width, int Height) override;

		virtual bool PrepareRender() override;
		virtual bool BeginRender(const std::string& PassName = "") override;
		virtual bool EndRender(std::function<bool(void)> AfterSortDrawCallback = nullptr) override;
		virtual bool SubmitRender() override;

		virtual std::string GetVertexShaderExtension() const override;
		virtual std::string GetFragmentShaderExtension() const override;
		virtual std::string GetGeometryShaderExtension() const override;
		virtual std::string GetHullShaderExtension() const override;
		virtual std::string GetDomainShaderExtension() const override;
		virtual std::string GetComputeShaderExtension() const override;

		virtual int GetWidth() const override;
		virtual int GetHeight() const override;
		
		virtual std::shared_ptr<graphics::IRenderPass> FindOffScreenRenderPass(const std::string& PassName) override;
		virtual bool CopyColorBuffer(const std::string& SrcPassName, const std::string& DstPassName) override;
		virtual bool CopyDepthBuffer(const std::string& SrcPassName, const std::string& DstPassName) override;

		virtual bool IsEnabledRuntimeShaderEditing() const override;

		virtual bool CheckValidShader(std::string& ErrorMsg, const std::vector<unsigned char>& ShaderCode, graphics::EShaderStage ShaderStage) override;

		WGPUCommandEncoder GetEncoder() { return m_Encoder; }
		WGPUCommandBuffer GetCommandBuffer() { return m_CommandBuffer; }

		WGPUTextureView GetSwapChainDepthTextureView() { return m_SwapChainDepthTextureView; }

		// SwapChain
		WGPUSwapChain GetSwapChain() { return m_SwapChain; }
		WGPUTextureFormat GetSwapChainFormat() const;

		//
		WGPUDevice GetLogicalDevice() const;
		WGPUQueue GetQueue() const;
		
		WGPURenderPassEncoder GetSwapChainRenderPass() const;
		WGPURenderPassEncoder GetCurrentRenderPass() const;
		
		// Buffer
		bool CreateBuffer(WGPUBuffer& Buffer, WGPUBufferUsageFlags Usage, void const* Data, uint64_t ByteSize);

		// 縮小も同時に行い、元のテクスチャを別のテクスチャにコピー
		//bool Blit
	};
}
#endif