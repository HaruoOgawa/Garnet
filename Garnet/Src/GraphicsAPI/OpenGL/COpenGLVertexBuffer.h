#pragma once

#ifdef USE_OPENGL
#include "COpenGLAPI.h"
#include "../../Graphics/CVertexBuffer.h"

namespace api
{
	class COpenGLVertexBuffer : public graphics::CVertexBuffer
	{
		api::COpenGLAPI* m_pGraphicsAPI;
	private:
		GLenum GetGLenumDataType(graphics::EDataType DataType);
	public:
		COpenGLVertexBuffer(api::COpenGLAPI* pGraphicsAPI);
		virtual ~COpenGLVertexBuffer();

		virtual bool Create() override;

		bool CreateVertexBuffer();
	};
}
#endif