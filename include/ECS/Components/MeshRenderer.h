// MeshRenderer.h

#pragma once

#include <glm/glm.hpp>

namespace NeneEngine::ECS {

	enum class PrimitiveType
	{
		Triangle,
		Quad,
		Cube
	};

	struct MeshRenderer
	{
		PrimitiveType primitive = PrimitiveType::Triangle;
		glm::vec4     color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

} // namespace NeneEngine::ECS
