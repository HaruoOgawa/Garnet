#pragma once

#ifdef USE_OPENGL
#include "COpenGLAPI.h"
#include "../../Graphics/CVertexBuffer.h"

namespace api
{
	class COpenGLRenderer;

	class COpenGLVertexBuffer : public graphics::CVertexBuffer
	{
		api::COpenGLAPI* m_pGraphicsAPI;

		GLuint m_VertexBuffer;

		// Ç±ÇÃVertexBufferÇéQè∆ÇµÇƒÇ¢ÇÈRendererList;
		std::vector<COpenGLRenderer*> m_RefRendererList;
	public:
		COpenGLVertexBuffer(api::COpenGLAPI* pGraphicsAPI);
		virtual ~COpenGLVertexBuffer();

		GLuint GetVertexBufferIndex() const;

		GLenum GetGLenumDataType(graphics::EDataType DataType) const;

		virtual bool Create() override;

		bool CreateVertexBuffer();

		void AddRefRenderer(COpenGLRenderer* Renderder);
		const std::vector<COpenGLRenderer*>& GetRefRendererList() const;
	};
}
#endif