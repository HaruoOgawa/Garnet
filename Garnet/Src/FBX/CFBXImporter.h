#pragma once

#ifdef USE_FBX

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <algorithm>

#include "../Interface/IGraphicsAPI.h"
#include "../File/CFile.h"

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

namespace fbx
{
	class CFBXImporter
	{
	private:
		static bool Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, bool IsUseObject, std::shared_ptr<object::C3DObject>& Object, 
			std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList,
			const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
			const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment);
		
		static bool Analyse(api::IGraphicsAPI* pGraphicsAPI, FbxScene* Scene, bool IsUseObject, std::shared_ptr<object::C3DObject>& Object,
			std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList,
			const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
			const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment);

		static bool CreateDrawInfo(api::IGraphicsAPI* pGraphicsAPI, std::vector<FbxMesh*>& pFbxMeshList, const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo,
			FbxNode* pFBXNode, std::vector<std::shared_ptr<graphics::CTexture>>& TextureList,
			std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::shared_ptr<animation::CSkin>& Skin);

		static bool CreateDummyMaterial(api::IGraphicsAPI* pGraphicsAPI, FbxNode* pFBXNode, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
			const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::shared_ptr<animation::CSkin>& Skin);

		static bool CreateMesh(FbxNode* pFBXNode, std::vector<FbxMesh*>& pFbxMeshList, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::shared_ptr<animation::CSkin>& Skin);

		static bool CreateNodeList(FbxScene* Scene, std::vector<FbxNode*>& pFbxNodeList, std::vector<std::shared_ptr<object::CNode>>& NodeList, std::vector<std::vector<int>>& RootNodeIndexList);
		
		static bool CreateNode(FbxNode* pFBXNode, std::vector<FbxNode*>& pFbxNodeList, std::vector<std::shared_ptr<object::CNode>>& NodeList);

		static bool ConnectNodeTo(std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<FbxNode*>& pFbxNodeList, const std::vector<FbxMesh*>& pFbxMeshList,
			const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, const std::shared_ptr<animation::CSkin>& Skin);

		static bool CreateAnimationSkin(FbxNode* pFBXNode, std::shared_ptr<animation::CSkin>& Skin, std::vector<FbxNode*>& FbxJointList, const std::vector<std::shared_ptr<object::CNode>>& NodeList);
		
		static void ApplyParentJointList(const std::shared_ptr<animation::CSkin>& Skin, const std::vector<std::shared_ptr<object::CNode>>& NodeList);
		
		static bool CreateAnimation(FbxScene* Scene, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList, const std::vector<std::shared_ptr<object::CNode>>& NodeList, 
			const std::shared_ptr<animation::CSkin>& Skin, const std::vector<FbxNode*>& FbxJointList);
		
		// Helper Function //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		template<class T>
		static bool ReadGeometryElement(std::vector<float>& AttributeData, int Dimention, T* pFbxElement, int CtrlPointIndex, int VertexCounter);

		static bool RecalculateTangent(std::vector<float>& TangentDat, const std::vector<float>& PosotionData, const std::vector<float>& TexcoordData, const std::vector<unsigned short>& Indices);

		static std::shared_ptr<object::CNode> GetJointNode(const std::string& JointName, const std::vector<std::shared_ptr<object::CNode>>& NodeList);

		static unsigned int FindJointIndexUsingName(const std::shared_ptr<animation::CSkin>& Skin, const std::string& JointName);
	public:
		static bool ImportFBX(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::shared_ptr<object::C3DObject>& Object,
			const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
			const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment);
		static bool ImportFBXAnimation(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList);
	};
}
#endif // USE_FBX