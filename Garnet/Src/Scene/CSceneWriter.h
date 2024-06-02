#pragma once

namespace scene
{
	class CSceneController;

	class CSceneWriter
	{
	public:
		static bool Write(CSceneController* pSceneController);
	};
}