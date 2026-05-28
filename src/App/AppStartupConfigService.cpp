#include "App/AppStartupConfigService.h"

#include <filesystem>

namespace NeneEngine
{
	std::filesystem::path ResolveStartupAppConfigPath()
	{
		return DefaultAppConfigPath();
	}

	LoadedAppConfigState LoadStartupAppConfigState(const std::filesystem::path& configPath)
	{
		LoadedAppConfigState state{};
		state.path = configPath;
		state.config = LoadAppConfig(state.path);

		if (std::filesystem::exists(state.path)) state.lastWriteTime = std::filesystem::last_write_time(state.path);

		return state;
	}

	LoadedAppConfigState LoadStartupAppConfigState()
	{
		return LoadStartupAppConfigState(ResolveStartupAppConfigPath());
	}
} // namespace NeneEngine
