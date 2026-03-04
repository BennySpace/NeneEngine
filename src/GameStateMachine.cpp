#include "GameStateMachine.h"

namespace NeneEngine
{

	GameStateMachine::~GameStateMachine() {
		while (!m_states.empty()) {
			PopState();
		}
	}

	void GameStateMachine::ChangeState(eastl::unique_ptr<IGameState> newState) {
		while (!m_states.empty()) {
			PopState();
		}
		PushState(eastl::move(newState));
	}

	void GameStateMachine::PushState(eastl::unique_ptr<IGameState> newState) {
		if (!m_states.empty()) {
			m_states.back()->OnExit();
		}

		m_states.push_back(eastl::move(newState));

		if (!m_states.empty()) {
			m_states.back()->OnEnter();
			spdlog::info("Pushed state: {}", typeid(*m_states.back()).name());
		}
	}

	void GameStateMachine::PopState() {
		if (m_states.empty()) return;

		m_states.back()->OnExit();
		spdlog::info("Popped state: {}", typeid(*m_states.back()).name());

		m_states.pop_back();

		if (!m_states.empty()) {
			m_states.back()->OnEnter();
		}
	}

	void GameStateMachine::Update(float deltaTime) {
		for (int i = static_cast<int>(m_states.size()) - 1; i >= 0; --i) {
			auto& state = m_states[static_cast<size_t>(i)];

			state->Update(deltaTime);

			if (!state->IsPausing()) {
				break;
			}
		}
	}

	void GameStateMachine::HandleInput() {
		if (!m_states.empty()) {
			m_states.back()->HandleInput();
		}
	}

} // namespace NeneEngine