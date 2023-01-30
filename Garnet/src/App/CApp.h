#pragma once

namespace app
{
	class CApp
	{
	private: 
		bool Release();
	public:
		CApp();
		virtual ~CApp();
		bool Initialize();
		bool RunLopp();
	};
}

