// MenuState.cpp

#include "states/MenuState.h"

#include <spdlog/spdlog.h>

void NeneEngine::MenuState::OnEnter()
{
	spdlog::info("MenuState entered");
}

void NeneEngine::MenuState::OnExit()
{
	spdlog::info("MenuState exited");
}

void NeneEngine::MenuState::Update(float /*dt*/)
{
	// TODO
}

void NeneEngine::MenuState::HandleInput()
{
	// TODO
}