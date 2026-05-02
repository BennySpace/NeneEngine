#pragma once

#include <filesystem>

#include <glm/glm.hpp>

namespace NeneEngine
{
	struct WindowConfig
	{
		glm::vec4 backgroundColor{ 0.1f, 0.1f, 0.2f, 1.0f };
	};

	struct AppConfig
	{
		WindowConfig window{};
	};

	[[nodiscard]] std::filesystem::path DefaultAppConfigPath();
	[[nodiscard]] AppConfig LoadAppConfig(const std::filesystem::path& configPath = DefaultAppConfigPath());
}
