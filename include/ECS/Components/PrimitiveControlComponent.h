// PrimitiveControlComponent.h

#pragma once

#include <cstddef>
#include <glm/glm.hpp>

namespace NeneEngine::ECS {

	struct PrimitiveControlComponent
	{
		float moveSpeed = 3.5f;
		float rotationStepRadians = 0.3926991f; // 22.5 degrees
		float scaleSmoothing = 10.0f;
		float rotationSmoothing = 10.0f;
		size_t currentScaleLevel = 0;
		float targetRotationRadians = 0.0f;
		glm::vec3 targetScale = { 1.0f, 1.0f, 1.0f };
	};

} // namespace NeneEngine::ECS
