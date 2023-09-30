#ifdef USE_INPUT_SYSTEM
#include "CInputState.h"

namespace input
{
	CInputState::CInputState(float MouseRotSpeed) :
		m_MousePos(glm::vec2(0.0f)),
		m_PrevMousePos(glm::vec2(0.0f)),
		m_WheelScrollAmount(glm::vec2(0.0f)),
		m_IsLocked(false),
		m_OnDownMouseLeft(false),
		m_OnDownMouseRight(false),
		m_OnKeyDown(false),
		m_KeyType(EKeyType::KEY_TYPE_NONE)
	{
	}

	CInputState::~CInputState()
	{
		Clear();
	}

	void CInputState::Clear()
	{
		if (!m_OnDownMouseLeft && !m_OnDownMouseRight && !m_OnKeyDown)
		{
			m_MousePos = glm::vec2(0.0f);
			m_PrevMousePos = glm::vec2(0.0f);
		}

		// マウスホイール量は毎回リセットする
		m_WheelScrollAmount = glm::vec2(0.0f);
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

	void CInputState::SetWheelScrollAmount(const glm::vec2& ScrollAmount)
	{
		m_WheelScrollAmount = ScrollAmount;
	}

	const glm::vec2& CInputState::GetWheelScrollAmount() const
	{
		return m_WheelScrollAmount;
	}

	bool CInputState::IsMouseWheeled()
	{
		return (glm::abs(m_WheelScrollAmount.y) > 0.0f);
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

	void CInputState::SetKeyDown(bool KeyDown)
	{
		m_OnKeyDown = KeyDown;
	}

	bool CInputState::IsKeyDown()const
	{
		return m_OnKeyDown;
	}

	void CInputState::SetKeyType(EKeyType KeyType)
	{
		m_KeyType = KeyType;
	}

	EKeyType CInputState::GetKeyType()const
	{
		return m_KeyType;
	}
}
#endif // #ifdef USE_INPUT_SYSTEM