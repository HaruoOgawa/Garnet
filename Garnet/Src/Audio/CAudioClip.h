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
		std::string m_FileName;
		std::string m_Extension;

		bool m_Loop;
		bool m_Playing;

	private:
		bool DoCmdWithError(const std::string Cmd);

		bool Release();
	public:
		CAudioClip();
		virtual ~CAudioClip();

		void SetFileName(const std::string& Name);
		const std::string& GetFileName() const;

		bool CreateFromFile(const std::string& FileName);
		bool CreateFromMemory(const std::vector<unsigned char>& Data);

		bool PlayLoop();
		bool PlayOneShot();

		bool Stop();

		bool IsPlaying() const;
	};
}