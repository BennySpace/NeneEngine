#pragma once

#include "App/AppConfig.h"

#include <filesystem>

namespace NeneEngine
{
	struct LoadedAppConfigState
	{
		AppConfig config{};
		std::filesystem::path path;
		std::filesystem::file_time_type lastWriteTime{};
	};

	[[nodiscard]] std::filesystem::path ResolveStartupAppConfigPath();
	[[nodiscard]] LoadedAppConfigState LoadStartupAppConfigState(const std::filesystem::path& configPath);
	[[nodiscard]] LoadedAppConfigState LoadStartupAppConfigState();
} // namespace NeneEngine
