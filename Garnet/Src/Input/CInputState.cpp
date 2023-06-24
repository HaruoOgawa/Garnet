#ifdef USE_INPUT_SYSTEM
#include "CInputState.h"

namespace input
{
	CInputState::CInputState(float MouseRotSpeed) :
		m_MousePos(glm::vec2(0.0f)),
		m_PrevMousePos(glm::vec2(0.0f)),
		m_OnDownMouseLeft(false),
		m_MouseRotSpeed(MouseRotSpeed)
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

	float CInputState::GetMouseRotSpeed() const
	{
		return m_MouseRotSpeed;
	}
}
#endif // #ifdef USE_INPUT_SYSTEM