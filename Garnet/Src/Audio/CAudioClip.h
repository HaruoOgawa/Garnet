#pragma once

#include <vector>

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

		std::vector<unsigned char> m_BinaryData;
	public:
		CAudioClip();
		virtual ~CAudioClip();

		bool Create(const std::vector<unsigned char>& Data);

		bool PlayLoop();
		bool PlayOneShot();

		bool Stop();
	};
}