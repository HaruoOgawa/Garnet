#include "CInputState.h"

namespace input
{
	CInputState::CInputState() :
		m_MousePos(glm::vec2(0.0f)),
		m_PrevMousePos(glm::vec2(0.0f)),
		m_WheelScrollAmount(glm::vec2(0.0f)),
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
		/*if (!m_OnDownMouseLeft && !m_OnDownMouseRight)
		{
			m_MousePos = glm::vec2(0.0f);
			m_PrevMousePos = glm::vec2(0.0f);
		}*/

		// マウスホイール量は毎回リセットする
		m_WheelScrollAmount = glm::vec2(0.0f);

		// Up状態のキーを全て削除する
		m_UpKeyMap.clear();
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

	const glm::vec2& CInputState::GetMousePos() const
	{
		return m_MousePos;
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

	void CInputState::SetKeyState(EKeyType KeyType, bool KeyDown)
	{
		auto key = m_KeyInputMap.find(KeyType);

		if (key == m_KeyInputMap.end())
		{
			// 新規追加
			m_KeyInputMap.insert({ KeyType , KeyDown });
		}
		else
		{
			// UpならUpKeyListに入れてフレームの最後に削除する
			if(!KeyDown)
			{
				m_KeyInputMap.erase(key);

				m_UpKeyMap.insert({ KeyType , KeyDown });
			}
		}
	}

	const std::map<EKeyType, bool>& CInputState::GetKeyInputMap() const
	{
		return m_KeyInputMap;
	}

	bool CInputState::IsKeyDown(EKeyType KeyType)
	{
		const auto& Key = m_KeyInputMap.find(KeyType);
		
		if (Key == m_KeyInputMap.end()) return false;

		return Key->second;
	}
	
	bool CInputState::IsKeyUp(EKeyType KeyType)
	{
		return (m_UpKeyMap.find(KeyType) != m_UpKeyMap.end());
	}
}