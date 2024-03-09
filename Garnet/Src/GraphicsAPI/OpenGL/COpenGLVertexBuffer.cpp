#ifdef USE_OPENGL
#include "COpenGLVertexBuffer.h"

namespace api
{
	COpenGLVertexBuffer::COpenGLVertexBuffer(api::COpenGLAPI* pGraphicsAPI):
		m_pGraphicsAPI(pGraphicsAPI),
		m_VertexArray(-1),
		m_VertexBuffer(-1)
	{
	}

	COpenGLVertexBuffer::~COpenGLVertexBuffer()
	{
		glDeleteVertexArrays(1, &m_VertexArray);
	}

	void COpenGLVertexBuffer::SetActive() const
	{
		if (m_VertexArray != -1)
		{
			glBindVertexArray(m_VertexArray);
		}
	}

	void COpenGLVertexBuffer::SetEnactive() const
	{
		glBindVertexArray(0);
	}

	GLuint COpenGLVertexBuffer::GetVertexBuffer() const
	{
		return m_VertexBuffer;
	}

	bool COpenGLVertexBuffer::CreateVertexArray()
	{
		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		glBindVertexArray(0);

		return true;
	}

	bool COpenGLVertexBuffer::Create()
	{
		if (!CreateVertexArray()) return false; // 頂点配列を作成

		SetActive();

		for (int location = 0; location < GetVertices().size(); location++)
		{
			const auto& data = GetVertices()[location];
			int dimention = GetAttributeDimensions()[location];
			GLenum attribDataType = GetGLenumDataType(GetAttribDataTypes()[location]);
			// byteStrideとは「１つ分」のデータと、次の「1つ分」のデータとの間の、読み取り場所の移動バイト長
			// http://muko.damember.org/gl4/html-ja/glVertexAttribPointer.xhtml
			GLsizei byteStride = GetAttribByteStrides()[location];

			glGenBuffers(1, &m_VertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), &data[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(location);
			if (attribDataType == GL_FLOAT)
			{
				glVertexAttribPointer(location, dimention, attribDataType, GL_FALSE, byteStride, 0);
			}
			else
			{
				// Interger Valuesの時はglVertexAttrib"I"Pointerの方を使用する
				// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
				glVertexAttribIPointer(location, dimention, attribDataType, byteStride, 0);
			}

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		SetEnactive();

		return true;
	}

	GLenum COpenGLVertexBuffer::GetGLenumDataType(graphics::EDataType DataType)
	{
		GLenum result = GL_FLOAT;

		switch (DataType)
		{
		case graphics::EDataType::TYPE_SIGNED_BYTE:
			result = GL_BYTE;
			break;
		case graphics::EDataType::TYPE_UNSIGNED_BYTE:
			result = GL_UNSIGNED_BYTE;
			break;
		case graphics::EDataType::TYPE_SIGNED_SHORT:
			result = GL_SHORT;
			break;
		case graphics::EDataType::TYPE_UNSIGNED_SHORT:
			result = GL_UNSIGNED_SHORT;
			break;
		case graphics::EDataType::TYPE_UNSIGNED_INT:
			result = GL_UNSIGNED_INT;
			break;
		case graphics::EDataType::TYPE_FLOAT:
			result = GL_FLOAT;
			break;
		default:
			result = GL_FLOAT;
			break;
		}

		return result;
	}
}
#endif