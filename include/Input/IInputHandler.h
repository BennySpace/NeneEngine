// IInputHandler.h

#pragma once

#include "Input/KeyCode.h"

#include <glm/glm.hpp>
#include <string_view>

namespace NeneEngine
{

	class IInputHandler
	{
	  public:
		virtual ~IInputHandler() = default;

		virtual bool IsKeyDown(KeyCode key) const = 0;
		virtual bool IsKeyPressed(KeyCode key) const = 0;
		virtual bool IsMouseButtonDown(int buttonIndex) const = 0;
		virtual bool IsMouseButtonPressed(int buttonIndex) const = 0;
		virtual glm::vec2 GetMousePosition() const = 0;
		virtual glm::vec2 GetMouseDelta() const = 0;
		virtual bool IsActionActive(std::string_view actionName) const = 0;
		virtual bool IsActionPressed(std::string_view actionName) const = 0;
	};

} // namespace NeneEngine
