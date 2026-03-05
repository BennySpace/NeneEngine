// MenuState.h

#pragma once

#include "GameStateInterface.h"

namespace NeneEngine
{

	class MenuState final : public GameStateInterface
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