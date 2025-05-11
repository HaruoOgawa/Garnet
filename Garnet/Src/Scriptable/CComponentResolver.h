#pragma once

#include <memory>
#include <string>

namespace app { class CApp; }

namespace scriptable
{
	class CComponent;

	class CComponentResolver
	{
	public:
		static std::shared_ptr<scriptable::CComponent> Resolve(app::CApp* pApp, const std::string& ComponentType, const std::string& ValueRegistry);
	};
}