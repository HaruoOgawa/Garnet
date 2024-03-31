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

		std::string m_FileName;
		std::vector<unsigned char> m_BinaryData;
	public:
		CAudioClip();
		virtual ~CAudioClip();

		bool Create(const std::vector<unsigned char>& Data, const std::string& FileName);

		bool PlayLoop();
		bool PlayOneShot();

		bool Stop();
	};
}