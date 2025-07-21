#pragma once
#ifdef USE_OPENGL
#include "../../Interface/IRenderPass.h"
#include "../ERenderPassFormat.h"
#include "COpenGLAPI.h"
#include <glm/glm.hpp>
#include <string>

namespace graphics { class CTexture; }

namespace api
{
	class COpenGLSubPass;
	class COpenGLTexture;

	class COpenGLRenderPass : public graphics::IRenderPass
	{
		// API
		api::COpenGLAPI* m_pGraphicsAPI;

		// Base Param
		std::string m_PassName;
		api::ERenderPassFormat m_RenderPassFormat;

		bool m_UseColorBuffer;
		bool m_UseDepthBuffer;
		bool m_UseMSAA;

		// SubPass
		std::shared_ptr<COpenGLSubPass> m_SubPass;
		std::shared_ptr<COpenGLSubPass> m_ResolveSubPass;

	private:
		bool CopyRenderPass();

	public:
		COpenGLRenderPass(api::COpenGLAPI* pGraphicsAPI, const std::string& PassName, ERenderPassFormat RenderPassFormat);
		virtual ~COpenGLRenderPass();

		virtual std::shared_ptr<graphics::CTexture> GetFrameTexture(int Index = 0) override;
		virtual const std::vector<std::shared_ptr<graphics::CTexture>>& GetFrameTextureList() const override;
		virtual const std::shared_ptr<graphics::CTexture>& GetDepthTexture() const override;

		bool Create(int Width, int Height, const graphics::SRenderPassState& PassState) override;

		virtual bool BeginRenderPass() override;
		virtual bool EndRenderPass() override;

		virtual int GetWidth() const override;
		virtual int GetHeight() const override;

		GLuint GetFrameBuffer() const;
	};
}
#endif