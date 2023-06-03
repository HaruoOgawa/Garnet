#pragma once
#include <memory>

#include "../../Interface/IApp.h"
#include "../../Interface/IGraphicsAPI.h"
#include <glm/glm.hpp>

namespace scene { class CScriptScene; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }

namespace app
{
	class CScriptApp: public IApp
	{
		std::shared_ptr<scene::CScriptScene> m_ScriptScene;
		std::shared_ptr<camera::CCamera> m_MainCamera;
		std::shared_ptr<projection::CProjection> m_Projection;
		glm::vec4 m_LightDir;
	public:
		CScriptApp();
		virtual ~CScriptApp();

		bool Release(api::IGraphicsAPI* pGraphicsAPI) override;

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI) override;
		bool ProcessInput(api::IGraphicsAPI* pGraphicsAPI) override;
		bool Resize(int Width, int Height) override;
		bool Update(api::IGraphicsAPI* pGraphicsAPI, float SecondsTime) override;
		bool Draw(api::IGraphicsAPI* pGraphicsAPI) override;
	};
}