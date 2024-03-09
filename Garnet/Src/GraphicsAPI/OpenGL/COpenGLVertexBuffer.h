#pragma once

#ifdef USE_OPENGL
#include "COpenGLAPI.h"
#include "../../Graphics/CVertexBuffer.h"

namespace api
{
	class COpenGLVertexBuffer : public graphics::CVertexBuffer
	{
		api::COpenGLAPI* m_pGraphicsAPI;

		GLuint m_VertexArray;
		GLuint m_VertexBuffer;
	private:
		bool CreateVertexArray();

		GLenum GetGLenumDataType(graphics::EDataType DataType);
	public:
		COpenGLVertexBuffer(api::COpenGLAPI* pGraphicsAPI);
		virtual ~COpenGLVertexBuffer();

		void SetActive() const;
		void SetEnactive() const;

		GLuint GetVertexBuffer() const;

		virtual bool Create() override;
	};
}
#endif