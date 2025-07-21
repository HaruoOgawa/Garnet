#ifdef USE_OPENGL
#include "COpenGLRenderer.h"
#include "COpenGLVertexBuffer.h"
#include "COpenGLIndexBuffer.h"
#include "COpenGLRenderPass.h"
#include "COpenGLMaterial.h"

namespace api
{
	COpenGLRenderer::COpenGLRenderer(api::COpenGLAPI* pGraphicsAPI):
		m_pGraphicsAPI(pGraphicsAPI),
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

	bool COpenGLRenderer::Create(const std::vector<std::string>& PassNameList, const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, const std::shared_ptr<graphics::CMaterial>& Material)
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

	bool COpenGLRenderer::Draw(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, 
		const std::shared_ptr<graphics::CMaterial>& Material)
	{
		const COpenGLVertexBuffer* pOpenGLVertexBuffer = static_cast<const COpenGLVertexBuffer*>(VertexBuffer.get());
		const COpenGLIndexBuffer* pOpenGLIndexBuffer = static_cast<const COpenGLIndexBuffer*>(IndexBuffer.get());
		api::COpenGLMaterial* pOpenGLMat = static_cast<api::COpenGLMaterial*>(Material.get());

		// レンダラーをバインド
		SetActive();

		// マテリアルをバインド
		pOpenGLMat->SetActive();

		// ユニフォームバッファの準備
		if (!pOpenGLMat->BuildDrawBuffer()) return false;

		// 描画方法の設定
		// Depth
		{
			if (pOpenGLMat->IsEnabledZTest())
			{
				glEnable(GL_DEPTH_TEST);
			}
			else
			{
				glDisable(GL_DEPTH_TEST);
			}

			graphics::EDepthFunc DepthFunc = pOpenGLMat->GetDepthFunc();
			switch (DepthFunc)
			{
			case graphics::EDepthFunc::Never:
				glDepthFunc(GL_NEVER);
				break;
			case graphics::EDepthFunc::Less:
				glDepthFunc(GL_LESS);
				break;
			case graphics::EDepthFunc::LessEqual:
				glDepthFunc(GL_LEQUAL);
				break;
			case graphics::EDepthFunc::Greater:
				glDepthFunc(GL_GREATER);
				break;
			case graphics::EDepthFunc::GreaterEqual:
				glDepthFunc(GL_GEQUAL);
				break;
			case graphics::EDepthFunc::Equal:
				glDepthFunc(GL_EQUAL);
				break;
			case graphics::EDepthFunc::NotEqual:
				glDepthFunc(GL_NOTEQUAL);
				break;
			case graphics::EDepthFunc::Always:
				glDepthFunc(GL_ALWAYS);
				break;
			default:
				glDepthFunc(GL_LESS);
				break;
			}
		
			glDepthMask((pOpenGLMat->IsEnabledZWrite()) ? GL_TRUE : GL_FALSE);
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
		case graphics::EBlendType::BLEND_TYPE_NONE:
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ZERO);
			break;
		case graphics::EBlendType::BLEND_TYPE_ADDITIVE:
			glEnable(GL_BLEND);
			//glBlendFunc(GL_ONE, GL_ONE);
			glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);
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

		// Stencil Test
		const auto& StencilParam = pOpenGLMat->GetStencilParam();
		if (StencilParam.Enabled)
		{
			glEnable(GL_STENCIL_TEST);
			GLenum sfail = GL_KEEP;
			GLenum dpfail = GL_KEEP;
			GLenum dppass = GL_KEEP;

			switch (StencilParam.SFail)
			{
			case graphics::EStencilOp::Keep:
				sfail = GL_KEEP;
				break;
			case graphics::EStencilOp::Replace:
				sfail = GL_REPLACE;
				break;
			case graphics::EStencilOp::Incr:
				sfail = GL_INCR;
				break;
			case graphics::EStencilOp::Decr:
				sfail = GL_DECR;
				break;
			default:
				sfail = GL_KEEP;
				break;
			}

			switch (StencilParam.DpFail)
			{
			case graphics::EStencilOp::Keep:
				dpfail = GL_KEEP;
				break;
			case graphics::EStencilOp::Replace:
				dpfail = GL_REPLACE;
				break;
			case graphics::EStencilOp::Incr:
				dpfail = GL_INCR;
				break;
			case graphics::EStencilOp::Decr:
				dpfail = GL_DECR;
				break;
			default:
				dpfail = GL_KEEP;
				break;
			}

			switch (StencilParam.DpPass)
			{
			case graphics::EStencilOp::Keep:
				dppass = GL_KEEP;
				break;
			case graphics::EStencilOp::Replace:
				dppass = GL_REPLACE;
				break;
			case graphics::EStencilOp::Incr:
				dppass = GL_INCR;
				break;
			case graphics::EStencilOp::Decr:
				dppass = GL_DECR;
				break;
			default:
				dppass = GL_KEEP;
				break;
			}

			GLenum func = GL_ALWAYS;

			switch (StencilParam.Func)
			{
			case graphics::EStencilFunc::Never:
				func = GL_NEVER;
				break;
			case graphics::EStencilFunc::Less:
				func = GL_LESS;
				break;
			case graphics::EStencilFunc::LessEqual:
				func = GL_LEQUAL;
				break;
			case graphics::EStencilFunc::Greater:
				func = GL_GREATER;
				break;
			case graphics::EStencilFunc::GreaterEqual:
				func = GL_GEQUAL;
				break;
			case graphics::EStencilFunc::Equal:
				func = GL_EQUAL;
				break;
			case graphics::EStencilFunc::NotEqual:
				func = GL_NOTEQUAL;
				break;
			case graphics::EStencilFunc::Always:
				func = GL_ALWAYS;
				break;
			default:
				func = GL_ALWAYS;
				break;
			}

			glStencilOp(sfail, dpfail, dppass);
			glStencilFunc(func, StencilParam.RefValue, StencilParam.Mask);
		}
		else
		{
			glDisable(GL_STENCIL_TEST);
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
		
		// フラグをもとに戻す
		glDepthMask(GL_TRUE);

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