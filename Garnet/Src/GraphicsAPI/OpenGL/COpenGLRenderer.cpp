#ifdef USE_OPENGL
#include "COpenGLRenderer.h"
#include "COpenGLVertexBuffer.h"
#include "COpenGLIndexBuffer.h"
#include "COpenGLRenderPass.h"
#include "COpenGLMaterial.h"

namespace api
{
	COpenGLRenderer::COpenGLRenderer(api::COpenGLAPI* pGraphicsAPI, const std::string& PassName):
		m_pGraphicsAPI(pGraphicsAPI),
		m_PassName(PassName),
		m_DynamicOffsetNum(0),
		m_InstanceCount(1),
		m_VertexArray(-1),
		m_VertexBuffer(nullptr)
	{
	}

	COpenGLRenderer::~COpenGLRenderer()
	{
		glDeleteVertexArrays(1, &m_VertexArray);
	}

	void COpenGLRenderer::SetActive() const
	{
		if (m_VertexArray != -1)
		{
			glBindVertexArray(m_VertexArray);
		}
	}

	const std::shared_ptr<graphics::CVertexBuffer>& COpenGLRenderer::GetVertexBuffer() const
	{
		return m_VertexBuffer;
	}

	bool COpenGLRenderer::CreateVertexArray()
	{
		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		//glBindVertexArray(0);

		return true;
	}

	bool COpenGLRenderer::Create(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, const std::shared_ptr<graphics::CMaterial>& Material)
	{
		m_InstanceCount = VertexBuffer->GetInstanceCount();

		// VertexArray
		CreateVertexArray();

		// VertexBuffer
		COpenGLVertexBuffer* pOpenGLVertexBuffer = static_cast<COpenGLVertexBuffer*>(VertexBuffer.get());
		if (!pOpenGLVertexBuffer->CreateVertexBuffer()) return false;
		
		// IndexBuffer
		COpenGLIndexBuffer* pOpenGLIndexBuffer = static_cast<COpenGLIndexBuffer*>(IndexBuffer.get());
		if (!pOpenGLIndexBuffer->CreateIndexBuffer()) return false;

		m_VertexBuffer = VertexBuffer;

		// レンダラーの参照を追加
		pOpenGLVertexBuffer->AddRefRenderer(this);

		return true;
	}

	bool COpenGLRenderer::Draw(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, const std::shared_ptr<graphics::CMaterial>& Material, int DynamicOffsetNum)
	{
		const COpenGLVertexBuffer* pOpenGLVertexBuffer = static_cast<const COpenGLVertexBuffer*>(VertexBuffer.get());
		const COpenGLIndexBuffer* pOpenGLIndexBuffer = static_cast<const COpenGLIndexBuffer*>(IndexBuffer.get());
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
			glDrawElementsInstanced(GL_TRIANGLES, pOpenGLIndexBuffer->GetIndicesCount(), pOpenGLIndexBuffer->GetGLIndiceType(), nullptr, m_InstanceCount);
		}
		else // 通常描画
		{
			glDrawElements(GL_TRIANGLES, pOpenGLIndexBuffer->GetIndicesCount(), pOpenGLIndexBuffer->GetGLIndiceType(), nullptr);
		}
		
		return true;
	}

	bool COpenGLRenderer::UpdateVertexBuffer(const std::vector<float>& PosAttribute, const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer)
	{
		const COpenGLVertexBuffer* pOpenGLVertexBuffer = static_cast<const COpenGLVertexBuffer*>(VertexBuffer.get());

		// 参照レンダラー全てを更新する
		const auto& RefRendererList = pOpenGLVertexBuffer->GetRefRendererList();
		for (const auto* Renderer : RefRendererList)
		{
			const COpenGLVertexBuffer* pRefRendererVertexBuffer = static_cast<const COpenGLVertexBuffer*>(Renderer->GetVertexBuffer().get());

			// ひとまず0番目に頂点位置が入っている前提でコピーを行う
			// 後ほど頂点バッファの列挙型を導入する
			int location = 0;

			GLuint PositionVertexBufferIndex = pRefRendererVertexBuffer->GetPositionVertexBufferIndex();
			if (PositionVertexBufferIndex == -1) continue;

			int dimention = pRefRendererVertexBuffer->GetAttributeDimensions()[location];
			GLenum attributeDataType = pRefRendererVertexBuffer->GetGLenumDataType(pRefRendererVertexBuffer->GetAttribDataTypes()[location]);
			GLsizei byteStride = pRefRendererVertexBuffer->GetAttribByteStrides()[location];

			//
			Renderer->SetActive();

			glBindBuffer(GL_ARRAY_BUFFER, PositionVertexBufferIndex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(PosAttribute[0]) * PosAttribute.size(), &PosAttribute[0], GL_DYNAMIC_DRAW);

			glEnableVertexAttribArray(location);
			if (attributeDataType == GL_FLOAT)
			{
				glVertexAttribPointer(location, dimention, attributeDataType, GL_FALSE, byteStride, 0);
			}
			else
			{
				glVertexAttribIPointer(location, dimention, attributeDataType, byteStride, 0);
			}

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		return true;
	}
}
#endif