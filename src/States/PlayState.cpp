// PlayState.cpp

#include "States/PlayState.h"
#include "App/GameStateMachine.h"
#include "App/NeneEngineApp.h"
#include "Core/CustomLogger.h"
#include "Input/InputDevice.h"
#include "States/PauseState.h"

void NeneEngine::PlayState::OnEnter()
{
	NENE_LOG_INFO("PlayState entered");
}

void NeneEngine::PlayState::OnPause()
{
	NENE_LOG_INFO("PlayState paused");
}

void NeneEngine::PlayState::OnResume()
{
	NENE_LOG_INFO("PlayState resumed");
}

void NeneEngine::PlayState::OnExit()
{
	NENE_LOG_INFO("PlayState exited");
}

void NeneEngine::PlayState::Update(float dt)
{
	m_context.app.UpdateAppSystems(dt);
	m_context.world.Update(dt);
}

void NeneEngine::PlayState::HandleInput()
{
	InputDevice* input = m_context.app.GetFocusedInput();
	if (input == nullptr) return;

	if (input->IsKeyPressed(KeyCode::Escape))
		m_context.stateMachine.PushState(eastl::make_unique<PauseState>(m_context));
}
