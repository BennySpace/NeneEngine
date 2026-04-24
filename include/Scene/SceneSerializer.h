// SceneSerializer.h

#pragma once

#include "ECS/World.h"

#include <filesystem>
#include <nlohmann/json.hpp>

namespace NeneEngine {

	class SceneSerializer final
	{
	public:
		static constexpr int CurrentVersion = 1;

		[[nodiscard]] static nlohmann::json Serialize(const ECS::World& world);
		static void Deserialize(const nlohmann::json& sceneJson, ECS::World& world);

		static void SaveToFile(const ECS::World& world, const std::filesystem::path& path);
		static void LoadFromFile(const std::filesystem::path& path, ECS::World& world);

	private:
		SceneSerializer() = delete;
	};

} // namespace NeneEngine
