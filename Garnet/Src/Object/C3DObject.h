#pragma once

#include "CObject.h"
#include "CNode.h"
#include "../Graphics/CMaterial.h"
#include "../Graphics/CTexture.h"
#include <vector>
#include <memory>

namespace object
{
	class CNode;

	class C3DObject : public CObject
	{
		std::vector<std::shared_ptr<CNode>> m_NodeList;
		std::vector<std::shared_ptr<graphics::CMaterial>> m_MaterialList;
		std::vector<std::shared_ptr<graphics::CTexture>> m_TextureList;
	public:
		C3DObject();
		virtual ~C3DObject();

		bool		 Create(api::IGraphicsAPI* pGraphicsAPI);
		virtual bool Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) override;
		virtual bool Draw() override;

		void AddNode(const std::shared_ptr<CNode>& Node);
		const std::vector<std::shared_ptr<CNode>>& GetNodeList() const;

		void AddMaterial(const std::shared_ptr<graphics::CMaterial>& Material);
		const std::vector<std::shared_ptr<graphics::CMaterial>>& GetMaterialList() const;
		
		void AddTexture(const std::shared_ptr<graphics::CTexture>& Texture);
		const std::vector<std::shared_ptr<graphics::CTexture>>& GetTextureList() const;
	};
}