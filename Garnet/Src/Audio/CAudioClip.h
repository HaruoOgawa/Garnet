#pragma once

#include <vector>
#include <string>

namespace audio
{
	enum class EAudioType
	{
		none = -1,
		wav,
		mp3,
	};

	class CAudioClip
	{
		bool m_Loop;
		bool m_Playing;

#ifndef __EMSCRIPTEN__
		std::vector<unsigned char> m_BinaryData;
#endif // !__EMSCRIPTEN__

	public:
		CAudioClip();
		virtual ~CAudioClip();

		bool Create(const std::vector<unsigned char>& Data, const std::string& FileName);

		bool PlayLoop();
		bool PlayOneShot();

		bool Stop();

		bool IsPlaying() const;
	};
}