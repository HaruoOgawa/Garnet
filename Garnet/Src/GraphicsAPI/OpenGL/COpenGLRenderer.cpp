#ifdef USE_OPENGL
#include "COpenGLRenderer.h"
#include "COpenGLRenderPass.h"
#include "COpenGLMaterial.h"
#include "../CRendererCreateInfo.h"
namespace renderer
{
	COpenGLRenderer::COpenGLRenderer(api::COpenGLAPI* pGraphicsAPI, const std::string& PassName):
		m_pGraphicsAPI(pGraphicsAPI),
		m_PassName(PassName),
		m_DynamicOffsetNum(0),
		m_IndiceType(GL_UNSIGNED_SHORT),
		m_InstanceCount(1),
		m_VertexArray(-1),
		m_IndicesCount(0)
	{
	}

	COpenGLRenderer::~COpenGLRenderer()
	{
		glDeleteVertexArrays(1, &m_VertexArray);
	}

	bool COpenGLRenderer::Create(const std::shared_ptr<CRendererCreateInfo>& createInfo, const std::shared_ptr<graphics::CMaterial>& Material)
	{
		m_InstanceCount = createInfo->GetInstanceCount();

		if (!CreateVertexArray()) return false; // 頂点配列を作成
		if (!CreateVertexBuffer(createInfo)) return false; // 頂点バッファを作成
		if (!CreateIndexBuffer(createInfo)) return false; // インデックスバッファを作成

		return true;
	}

	bool COpenGLRenderer::Draw(const std::shared_ptr<graphics::CMaterial>& Material, int DynamicOffsetNum)
	{
		api::COpenGLMaterial* pOpenGLMat = static_cast<api::COpenGLMaterial*>(Material.get());

		// ユニフォームバッファの準備
		if (!pOpenGLMat->BuildDrawBuffer(DynamicOffsetNum)) return false;

		// マテリアルをバインド
		pOpenGLMat->SetActive();

		// レンダラーをバインド
		SetActive();

		// 描画方法の設定
		// ZTest
		if (pOpenGLMat->IsEnabledZTest())
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
		}
		else
		{
			//glDisable(GL_DEPTH_TEST);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_ALWAYS);
		}

		// Culling
		switch (pOpenGLMat->GetCullMode())
		{
		case graphics::ECullMode::CULL_BACK:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;

		case graphics::ECullMode::CULL_FRONT:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			break;

		case graphics::ECullMode::CULL_NONE:
			glDisable(GL_CULL_FACE);
			break;

		default:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;
		}

		// Alpha Test(ひとまず通常のアルファブレンドのみ)
		switch (pOpenGLMat->GetBlendType())
		{
		case graphics::EBlendType::BLEND_TYPE_ADDITIVE:
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ZERO);

			break;
		case graphics::EBlendType::BLEND_TYPE_TRANSPARENT_ALPHA:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			break;
		default:
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ZERO);

			break;
		}

		// 描画を実行
		// あとで描画形式をカスタマイズできるようする(GL_TRIANGLEとかGL_LINEとかのやつ)
		if (m_InstanceCount > 1) // インスタンス描画
		{
			glDrawElementsInstanced(GL_TRIANGLES, m_IndicesCount, m_IndiceType, nullptr, m_InstanceCount);
		}
		else // 通常描画
		{
			glDrawElements(GL_TRIANGLES, m_IndicesCount, m_IndiceType, nullptr);
		}
		
		return true;
	}

	void COpenGLRenderer::SetActive()
	{
		if (m_VertexArray != -1)
		{
			glBindVertexArray(m_VertexArray);
		}
	}

	bool COpenGLRenderer::CreateVertexArray()
	{
		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		return true;
	}

	bool COpenGLRenderer::CreateVertexBuffer(const std::shared_ptr<CRendererCreateInfo>& createInfo)
	{
		for (int location = 0; location < createInfo->GetVertices().size(); location++)
		{
			GLuint vertexBuffer;

			const auto& data = createInfo->GetVertices()[location];
			int dimention = createInfo->GetAttributeDimensions()[location];
			GLenum attribDataType = GetGLenumDataType(createInfo->GetAttribDataTypes()[location]);
			// byteStrideとは「１つ分」のデータと、次の「1つ分」のデータとの間の、読み取り場所の移動バイト長
			// http://muko.damember.org/gl4/html-ja/glVertexAttribPointer.xhtml
			GLsizei byteStride = createInfo->GetAttribByteStrides()[location];

			glGenBuffers(1, &vertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
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

		return true;
	}

	bool COpenGLRenderer::CreateIndexBuffer(const std::shared_ptr<CRendererCreateInfo>& createInfo)
	{
		GLuint indexBuffer;

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		
		if (createInfo->GetIndiceType() == renderer::EIndiceType::UNSIGNED_SHORT)
		{
			m_IndiceType = GL_UNSIGNED_SHORT;
			m_IndicesCount = static_cast<GLsizei>(createInfo->GetIndices().size());

			glBufferData(GL_ELEMENT_ARRAY_BUFFER, createInfo->GetIndices().size() * sizeof(unsigned short), createInfo->GetIndices().data(), GL_STATIC_DRAW);
		}
		else if (createInfo->GetIndiceType() == renderer::EIndiceType::UNSIGNED_INT)
		{
			m_IndiceType = GL_UNSIGNED_INT;
			m_IndicesCount = static_cast<GLsizei>(createInfo->GetUINTIndices().size());

			glBufferData(GL_ELEMENT_ARRAY_BUFFER, createInfo->GetUINTIndices().size() * sizeof(unsigned int), createInfo->GetUINTIndices().data(), GL_STATIC_DRAW);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return true;
	}

	// Helper
	GLenum COpenGLRenderer::GetGLenumDataType(EDataType DataType)
	{
		GLenum result = GL_FLOAT;

		switch (DataType)
		{
		case renderer::EDataType::TYPE_SIGNED_BYTE:
			result = GL_BYTE;
			break;
		case renderer::EDataType::TYPE_UNSIGNED_BYTE:
			result = GL_UNSIGNED_BYTE;
			break;
		case renderer::EDataType::TYPE_SIGNED_SHORT:
			result = GL_SHORT;
			break;
		case renderer::EDataType::TYPE_UNSIGNED_SHORT:
			result = GL_UNSIGNED_SHORT;
			break;
		case renderer::EDataType::TYPE_UNSIGNED_INT:
			result = GL_UNSIGNED_INT;
			break;
		case renderer::EDataType::TYPE_FLOAT:
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