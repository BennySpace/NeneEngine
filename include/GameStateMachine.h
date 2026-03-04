#pragma once

#include "GameStateInterface.h"

#include <EASTL/vector.h>
#include <EASTL/unique_ptr.h>
#include <memory>
#include <spdlog/spdlog.h>

namespace NeneEngine
{

	class GameStateMachine
	{
	public:
		GameStateMachine() = default;
		~GameStateMachine();

		void ChangeState(eastl::unique_ptr<GameStateInterface> newState);

		void PushState(eastl::unique_ptr<GameStateInterface> newState);

		void PopState();

		void Update(float deltaTime);

		void HandleInput();

		bool IsEmpty() const { return m_states.empty(); }

		GameStateInterface* GetCurrentState() const {
			return m_states.empty() ? nullptr : m_states.back().get();
		}

	private:
		eastl::vector<eastl::unique_ptr<GameStateInterface>> m_states;
	}

} // namespace NeneEngine