#pragma once

#ifdef USE_BINARY_WRITE

#include <memory>
#include <vector>
#include <string>

namespace binary { class CBinaryWriter; }

namespace timeline
{
	class CTimelineClip;

	class CTimelineExporter
	{
	private:
		static bool WriteTrack(binary::CBinaryWriter& Writer, const std::shared_ptr<CTimelineClip>& Clip);
		static bool WriteSampler(binary::CBinaryWriter& Writer, const std::shared_ptr<CTimelineClip>& Clip);
	public:
		static bool Export(const std::string& FileName, const std::shared_ptr<CTimelineClip>& Clip);
	};
}
#endif