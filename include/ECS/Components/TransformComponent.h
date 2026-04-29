// Transform.h

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace NeneEngine::ECS {

	struct TransformComponent
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		glm::mat4 GetModelMatrix() const
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, position);
			model = model * glm::mat4_cast(rotation);
			model = glm::scale(model, scale);

			return model;
		}
	};

} // namespace NeneEngine::ECS
