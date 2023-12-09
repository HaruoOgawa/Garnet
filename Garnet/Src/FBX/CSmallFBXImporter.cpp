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
	bool CSmallFBXImporter::ImportFBX(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, object::C3DObject* Object,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame)
	{
		std::vector<std::shared_ptr<animation::CAnimationClip>> AnimationClipList;

		if (!Import(pGraphicsAPI, Data, true, Object, AnimationClipList, MaterialFrame)) return false;

		return true;
	}

	bool CSmallFBXImporter::ImportFBXAnimation(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList)
	{
		std::shared_ptr<object::C3DObject> Object = std::make_shared<object::C3DObject>("", "");

		if (!Import(pGraphicsAPI, Data, false, Object.get(), AnimationClipList, nullptr)) return false;

		return true;
	}

	bool CSmallFBXImporter::Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, bool IsUseObject, object::C3DObject* Object,
		std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame)
	{
		std::istringstream stream(std::string(Data.begin(), Data.end()));

		sfbx::DocumentPtr doc = sfbx::MakeDocument();
		doc->readBinary(stream);

		for (const auto& RootNode : doc->getRootNodes())
		{
			Console::Log("[CPP] RootNode->getName(): %s\n", RootNode->getName().data());

			for (const auto& Child : RootNode->getChildren())
			{
				Console::Log("[CPP] Child->getName(): %s\n", Child->getName().data());
			}
		}

		return true;
	}
}
#endif