#pragma once
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <glm/glm.hpp>
#include "CPresetPrimitive.h"

namespace graphics {
	class IRenderer;
}

namespace api { class IGraphicsAPI; }

namespace graphics
{
	class CMaterial;
	class CVertexBuffer;
	class CIndexBuffer;

	class CPrimitive
	{
		std::shared_ptr<CVertexBuffer> m_VertexBuffer;
		std::shared_ptr<CIndexBuffer> m_IndexBuffer;

		std::shared_ptr<graphics::IRenderer>				 m_Renderer;
		std::shared_ptr<graphics::IRenderer>				 m_DepthRenderer;
		
		int													 m_MaterialIndex;

		// モーフデータ
		std::vector<std::map<int, glm::vec3>> m_MorphDataList;
	public:
		CPrimitive(const std::shared_ptr<CVertexBuffer>& VertexBuffer, const std::shared_ptr<CIndexBuffer>& IndexBuffer, int MaterialIndex);
		virtual ~CPrimitive();

		void Release();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const std::string& PassName, const std::shared_ptr<graphics::CMaterial>& Material, bool IsDepth);

		bool Draw(const std::shared_ptr<CMaterial>& Material, int DynamicOffsetNum, bool IsDepth);

		void SetMaterialIndex(int Index);
		int GetMaterialIndex() const;

		void SetMorphDataList(const std::vector<std::map<int, glm::vec3>>& MorphDataList);
		const std::vector<std::map<int, glm::vec3>>& GetMorphDataList() const;
	};
}