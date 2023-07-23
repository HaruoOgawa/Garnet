#ifdef USE_OPENGL

#include "CDemoAppManager.h"

namespace app
{
	CDemoAppManager::CDemoAppManager(app::EAppType AppType):
		m_IsRunLoop(true)
	{
	}

	CDemoAppManager::~CDemoAppManager()
	{

	}

	bool CDemoAppManager::Initialize()
	{
		return true;
	}

	bool CDemoAppManager::RunLopp()
	{
		return true;
	}

	void CDemoAppManager::ResizeWindow(int w, int h)
	{
	}
}
#endif // USE_OPENGL