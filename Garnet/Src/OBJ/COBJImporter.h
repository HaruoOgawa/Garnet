#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <unordered_map>
#include <tuple>
#include <glm/glm.hpp>

namespace api { class IGraphicsAPI; }
namespace object { class C3DObject; }
namespace graphics { class CMaterialFrame; }
namespace resource { class C3DObjectLoader; }

namespace obj
{ 
	enum class EOBJAttributeType
	{
		None = -1,

		Position = 0,
		Texcoord = 1,
		Normal = 2,

		Max = 3,
	};

	class COBJImporter
	{
	private:
		static void Analyze(const std::vector<std::string>& ParamList, const std::string& CurrentLine,
			const std::string& BaseDir, const std::shared_ptr<object::C3DObject>& Object,
			resource::C3DObjectLoader* p3DObjectLoader, std::string& CurrentMeshName,
			std::vector<float>& Positions, std::vector<float>& Texcoords, std::vector<float>& Normals,
			std::map<std::string, std::vector<std::string>>& VertexDataIndexes);

		static bool Build(const std::shared_ptr<object::C3DObject>& Object,
			const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList,
			const std::vector<float>& SrcPositions, const std::vector<float>& SrcTexcoords, const std::vector<float>& SrcNormals,
			const std::map<std::string, std::vector<std::string>>& SrcVertexDataIndexes);

		static std::tuple<int, int, int> GetDataStructure(const std::string& Src, bool ExistPosition, bool ExistTexcoord, bool ExistNormal);

		static void PushVertexData(int Index, int Dimention, const std::vector<float>& SrcData, std::vector<float>& DstVertexData);

		static glm::vec3 RecalcNormalFromLastPosition(const std::vector<float>& SrcPositions, int NumOfData,
			int PosIndex_0, int PosIndex_1, int PosIndex_2);
		static void AddVertexNormal(int NumOfData, int Index, int Dimention, const glm::vec3& Normal, std::vector<float>& ResultNormals);
		static void NormalizeNormals(std::vector<float>& ResultNormals);
	public:
		static bool Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, 
			const std::string& BaseDir, const std::shared_ptr<object::C3DObject>& Object,
			const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList, 
			resource::C3DObjectLoader* p3DObjectLoader);
	};
}