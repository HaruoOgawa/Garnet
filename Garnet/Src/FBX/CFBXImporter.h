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
		
		static bool Analyse(api::IGraphicsAPI* pGraphicsAPI, fbxsdk::FbxScene* Scene, bool IsUseObject, std::shared_ptr<object::C3DObject>& Object,
			const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
			const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment);

		static bool Analyse(api::IGraphicsAPI* pGraphicsAPI, std::vector<FbxMesh*>& pFbxMeshList, const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo,
			fbxsdk::FbxNode* pFBXNode, std::vector<std::shared_ptr<graphics::CTexture>>& TextureList,
			std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList);

		static bool CreateDummyMaterial(api::IGraphicsAPI* pGraphicsAPI, fbxsdk::FbxNode* pFBXNode, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
			const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList);

		static bool CreateMesh(fbxsdk::FbxNode* pFBXNode, std::vector<FbxMesh*>& pFbxMeshList, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList);

		static bool CreateNodeList(fbxsdk::FbxScene* Scene, const std::vector<FbxMesh*>& pFbxMeshList, std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList,
			const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, std::vector<std::vector<int>>& RootNodeIndexList);
		
		static bool CreateNode(fbxsdk::FbxNode* pFBXNode, const std::vector<FbxMesh*>& pFbxMeshList, std::vector<FbxNode*>& pFbxNodeList,
			std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList);
	public:
		static bool ImportFBX(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::shared_ptr<object::C3DObject>& Object,
			const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
			const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment);
		static bool ImportFBXAnimation(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList);
	};
}
#endif // USE_FBX