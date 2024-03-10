#ifdef USE_OPENGL
#include "COpenGLVertexBuffer.h"

namespace api
{
	COpenGLVertexBuffer::COpenGLVertexBuffer(api::COpenGLAPI* pGraphicsAPI):
		m_pGraphicsAPI(pGraphicsAPI),
		m_VertexBuffer(-1)
	{
	}

	COpenGLVertexBuffer::~COpenGLVertexBuffer()
	{
	}

	GLuint COpenGLVertexBuffer::GetVertexBufferIndex() const
	{
		return m_VertexBuffer;
	}

	bool COpenGLVertexBuffer::Create()
	{
		return true;
	}

	bool COpenGLVertexBuffer::CreateVertexBuffer()
	{
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
			//glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), &data[0], GL_STATIC_DRAW);
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), &data[0], GL_DYNAMIC_DRAW);

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

			//glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		return true;
	}

	GLenum COpenGLVertexBuffer::GetGLenumDataType(graphics::EDataType DataType) const
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

	void COpenGLVertexBuffer::AddRefRenderer(COpenGLRenderer* Renderder)
	{
		m_RefRendererList.push_back(Renderder);
	}

	const std::vector<COpenGLRenderer*>& COpenGLVertexBuffer::GetRefRendererList() const
	{
		return m_RefRendererList;
	}
}
#endif