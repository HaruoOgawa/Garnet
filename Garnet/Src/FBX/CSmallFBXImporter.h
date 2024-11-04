#pragma once
#if defined(USE_FBX) && defined(USE_SMALL_FBX)

// EmscriptenBuildで使用したいのでWebブラウザとデスクトップのWebGPUではSmallFBXを使用する
// その他GraphicsAPIでは通常通りAutoDeskのFBX SDKを引き続き使用する
// https://github.com/i-saint/SmallFBX

#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <memory>
#include <iostream>
#include <chrono>

#include <sstream>
#include <iostream>

#ifdef __cpp_lib_span
#include <span>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include "../Interface/IGraphicsAPI.h"
#include "../LoadWorker/CFile.h"
#include "../Math/CMath.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <SmallFBX.h>

namespace graphics
{
	class CTexture;
	class CTextureSet;
	class CMaterial;
	class CMesh;
	class CMaterialCreateInfo;
	class CMaterialFrame;
}

namespace graphics
{
	enum class EDataType;
}

namespace animation
{
	class CSkeleton;
	class CBone;
	class CAnimationClip;
	class CAnimationSampler;
	enum class ERigType;
}

namespace object {
	class C3DObject;
	class CNode;
}

namespace resource {
	class CLoadWorker;
	class C3DObjectLoader;
}

namespace fbx
{
	class CSmallFBXImporter
	{
	private:
		static bool Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, bool IsUseObject, object::C3DObject* Object,
			std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, animation::ERigType RigType);

		static bool Analyse(api::IGraphicsAPI* pGraphicsAPI, const sfbx::DocumentPtr& Doc, bool IsUseObject, object::C3DObject* Object,
			std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, animation::ERigType RigType);

		static bool CreateNodeList(const sfbx::DocumentPtr& Doc, std::vector<sfbx::Object*>& pFbxNodeList, std::vector<std::shared_ptr<object::CNode>>& NodeList, std::vector<int>& RootNodeIndexList, const bool IsMixamoFbx);
		static bool CreateNode(sfbx::Object* pFBXNode, std::vector<sfbx::Object*>& pFbxNodeList, std::vector<std::shared_ptr<object::CNode>>& NodeList, const bool IsMixamoFbx);
		static bool ConnectNodeTo(std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<sfbx::Object*>& pFbxNodeList, const std::vector<sfbx::Mesh*>& pFbxMeshList, const std::shared_ptr<animation::CSkeleton>& Skeleton);

		static bool CreateDrawInfo(api::IGraphicsAPI* pGraphicsAPI, std::vector<sfbx::Mesh*>& pFbxMeshList, const std::vector<sfbx::Object*>& FbxBoneList, const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame,
			sfbx::Object* pFBXNode, std::vector<std::shared_ptr<graphics::CTexture>>& TextureList,
			std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::shared_ptr<animation::CSkeleton>& Skeleton, const bool IsMixamoFbx);

		static bool CreateMaterial(api::IGraphicsAPI* pGraphicsAPI, sfbx::Mesh* pFbxMesh, std::vector<sfbx::Material*>& pFbxMaterialList, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, const std::shared_ptr<animation::CSkeleton>& Skeleton);

		static bool CreateDummyMaterial(api::IGraphicsAPI* pGraphicsAPI, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::shared_ptr<animation::CSkeleton>& Skeleton);

		static bool CreateMesh(api::IGraphicsAPI* pGraphicsAPI, sfbx::Object* pFBXNode, std::vector<sfbx::Mesh*>& pFbxMeshList, const std::vector<sfbx::Material*>& pFbxMaterialList, const std::vector<sfbx::Object*>& FbxBoneList,
			std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, const std::shared_ptr<animation::CSkeleton>& Skeleton, const bool IsMixamoFbx);

		static bool CreateAnimationSkeleton(sfbx::Object* pFBXNode, std::shared_ptr<animation::CSkeleton>& Skeleton, std::vector<sfbx::Object*>& FbxBoneList, const std::vector<std::shared_ptr<object::CNode>>& NodeList);

		static void ApplyParentBoneList(const std::shared_ptr<animation::CSkeleton>& Skeleton, const std::vector<std::shared_ptr<object::CNode>>& NodeList);

		static bool CreateAnimation(const sfbx::DocumentPtr& Doc, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList, const std::vector<std::shared_ptr<object::CNode>>& NodeList,
			const std::shared_ptr<animation::CSkeleton>& Skeleton, const std::vector<sfbx::Object*>& FbxBoneList, const bool IsMixamoFbx);

		// Helper Function //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static bool RecalculateTangent(std::vector<float>& TangentData, const std::vector<float>& PosotionData, const std::vector<float>& TexcoordData, const std::vector<unsigned short>& Indices);

		static std::shared_ptr<object::CNode> GetBoneNode(const std::string& BoneName, const std::vector<std::shared_ptr<object::CNode>>& NodeList);

		static unsigned int FindBoneIndexUsingName(const std::shared_ptr<animation::CSkeleton>& Skeleton, const std::string& BoneName);

		static bool CheckIsMixamo(sfbx::Object* pFBXNode);
	public:
		static bool ImportFBX(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, object::C3DObject* Object,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, resource::C3DObjectLoader* p3DObjectLoader, animation::ERigType RigType);
		static bool ImportFBXAnimation(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList, animation::ERigType RigType);
	};
}
#endif