#ifdef __DAWN__
#include "CWebGPURenderPass.h"
#include "CWebGPUAPI.h"
#include "CWebGPUTexture.h"
#include "../../Debug/Message/Console.h"

namespace api
{
	CWebGPURenderPass::CWebGPURenderPass(api::CWebGPUAPI* pGraphicsAPI, const std::string& PassName, ERenderPassFormat RenderPassFormat):
		m_pGraphicsAPI(pGraphicsAPI),
		
		m_PassName(PassName),
		m_RenderPassFormat(RenderPassFormat),
		m_FrameTexture(nullptr),
		m_DepthTexture(nullptr),

		m_RenderPass(nullptr)
	{
	}

	CWebGPURenderPass::~CWebGPURenderPass()
	{
	}

	std::shared_ptr<graphics::CTexture> CWebGPURenderPass::GetFrameTexture()
	{
		return m_FrameTexture;
	}

	std::shared_ptr<graphics::CTexture> CWebGPURenderPass::GetDepthTexture()
	{
		return m_DepthTexture;
	}

	bool CWebGPURenderPass::Create(int Width, int Height)
	{
		m_FrameTexture = std::make_shared<CWebGPUTexture>(m_pGraphicsAPI, false);
		if (!m_FrameTexture->CreateFrameTexture(Width, Height, m_RenderPassFormat)) return false;
		
		m_DepthTexture = std::make_shared<CWebGPUTexture>(m_pGraphicsAPI, false);
		if (!m_DepthTexture->CreateFrameTexture(Width, Height, api::ERenderPassFormat::DEPTH_RENDERPASS)) return false;

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
		depthStencilAttachment.view = m_DepthTexture->GetTextureImageView(); // デプステクスチャ
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
