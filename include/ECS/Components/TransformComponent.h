// Transform.h

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace NeneEngine::ECS {

	struct TransformComponent
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		glm::mat4 GetModelMatrix() const;
	};

} // namespace NeneEngine::ECS
