#pragma once

#ifdef USE_INPUT_SYSTEM
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

		void StartMousePos(const glm::vec2& MousePos);
		void SetMousePos(const glm::vec2& MousePos);
		void SetDownMouseLeft(bool OnDownMouseLeft);

		bool IsDownMouseLeft()const { return m_OnDownMouseLeft; }
		glm::vec2 GetDragAmount();
	};
}
#endif // #ifdef USE_INPUT_SYSTEM