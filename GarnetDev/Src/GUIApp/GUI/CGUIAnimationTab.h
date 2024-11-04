#pragma once

#ifdef USE_GUIENGINE
#include <imgui.h>
#include <string>
#include <vector>
#include <memory>

namespace animation { 
	class CSkeleton; 
	enum class ERigType;
}

namespace gui
{
	class CGUIAnimationTab
	{
	private:
		static std::string GetStrFromRigType(animation::ERigType RigType);
	public:
		static bool DrawTab(const std::shared_ptr<animation::CSkeleton>& Skeleton);

		static bool DrawSkeleton(const std::shared_ptr<animation::CSkeleton>& Skeleton);
	};
}
#endif