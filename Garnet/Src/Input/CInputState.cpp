#include "CInputState.h"

namespace input
{
	CInputState::CInputState():
		m_MousePos(glm::vec2(0.0f)),
		m_PrevMousePos(glm::vec2(0.0f)),
		m_OnDownMouseLeft(false)
	{
	}

	CInputState::~CInputState()
	{
		Clear();
	}

	void CInputState::Clear()
	{
		m_MousePos = glm::vec2(0.0f);
		m_PrevMousePos = glm::vec2(0.0f);
		m_OnDownMouseLeft = false;
	}

	void CInputState::SetMouseState(const glm::vec2& MousePos, bool OnDownMouseLeft)
	{
		//
		m_PrevMousePos = m_MousePos;

		//
		m_MousePos = MousePos;
		m_OnDownMouseLeft = OnDownMouseLeft;
	}

	glm::vec2 CInputState::GetDragAmount()
	{
		glm::vec2 result = m_MousePos - m_PrevMousePos;

		return result;
	}
}