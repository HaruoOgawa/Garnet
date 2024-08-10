#pragma once
#include <vector>
#include <memory>
#include <vector>
#include <map>
#include "CPrimitive.h"
#include <glm/glm.hpp>

namespace graphics
{
	class CMaterial;
	class CPrimitive;
	class CVertexBuffer;
	class CIndexBuffer;

	class CMesh
	{
		std::vector<std::shared_ptr<CPrimitive>> m_PrimitiveList;

		std::vector<std::shared_ptr<CVertexBuffer>> m_VertexBufferList;
		std::vector<std::shared_ptr<CIndexBuffer>> m_IndexBufferList;

		// モーフデータ
		std::map<int, std::vector<std::map<int, glm::vec3>>> m_MorphDataList;
	public:
		CMesh();
		virtual ~CMesh();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, std::string PassName, std::string DepthPassName);

		bool CreateBuffer();

		void AddVertexBuffer(const std::shared_ptr<CVertexBuffer>& Buffer);
		void AddIndexBuffer(const std::shared_ptr<CIndexBuffer>& Buffer);

		void AddPrimitive(const std::shared_ptr<CPrimitive>& Primitive);
		const std::vector<std::shared_ptr<CPrimitive>>& GetPrimitiveList() const;

		void CreatePresetSimpleMesh(const std::shared_ptr<CVertexBuffer>& VertexBuffer, const std::shared_ptr<CIndexBuffer>& IndexBuffer, int MaterialIndex, graphics::EPresetPrimitiveType PresetType);

		void SetMorphDataList(const std::shared_ptr<CPrimitive>& Primitive, int PrimitiveIndex, const std::vector<std::map<int, glm::vec3>>& MorphDataList);
		const std::map<int, std::vector<std::map<int, glm::vec3>>>& GetMorphDataList() const;
	};
}