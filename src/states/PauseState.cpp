// PauseState.cpp

#include "PauseState.h"

#include <spdlog/spdlog.h>

void PauseState::OnEnter()
{
	spdlog::info("PauseState entered");
}

void PauseState::OnExit()
{
	spdlog::info("PauseState exited");
}

void PauseState::Update(float /*dt*/)
{
	// TODO
}

void PauseState::HandleInput()
{
	// TODO
}