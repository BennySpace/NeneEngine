// PlayState.cpp

#include "PlayState.h"

#include <spdlog/spdlog.h>

void PlayState::OnEnter()
{
	spdlog::info("PlayState entered");
}

void PlayState::OnExit()
{
	spdlog::info("PlayState exited");
}

void PlayState::Update(float /*dt*/)
{
	// TODO
}

void PlayState::HandleInput()
{
	// TODO
}