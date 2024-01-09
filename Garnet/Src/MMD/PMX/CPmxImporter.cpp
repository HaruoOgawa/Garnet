#ifdef USE_MMD
#include "CPmxImporter.h"
#include "../../Debug/Message/Console.h"

namespace mmd
{
	bool CPmxImporter::ImportPmx(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, object::C3DObject* Object,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame)
	{
		CPmxModel model;

		// ƒoƒCƒiƒŠ‚Ì‰ğÍ
		if (!model.Analyse(Data))
		{
			Console::Log("[Cpp Log] Error - Failed to Analyse Pmx Binary.\n");

			return false;
		}

		return true;
	}
}
#endif // USE_MMD