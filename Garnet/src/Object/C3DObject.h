#pragma once

#include "CObject.h"
#include "CNode.h"
#include "../Graphics/CMaterial.h"
#include <vector>
#include <memory>

namespace object
{
	class CNode;

	class C3DObject : public CObject
	{
		std::vector<std::shared_ptr<CNode>> m_NodeList;
		std::vector<std::shared_ptr<graphics::CMaterial>> m_MaterialList;
	public:
		C3DObject();
		virtual ~C3DObject();

		virtual bool Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection) override;
		virtual bool Draw() override;

		void AddNode(const std::shared_ptr<CNode>& Node);
		void AddMaterial(const std::shared_ptr<graphics::CMaterial>& Material);
		const std::vector<std::shared_ptr<graphics::CMaterial>>& GetMaterialList() const;
	};
}