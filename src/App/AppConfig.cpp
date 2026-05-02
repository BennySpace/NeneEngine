#include "App/AppConfig.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <Windows.h>

#include "../external/DiligentEngine/DiligentCore/Graphics/GraphicsAccessories/interface/ColorConversion.h"
#include "Core/CustomLogger.h"

namespace NeneEngine
{
	namespace
	{
		constexpr glm::vec4 kDefaultBackgroundColor{ 0.1f, 0.1f, 0.2f, 1.0f };

		[[nodiscard]] std::filesystem::path FindConfigPathFrom(const std::filesystem::path& startDirectory)
		{
			for (auto current = startDirectory; !current.empty(); current = current.parent_path())
			{
				const auto candidate = current / "assets" / "config" / "app.json";
				if (std::filesystem::exists(candidate))
					return candidate;

				if (current == current.root_path())
					break;
			}

			return {};
		}

		[[nodiscard]] std::filesystem::path GetExecutableDirectory()
		{
			wchar_t modulePath[MAX_PATH]{};
			const DWORD pathLength = GetModuleFileNameW(nullptr, modulePath, MAX_PATH);
			if (pathLength == 0 || pathLength == MAX_PATH)
				return {};

			return std::filesystem::path(modulePath).parent_path();
		}

		[[nodiscard]] glm::vec4 ReadBackgroundColorOrDefault(const nlohmann::json& root)
		{
			const auto windowIt = root.find("window");
			if (windowIt == root.end() || !windowIt->is_object())
			{
				LOG_WARN("App config: 'window' section is missing or invalid, using default background color");
				return kDefaultBackgroundColor;
			}

			const auto colorIt = windowIt->find("backgroundColor");
			if (colorIt == windowIt->end() || !colorIt->is_object())
			{
				LOG_WARN("App config: 'window.backgroundColor' is missing or invalid, using default background color");
				return kDefaultBackgroundColor;
			}

			const auto& color = *colorIt;
			const auto hasValidComponents =
				color.contains("r") && color["r"].is_number() &&
				color.contains("g") && color["g"].is_number() &&
				color.contains("b") && color["b"].is_number();

			if (!hasValidComponents)
			{
				LOG_WARN("App config: 'window.backgroundColor' must contain numeric r/g/b, using default background color");
				return kDefaultBackgroundColor;
			}

			glm::vec4 parsedColor{
				color["r"].get<float>(),
				color["g"].get<float>(),
				color["b"].get<float>(),
				1.0f
			};

			const bool hasValidRgbRange =
				parsedColor.r >= 0.0f && parsedColor.r <= 255.0f &&
				parsedColor.g >= 0.0f && parsedColor.g <= 255.0f &&
				parsedColor.b >= 0.0f && parsedColor.b <= 255.0f;
			if (!hasValidRgbRange)
			{
				LOG_WARN("App config: 'window.backgroundColor' RGB components must be in 0..255, using default background color");
				return kDefaultBackgroundColor;
			}

			parsedColor.r /= 255.0f;
			parsedColor.g /= 255.0f;
			parsedColor.b /= 255.0f;

			// Config RGB values are treated as standard sRGB palette bytes.
			// Convert them to linear space before clearing the render target.
			parsedColor.r = Diligent::GammaToLinear(parsedColor.r);
			parsedColor.g = Diligent::GammaToLinear(parsedColor.g);
			parsedColor.b = Diligent::GammaToLinear(parsedColor.b);

			return parsedColor;
		}
	}

	std::filesystem::path DefaultAppConfigPath()
	{
		if (const auto fromCurrentDirectory = FindConfigPathFrom(std::filesystem::current_path()); !fromCurrentDirectory.empty())
			return fromCurrentDirectory;

		if (const auto executableDirectory = GetExecutableDirectory(); !executableDirectory.empty())
		{
			if (const auto fromExecutableDirectory = FindConfigPathFrom(executableDirectory); !fromExecutableDirectory.empty())
				return fromExecutableDirectory;
		}

		return std::filesystem::path{ "assets" } / "config" / "app.json";
	}

	AppConfig LoadAppConfig(const std::filesystem::path& configPath)
	{
		AppConfig config{};
		config.window.backgroundColor = kDefaultBackgroundColor;

		std::ifstream input(configPath);
		if (!input.is_open())
		{
			LOG_WARN("App config: failed to open '{}', using defaults", configPath.string());
			return config;
		}

		try
		{
			nlohmann::json root;
			input >> root;

			config.window.backgroundColor = ReadBackgroundColorOrDefault(root);
			LOG_INFO(
				"App config loaded from '{}'; backgroundColor=({:.2f}, {:.2f}, {:.2f}, {:.2f})",
				configPath.string(),
				config.window.backgroundColor.r,
				config.window.backgroundColor.g,
				config.window.backgroundColor.b,
				config.window.backgroundColor.a);
		}
		catch (const std::exception& ex)
		{
			LOG_WARN("App config: failed to parse '{}': {}. Using defaults", configPath.string(), ex.what());
		}

		return config;
	}
}
