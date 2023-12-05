#if defined(USE_FBX) && defined(USE_SMALL_FBX)
#include "CSmallFBXImporter.h"

#include "../Animation/CBoneNameProvider.h"

#include "../Object/C3DObject.h"

#include "../Debug/Message/Console.h"
#include "../Math/CMath.h"

#include "../Animation/CAnimationClip.h"
#include "../Animation/CSkin.h"
#include "../Animation/CJoint.h"

#include "../Graphics/CMaterialFrame.h"

namespace fbx
{
	bool CSmallFBXImporter::ImportFBX(api::IGraphicsAPI* pGraphicsAPI, const std::string& FileName, object::C3DObject* Object,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame)
	{
		std::vector<std::shared_ptr<animation::CAnimationClip>> AnimationClipList;

		if (!Import(pGraphicsAPI, FileName, true, Object, AnimationClipList, MaterialFrame)) return false;

		return true;
	}

	bool CSmallFBXImporter::ImportFBXAnimation(api::IGraphicsAPI* pGraphicsAPI, const std::string& FileName, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList)
	{
		std::shared_ptr<object::C3DObject> Object = std::make_shared<object::C3DObject>("", "");

		if (!Import(pGraphicsAPI, FileName, false, Object.get(), AnimationClipList, nullptr)) return false;

		return true;
	}

	bool CSmallFBXImporter::Import(api::IGraphicsAPI* pGraphicsAPI, const std::string& FileName, bool IsUseObject, object::C3DObject* Object,
		std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame)
	{
		sfbx::DocumentPtr doc = sfbx::MakeDocument(FileName);
		sfbx::Model* model = doc->getRootModel();

		return true;
	}
}
#endif