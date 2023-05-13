#pragma once

#include <memory>
#include "../Graphics/CMesh.h"
#include "../Math/CTransform.h"

namespace graphics{ class CMesh; }
namespace math { class CTransform; }

namespace object
{
	class CNode
	{
		std::shared_ptr<math::CTransform> m_Transform;
		std::shared_ptr<graphics::CMesh> m_Mesh;
	public:
		CNode(const std::shared_ptr<graphics::CMesh>& Mesh);
		virtual ~CNode();

		const std::shared_ptr<graphics::CMesh>& GetMesh() const;

		void SetTransform(std::shared_ptr<math::CTransform>& Transform);
		const std::shared_ptr<math::CTransform>& GetTransform() const;

		const glm::vec3& GetPos() const;
		void SetPos(const glm::vec3& Pos);

		const glm::vec3& GetRot() const;
		void SetRot(const glm::vec3& Rot);

		const glm::vec3& GetScale() const;
		void SetScale(const glm::vec3& Scale);
	};
}