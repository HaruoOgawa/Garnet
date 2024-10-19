#pragma once

#ifdef USE_MMD
#include <vector>
#include <memory>
#include <map>

#include "../../Interface/IGraphicsAPI.h"
#include "../../Interface/IResource.h"
#include "../../Interface/IPhysicsEngine.h"
#include "CPmxModel.h"
#include "../../Math/CMath.h"

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

namespace mmd
{
	class CPmxImporter
	{
	private:
		static bool CreateAnimationSkeleton(const CPmxModel& model, std::shared_ptr<animation::CSkeleton>& Skeleton, std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::shared_ptr<object::CNode>& RootNode);
		static bool CalcInverseBindPose(std::shared_ptr<animation::CSkeleton>& Skeleton);

		static bool CreateMaterialList(api::IGraphicsAPI* pGraphicsAPI, const CPmxModel& model, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, 
			std::map<int, std::vector<std::shared_ptr<graphics::CMaterial>>>& SharedToonUsingList,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, const std::shared_ptr<animation::CSkeleton>& Skeleton);

		static bool CreateMeshList(api::IGraphicsAPI* pGraphicsAPI, object::C3DObject* Object, const CPmxModel& model, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, 
			const std::shared_ptr<object::CNode>& RootNode, std::vector<std::shared_ptr<object::CNode>>& NodeList,
			const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, bool ExistSkeleton);

		static bool CreateTextureList(api::IGraphicsAPI* pGraphicsAPI, resource::C3DObjectLoader* p3DObjectLoader, const std::string& ModelFileName, const CPmxModel& model, 
			std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, std::map<int, std::vector<std::shared_ptr<graphics::CMaterial>>>& SharedToonUsingList);

		static bool CreateRigidbody(physics::IPhysicsEngine* pPhysicsEngine, const CPmxModel& model, std::shared_ptr<animation::CSkeleton>& Skeleton, 
			std::vector<std::shared_ptr<physics::IPhysicsObject>>& PhysicsObjectList);
		static bool CreateJoint(physics::IPhysicsEngine* pPhysicsEngine, const CPmxModel& model, std::shared_ptr<animation::CSkeleton>& Skeleton, 
			const std::vector<std::shared_ptr<physics::IPhysicsObject>>& PhysicsObjectList);

		static glm::vec3 CovertToZYX(const glm::vec3& Val);
	public:
		static bool ImportPmx(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, const std::string& ModelFileName, const std::vector<unsigned char>& Data, object::C3DObject* Object,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, resource::C3DObjectLoader* p3DObjectLoader);
	};
}
#endif // USE_MMD