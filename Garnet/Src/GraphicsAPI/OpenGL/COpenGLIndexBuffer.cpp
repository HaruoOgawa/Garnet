#ifdef USE_OPENGL
#include "COpenGLIndexBuffer.h"

namespace api
{
	COpenGLIndexBuffer::COpenGLIndexBuffer(api::COpenGLAPI* pGraphicsAPI):
		m_pGraphicsAPI(pGraphicsAPI),
		m_GLIndiceType(GL_UNSIGNED_SHORT),
		m_IndicesCount(0)
	{
	}

	COpenGLIndexBuffer::~COpenGLIndexBuffer()
	{
	}

	GLenum COpenGLIndexBuffer::GetGLIndiceType() const
	{
		return m_GLIndiceType;
	}

	GLsizei COpenGLIndexBuffer::GetIndicesCount() const
	{
		return m_IndicesCount;
	}

	bool COpenGLIndexBuffer::Create()
	{
		return true;
	}

	bool COpenGLIndexBuffer::CreateIndexBuffer()
	{
		GLuint indexBuffer;

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

		if (GetIndiceType() == graphics::EIndiceType::UNSIGNED_SHORT)
		{
			m_GLIndiceType = GL_UNSIGNED_SHORT;
			m_IndicesCount = static_cast<GLsizei>(GetIndices().size());

			glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetIndices().size() * sizeof(unsigned short), GetIndices().data(), GL_STATIC_DRAW);
		}
		else if (GetIndiceType() == graphics::EIndiceType::UNSIGNED_INT)
		{
			m_GLIndiceType = GL_UNSIGNED_INT;
			m_IndicesCount = static_cast<GLsizei>(GetUINTIndices().size());

			glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetUINTIndices().size() * sizeof(unsigned int), GetUINTIndices().data(), GL_STATIC_DRAW);
		}

		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		return true;
	}
}
#endif