#pragma once

#ifdef USE_BINARY_READ

#include <memory>
#include <vector>

namespace timeline
{
	class CTimelineClip;

	class CTimelineImporter
	{
	public:
		static bool Import(const std::vector<unsigned char>& Data, std::shared_ptr<CTimelineClip>& DstClip);
	};
}
#endif