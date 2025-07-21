#pragma once

#include <vector>
#include <memory>
#include <set>
#include <string>

#include "E3DObjectType.h"
#include "CNode.h"
#include "../Graphics/CMesh.h"
#include "../Graphics/CMaterial.h"
#include "../Graphics/CMorphController.h"
#include "../Graphics/CTextureSet.h"
#include "../Graphics/CMaterialFrame.h"
#include "../Math/CTransform.h"
#ifdef USE_ANIMATION
#include "../Animation/CAnimationController.h"
#include "../Animation/CBlendShapeController.h"
#endif
#include "../Interface/IResource.h"

namespace resource { class CLoadWorker; }

namespace object
{
	class CNode;

	class C3DObject : public std::enable_shared_from_this<C3DObject>
	{
		bool m_IsCreated;

		std::string m_FileName;

		std::vector<std::string> m_DefaultMaterialFrameList;

		std::vector<std::string> m_PassNameList;

		// コンポーネント
		std::vector<std::shared_ptr<scriptable::CComponent>> m_ComponentList;
		
		std::string						  m_ObjectName;
		std::shared_ptr<math::CTransform> m_ObjectTransform;

		bool m_Enabled;

		std::vector<std::shared_ptr<CNode>> m_NodeList;
		std::vector<std::shared_ptr<graphics::CMesh>> m_MeshList;

		//
		std::set<std::shared_ptr<CNode>> m_TLNodeList;
		std::set<std::shared_ptr<graphics::CMaterial>> m_TLMaterial;

		std::shared_ptr<graphics::CMorphController> m_MorphController;
		
		std::vector<int> m_RootNodeIndexList;

		std::shared_ptr<graphics::CTextureSet> m_TextureSet;

		std::shared_ptr<graphics::CMaterialFrame> m_DepthMF;
#ifdef USE_ANIMATION
		std::shared_ptr<animation::CAnimationController> m_AnimationController;
		std::shared_ptr<animation::CBlendShapeController> m_BlendShapeController;
		std::vector<glm::mat4> m_CurrentSkinMatrixList;
#endif
	private:
		void AsignTrackRef();

		void CalcWorldMatrix(std::shared_ptr<CNode>& Node, const glm::mat4& ParentWorldMatrix);

		void ApplyParentNode(std::shared_ptr<CNode>& Node, const std::shared_ptr<CNode>& ParentNode);

		bool DrawFromNode(const std::shared_ptr<CNode>& Node, api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera,
			const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);

		bool Draw(const std::shared_ptr<CNode>& Node, api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera,
			const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);

		// 物理
		void CreatePhysics(physics::IPhysicsEngine* pPhysicsEngine);
		void ApplyPhysicsConstraint(physics::IPhysicsEngine* pPhysicsEngine);

		void ApplyPhysicsWorldMatrix();
		void AlignPhysicsJoint();
	public:
		C3DObject();
		virtual ~C3DObject();

		void Reset();

		void SetFileName(const std::string& Name);
		const std::string& GetFileName() const;
		
		void AddDefaultMaterialFrame(const std::string& Name);
		const std::vector<std::string>& GetDefaultMaterialFrameList() const;

		void SetObjectName(const std::string& Name);
		const std::string& GetObjectName() const;

		const std::vector<std::string>& GetPassNameList() const;
		void AddPassName(const std::string& Name);

		// コンポーネント
		void AddComponent(const std::shared_ptr<scriptable::CComponent>& Component);
		const std::vector<std::shared_ptr<scriptable::CComponent>>& GetComponentList() const;

		bool HasTLTrackContent() const;
		void AddTLNode(const std::shared_ptr<CNode>& Node);
		const std::set<std::shared_ptr<CNode>>& GetTLNodeList() const;
		void AddTLMaterial(const std::shared_ptr<graphics::CMaterial>& Material);
		const std::set<std::shared_ptr<graphics::CMaterial>>& GetTLMaterial() const;

		const std::shared_ptr<math::CTransform>& GetObjectTransform() const;

		void SetEnabled(bool Flag);
		bool IsEnabled() const;

