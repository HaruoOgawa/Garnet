#pragma once

#ifdef USE_BINARY_READ

#include <memory>
#include <vector>

namespace binary { class CBinaryReader; }

namespace timeline
{
	class CTimelineClip;

	class CTimelineImporter
	{
	private:
		static bool ReadTrack(binary::CBinaryReader& Reader, std::shared_ptr<CTimelineClip>& DstClip);
		static bool ReadSampler(binary::CBinaryReader& Reader, std::shared_ptr<CTimelineClip>& DstClip);
	public:
		static bool Import(const std::vector<unsigned char>& Data, std::shared_ptr<CTimelineClip>& DstClip);
	};
}
#endif