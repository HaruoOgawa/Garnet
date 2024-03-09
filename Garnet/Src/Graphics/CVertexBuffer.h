#pragma once
#include <vector>

namespace graphics
{
	enum class EDataType
	{
		TYPE_SIGNED_BYTE = 5120,
		TYPE_UNSIGNED_BYTE = 5121,
		TYPE_SIGNED_SHORT = 5122,
		TYPE_UNSIGNED_SHORT = 5123,
		TYPE_UNSIGNED_INT = 5125,
		TYPE_FLOAT = 5126,
	};

	class CVertexBuffer
	{
	protected:
		std::vector<std::vector<float>> m_Vertices;

		std::vector<int> m_AttributeDimensions;
		std::vector<EDataType> m_AttribDataTypes;
		std::vector<int> m_AttribByteStrides;

		int  m_InstanceCount;

	public:
		CVertexBuffer();
		virtual ~CVertexBuffer();

		virtual bool Create() = 0;

		virtual void SetVertices(const std::vector<std::vector<float>>& Vertices);
		virtual const std::vector<std::vector<float>>& GetVertices() const;

		virtual void SetAttributeDimensions(const std::vector<int>& AttributeDimensions);
		virtual const std::vector<int>& GetAttributeDimensions() const;

		virtual void SetAttribDataTypes(const std::vector<EDataType>& AttribDataTypes);
		virtual const std::vector<EDataType>& GetAttribDataTypes() const;

		virtual void SetAttribByteStrides(const std::vector<int>& AttribByteStrides);
		virtual const std::vector<int>& GetAttribByteStrides() const;

#ifdef USE_GPGPU
		virtual void SetInstanceDrawCount(int InstanceCount);
#endif // USE_GPGPU
		virtual int  GetInstanceCount() const;
	};
}