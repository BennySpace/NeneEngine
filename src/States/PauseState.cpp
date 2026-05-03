// PauseState.cpp

#include "States/PauseState.h"
#include "App/GameStateMachine.h"
#include "App/NeneEngineApp.h"
#include "Input/InputDevice.h"

#include <spdlog/spdlog.h>

void NeneEngine::PauseState::OnEnter()
{
	spdlog::info("PauseState entered");
}

void NeneEngine::PauseState::OnPause()
{
	spdlog::info("PauseState paused");
}

void NeneEngine::PauseState::OnResume()
{
	spdlog::info("PauseState resumed");
}

void NeneEngine::PauseState::OnExit()
{
	spdlog::info("PauseState exited");
}

void NeneEngine::PauseState::Update(float /*dt*/)
{
}

void NeneEngine::PauseState::HandleInput()
{
	InputDevice* input = m_context.app.GetFocusedInput();
	if (input == nullptr)
		return;

	if (input->IsKeyPressed(KeyCode::Escape))
		m_context.stateMachine.PopState();

	if (input->IsKeyPressed(KeyCode::Q))
		m_context.app.RequestShutdown();
}
