#pragma once
#ifdef USE_GLTF

#include <vector>
#include <memory>
#include <string>
#include <map>

#include "../Interface/IGraphicsAPI.h"

namespace graphics
{
	class CTexture;
	class CMaterial;
	class CMesh;
	class CMaterialCreateInfo;
}

namespace object { 
	class C3DObject; 
	class CNode;
}

namespace tinygltf { class Model; }

namespace gltf
{
	class CGLTFImporter
	{
	private:
		static bool CreateTexture(api::IGraphicsAPI* pGraphicsAPI, const tinygltf::Model& model, std::vector<std::shared_ptr<graphics::CTexture>>& TextureList);
		static bool CreateMaterial(api::IGraphicsAPI* pGraphicsAPI, const tinygltf::Model& model, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, 
			const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);
		static bool CreateMesh(const tinygltf::Model& model, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList);
		static bool CreateNode(const tinygltf::Model& model, std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, 
			const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, std::vector<std::vector<int>>& RootNodeIndexList);

		// Helper Function
		static bool RecalculateTangent(std::vector<float>& TangentData, std::vector<float>& BioTangentData, const std::vector<float>& PosotionData, const std::vector<float>& TexcoordData, const std::vector<unsigned short>& Indices);
	public:
		static bool Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::shared_ptr<object::C3DObject>& Object, 
			std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::vector<std::shared_ptr<graphics::CTexture>>& CubeTexList);
	};
}
#endif // USE_GLTF