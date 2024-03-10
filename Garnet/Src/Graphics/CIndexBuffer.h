#pragma once
#include <vector>

namespace graphics
{
	enum class EIndiceType
	{
		UNSIGNED_SHORT,
		UNSIGNED_INT,
	};

	class CIndexBuffer
	{
	protected:
		EIndiceType m_IndiceType;
		std::vector<unsigned short> m_Indices;
		std::vector<unsigned int> m_UINTIndices;

	public:
		CIndexBuffer();
		virtual ~CIndexBuffer();

		virtual bool Create() = 0;

		virtual void Release();

		virtual void SetIndices(const std::vector<unsigned short>& Indices);
		virtual const std::vector<unsigned short>& GetIndices() const;

		virtual void SetUINTIndices(const std::vector<unsigned int>& Indices);
		virtual const std::vector<unsigned int>& GetUINTIndices() const;

		virtual EIndiceType GetIndiceType() const;
	};
}