// PauseState.cpp

#include "States/PauseState.h"
#include "App/GameStateMachine.h"
#include "App/NeneEngineApp.h"
#include "Core/CustomLogger.h"
#include "Input/InputDevice.h"

void NeneEngine::PauseState::OnEnter()
{
	NENE_LOG_INFO("PauseState entered");
}

void NeneEngine::PauseState::OnPause()
{
	NENE_LOG_INFO("PauseState paused");
}

void NeneEngine::PauseState::OnResume()
{
	NENE_LOG_INFO("PauseState resumed");
}

void NeneEngine::PauseState::OnExit()
{
	NENE_LOG_INFO("PauseState exited");
}

void NeneEngine::PauseState::Update(float /*dt*/) {}

void NeneEngine::PauseState::HandleInput()
{
	InputDevice* input = m_context.app.GetFocusedInput();
	if (input == nullptr) return;

	if (input->IsKeyPressed(KeyCode::Escape)) m_context.stateMachine.PopState();

	if (input->IsKeyPressed(KeyCode::Q)) m_context.app.RequestShutdown();
}
