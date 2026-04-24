// MovementSystem.h

#pragma once

#include "ECS/Systems/ISystem.h"

namespace NeneEngine::ECS {

	class MovementSystem final : public ISystem
	{
	public:
		void Update(World& world, float deltaTime) override;
	};

} // namespace NeneEngine::ECS
