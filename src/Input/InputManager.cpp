// InputManager.cpp

#include "Input/InputManager.h"
#include "Input/InputDevice.h"

namespace NeneEngine
{
	namespace
	{
		constexpr int kMouseButtonCount = 5;
	}

	InputManager::InputManager(const InputDevice* inputDevice) : m_inputDevice(inputDevice) {}

	void InputManager::SetInputDevice(const InputDevice* inputDevice)
	{
		if (m_inputDevice == inputDevice) return;

		m_inputDevice = inputDevice;
		ResetState();
	}

	void InputManager::UpdateState()
	{
		ResetState();
		if (m_inputDevice == nullptr) return;

		for (uint16_t value = static_cast<uint16_t>(KeyCode::Backspace);
		     value <= static_cast<uint16_t>(KeyCode::RightAlt); ++value)
		{
			const KeyCode key = static_cast<KeyCode>(value);
			if (m_inputDevice->IsKeyDown(key)) m_keyDownStates[key] = true;
			if (m_inputDevice->IsKeyPressed(key)) m_keyPressedStates[key] = true;
		}

		for (int buttonIndex = 0; buttonIndex < kMouseButtonCount; ++buttonIndex)
		{
			const KeyCode mouseKey = MouseButtonIndexToKeyCode(buttonIndex);
			m_mouseButtonDownStates[buttonIndex] = m_inputDevice->IsKeyDown(mouseKey);
			m_mouseButtonPressedStates[buttonIndex] = m_inputDevice->IsKeyPressed(mouseKey);
		}

		m_mousePosition = m_inputDevice->GetMousePosition();
	}

	bool InputManager::IsKeyDown(KeyCode key) const
	{
		const auto it = m_keyDownStates.find(key);
		return it != m_keyDownStates.end() && it->second;
	}

	bool InputManager::IsKeyPressed(KeyCode key) const
	{
		const auto it = m_keyPressedStates.find(key);
		return it != m_keyPressedStates.end() && it->second;
	}

	bool InputManager::IsMouseButtonDown(int buttonIndex) const
	{
		const auto it = m_mouseButtonDownStates.find(buttonIndex);
		return it != m_mouseButtonDownStates.end() && it->second;
	}

	bool InputManager::IsMouseButtonPressed(int buttonIndex) const
	{
		const auto it = m_mouseButtonPressedStates.find(buttonIndex);
		return it != m_mouseButtonPressedStates.end() && it->second;
	}

	void InputManager::BindAction(const std::string& actionName, KeyCode keyCode)
	{
		m_actionsMap[actionName] = keyCode;
	}

	bool InputManager::IsActionActive(const std::string& actionName) const
	{
		const auto it = m_actionsMap.find(actionName);
		if (it == m_actionsMap.end()) return false;

		return IsKeyDown(it->second);
	}

	bool InputManager::IsActionPressed(const std::string& actionName) const
	{
		const auto it = m_actionsMap.find(actionName);
		if (it == m_actionsMap.end()) return false;

		return IsKeyPressed(it->second);
	}

	void InputManager::ResetState()
	{
		m_keyDownStates.clear();
		m_keyPressedStates.clear();
		m_mouseButtonDownStates.clear();
		m_mouseButtonPressedStates.clear();
		m_mousePosition = {0.0f, 0.0f};
	}

	KeyCode InputManager::MouseButtonIndexToKeyCode(int buttonIndex)
	{
		switch (buttonIndex)
		{
		case 0:
			return KeyCode::MouseLeft;
		case 1:
			return KeyCode::MouseRight;
		case 2:
			return KeyCode::MouseMiddle;
		case 3:
			return KeyCode::MouseX1;
		case 4:
			return KeyCode::MouseX2;
		default:
			return KeyCode::None;
		}
	}

} // namespace NeneEngine
