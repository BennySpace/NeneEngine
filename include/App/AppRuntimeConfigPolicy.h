#pragma once

#include "App/AppConfig.h"

namespace NeneEngine
{
	struct AppConfigHotReloadPolicyResult
	{
		// Contains only the subset of config that is safe to apply after startup.
		AppConfig runtimeAppliedConfig{};
		bool requiresRestart = false;
	};

	[[nodiscard]] AppConfigHotReloadPolicyResult EvaluateAppConfigHotReload(const AppConfig& currentConfig,
	                                                                        const AppConfig& updatedConfig);
} // namespace NeneEngine
