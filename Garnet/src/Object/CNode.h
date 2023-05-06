#pragma once

#include <memory>
#include "../Graphics/CMesh.h"

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

		bool Create();
		bool Update(float SecondsTime);
		bool Draw();
	};
}