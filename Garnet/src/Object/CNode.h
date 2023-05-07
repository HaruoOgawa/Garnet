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
	};
}