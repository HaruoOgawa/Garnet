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

		m_VertexArray(-1),
		m_IndicesCount(0)
	{
	}

	COpenGLRenderer::~COpenGLRenderer()
	{
	}

	bool COpenGLRenderer::Create(const std::shared_ptr<CRendererCreateInfo>& createInfo, const std::shared_ptr<graphics::CMaterial>& Material)
	{
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

		// 描画を実行
		// あとで描画形式をカスタマイズできるようする
		glDrawElements(GL_TRIANGLES, m_IndicesCount, GL_UNSIGNED_SHORT, nullptr);

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

			glGenBuffers(1, &vertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(location);
			glVertexAttribPointer(location, dimention, GL_FLOAT, GL_FALSE, dimention * sizeof(float), 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		return true;
	}

	bool COpenGLRenderer::CreateIndexBuffer(const std::shared_ptr<CRendererCreateInfo>& createInfo)
	{
		GLuint indexBuffer;

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, createInfo->GetIndices().size() * sizeof(unsigned short), createInfo->GetIndices().data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		m_IndicesCount = static_cast<GLsizei>(createInfo->GetIndices().size());

		return true;
	}
}
#endif