// HierarchyComponent.h

#pragma once

#include "ECS/Entity.h"

#include <vector>

namespace NeneEngine::ECS {

	struct HierarchyComponent
	{
		Entity parent = NullEntity;
		std::vector<Entity> children{};
	};

} // namespace NeneEngine::ECS
