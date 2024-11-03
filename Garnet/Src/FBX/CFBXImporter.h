#pragma once

#if defined(USE_FBX) && !defined(USE_SMALL_FBX)

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <algorithm>

#include "../Interface/IGraphicsAPI.h"
#include "../LoadWorker/CFile.h"
#include "../Math/CMath.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <fbxsdk.h>
using namespace fbxsdk;

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
	class CFBXImporter
	{
	private:
		static bool Import(api::IGraphicsAPI* pGraphicsAPI, const std::string& FileName, bool IsUseObject, object::C3DObject* Object,
			std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame);
		
		static bool Analyse(api::IGraphicsAPI* pGraphicsAPI, FbxScene* Scene, bool IsUseObject, object::C3DObject* Object,
			std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame);

		static bool CreateDrawInfo(api::IGraphicsAPI* pGraphicsAPI, std::vector<FbxMesh*>& pFbxMeshList, const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame,
			const std::vector<FbxNode*>& pFbxNodeList, FbxNode* pFBXNode, std::vector<std::shared_ptr<graphics::CTexture>>& TextureList,
			std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::shared_ptr<animation::CSkeleton>& Skeleton, const bool IsMixamoFbx);

		static bool CreateMaterial(api::IGraphicsAPI* pGraphicsAPI, FbxNode* pFBXNode, std::vector<FbxSurfaceMaterial*>& pFbxMaterialList, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, const std::shared_ptr<animation::CSkeleton>& Skeleton);
		
		static bool CreateDummyMaterial(api::IGraphicsAPI* pGraphicsAPI, FbxNode* pFBXNode, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::shared_ptr<animation::CSkeleton>& Skeleton);

		static bool CreateMesh(api::IGraphicsAPI* pGraphicsAPI, int NodeIndex, FbxNode* pFBXNode, std::vector<FbxMesh*>& pFbxMeshList, const std::vector<FbxSurfaceMaterial*>& pFbxMaterialList,
			std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, const std::shared_ptr<animation::CSkeleton>& Skeleton, const bool IsMixamoFbx);

		static bool CreateNodeList(FbxScene* Scene, std::vector<FbxNode*>& pFbxNodeList, std::vector<std::shared_ptr<object::CNode>>& NodeList, std::vector<int>& RootNodeIndexList, const bool IsMixamoFbx);
		
		static bool CreateNode(FbxNode* pFBXNode, std::vector<FbxNode*>& pFbxNodeList, std::vector<std::shared_ptr<object::CNode>>& NodeList, const bool IsMixamoFbx);

		static bool ConnectNodeTo(std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<FbxNode*>& pFbxNodeList, const std::vector<FbxMesh*>& pFbxMeshList, const std::shared_ptr<animation::CSkeleton>& Skeleton);

		static bool CreateAnimationSkeleton(FbxNode* pFBXNode, std::shared_ptr<animation::CSkeleton>& Skeleton, std::vector<FbxNode*>& FbxBoneList, const std::vector<std::shared_ptr<object::CNode>>& NodeList, const bool IsMixamoFbx);
		
		static void ApplyParentBoneList(const std::shared_ptr<animation::CSkeleton>& Skeleton, const std::vector<std::shared_ptr<object::CNode>>& NodeList);
		
		static bool CreateAnimation(FbxScene* Scene, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList, const std::vector<std::shared_ptr<object::CNode>>& NodeList, 
			const std::shared_ptr<animation::CSkeleton>& Skeleton, const std::vector<FbxNode*>& FbxBoneList, const bool IsMixamoFbx);
		
		// Helper Function //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		template<class T>
		static bool ReadGeometryElement(std::vector<float>& AttributeData, int Dimention, T* pFbxElement, int CtrlPointIndex, int VertexCounter);

		static bool RecalculateTangent(std::vector<float>& TangentDat, const std::vector<float>& PosotionData, const std::vector<float>& TexcoordData, const std::vector<unsigned short>& Indices);

		static std::shared_ptr<object::CNode> GetBoneNode(const std::string& BoneName, const std::vector<std::shared_ptr<object::CNode>>& NodeList);

		static unsigned int FindBoneIndexUsingName(const std::shared_ptr<animation::CSkeleton>& Skeleton, const std::string& BoneName);

		static bool CheckIsMixamo(FbxNode* pFBXNode);
	public:
		static bool ImportFBX(api::IGraphicsAPI* pGraphicsAPI, const std::string& FileName, object::C3DObject* Object,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, resource::C3DObjectLoader* p3DObjectLoader);
		static bool ImportFBXAnimation(api::IGraphicsAPI* pGraphicsAPI, const std::string& FileName, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList);
	};
}
#endif // USE_FBX