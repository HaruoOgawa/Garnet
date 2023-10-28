#pragma once
#include <vector>

namespace renderer
{
	enum class EIndiceType
	{
		UNSIGNED_SHORT,
		UNSIGNED_INT,
	};

	enum class EDataType
	{
		TYPE_SIGNED_BYTE = 5120,
		TYPE_UNSIGNED_BYTE = 5121,
		TYPE_SIGNED_SHORT = 5122,
		TYPE_UNSIGNED_SHORT = 5123,
		TYPE_UNSIGNED_INT = 5125,
		TYPE_FLOAT = 5126,
	};

	class CRendererCreateInfo
	{
		// VertexBuffer
		std::vector<std::vector<float>> m_Vertices;
		
		EIndiceType m_IndiceType;
		std::vector<unsigned short> m_Indices;
		std::vector<unsigned int> m_UINTIndices;

		std::vector<int> m_AttributeDimensions;
		std::vector<EDataType> m_AttribDataTypes;
		std::vector<int> m_AttribByteStrides;

		int  m_InstanceCount;
	public:
		CRendererCreateInfo();
		virtual ~CRendererCreateInfo();
		
		// VertexBuffer
		void SetVertices(const std::vector<std::vector<float>>& Vertices);
		const std::vector<std::vector<float>>& GetVertices() const;

		void SetIndices(const std::vector<unsigned short>& Indices);
		const std::vector<unsigned short>& GetIndices() const;
		
		void SetUINTIndices(const std::vector<unsigned int>& Indices);
		const std::vector<unsigned int>& GetUINTIndices() const;

		EIndiceType GetIndiceType() const;

		void SetAttributeDimensions(const std::vector<int>& AttributeDimensions);
		const std::vector<int>& GetAttributeDimensions() const;

		void SetAttribDataTypes(const std::vector<EDataType>& AttribDataTypes);
		const std::vector<EDataType>& GetAttribDataTypes() const;

		void SetAttribByteStrides(const std::vector<int>& AttribByteStrides);
		const std::vector<int>& GetAttribByteStrides();

#ifdef USE_GPGPU
		void SetInstanceDrawCount(int InstanceCount);
#endif // USE_GPGPU
		int  GetInstanceCount() const;
	};
}