#include "CAudioClip.h"

#include <Windows.h>

namespace audio
{
	CAudioClip::CAudioClip():
		m_Loop(false)
	{
	}

	CAudioClip::~CAudioClip()
	{
	}

	bool CAudioClip::Create(const std::vector<unsigned char>& Data)
	{
		m_BinaryData = Data;

		return true;
	}

	bool CAudioClip::PlayLoop()
	{
		m_Loop = true;

#ifdef __EMSCRIPTEN__
		return true;
#else
		auto Flags = SND_MEMORY | SND_ASYNC;
		Flags |= SND_LOOP;
		return PlaySound((LPCTSTR)(&m_BinaryData[0]), NULL, Flags);
#endif // __EMSCRIPTEN__
	}

	bool CAudioClip::PlayOneShot()
	{
		m_Loop = false;

#ifdef __EMSCRIPTEN__
		return true;
#else
		auto Flags = SND_MEMORY | SND_ASYNC;
		return PlaySound((LPCTSTR)(&m_BinaryData[0]), NULL, Flags);
#endif // __EMSCRIPTEN__
	}

	bool CAudioClip::Stop()
	{
#ifdef __EMSCRIPTEN__
		return true;
#else
		return PlaySound(NULL, 0, 0);
#endif // __EMSCRIPTEN__
	}
}