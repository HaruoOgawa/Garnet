#pragma once

#include <vector>
#include <memory>

#include "E3DObjectType.h"
#include "CNode.h"
#include "../Graphics/CMesh.h"
#include "../Graphics/CMaterial.h"
#include "../Graphics/CTextureSet.h"
#include "../Graphics/CMaterialFrame.h"
#include "../Math/CTransform.h"
#ifdef USE_ANIMATION
#include "../Animation/CAnimationController.h"
#endif
#include "../Interface/IResource.h"

namespace resource { class CLoadWorker; }

namespace object
{
	class CNode;

	class C3DObject
	{
		//
		bool m_IsCreated;
		bool m_ExistFirstDelayResource;

		std::vector<unsigned char> m_BinaryData;
		std::string m_FileName;
		std::vector<std::shared_ptr<resource::IResource>> m_RuntimeLoadResourceList;

		//
		const std::string m_PassName;
		const std::string m_DepthPassName;
		
		std::shared_ptr<math::CTransform> m_ObjectTransform;

		std::vector<std::shared_ptr<CNode>> m_NodeList;
		std::vector<std::shared_ptr<graphics::CMesh>> m_MeshList;
		std::vector<std::shared_ptr<graphics::CMaterial>> m_MaterialList;
		
		std::vector<std::vector<int>> m_RootNodeIndexList;

		std::shared_ptr<graphics::CTextureSet> m_TextureSet;

		std::shared_ptr<graphics::CMaterialFrame> m_DepthMF;
#ifdef USE_ANIMATION
		std::shared_ptr<animation::CAnimationController> m_AnimationController;
		std::vector<glm::mat4> m_CurrentSkinMatrixList;
#endif
	private:
		void CalcWorldMatrix(std::shared_ptr<CNode>& Node, const glm::mat4& ParentWorldMatrix);

		void ApplyParentNode(std::shared_ptr<CNode>& Node, const std::shared_ptr<CNode>& ParentNode);
	public:
		C3DObject(const std::string& PassName, const std::string& DepthPassName);
		virtual ~C3DObject();

		void SetBinaryData(const std::vector<unsigned char>& Data, const std::string& FileName);

		static bool CreateSimply(api::IGraphicsAPI* pGraphicsAPI, std::shared_ptr<object::C3DObject>& Object,
			const std::shared_ptr<renderer::CRendererCreateInfo>& createInfo,
			const std::shared_ptr<graphics::CMaterial>& Material, const std::shared_ptr<graphics::CMaterialFrame>& DepthMF);

		bool		 CreateFromMemory(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CMaterialFrame>& BaseMF, const std::shared_ptr<graphics::CMaterialFrame>& DepthMF, E3DObjectType ObjectType);
		bool		 Create(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterialFrame>& DepthMF);
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, float DeltaSecondsTime);
		virtual bool Draw(bool IsDepthPass, bool DrawOutline, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& DebugSphere = nullptr);

		void ApplyDefaultLocalTransform();

		void CalcWorldMatrix();

		void ApplyParentNode();

		void AddNode(const std::shared_ptr<CNode>& Node);
		const std::vector<std::shared_ptr<CNode>>& GetNodeList() const;
		
		void AddMesh(const std::shared_ptr<graphics::CMesh>& Mesh);
		const std::vector<std::shared_ptr<graphics::CMesh>>& GetMeshList() const;

		void AddMaterial(const std::shared_ptr<graphics::CMaterial>& Material);
		const std::vector<std::shared_ptr<graphics::CMaterial>>& GetMaterialList() const;
		
#ifdef USE_ANIMATION
		void SetAnimationSkeleton(const std::shared_ptr<animation::CSkeleton>& Skeleton);
		void AddAnimationClip(const std::shared_ptr<animation::CAnimationClip>& Clip);
		void AddHumanoidAnimationClip(const std::shared_ptr<animation::CAnimationClip>& SourceClip, const std::string& MotionName, animation::SAnimationLayout Layout, bool IsLoop);

		const std::vector<std::shared_ptr<animation::CAnimationClip>>& GetAnimationClipList() const;
#endif

		void SetRootNodeIndexList(const std::vector<std::vector<int>>& RootNodeIndexList);
		const std::vector<std::vector<int>>& GetRootNodeIndexList() const;

		const glm::vec3& GetPos() const;
		void SetPos(const glm::vec3& Pos);

		const glm::quat& GetRot() const;
		void SetRot(const glm::quat& Rot);

		const glm::vec3& GetScale() const;
		void SetScale(const glm::vec3& Scale);

		void ChangeMotion(int Index); // インデックス指定でモーションを変更
		void ChangeMotion(const std::string& MotionName); // 名前指定でモーションを変更
		
		const std::shared_ptr<graphics::CTextureSet>& GetTextureSet() const;

		void AddRuntimeLoadResource(const std::shared_ptr <resource::IResource>& Resource);
	};
}