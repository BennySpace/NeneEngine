// RenderTypes.h

#pragma once

#include <compare>
#include <cstdint>

#include <glm/glm.hpp>

namespace NeneEngine {

	struct MeshId
	{
		uint32_t value = 0;

		constexpr bool IsValid() const noexcept { return value != 0; }
		auto operator<=>(const MeshId&) const = default;
	};

	struct MaterialId
	{
		uint32_t value = 0;

		constexpr bool IsValid() const noexcept { return value != 0; }
		auto operator<=>(const MaterialId&) const = default;
	};

	struct ShaderId
	{
		uint32_t value = 0;

		constexpr bool IsValid() const noexcept { return value != 0; }
		auto operator<=>(const ShaderId&) const = default;
	};

	enum class PrimitiveType : uint8_t
	{
		Line,
		Triangle,
		Quad,
		Cube
	};

	struct Material
	{
		MaterialId materialId{};
		ShaderId   shaderId{};
		glm::vec4  tint = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	struct RenderItem
	{
		glm::mat4     modelMatrix = glm::mat4(1.0f);
		glm::mat4     viewMatrix = glm::mat4(1.0f);
		glm::mat4     projectionMatrix = glm::mat4(1.0f);
		glm::mat4     viewProjectionMatrix = glm::mat4(1.0f);
		glm::mat4     modelViewProjectionMatrix = glm::mat4(1.0f);
		PrimitiveType primitiveType = PrimitiveType::Triangle;
		MeshId        meshId{};
		MaterialId    materialId{};
		ShaderId      shaderId{};
		glm::vec4     tint = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

} // namespace NeneEngine
