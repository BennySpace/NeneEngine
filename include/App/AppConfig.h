#pragma once

#include "Input/KeyCode.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

namespace NeneEngine
{
	struct WindowDefinitionConfig
	{
		std::string title = "NeneEngine";
		uint32_t width = 1280;
		uint32_t height = 720;
		bool isMain = false;
	};

	struct WindowConfig
	{
		glm::vec4 backgroundColor{0.1f, 0.1f, 0.2f, 1.0f};
	};

	struct InputConfig
	{
		std::unordered_map<std::string, std::vector<KeyCode>> actions{};
	};

	struct AppConfig
	{
		WindowConfig window{};
		InputConfig input{};
		std::vector<WindowDefinitionConfig> windows{};
	};

	[[nodiscard]] std::filesystem::path DefaultAppConfigPath();
	[[nodiscard]] AppConfig LoadAppConfig(const std::filesystem::path& configPath = DefaultAppConfigPath());
} // namespace NeneEngine
