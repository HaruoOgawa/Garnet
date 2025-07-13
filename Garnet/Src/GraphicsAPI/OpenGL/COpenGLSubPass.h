#pragma once

#ifdef USE_OPENGL

#include "../ERenderPassFormat.h"
#include "COpenGLAPI.h"
#include <glm/glm.hpp>
#include <string>

namespace graphics { class CTexture; }

namespace api
{
	class COpenGLTexture;

	class COpenGLSubPass
	{
		// API
		api::COpenGLAPI* m_pGraphicsAPI;

		// Base Param
		std::string m_PassName;
		int m_Width;
		int m_Height;
		api::ERenderPassFormat m_RenderPassFormat;
		graphics::SRenderPassState m_PassState;

		std::vector<std::shared_ptr<graphics::CTexture>> m_FrameTextureList;
		std::shared_ptr<graphics::CTexture> m_DepthTexture;

		// Frame Buffer
		GLuint m_FrameBuffer;

		// Color Buffer
		std::vector<GLuint> m_ColorBufferList;

		// Depth Buffer
		GLuint m_DepthBuffer;

		// Attachment
		std::vector<unsigned int> m_Attachments;

	private:
		bool CreateFrameBuffer();
		bool CreateColorBuffer(int AttachmentIndex, const graphics::SRenderPassState& PassState, bool IsMSAASubPass);
		bool CreateDepthBuffer(const graphics::SRenderPassState& PassState, bool IsMSAASubPass);

	public:
		COpenGLSubPass(api::COpenGLAPI* pGraphicsAPI, const std::string& PassName, ERenderPassFormat RenderPassFormat);
		~COpenGLSubPass();

		std::shared_ptr<graphics::CTexture> GetFrameTexture(int Index = 0);
		const std::vector<std::shared_ptr<graphics::CTexture>>& GetFrameTextureList() const;
		const std::shared_ptr<graphics::CTexture>& GetDepthTexture() const;

		bool Create(int Width, int Height, const graphics::SRenderPassState& PassState, bool IsMSAASubPass);

		bool BeginRenderPass();
		bool EndRenderPass();

		int GetWidth() const;
		int GetHeight() const;

		GLuint GetFrameBuffer() const;

		// Attachment
		const std::vector<unsigned int>& GetAttachments() const;
	};
}
#endif