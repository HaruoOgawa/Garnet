#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>

namespace api { class IGraphicsAPI; }
namespace object { class C3DObject; }
namespace graphics { 
	class CMaterialFrame; 
	class CMaterial;
	class CTexture;
}
namespace resource { class C3DObjectLoader; }

namespace obj
{
	class CMTLImporter
	{
		static void Analyze(api::IGraphicsAPI* pGraphicsAPI,
			const std::vector<std::string>& ParamList, const std::string& CurrentLine, const std::string& BaseDir, 
			const std::shared_ptr<object::C3DObject>& Object, resource::C3DObjectLoader* p3DObjectLoader, std::string& CurrentMeshName,
			std::map<std::string, std::vector<std::shared_ptr<graphics::CMaterial>>>& MaterialMap,
			std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, std::map<std::string, int>& TextureIndexMap);

		static void EraseSpaceFromParamList(std::vector<std::string>& ParamList);

		static bool IsSkipChara(char c);

		static void SetMaterialUniform_Float(const std::string& CurrentMeshName, const std::string& UniformName, const std::vector<float> Values,
			std::map<std::string, std::vector<std::shared_ptr<graphics::CMaterial>>>& MaterialMap);
			
		static void SetMaterialUniform_Int(const std::string& CurrentMeshName, const std::string& UniformName, const std::vector<int> Values,
			std::map<std::string, std::vector<std::shared_ptr<graphics::CMaterial>>>& MaterialMap);

		static void SetTextureUniform(api::IGraphicsAPI* pGraphicsAPI, const std::string& BaseDir,
			resource::C3DObjectLoader* p3DObjectLoader,
			const std::string& CurrentMeshName, const std::string& UniformName, const std::string& TextureName,
			std::map<std::string, std::vector<std::shared_ptr<graphics::CMaterial>>>& MaterialMap,
			std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, std::map<std::string, int>& TextureIndexMap);
	public:
		static bool Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data,
			const std::string& BaseDir, const std::shared_ptr<object::C3DObject>& Object,
			const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList,
			resource::C3DObjectLoader* p3DObjectLoader);
	};
}