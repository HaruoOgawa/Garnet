#pragma once
#include <vector>
#include <memory>
#include "CPrimitive.h"

namespace graphics
{
	class CPrimitive;

	class CMesh
	{
		std::vector<std::shared_ptr<CPrimitive>> m_PrimitiveList;
	public:
		CMesh();
		virtual ~CMesh();

		void AddPrimitive(const std::shared_ptr<CPrimitive>& Primitive);
		const std::vector<std::shared_ptr<CPrimitive>>& GetPrimitiveList() const;

		void CreateSimpleMesh(const std::shared_ptr<renderer::CRendererCreateInfo>& createInfo, int MaterialIndex);
	};
}