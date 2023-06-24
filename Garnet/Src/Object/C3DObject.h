#pragma once

#include "CObject.h"
#include "CNode.h"
#include "../Graphics/CMaterial.h"
#include "../Graphics/CTexture.h"
#include "../Math/CTransform.h"
#include <vector>
#include <memory>

namespace object
{
	class CNode;

	class C3DObject : public CObject
	{
		std::shared_ptr<math::CTransform> m_ObjectTransform;

		std::vector<std::shared_ptr<CNode>> m_NodeList;
		std::vector<std::shared_ptr<graphics::CMesh>> m_MeshList;
		std::vector<std::shared_ptr<graphics::CMaterial>> m_MaterialList;
		std::vector<std::shared_ptr<graphics::CTexture>> m_TextureList;
		std::vector<std::shared_ptr<graphics::CTexture>> m_CubeMapList;

		std::vector<std::vector<int>> m_RootNodeIndexList;
	private:
		void CalcWorldMatrix();
		void CalcWorldMatrix(std::shared_ptr<CNode>& Node, const glm::mat4& ParentWorldMatrix);
	public:
		C3DObject();
		virtual ~C3DObject();

		bool		 Create(api::IGraphicsAPI* pGraphicsAPI);
		virtual bool Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) override;
		virtual bool Draw() override;

		void AddNode(const std::shared_ptr<CNode>& Node);
		const std::vector<std::shared_ptr<CNode>>& GetNodeList() const;
		
		void AddMesh(const std::shared_ptr<graphics::CMesh>& Mesh);
		const std::vector<std::shared_ptr<graphics::CMesh>>& GetMeshList() const;

		void AddMaterial(const std::shared_ptr<graphics::CMaterial>& Material);
		const std::vector<std::shared_ptr<graphics::CMaterial>>& GetMaterialList() const;
		
		void AddTexture(const std::shared_ptr<graphics::CTexture>& Texture);
		const std::vector<std::shared_ptr<graphics::CTexture>>& GetTextureList() const;
		
		void AddCubeMap(const std::shared_ptr<graphics::CTexture>& CubeMap);
		const std::vector<std::shared_ptr<graphics::CTexture>>& GetCubeMapList() const;

		void SetRootNodeIndexList(const std::vector<std::vector<int>>& RootNodeIndexList);
		const std::vector<std::vector<int>>& GetRootNodeIndexList() const;

		const glm::vec3& GetPos() const;
		void SetPos(const glm::vec3& Pos);

		const glm::vec3& GetRot() const;
		void SetRot(const glm::vec3& Rot);

		const glm::vec3& GetScale() const;
		void SetScale(const glm::vec3& Scale);
	};
}