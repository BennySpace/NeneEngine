#pragma once

#include "ECS/World.h"

#include <filesystem>
#include <glm/glm.hpp>
#include <optional>
#include <string>
#include <vector>

namespace NeneEngine
{

	struct SceneEntityVisibilityOverrideConfig
	{
		std::string tag;
		bool visible = true;
	};

	struct SceneEntityTransformOverrideConfig
	{
		std::string tag;
		std::optional<glm::vec3> position;
		std::optional<glm::vec3> rotationDegrees;
		std::optional<glm::vec3> scale;
	};

	struct SceneEntityMaterialOverrideConfig
	{
		std::string tag;
		std::optional<glm::vec4> tint;
	};

	struct SceneConfig
	{
		std::vector<SceneEntityVisibilityOverrideConfig> entityVisibilityOverrides;
		std::vector<SceneEntityTransformOverrideConfig> entityTransformOverrides;
		std::vector<SceneEntityMaterialOverrideConfig> entityMaterialOverrides;
	};

	[[nodiscard]] SceneConfig LoadSceneConfig(const std::filesystem::path& configPath);
	void ApplySceneConfig(ECS::World& world, const SceneConfig& config);

} // namespace NeneEngine
