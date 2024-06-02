#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>

namespace input
{
	enum class EKeyType
	{
		KEY_TYPE_NONE,

		KEY_TYPE_CONTROL,

		KEY_TYPE_W,
		KEY_TYPE_A,
		KEY_TYPE_S,
		KEY_TYPE_D,
		
		KEY_TYPE_1,
		KEY_TYPE_2,
		KEY_TYPE_3,
		KEY_TYPE_4,
		KEY_TYPE_5,
	};

	class CInputState
	{
		glm::vec2 m_MousePos;
		glm::vec2 m_PrevMousePos;

		glm::vec2 m_WheelScrollAmount;

		bool m_IsLocked;

		bool m_OnDownMouseLeft;
		bool m_OnDownMouseRight;

		std::map<EKeyType, bool> m_KeyInputMap;
		std::map<EKeyType, bool> m_UpKeyMap;
	public:
		CInputState();
		virtual ~CInputState();

		void Clear();

		void StartMousePos(const glm::vec2& MousePos);
		void SetMousePos(const glm::vec2& MousePos);

		void SetWheelScrollAmount(const glm::vec2& ScrollAmount);
		const glm::vec2& GetWheelScrollAmount() const;
		bool IsMouseWheeled();

		void SetLock(bool State);
		bool IsLocked()const;
		
		void SetDownMouseLeft(bool OnDownMouseLeft);
		bool IsDownMouseLeft()const;
		
		void SetDownMouseRight(bool OnDownMouseRight);
		bool IsDownMouseRight()const;
		glm::vec2 GetDragAmount();

		void SetKeyState(EKeyType KeyType, bool KeyDown);
		const std::map<EKeyType, bool>& GetKeyInputMap() const;

		bool IsKeyDown(EKeyType KeyType);
		bool IsKeyUp(EKeyType KeyType);
	};
}