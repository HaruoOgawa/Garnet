#ifdef USE_INPUT_SYSTEM
#include "CInputState.h"

namespace input
{
	CInputState::CInputState(float MouseRotSpeed) :
		m_MousePos(glm::vec2(0.0f)),
		m_PrevMousePos(glm::vec2(0.0f)),
		m_IsLocked(false),
		m_OnDownMouseLeft(false),
		m_OnDownMouseRight(false)
	{
	}

	CInputState::~CInputState()
	{
		Clear();
	}

	void CInputState::Clear()
	{
		if (!m_OnDownMouseLeft && !m_OnDownMouseRight)
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

	void CInputState::SetLock(bool State)
	{
		m_IsLocked = State;
	}

	bool CInputState::IsLocked()const
	{
		return m_IsLocked;
	}

	void CInputState::SetDownMouseLeft(bool OnDownMouseLeft)
	{
		m_OnDownMouseLeft = OnDownMouseLeft;
	}

	bool CInputState::IsDownMouseLeft()const 
	{
		return m_OnDownMouseLeft; 
	}

	void CInputState::SetDownMouseRight(bool OnDownMouseRight)
	{
		m_OnDownMouseRight = OnDownMouseRight;
	}

	bool CInputState::IsDownMouseRight()const
	{
		return m_OnDownMouseRight;
	}

	glm::vec2 CInputState::GetDragAmount()
	{
		glm::vec2 result = m_MousePos - m_PrevMousePos;

		return result;
	}
}
#endif // #ifdef USE_INPUT_SYSTEM