#pragma once

#include <memory>
#include "SAppSettings.h"

namespace app
{
	class IApp;

	class CAppRunner
	{
	public:
		static bool Run(const std::shared_ptr<app::IApp>& App, SAppSettings Settings);
	};
}