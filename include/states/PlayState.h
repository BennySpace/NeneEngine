// PlayState.h

#pragma once

#include "GameState.h"

namespace NeneEngine
{

	class PlayState final : public GameState
	{
	public:
		void OnEnter() override;
		void OnExit() override;
		void Update(float dt) override;
		void HandleInput() override;
	};

} // namespace NeneEngine