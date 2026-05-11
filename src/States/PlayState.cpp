// PlayState.cpp

#include "States/PlayState.h"
#include "App/GameStateMachine.h"
#include "App/NeneEngineApp.h"
#include "Input/InputDevice.h"
#include "States/PauseState.h"

#include <spdlog/spdlog.h>

void NeneEngine::PlayState::OnEnter()
{
	spdlog::info("PlayState entered");
}

void NeneEngine::PlayState::OnPause()
{
	spdlog::info("PlayState paused");
}

void NeneEngine::PlayState::OnResume()
{
	spdlog::info("PlayState resumed");
}

void NeneEngine::PlayState::OnExit()
{
	spdlog::info("PlayState exited");
}

void NeneEngine::PlayState::Update(float dt)
{
	m_context.world.Update(dt);
}

void NeneEngine::PlayState::HandleInput()
{
	InputDevice* input = m_context.app.GetFocusedInput();
	if (input == nullptr) return;

	if (input->IsKeyPressed(KeyCode::Escape))
		m_context.stateMachine.PushState(eastl::make_unique<PauseState>(m_context));
}
