// ColliderComponent.h

#pragma once

#include <glm/glm.hpp>

namespace NeneEngine::ECS
{

	enum class ColliderType
	{
		Box,
		Sphere
	};

	struct ColliderComponent
	{
		ColliderType type = ColliderType::Box;
		glm::vec3 halfExtents = {0.5f, 0.5f, 0.5f};
		glm::vec3 offset = {0.0f, 0.0f, 0.0f};
	};

} // namespace NeneEngine::ECS
