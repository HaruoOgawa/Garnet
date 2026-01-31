#pragma once

namespace graphics
{
	class CDrawObj
	{
		int m_RenderQueue;
		float m_ToCameraDist;

	public:
		CDrawObj(int RenderQueue, float ToCameraDist);
		virtual ~CDrawObj();

		virtual int GetRenderQueue() const;

		virtual float GetToCameraDist() const;

		virtual bool Draw() = 0;
	};
}
