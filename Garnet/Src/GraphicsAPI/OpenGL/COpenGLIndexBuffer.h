#pragma once

#ifdef USE_OPENGL
#include "COpenGLAPI.h"
#include "../../Graphics/CIndexBuffer.h"

namespace api
{
	class COpenGLIndexBuffer : public graphics::CIndexBuffer
	{
		api::COpenGLAPI* m_pGraphicsAPI;

		GLenum m_GLIndiceType;
		GLsizei m_IndicesCount;
	public:
		COpenGLIndexBuffer(api::COpenGLAPI* pGraphicsAPI);
		virtual ~COpenGLIndexBuffer();

		GLenum GetGLIndiceType() const;
		GLsizei GetIndicesCount() const;

		virtual bool Create() override;

		bool CreateIndexBuffer();
	};
}
#endif