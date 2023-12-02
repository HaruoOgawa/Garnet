#include "C3DObjectImporter.h"

namespace object
{
	bool C3DObjectImporter::ImportSimply(api::IGraphicsAPI* pGraphicsAPI, std::shared_ptr<object::C3DObject>& Object,
		const std::shared_ptr<renderer::CRendererCreateInfo>& createInfo,
		const std::shared_ptr<graphics::CMaterial>& Material, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
		const std::shared_ptr<resource::CFile>& DepthVertex, const std::shared_ptr<resource::CFile>& DepthFragment)
	{
		// Material
		Object->AddMaterial(Material);

		// Mesh
		std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();
		std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(createInfo, 0);
		Mesh->AddPrimitive(Primitive);

		Object->AddMesh(Mesh);

		// Node
		std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, Object->GetMeshList(), Object->GetMaterialList());
		Object->AddNode(Node);

		// Create
		if (!Object->Create(pGraphicsAPI, DepthVertex, DepthFragment, TextureSet)) return false;

		return true;
	}
}