#pragma once
#include "../../Interface/IApp.h"

namespace app
{
	class CScriptApp: public IApp
	{
	public:
		CScriptApp();
		virtual ~CScriptApp();

		bool Initialize() override;
		bool ProcessInput() override;
		bool Update() override;
		bool Draw() override;
	};
}