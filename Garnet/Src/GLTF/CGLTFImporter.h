#pragma once
#ifdef USE_GLTF

#include <vector>
#include <memory>
#include <string>
#include <map>

#include "../Interface/IGraphicsAPI.h"
#include "../File/CFile.h"

namespace graphics
{
	class CTexture;
	class CTextureSet;
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
		static bool Import(api::IGraphicsAPI* pGraphicsAPI, tinygltf::Model model, std::shared_ptr<object::C3DObject>& Object,
			std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
			const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment);

		static bool CreateTexture(api::IGraphicsAPI* pGraphicsAPI, const tinygltf::Model& model, std::vector<std::shared_ptr<graphics::CTexture>>& TextureList);

		static bool CreateMaterial(api::IGraphicsAPI* pGraphicsAPI, const tinygltf::Model& model, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, 
			std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet);

		static bool CreateDummyMaterial(api::IGraphicsAPI* pGraphicsAPI, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
			std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList);

		static bool CreateMesh(const tinygltf::Model& model, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList);

		static bool CreateNode(const tinygltf::Model& model, std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, 
			const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, std::vector<std::vector<int>>& RootNodeIndexList);

		static bool CreateAnimation(const tinygltf::Model& model);

		// Helper Function
		static bool RecalculateTangent(std::vector<float>& TangentDat, const std::vector<float>& PosotionData, const std::vector<float>& TexcoordData, const std::vector<unsigned short>& Indices);
		static bool RecalculateTangentWithUINT(std::vector<float>& TangentData, const std::vector<float>& PosotionData, const std::vector<float>& TexcoordData, const std::vector<unsigned int>& Indices);
	public:
		static bool ImportFromMemory(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::shared_ptr<object::C3DObject>& Object,
			std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
			const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment);
		
		static bool ImportFromString(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, const std::string& BaseDir, std::shared_ptr<object::C3DObject>& Object,
			std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
			const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment);
	};
}
#endif // USE_GLTF