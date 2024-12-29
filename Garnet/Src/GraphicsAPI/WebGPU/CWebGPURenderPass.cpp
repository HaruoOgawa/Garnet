#ifdef USE_WEBGPU
#include "CWebGPURenderPass.h"
#include "CWebGPUAPI.h"
#include "CWebGPUTexture.h"
#include "../../Message/Console.h"

namespace api
{
	CWebGPURenderPass::CWebGPURenderPass(api::CWebGPUAPI* pGraphicsAPI, const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor):
		m_pGraphicsAPI(pGraphicsAPI),
		
		m_PassName(PassName),
		m_InitColor(InitColor),
		m_RenderPassFormat(RenderPassFormat),
		m_DepthTexture(nullptr),

		m_RenderPass(nullptr)
	{
	}

	CWebGPURenderPass::~CWebGPURenderPass()
	{
	}

	std::shared_ptr<graphics::CTexture> CWebGPURenderPass::GetFrameTexture(int Index)
	{
		if (Index < 0 || Index >= static_cast<int>(m_FrameTextureList.size())) return nullptr;

		return m_FrameTextureList[Index];
	}

	const std::vector<std::shared_ptr<graphics::CTexture>>& CWebGPURenderPass::GetFrameTextureList() const
	{
		return m_FrameTextureList;
	}

	const std::shared_ptr<graphics::CTexture>& CWebGPURenderPass::GetDepthTexture() const
	{
		return m_DepthTexture;
	}

	bool CWebGPURenderPass::Create(int Width, int Height, int RenderTargetCount)
	{
		graphics::STextureSamplerParam SamplerParam;
		SamplerParam.FilterMode = graphics::ETextureFilterMode::LINEAR;
		SamplerParam.WrapMode = graphics::ETextureWrapMode::CLAMP_TO_EDGE;

		for (int AttachmentIndex = 0; AttachmentIndex < RenderTargetCount; AttachmentIndex++)
		{
			auto FrameTexture = std::make_shared<CWebGPUTexture>(m_pGraphicsAPI, false, SamplerParam);
			if (!FrameTexture->CreateFrameTexture(Width, Height, m_RenderPassFormat)) return false;

			m_FrameTextureList.push_back(FrameTexture);
		}
		
		m_DepthTexture = std::make_shared<CWebGPUTexture>(m_pGraphicsAPI, false, SamplerParam);
		if (!m_DepthTexture->CreateFrameTexture(Width, Height, api::ERenderPassFormat::DEPTH_FLOAT_RENDERPASS)) return false;

		return true;
	}

	bool CWebGPURenderPass::BeginRenderPass()
	{
		// レンダーパスの設定
		std::vector<WGPURenderPassColorAttachment> renderPassColorAttachments;
		for (const auto& FrameTexture : m_FrameTextureList)
		{
			WGPURenderPassColorAttachment colorAttachment = {};
			colorAttachment.view = static_cast<CWebGPUTexture*>(FrameTexture.get())->GetTextureImageView(); // レンダリングの描画先テクスチャを指定
			colorAttachment.resolveTarget = nullptr; // マルチサンプリングの設定
			colorAttachment.loadOp = WGPULoadOp_Clear; // レンダー パスを実行する前にビューで実行するロード操作を示します。例えばクリア値に初期化するだったり
			colorAttachment.storeOp = WGPUStoreOp_Store; // レンダリング実行後の操作
			colorAttachment.clearValue = WGPUColor{ m_InitColor.x, m_InitColor.y, m_InitColor.z, m_InitColor.w }; // 初期カラー

			renderPassColorAttachments.push_back(colorAttachment);
		}

		// デプスステンシルバッファの設定
		WGPURenderPassDepthStencilAttachment depthStencilAttachment;
		depthStencilAttachment.view = static_cast<CWebGPUTexture*>(m_DepthTexture.get())->GetTextureImageView(); // デプステクスチャ
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
		renderPassDesc.colorAttachmentCount = static_cast<uint32_t>(renderPassColorAttachments.size());
		renderPassDesc.colorAttachments = &renderPassColorAttachments[0]; // レンダーパスのカラーフォーマットを指定
		renderPassDesc.depthStencilAttachment = &depthStencilAttachment; // デプスステンシルバッファ
		renderPassDesc.timestampWriteCount = 0;
		renderPassDesc.timestampWrites = nullptr; // レンダリングの同期用のオブジェクト領域
		renderPassDesc.nextInChain = nullptr; // 拡張機

		// レンダーパス開始
		m_RenderPass = wgpuCommandEncoderBeginRenderPass(m_pGraphicsAPI->GetEncoder(), &renderPassDesc);

		return true;
	}

	bool CWebGPURenderPass::EndRenderPass()
	{
		// レンダーパス終了
		wgpuRenderPassEncoderEnd(m_RenderPass);

		return true;
	}
}
#endif
