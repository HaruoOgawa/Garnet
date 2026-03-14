#include "COBJImporter.h"
#include "../../Interface/IGraphicsAPI.h"
#include "../../Object/C3DObject.h"
#include "../../Graphics/CMaterialFrame.h"
#include "../../LoadWorker/C3DObjectLoader.h"

namespace obj
{
	bool COBJImporter::Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data,
		const std::string& BaseDir, object::C3DObject* Object,
		const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList, resource::C3DObjectLoader* p3DObjectLoader)
	{
		return true;
	}
}