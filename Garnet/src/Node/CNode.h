#pragma once

namespace node
{
	class CNode
	{
	public:
		CNode();
		virtual ~CNode();

		bool Create();
		bool Update();
		bool Draw();
	};
}