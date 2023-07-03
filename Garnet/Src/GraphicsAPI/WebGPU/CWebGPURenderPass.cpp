#ifdef __DAWN__
#include "CWebGPURenderPass.h"
#include "CWebGPUAPI.h"
#include "CWebGPUTexture.h"
#include "../../Debug/Message/Console.h"

namespace api
{
	CWebGPURenderPass::CWebGPURenderPass(api::CWebGPUAPI* pGraphicsAPI, const std::string& PassName, int Width, int Height, ERenderPassFormat RenderPassFormat):
		m_pGraphicsAPI(pGraphicsAPI),
		
		m_PassName(PassName),
		m_Width(Width),
		m_Height(Height),
		m_RenderPassFormat(RenderPassFormat),
		m_FrameTexture(nullptr),

		m_RenderPass(nullptr),
		m_DepthTexture(nullptr),
		m_DepthTextureView(nullptr),

		m_Encoder(nullptr),
		m_CommandBuffer(nullptr)
	{
	}

	CWebGPURenderPass::~CWebGPURenderPass()
	{
	}

	std::shared_ptr<graphics::CTexture> CWebGPURenderPass::GetFrameTexture()
	{
		return m_FrameTexture;
	}

	bool CWebGPURenderPass::Create()
	{
		m_FrameTexture = std::make_shared<CWebGPUTexture>(m_pGraphicsAPI, false);
		if (!m_FrameTexture->CreateFrameTexture(m_Width, m_Height, m_RenderPassFormat)) return false;

		if (!CreateDepthTexture()) return false; // デプステクスチャを生成

		return true;
	}

	bool CWebGPURenderPass::BeginRenderPass()
	{
		// レンダーパスの設定
		WGPURenderPassColorAttachment renderPassColorAttachment = {};
		renderPassColorAttachment.view = m_FrameTexture->GetTextureImageView(); // レンダリングの描画先テクスチャを指定
		renderPassColorAttachment.resolveTarget = nullptr; // マルチサンプリングの設定
		renderPassColorAttachment.loadOp = WGPULoadOp_Clear; // レンダー パスを実行する前にビューで実行するロード操作を示します。例えばクリア値に初期化するだったり
		renderPassColorAttachment.storeOp = WGPUStoreOp_Store; // レンダリング実行後の操作
		renderPassColorAttachment.clearValue = WGPUColor{ 0.0f, 0.0f, 0.0f, 1.0f }; // 初期カラー

		// デプスステンシルバッファの設定
		WGPURenderPassDepthStencilAttachment depthStencilAttachment;
		depthStencilAttachment.view = m_DepthTextureView; // デプステクスチャ
		depthStencilAttachment.depthClearValue = 1.0f; // デプスの初期値
		depthStencilAttachment.depthLoadOp = WGPULoadOp_Clear; // 処理開始時(ロード)にどうするか。ここでは全てクリアする
		depthStencilAttachment.depthStoreOp = WGPUStoreOp_Store; // デプスデータの保存処理(ストア)の時どうするか。普通に保存する
		depthStencilAttachment.depthReadOnly = false;

		depthStencilAttachment.stencilClearValue = 0;
#ifndef __EMSCRIPTEN__
		// このパラメーターはWebブラウザ側のWebGPUでは非推奨とのこと. https://github.com/emscripten-core/emscripten/issues/16471
		depthStencilAttachment.stencilLoadOp = WGPULoadOp_Clear;
		depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Store;
		depthStencilAttachment.stencilReadOnly = true;
#endif // !__EMSCRIPTEN__

		//
		WGPURenderPassDescriptor renderPassDesc = {};
		renderPassDesc.colorAttachmentCount = 1;
		renderPassDesc.colorAttachments = &renderPassColorAttachment; // レンダーパスのカラーフォーマットを指定
		renderPassDesc.depthStencilAttachment = &depthStencilAttachment; // デプスステンシルバッファ
		renderPassDesc.timestampWriteCount = 0;
		renderPassDesc.timestampWrites = nullptr; // レンダリングの同期用のオブジェクト領域
		renderPassDesc.nextInChain = nullptr; // 拡張機

		// コマンドエンコーダーを生成
		// (コマンドバッファの生成に必要なもの)
		WGPUCommandEncoderDescriptor encoderDesc = {};
		encoderDesc.nextInChain = nullptr;
		encoderDesc.label = "Command Encoder";
		m_Encoder = wgpuDeviceCreateCommandEncoder(m_pGraphicsAPI->GetLogicalDevice(), &encoderDesc);
		if (!m_Encoder)
		{
			Console::Log("Failed to Create Encorder\n");
			return false;
		}

		// レンダーパス開始
		m_RenderPass = wgpuCommandEncoderBeginRenderPass(m_Encoder, &renderPassDesc);

		return true;
	}

	bool CWebGPURenderPass::EndRenderPass()
	{
		// レンダーパス終了
		wgpuRenderPassEncoderEnd(m_RenderPass);

		//
#ifdef __EMSCRIPTEN__
		//wgpuTextureViewDrop(m_NextTexture);
#endif // __EMSCRIPTEN__

		// コマンドバッファを生成
		WGPUCommandBufferDescriptor cmdBufferDesc = {};
		cmdBufferDesc.nextInChain = nullptr;
		cmdBufferDesc.label = "Command Buffer";
		m_CommandBuffer = wgpuCommandEncoderFinish(m_Encoder, &cmdBufferDesc);
		if (!m_CommandBuffer)
		{
			Console::Log("Failed to Create CommandBuffer\n");
			return false;
		}
		// コマンドの実行
		wgpuQueueSubmit(m_pGraphicsAPI->GetQueue(), 1, &m_CommandBuffer);

		return true;
	}

	bool CWebGPURenderPass::CreateDepthTexture()
	{
		if (m_DepthTexture)
		{
			wgpuTextureDestroy(m_DepthTexture);
			m_DepthTexture = nullptr;
		}

		WGPUTextureFormat depthTextureFormat = WGPUTextureFormat_Depth24Plus;

		// Textureを生成
		WGPUTextureDescriptor depthTextureDesc{};
		depthTextureDesc.nextInChain = nullptr;
		depthTextureDesc.dimension = WGPUTextureDimension_2D;
		depthTextureDesc.format = depthTextureFormat;
		depthTextureDesc.mipLevelCount = 1;
		depthTextureDesc.sampleCount = 1;
		depthTextureDesc.size = { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height), 1 };
		depthTextureDesc.usage = WGPUTextureUsage_RenderAttachment;
		depthTextureDesc.viewFormatCount = 1;
		depthTextureDesc.viewFormats = &depthTextureFormat;
		m_DepthTexture = wgpuDeviceCreateTexture(m_pGraphicsAPI->GetLogicalDevice(), &depthTextureDesc);
		
		// TextureViewを生成
		WGPUTextureViewDescriptor depthTextureViewDesc{};
		depthTextureViewDesc.nextInChain = nullptr;
		depthTextureViewDesc.aspect = WGPUTextureAspect_DepthOnly;
		depthTextureViewDesc.baseArrayLayer = 0;
		depthTextureViewDesc.arrayLayerCount = 1;
		depthTextureViewDesc.baseMipLevel = 0;
		depthTextureViewDesc.mipLevelCount = 1;
		depthTextureViewDesc.dimension = WGPUTextureViewDimension_2D;
		depthTextureViewDesc.format = depthTextureFormat;

		m_DepthTextureView = wgpuTextureCreateView(m_DepthTexture, &depthTextureViewDesc);

		return true;
	}
}
#endif
