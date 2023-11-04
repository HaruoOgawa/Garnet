#pragma once

#include "CNode.h"
#include "../Graphics/CMaterial.h"
#include "../Graphics/CTextureSet.h"
#include "../Math/CTransform.h"
#include "../Animation/CSkin.h"
#include "../Animation/CAnimationClip.h"
#include <vector>
#include <memory>

namespace object
{
	class CNode;

	class C3DObject
	{
		const std::string m_PassName;
		const std::string m_DepthPassName;
		
		std::shared_ptr<math::CTransform> m_ObjectTransform;

		std::vector<std::shared_ptr<CNode>> m_NodeList;
		std::vector<std::shared_ptr<graphics::CMesh>> m_MeshList;
		std::vector<std::shared_ptr<graphics::CMaterial>> m_MaterialList;
		
		std::vector<std::vector<int>> m_RootNodeIndexList;

		std::shared_ptr<graphics::CTextureSet> m_TextureSet;

		std::vector<std::shared_ptr<animation::CSkin>> m_AnimationSkinList;
		std::vector<std::shared_ptr<animation::CAnimationClip>> m_AnimationClipList;
		int m_CurrentClipIndex;
	private:
		void CalcWorldMatrix();
		void CalcWorldMatrix(std::shared_ptr<CNode>& Node, const glm::mat4& ParentWorldMatrix);

		void ApplyParentNode();
		void ApplyParentNode(std::shared_ptr<CNode>& Node, const std::shared_ptr<CNode>& ParentNode);

		bool IsPlayingAnimation();
	public:
		C3DObject(const std::string& PassName, const std::string& DepthPassName);
		virtual ~C3DObject();

		bool		 Create(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment, const std::shared_ptr<graphics::CTextureSet>& TextureSet);
		virtual bool Update(float DeltaSecondsTime);
		virtual bool Draw(bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, 
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& DebugSphere = nullptr);

		void AddNode(const std::shared_ptr<CNode>& Node);
		const std::vector<std::shared_ptr<CNode>>& GetNodeList() const;
		
		void AddMesh(const std::shared_ptr<graphics::CMesh>& Mesh);
		const std::vector<std::shared_ptr<graphics::CMesh>>& GetMeshList() const;

		void AddMaterial(const std::shared_ptr<graphics::CMaterial>& Material);
		const std::vector<std::shared_ptr<graphics::CMaterial>>& GetMaterialList() const;
		
		void AddAnimationSkin(const std::shared_ptr<animation::CSkin >& Skin);
		void AddAnimationClip(const std::shared_ptr<animation::CAnimationClip>& Clip);

		void SetRootNodeIndexList(const std::vector<std::vector<int>>& RootNodeIndexList);
		const std::vector<std::vector<int>>& GetRootNodeIndexList() const;

		const glm::vec3& GetPos() const;
		void SetPos(const glm::vec3& Pos);

		const glm::quat& GetRot() const;
		void SetRot(const glm::quat& Rot);

		const glm::vec3& GetScale() const;
		void SetScale(const glm::vec3& Scale);

		void SetPlayClipIndex(int Index);
	};
}