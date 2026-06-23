#include "CComponentResolver.h"

#include "../AppCore/CApp.h"
#include "CommonComponent/CDirectionalLightComponent.h"
#include "CommonComponent/CPointLightComponent.h"
#include "CommonComponent/CSpotLightRenderer.h"
#include "CommonComponent/CSpotLightDMXController.h"
#include "CommonComponent/CIndirectLightComponent.h"

namespace scriptable
{
	std::shared_ptr<scriptable::CComponent> CComponentResolver::Resolve(app::CApp* pApp, const std::string& ComponentType, const std::string& ValueRegistry)
	{
		// ユーザー定義コンポーネントを取得
		auto Component = pApp->CreateComponent(ComponentType, ValueRegistry);
		if (Component) return Component;

		// 存在しなければエンジン標準定義コンポーネントを取得
		if (ComponentType == "DirectionalLight")
		{
			return std::make_shared<scriptable::CDirectionalLightComponent>(ComponentType, ValueRegistry);
		}
		else if (ComponentType == "PointLight")
		{
			return std::make_shared<scriptable::CPointLightComponent>(ComponentType, ValueRegistry);
		}
		else if (ComponentType == "SpotLight")
		{
			return std::make_shared<scriptable::CSpotLightRenderer>(ComponentType, ValueRegistry);
		}
		else if (ComponentType == "RectLight")
		{
		}
		else if (ComponentType == "IndirectLight")
		{
			return std::make_shared<scriptable::CIndirectLightComponent>(ComponentType, ValueRegistry);
		}
		else if (ComponentType == "SpotLightDMXController")
		{
			return std::make_shared<scriptable::CSpotLightDMXController>(ComponentType, ValueRegistry);
		}

		return nullptr;
	}
}