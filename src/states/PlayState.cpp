// PlayState.cpp

#include "states/PlayState.h"

#include <spdlog/spdlog.h>

void NeneEngine::PlayState::OnEnter()
{
	spdlog::info("PlayState entered");
}

void NeneEngine::PlayState::OnExit()
{
	spdlog::info("PlayState exited");
}

void NeneEngine::PlayState::Update(float /*dt*/)
{
	// TODO
}

void NeneEngine::PlayState::HandleInput()
{
	// TODO
}