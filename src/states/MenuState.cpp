// MenuState.cpp

#include "MenuState.h"

#include <spdlog/spdlog.h>

void MenuState::OnEnter()
{
	spdlog::info("MenuState entered");
}

void MenuState::OnExit()
{
	spdlog::info("MenuState exited");
}

void MenuState::Update(float /*dt*/)
{
	// TODO
}

void MenuState::HandleInput()
{
	// TODO
}