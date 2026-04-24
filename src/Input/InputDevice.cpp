// InputDevice.cpp

#include "Input/InputDevice.h"

namespace NeneEngine {

	void InputDevice::NotifyKeyDown(KeyCode key)
	{
		const bool wasDown = IsKeyDown(key);
		m_pressedKeys.insert(key);

		KeyDown.Broadcast(KeyEvent{ key, wasDown });
	}

	void InputDevice::NotifyKeyUp(KeyCode key)
	{
		const bool wasDown = IsKeyDown(key);
		m_pressedKeys.erase(key);

		KeyUp.Broadcast(KeyEvent{ key, wasDown });
	}

	void InputDevice::NotifyMouseMove(glm::vec2 position)
	{
		const glm::vec2 delta = position - m_mousePosition;
		NotifyMouseDelta(delta, position);
	}

	void InputDevice::NotifyMouseDelta(glm::vec2 delta, glm::vec2 position)
	{
		m_mouseDelta += delta;
		m_mousePosition = position;

		MouseMoved.Broadcast(MouseMoveEvent{ m_mousePosition, delta });
	}

	void InputDevice::NotifyMouseWheel(float delta)
	{
		m_mouseWheelDelta += delta;

		MouseWheel.Broadcast(MouseWheelEvent{ m_mousePosition, delta });
	}

	void InputDevice::EndFrame()
	{
		m_mouseDelta = { 0.0f, 0.0f };
		m_mouseWheelDelta = 0.0f;
	}

	bool InputDevice::IsKeyDown(KeyCode key) const
	{
		return m_pressedKeys.contains(key);
	}

} // namespace NeneEngine
