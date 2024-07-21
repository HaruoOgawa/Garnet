#pragma once

#ifdef USE_BINARY_WRITE

#include <memory>
#include <vector>
#include <string>

namespace timeline
{
	class CTimelineClip;

	class CTimelineExporter
	{
	public:
		static bool Export(const std::string& FileName, const std::shared_ptr<CTimelineClip>& Clip);
	};
}
#endif