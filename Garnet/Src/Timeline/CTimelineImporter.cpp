#ifdef USE_BINARY_READ
#include "CTimelineImporter.h"
#include "CTimelineClip.h"
#include "../Binary/CBinaryReader.h"
#include "../Message/Console.h"

namespace timeline
{
	bool CTimelineImporter::Import(const std::vector<unsigned char>& Data, std::shared_ptr<CTimelineClip>& DstClip)
	{
		// Console::Log("[Timeline Import Error]\n");

		if (Data.empty())
		{
			Console::Log("[Timeline Import Error] Data is empty.\n");
			return false;
		}

		if (!DstClip)
		{
			Console::Log("[Timeline Import Error] DstClip is empty.\n");
			return false;
		}

		binary::CBinaryReader Reader = binary::CBinaryReader(Data);

		// ƒwƒbƒ_‚ð“Ç‚Þ(GLTL)
		std::string Head = std::string();
		if (!Reader.GetString(Head, 4)) return false;

		if (Head != "GTTL")
		{
			Console::Log("[Timeline Import Error] Invalid Head (%s)\n", Head.c_str());
			return false;
		}

		float MaxTime = 0.0f;
		if (!Reader.GetFloat(MaxTime)) return false;

		return true;
	}
}
#endif