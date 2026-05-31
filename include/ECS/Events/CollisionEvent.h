#pragma once

#include "ECS/Entity.h"

namespace NeneEngine::ECS
{

	struct CollisionEvent
	{
		Entity entityA = NullEntity;
		Entity entityB = NullEntity;
	};

} // namespace NeneEngine::ECS
