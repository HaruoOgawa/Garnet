#pragma once

#ifdef USE_MMD
#include <vector>
#include <memory>

#include "../../Interface/IGraphicsAPI.h"
#include "../../Interface/IResource.h"
#include "CPmxModel.h"

namespace graphics
{
	class CTexture;
	class CTextureSet;
	class CMaterial;
	class CMesh;
	class CMaterialCreateInfo;
	class CMaterialFrame;
}

namespace renderer
{
	enum class EDataType;
}

namespace animation
{
	class CSkin;
	class CJoint;
	class CAnimationClip;
	class CAnimationSampler;
	enum class EKeyFrameType;
}

namespace object {
	class C3DObject;
	class CNode;
}

namespace resource { class CLoadWorker; }

namespace mmd
{
	class CPmxImporter
	{
	private:
		static bool CreateMaterialList(api::IGraphicsAPI* pGraphicsAPI, const CPmxModel& model, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, const std::shared_ptr<animation::CSkin>& Skin);

		static bool CreateMeshList(const CPmxModel& model, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::shared_ptr<object::CNode>& RootNode, std::vector<std::shared_ptr<object::CNode>>& NodeList,
			const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList);

		static bool CreateTextureList(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::string& ModelFileName, const CPmxModel& model, std::vector<std::shared_ptr<graphics::CTexture>>& TextureList,
			std::vector<std::shared_ptr<resource::IResource>>& RuntimeLoadResourceList);
	public:
		static bool ImportPmx(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::string& ModelFileName, const std::vector<unsigned char>& Data, object::C3DObject* Object,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame);
	};
}
#endif // USE_MMD