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

		const std::string m_PassName;
		int	m_DynamicOffsetNum;
		int m_InstanceCount;

		GLuint m_VertexArray;
	private:
		bool CreateVertexArray();
	public:
		COpenGLRenderer(api::COpenGLAPI* pGraphicsAPI, const std::string& PassName);
		virtual ~COpenGLRenderer();

		void SetActive() const;

		bool Create(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, const std::shared_ptr<graphics::CMaterial>& Material) override;
		bool Draw(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, const std::shared_ptr<graphics::CMaterial>& Material, int DynamicOffsetNum) override;
	};
}

#endif