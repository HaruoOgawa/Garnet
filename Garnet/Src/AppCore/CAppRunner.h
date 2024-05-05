#pragma once

#include <memory>

namespace app
{
	class IApp;

	class CAppRunner
	{
	public:
		static bool Run(const std::shared_ptr<app::IApp>& App);
	};
}