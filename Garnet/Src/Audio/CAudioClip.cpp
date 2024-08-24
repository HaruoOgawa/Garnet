#include "CAudioClip.h"
#include "../Format/CPathFormatter.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <array>
#include <Windows.h>
#include "../Message/Console.h"
#endif

namespace audio
{
	CAudioClip::CAudioClip():
		m_FileName(std::string()),
		m_Extension(std::string()),
		m_Loop(false),
		m_Playing(false)
	{
	}

	CAudioClip::~CAudioClip()
	{
		Stop();
		Release();
	}

	void CAudioClip::SetFileName(const std::string& Name)
	{
		m_FileName = Name;
		m_Extension = format::CPathFormatter::GetExtention(Name);
	}

	const std::string& CAudioClip::GetFileName() const
	{
		return m_FileName;
	}

	bool CAudioClip::DoCmdWithError(const std::string Cmd)
	{
#ifdef __EMSCRIPTEN__
		return true;
#else
		std::array<char, MAXERRORLENGTH> errorString;
		mciGetErrorStringA(
			mciSendStringA(
				Cmd.c_str(),
				nullptr,
				0,
				nullptr),
			errorString.data(), MAXERRORLENGTH);

		Console::Log("[Audio Log] %s\n", errorString.data());

		/*return .empty();*/
		return true;
#endif
	}

	bool CAudioClip::CreateFromFile(const std::string& FileName)
	{
		SetFileName(FileName);

#ifdef __EMSCRIPTEN__
		EM_ASM({
			const fileName = Module.UTF8ToString($0);

			window.g3dCreateAudio(fileName);
			}, FileName.c_str());
#else
		std::string Cmd = "open " + FileName + " alias " + m_Extension;
		if (!DoCmdWithError(Cmd)) return false;
#endif
		return true;
	}

	bool CAudioClip::CreateFromMemory(const std::vector<unsigned char>& Data)
	{
		// ToDo: 未実装。SoundShaderとかにこの仕組みを使う。
		// https://chatgpt.com/c/1639c201-8caa-48af-a5b8-3631bc825c53
		// 基本的な流れとしては
		// VC++: バイナリを一時ファイルに書き出してこれを元にAudio Load。Loadが終わったらstd::removeで一時ファイルを削除
		// Web: JavaScriptにバイナリから再生できるAPIが存在するのでそれをそのまま使う

		return true;
	}

	bool CAudioClip::Release()
	{
#ifdef __EMSCRIPTEN__
		return true;
#else
		std::string Cmd = "close " + m_Extension;
		return DoCmdWithError(Cmd);
#endif
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
		std::string Cmd = "play " + m_Extension;
		return DoCmdWithError(Cmd);
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
		std::string Cmd = "play " + m_Extension;
		return DoCmdWithError(Cmd);
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
		std::string Cmd = "pause " + m_Extension;
		return DoCmdWithError(Cmd);
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