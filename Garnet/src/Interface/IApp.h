#pragma once

namespace app
{
	class IApp
	{
	public:
		virtual bool Initialize() = 0;
		virtual bool ProcessInput() = 0;
		virtual bool Update() = 0;
		virtual bool Draw() = 0;
	};
}