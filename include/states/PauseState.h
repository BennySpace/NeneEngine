// PauseState.h

#pragma once

#include "IGameState.h"

namespace NeneEngine
{

	class PauseState final : public IGameState
	{
	public:
		void OnEnter() override;
		void OnExit() override;
		void Update(float dt) override;
		void HandleInput() override;

		bool IsTransparent() const override { return false; }
		bool IsPausing() const override { return true; }
	};

} // namespace NeneEngine
