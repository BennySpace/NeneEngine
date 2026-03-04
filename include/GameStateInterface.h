#pragma once

#include <cstdint>
#include <string>

namespace NeneEngine
{

	class Application;

	class GameStateInterface
	{
	public:
		virtual ~IGameState() = default;

		virtual void OnEnter() = 0;

		virtual void OnExit() = 0;

		virtual void Update(float deltaTime) = 0;

		virtual void HandleInput() = 0;
	}

} // namespace NeneEngine