		bool AddPresetSimply(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine,
			const std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>>& createInfo,
			graphics::EPresetPrimitiveType PresetType,
			const std::shared_ptr<graphics::CMaterial>& Material,
			const std::shared_ptr<math::CTransform> NodeTransform = std::make_shared<math::CTransform>(), const std::shared_ptr<physics::IPhysicsObject>& PhysicsObject = nullptr);

		bool CreatePresetSimply(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine,
			const std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>>& createInfo,
			graphics::EPresetPrimitiveType PresetType,
			const std::shared_ptr<graphics::CMaterial>& Material, 
			const std::shared_ptr<math::CTransform> NodeTransform = std::make_shared<math::CTransform>(), const std::shared_ptr<physics::IPhysicsObject>& PhysicsObject = nullptr);
		
		bool Create(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine);
		
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, float DeltaSecondsTime, resource::CLoadWorker* pLoadWorker, 
			const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState);
		virtual bool LateUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, float DeltaSecondsTime);
		virtual bool FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, float DeltaSecondsTime);
		
		virtual bool Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& DebugSphere = nullptr);

		virtual bool DrawDebugBone(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& DebugSphere);
		virtual bool DrawDebugPhysics(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& DebugSphere);

		void ApplyDefaultLocalTransform();

		void CalcWorldMatrix();
		glm::mat4 CalcNoScaledWorldMatrix(const std::shared_ptr<object::CNode>& ChildNode);

		void ApplyParentNode();

		void AddNode(const std::shared_ptr<CNode>& Node);
		void ReserveNodeCount(size_t Count);
		const std::vector<std::shared_ptr<CNode>>& GetNodeList() const;
		std::shared_ptr<CNode> FindNodeByName(const std::string& Name);
		std::shared_ptr<CNode> FindNodeByIndex(int Index);

		void AddMesh(const std::shared_ptr<graphics::CMesh>& Mesh);
		const std::vector<std::shared_ptr<graphics::CMesh>>& GetMeshList() const;

		void AddMorphNode(const std::shared_ptr<CNode>& Node);

#ifdef USE_ANIMATION
		const std::shared_ptr<animation::CAnimationController>& GetAnimationController() const;
		const std::shared_ptr<animation::CBlendShapeController>& GetBlendShapeController() const;

		void AddBlendShapeNode(const std::shared_ptr<CNode>& Node);

		void SetAnimationSkeleton(const std::shared_ptr<animation::CSkeleton>& Skeleton);
		void AddAnimationClip(const std::shared_ptr<animation::CAnimationClip>& Clip, const std::string& MotionName, animation::SAnimationLayout Layout, bool IsLoop);
		void AddHumanoidAnimationClip(const std::shared_ptr<animation::CAnimationClip>& SourceClip, const std::string& MotionName, 
			animation::SAnimationLayout Layout, bool IsLoop, bool UseIK);
		void AddBlendShapeClip(const std::shared_ptr<animation::CBlendShapeClip>& Clip, const std::string& MotionName, bool IsLoop);

		const std::vector<std::shared_ptr<animation::CAnimationClip>>& GetAnimationClipList() const;

		bool IsPlayingAnimation() const;
#endif
		void SetRootNodeIndexList(const std::vector<int>& RootNodeIndexList);
		void AddRootNodeIndex(int Index);
		const std::vector<int>& GetRootNodeIndexList() const;

		const glm::vec3& GetPos() const;
		void SetPos(const glm::vec3& Pos);

		const glm::quat& GetRot() const;
		void SetRot(const glm::quat& Rot);

		const glm::vec3& GetScale() const;
		void SetScale(const glm::vec3& Scale);

#ifdef USE_ANIMATION
		void ChangeMotion(int Index); // インデックス指定でモーションを変更
		void ChangeMotion(const std::string& MotionName); // 名前指定でモーションを変更
		void PlayBlendShape(const std::string& MotionName);
		void StopBlendShape(const std::string& MotionName);
#endif // USE_ANIMATION
		
		const std::shared_ptr<graphics::CTextureSet>& GetTextureSet() const;
	};
}