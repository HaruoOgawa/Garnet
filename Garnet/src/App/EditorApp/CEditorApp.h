#pragma once
#include "../../Interface/IApp.h"

namespace app
{
	class CEditorApp : public IApp
	{
	public:
		CEditorApp();
		virtual ~CEditorApp();

		bool Initialize() override;
		bool ProcessInput() override;
		bool Update() override;
		bool Draw() override;
	};
}