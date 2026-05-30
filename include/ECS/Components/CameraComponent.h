// CameraComponent.h

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace NeneEngine::ECS
{

	struct CameraComponent
	{
		glm::vec3 forward = {0.0f, 0.0f, -1.0f};
		glm::vec3 up = {0.0f, 1.0f, 0.0f};
		float fovDegrees = 60.0f;
		float nearPlane = 0.1f;
		float farPlane = 100.0f;
		float aspectRatio = 16.0f / 9.0f;
		bool isPrimary = true;

		[[nodiscard]] glm::mat4 GetViewMatrix(const glm::vec3& position) const
		{
			return glm::lookAt(position, position + forward, up);
		}

		[[nodiscard]] glm::mat4 GetProjectionMatrix() const
		{
			return glm::perspective(glm::radians(fovDegrees), aspectRatio, nearPlane, farPlane);
		}
	};

} // namespace NeneEngine::ECS
