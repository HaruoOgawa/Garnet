#pragma once

#ifdef USE_OPENGL

#include <string>
#include "../../Interface/IRenderer.h"
#include "COpenGLAPI.h"

namespace api {
	class COpenGLAPI;
	class COpenGLMaterial;
}
namespace renderer
{
	class CRendererCreateInfo;

	class COpenGLRenderer : public IRenderer
	{
		// API
		api::COpenGLAPI* m_pGraphicsAPI;

		const std::string m_PassName;
		int	m_DynamicOffsetNum;
		int m_InstanceCount;

		//
		GLuint m_VertexArray;

		GLenum m_IndiceType;
		GLsizei m_IndicesCount;
	private:
		bool CreateVertexArray();
		bool CreateVertexBuffer(const std::shared_ptr<CRendererCreateInfo>& createInfo);
		bool CreateIndexBuffer(const std::shared_ptr<CRendererCreateInfo>& createInfo);
	public:
		COpenGLRenderer(api::COpenGLAPI* pGraphicsAPI, const std::string& PassName);
		virtual ~COpenGLRenderer();

		bool Create(const std::shared_ptr<CRendererCreateInfo>& createInfo, const std::shared_ptr<graphics::CMaterial>& Material) override;
		bool Draw(const std::shared_ptr<graphics::CMaterial>& Material, int DynamicOffsetNum) override;

		void SetActive();
	};
}

#endif