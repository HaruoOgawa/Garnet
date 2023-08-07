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
	class COpenGLTexture;

	class COpenGLRenderPass : public graphics::IRenderPass
	{
		// API
		api::COpenGLAPI* m_pGraphicsAPI;

		// Base Param
		std::string m_PassName;
		int m_Width;
		int m_Height;
		glm::vec4 m_InitColor;
		api::ERenderPassFormat m_RenderPassFormat;
		std::shared_ptr<COpenGLTexture> m_FrameTexture;
		std::shared_ptr<COpenGLTexture> m_DepthTexture;

		// Frame Buffer
		GLuint m_FrameBuffer;

		// Color Buffer
		GLuint m_ColorBuffer;

		// Depth Buffer
		GLuint m_DepthBuffer;
	private:
		bool CreateFrameBuffer();
		bool CreateColorBuffer();
		bool CreateDepthBuffer();
	public:
		COpenGLRenderPass(api::COpenGLAPI* pGraphicsAPI, const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor);
		virtual ~COpenGLRenderPass();

		virtual std::shared_ptr<graphics::CTexture> GetFrameTexture() override;
		virtual std::shared_ptr<graphics::CTexture> GetDepthTexture() override;

		bool Create(int Width, int Height) override;

		virtual bool BeginRenderPass() override;
		virtual bool EndRenderPass() override;
	};
}
#endif