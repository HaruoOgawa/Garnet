#pragma once

#ifdef USE_OPENGL

#include <string>
#include "../../Interface/IRenderer.h"
#include "COpenGLAPI.h"

namespace graphics
{
	class CVertexBuffer;
	class CIndexBuffer;
	enum class EDataType;
	enum class EIndiceType;
}

namespace api
{
	class COpenGLAPI;
	class COpenGLMaterial;

	class COpenGLRenderer : public graphics::IRenderer
	{
		// API
		api::COpenGLAPI* m_pGraphicsAPI;

		int m_InstanceCount;

		std::shared_ptr<graphics::CVertexBuffer> m_VertexBuffer;

		GLuint m_VertexArray;
	private:
		bool CreateVertexArray();
	public:
		COpenGLRenderer(api::COpenGLAPI* pGraphicsAPI);
		virtual ~COpenGLRenderer();

		void SetActive() const;

		const std::shared_ptr<graphics::CVertexBuffer>& GetVertexBuffer() const;

		virtual bool Create(const std::vector<std::string>& PassNameList, const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, const std::shared_ptr<graphics::CMaterial>& Material) override;
		virtual bool Draw(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, 
			const std::shared_ptr<graphics::CMaterial>& Material) override;

		virtual bool UpdateVertexBuffer(const std::vector<float>& PosAttribute, const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer) override;
	};
}

#endif