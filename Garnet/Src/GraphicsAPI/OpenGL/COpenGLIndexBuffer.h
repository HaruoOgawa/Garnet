#pragma once

#ifdef USE_OPENGL
#include "COpenGLAPI.h"
#include "../../Graphics/CIndexBuffer.h"

namespace api
{
	class COpenGLIndexBuffer : public graphics::CIndexBuffer
	{
		api::COpenGLAPI* m_pGraphicsAPI;

		GLuint m_IndexBuffer;
		GLenum m_GLIndiceType;
		GLsizei m_IndicesCount;
	public:
		COpenGLIndexBuffer(api::COpenGLAPI* pGraphicsAPI);
		virtual ~COpenGLIndexBuffer();

		GLuint GetIndexBuffer() const;
		GLenum GetGLIndiceType() const;
		GLsizei GetIndicesCount() const;

		virtual bool Create() override;
	};
}
#endif