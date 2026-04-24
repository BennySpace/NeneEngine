// TestScene.h

#pragma once

#include "ECS/World.h"

#include <cstdint>
#include <filesystem>

namespace NeneEngine::TestScene {

	std::filesystem::path DefaultScenePath();

	void Create(ECS::World& world, uint32_t width, uint32_t height);
	void LoadOrCreate(ECS::World& world, uint32_t width, uint32_t height, const std::filesystem::path& scenePath = DefaultScenePath());

} // namespace NeneEngine::TestScene
