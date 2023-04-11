#pragma once
#include <vector>
#include <memory>

namespace graphics
{
	class CPrimitive;

	class CMesh
	{
		std::vector<std::shared_ptr<CPrimitive>> m_PrimitiveList;
	public:
		CMesh();
		virtual ~CMesh();

		bool Release();

		bool Update(float SecondsTime);
		bool Draw();

		void AddPrimitive(const std::shared_ptr<CPrimitive>& Primitive);
	};
}