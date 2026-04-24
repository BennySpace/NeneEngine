// MovementComponent.h

#pragma once

#include <glm/glm.hpp>

namespace NeneEngine::ECS {

	struct MovementComponent
	{
		glm::vec3 velocity = { 0.0f, 0.0f, 0.0f };

		glm::vec3 origin = { 0.0f, 0.0f, 0.0f };
		glm::vec3 oscillationAxis = { 1.0f, 0.0f, 0.0f };
		float oscillationAmplitude = 0.0f;
		float oscillationSpeed = 1.0f;
		float elapsedTime = 0.0f;
		bool useOscillation = false;
	};

} // namespace NeneEngine::ECS
