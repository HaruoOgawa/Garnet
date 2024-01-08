#include "CPmxImporter.h"

#ifdef USE_MMD
namespace mmd
{
	bool CPmxImporter::ImportPmx(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, object::C3DObject* Object,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame)
	{
		return true;
	}
}
#endif // USE_MMD