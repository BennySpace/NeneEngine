// RigidbodyComponent.h

#pragma once

#include <glm/glm.hpp>

namespace NeneEngine::ECS
{

	struct RigidbodyComponent
	{
		glm::vec3 velocity = {0.0f, 0.0f, 0.0f};
		glm::vec3 acceleration = {0.0f, 0.0f, 0.0f};
		float mass = 1.0f;
		bool useGravity = true;
	};

} // namespace NeneEngine::ECS
