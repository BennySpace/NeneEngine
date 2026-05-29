// InputManager.h

#pragma once

#include "Input/IInputHandler.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace NeneEngine
{
	class InputDevice;

	class InputManager final : public IInputHandler
	{
	  public:
		InputManager() = default;
		explicit InputManager(const InputDevice* inputDevice);

		void SetInputDevice(const InputDevice* inputDevice);
		const InputDevice* GetInputDevice() const { return m_inputDevice; }
		bool HasInputDevice() const { return m_inputDevice != nullptr; }

		void UpdateState();

		bool IsKeyDown(KeyCode key) const override;
		bool IsKeyPressed(KeyCode key) const override;
		bool IsMouseButtonDown(int buttonIndex) const override;
		bool IsMouseButtonPressed(int buttonIndex) const override;
		glm::vec2 GetMousePosition() const override { return m_mousePosition; }
		glm::vec2 GetMouseDelta() const override { return m_mouseDelta; }

		void BindAction(const std::string& actionName, KeyCode keyCode);
		void SetActionBindings(const std::string& actionName, std::vector<KeyCode> keyCodes);
		void ClearActionBindings();
		bool IsActionActive(const std::string& actionName) const;
		bool IsActionPressed(const std::string& actionName) const;

	  private:
		void ResetState();
		static KeyCode MouseButtonIndexToKeyCode(int buttonIndex);

		const InputDevice* m_inputDevice = nullptr;
		std::unordered_map<KeyCode, bool> m_keyDownStates;
		std::unordered_map<KeyCode, bool> m_keyPressedStates;
		std::unordered_map<int, bool> m_mouseButtonDownStates;
		std::unordered_map<int, bool> m_mouseButtonPressedStates;
		glm::vec2 m_mousePosition = {0.0f, 0.0f};
		glm::vec2 m_mouseDelta = {0.0f, 0.0f};
		std::unordered_map<std::string, std::vector<KeyCode>> m_actionsMap;
	};

} // namespace NeneEngine
