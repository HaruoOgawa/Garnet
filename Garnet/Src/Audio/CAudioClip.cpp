#include "CAudioClip.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <Windows.h>
#endif

namespace audio
{
	CAudioClip::CAudioClip():
		m_Loop(false),
		m_FileName(std::string())
	{
	}

	CAudioClip::~CAudioClip()
	{
		Stop();
	}

	bool CAudioClip::Create(const std::vector<unsigned char>& Data, const std::string& FileName)
	{
#ifdef __EMSCRIPTEN__
		m_FileName = FileName;
#else
		m_BinaryData = Data;
#endif

		return true;
	}

	bool CAudioClip::PlayLoop()
	{
		m_Loop = true;
		
#ifdef __EMSCRIPTEN__
		EM_ASM({
			const fileName = Module.UTF8ToString($0);
			const loop = ($1 != 0);

			window.g3dPlayAudio(fileName, loop);
		}, m_FileName.c_str(), 1);

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
		EM_ASM({
			const fileName = Module.UTF8ToString($0);
			const loop = ($1 != 0);

			window.g3dPlayAudio(fileName, loop);
		}, m_FileName.c_str(), 0);

		return true;
#else
		auto Flags = SND_MEMORY | SND_ASYNC;
		return PlaySound((LPCTSTR)(&m_BinaryData[0]), NULL, Flags);
#endif // __EMSCRIPTEN__
	}

	bool CAudioClip::Stop()
	{
#ifdef __EMSCRIPTEN__
		EM_ASM({
			window.g3dStopAudio();
		});

		return true;
#else
		return PlaySound(NULL, 0, 0);
#endif // __EMSCRIPTEN__
	}
}