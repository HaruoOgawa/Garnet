#pragma once

#include <memory>
#include "SAppSettings.h"

namespace app
{
	class CApp;

	class CAppRunner
	{
	public:
		static bool Run(const std::shared_ptr<app::CApp>& App, SAppSettings Settings);
	};
}