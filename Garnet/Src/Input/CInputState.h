#pragma once

#include <glm/glm.hpp>

namespace input
{
	class CInputState
	{
		glm::vec2 m_MousePos;
		glm::vec2 m_PrevMousePos;

		bool m_OnDownMouseLeft;
	public:
		CInputState();
		virtual ~CInputState();

		void Clear();

		void SetMouseState(const glm::vec2& MousePos, bool OnDownMouseLeft);

		bool IsDownMouseLeft()const { return m_OnDownMouseLeft; }
		glm::vec2 GetDragAmount();
	};
}