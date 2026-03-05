#pragma once

#include <cstdint>
#include <string>

namespace NeneEngine
{

	class Application;

	class GameStateInterface
	{
	public:
		virtual ~GameStateInterface() = default;

		virtual void OnEnter() = 0;

		virtual void OnExit() = 0;

		virtual void Update(float deltaTime) = 0;

		virtual void HandleInput() = 0;

		virtual bool IsTransparent() const { return false; }
		virtual bool IsPausing() const { return false; }
	};

} // namespace NeneEngine