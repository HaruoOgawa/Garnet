#pragma once
#include "../../Interface/IApp.h"

namespace app
{
	class CMainApp : public IApp
	{
	public:
		CMainApp();
		virtual ~CMainApp();

		bool Initialize() override;
		bool ProcessInput() override;
		bool Update() override;
		bool Draw() override;
	};
}