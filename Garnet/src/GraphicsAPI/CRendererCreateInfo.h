#pragma once
#include <vector>

namespace renderer
{
	class CRendererCreateInfo
	{
		// VertexBuffer
		std::vector<std::vector<float>> m_Vertices;
		std::vector<uint16_t> m_Indices;
		std::vector<int> m_AttributeDimensions;
	public:
		CRendererCreateInfo();
		virtual ~CRendererCreateInfo();
		
		// VertexBuffer
		void SetVertices(const std::vector<std::vector<float>>& Vertices);
		const std::vector<std::vector<float>>& GetVertices() const;

		void SetIndices(const std::vector<uint16_t>& Indices);
		const std::vector<uint16_t>& GetIndices() const;

		void SetAttributeDimensions(const std::vector<int>& AttributeDimensions);
		const std::vector<int>& GetAttributeDimensions() const;
	};
}