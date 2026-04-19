// PauseState.cpp

#include "states/PauseState.h"

#include <spdlog/spdlog.h>

void NeneEngine::PauseState::OnEnter()
{
	spdlog::info("PauseState entered");
}

void NeneEngine::PauseState::OnExit()
{
	spdlog::info("PauseState exited");
}

void NeneEngine::PauseState::Update(float /*dt*/)
{
	// TODO
}

void NeneEngine::PauseState::HandleInput()
{
	// TODO
}
