#include "App/AppRuntimeConfigPolicy.h"

namespace NeneEngine
{
	namespace
	{
		bool WindowDefinitionsEqual(const WindowDefinitionConfig& lhs, const WindowDefinitionConfig& rhs)
		{
			return lhs.title == rhs.title && lhs.width == rhs.width && lhs.height == rhs.height &&
			       lhs.isMain == rhs.isMain;
		}

		bool WindowListsEqual(const std::vector<WindowDefinitionConfig>& lhs,
		                      const std::vector<WindowDefinitionConfig>& rhs)
		{
			if (lhs.size() != rhs.size()) return false;

			for (size_t index = 0; index < lhs.size(); ++index)
			{
				if (!WindowDefinitionsEqual(lhs[index], rhs[index])) return false;
			}

			return true;
		}
	} // namespace

	AppConfigHotReloadPolicyResult EvaluateAppConfigHotReload(const AppConfig& currentConfig,
	                                                          const AppConfig& updatedConfig)
	{
		AppConfigHotReloadPolicyResult result{};
		result.runtimeAppliedConfig = currentConfig;
		result.runtimeAppliedConfig.window = updatedConfig.window;
		result.runtimeAppliedConfig.input = updatedConfig.input;
		result.requiresRestart = !WindowListsEqual(currentConfig.windows, updatedConfig.windows);
		return result;
	}
} // namespace NeneEngine
