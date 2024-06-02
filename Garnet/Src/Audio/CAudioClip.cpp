#include "CAudioClip.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <Windows.h>
#endif

namespace audio
{
	CAudioClip::CAudioClip():
		m_FileName(std::string()),
		m_Loop(false),
		m_Playing(false)
	{
	}

	CAudioClip::~CAudioClip()
	{
		Stop();
	}

	void CAudioClip::SetFileName(const std::string& Name)
	{
		m_FileName = Name;
	}

	const std::string& CAudioClip::GetFileName() const
	{
		return m_FileName;
	}

	bool CAudioClip::Create(const std::vector<unsigned char>& Data, const std::string& FileName)
	{
#ifdef __EMSCRIPTEN__
		EM_ASM({
			const fileName = Module.UTF8ToString($0);

			window.g3dCreateAudio(fileName);
			}, FileName.c_str());
#else
		m_BinaryData = Data;
#endif

		return true;
	}

	bool CAudioClip::PlayLoop()
	{
		m_Loop = true;
		m_Playing = true;

#ifdef __EMSCRIPTEN__
		EM_ASM({
			const loop = ($0 != 0);

			window.g3dPlayAudio(loop);
		}, 1);

		return true;
#else
		auto Flags = SND_MEMORY | SND_ASYNC;
		Flags |= SND_LOOP;
		return PlaySound((LPCTSTR)(&m_BinaryData[0]), NULL, Flags);
#endif // __EMSCRIPTEN__
	}

	bool CAudioClip::PlayOneShot()
	{
		if (m_Playing)
		{
			Stop();
		}

		m_Loop = false;
		m_Playing = true;

#ifdef __EMSCRIPTEN__
		EM_ASM({
			const loop = ($0 != 0);

			window.g3dPlayAudio(loop);
		}, 0);

		return true;
#else
		auto Flags = SND_MEMORY | SND_ASYNC;
		return PlaySound((LPCTSTR)(&m_BinaryData[0]), NULL, Flags);
#endif // __EMSCRIPTEN__
	}

	bool CAudioClip::Stop()
	{
		m_Playing = false;

#ifdef __EMSCRIPTEN__
		EM_ASM({
			window.g3dStopAudio();
		});

		return true;
#else
		return PlaySound(NULL, 0, 0);
#endif // __EMSCRIPTEN__
	}

	bool CAudioClip::IsPlaying() const
	{
		if (!m_Playing) return m_Playing;

#ifdef __EMSCRIPTEN__
		int Result = EM_ASM_INT({
			return window.g3dIsAudioPlaying();
		});

		return (Result != 0);
#else
		return false;
#endif
	}
}