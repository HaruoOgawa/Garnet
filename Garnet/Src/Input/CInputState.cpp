#ifdef USE_INPUT_SYSTEM
#include "CInputState.h"

namespace input
{
	CInputState::CInputState() :
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
		if (!m_OnDownMouseLeft)
		{
			m_MousePos = glm::vec2(0.0f);
			m_PrevMousePos = glm::vec2(0.0f);
		}
	}

	void CInputState::StartMousePos(const glm::vec2& MousePos)
	{
		m_PrevMousePos = MousePos;
		m_MousePos = MousePos;
	}
	
	void CInputState::SetMousePos(const glm::vec2& MousePos)
	{
		m_PrevMousePos = m_MousePos;
		m_MousePos = MousePos;
	}

	void CInputState::SetDownMouseLeft(bool OnDownMouseLeft)
	{
		m_OnDownMouseLeft = OnDownMouseLeft;
	}

	glm::vec2 CInputState::GetDragAmount()
	{
		glm::vec2 result = m_MousePos - m_PrevMousePos;

		return result;
	}
}
#endif // #ifdef USE_INPUT_SYSTEM