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
		constexpr uint32_t kDefaultWindowWidth = 1280;
		constexpr uint32_t kDefaultWindowHeight = 720;
		constexpr std::string_view kDefaultWindowTitle = "NeneEngine";

		[[nodiscard]] std::filesystem::path FindConfigPathFrom(const std::filesystem::path& startDirectory)
		{
			for (auto current = startDirectory; !current.empty(); current = current.parent_path())
			{
				const auto candidate = current / "assets" / "config" / "engine.json";
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
				NENE_LOG_WARN("App config: 'window' section is missing or invalid, using default background color");
				return kDefaultBackgroundColor;
			}

			const auto colorIt = windowIt->find("backgroundColor");
			if (colorIt == windowIt->end() || !colorIt->is_object())
			{
				NENE_LOG_WARN("App config: 'window.backgroundColor' is missing or invalid, using default background color");
				return kDefaultBackgroundColor;
			}

			const auto& color = *colorIt;
			const auto hasValidComponents =
				color.contains("r") && color["r"].is_number() &&
				color.contains("g") && color["g"].is_number() &&
				color.contains("b") && color["b"].is_number();

			if (!hasValidComponents)
			{
				NENE_LOG_WARN("App config: 'window.backgroundColor' must contain numeric r/g/b, using default background color");
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
				NENE_LOG_WARN("App config: 'window.backgroundColor' RGB components must be in 0..255, using default background color");
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

		[[nodiscard]] std::vector<WindowDefinitionConfig> ReadWindowsOrDefault(const nlohmann::json& root)
		{
			std::vector<WindowDefinitionConfig> windows;

			const auto windowsIt = root.find("windows");
			if (windowsIt == root.end())
			{
				windows.push_back(WindowDefinitionConfig{
					std::string(kDefaultWindowTitle),
					kDefaultWindowWidth,
					kDefaultWindowHeight,
					true
				});
				return windows;
			}

			if (!windowsIt->is_array() || windowsIt->empty())
			{
				NENE_LOG_WARN("App config: 'windows' must be a non-empty array, using default main window");
				windows.push_back(WindowDefinitionConfig{
					std::string(kDefaultWindowTitle),
					kDefaultWindowWidth,
					kDefaultWindowHeight,
					true
				});
				return windows;
			}

			bool hasMainWindow = false;
			size_t windowIndex = 0;
			for (const auto& windowValue : *windowsIt)
			{
				++windowIndex;
				if (!windowValue.is_object())
				{
					NENE_LOG_WARN("App config: windows[{}] is not an object, skipping", windowIndex - 1);
					continue;
				}

				WindowDefinitionConfig windowConfig{};
				windowConfig.title = windowValue.value("title", std::string(kDefaultWindowTitle));
				windowConfig.width = windowValue.value("width", kDefaultWindowWidth);
				windowConfig.height = windowValue.value("height", kDefaultWindowHeight);
				windowConfig.isMain = windowValue.value("isMain", false);

				if (windowConfig.title.empty())
					windowConfig.title = std::string(kDefaultWindowTitle);

				if (windowConfig.width == 0 || windowConfig.height == 0)
				{
					NENE_LOG_WARN("App config: windows[{}] has invalid size {}x{}, skipping", windowIndex - 1, windowConfig.width, windowConfig.height);
					continue;
				}

				hasMainWindow = hasMainWindow || windowConfig.isMain;
				windows.push_back(std::move(windowConfig));
			}

			if (windows.empty())
			{
				NENE_LOG_WARN("App config: no valid window definitions found, using default main window");
				windows.push_back(WindowDefinitionConfig{
					std::string(kDefaultWindowTitle),
					kDefaultWindowWidth,
					kDefaultWindowHeight,
					true
				});
				return windows;
			}

			if (!hasMainWindow)
			{
				NENE_LOG_WARN("App config: no window marked as main, first window will be used as main");
				windows.front().isMain = true;
			}

			return windows;
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

		return std::filesystem::path{ "assets" } / "config" / "engine.json";
	}

	AppConfig LoadAppConfig(const std::filesystem::path& configPath)
	{
		AppConfig config{};
		config.window.backgroundColor = kDefaultBackgroundColor;
		config.windows.push_back(WindowDefinitionConfig{
			std::string(kDefaultWindowTitle),
			kDefaultWindowWidth,
			kDefaultWindowHeight,
			true
		});

		std::ifstream input(configPath);
		if (!input.is_open())
		{
			NENE_LOG_WARN("App config: failed to open '{}', using defaults", configPath.string());
			return config;
		}

		try
		{
			nlohmann::json root;
			input >> root;

			config.window.backgroundColor = ReadBackgroundColorOrDefault(root);
			config.windows = ReadWindowsOrDefault(root);
			NENE_LOG_INFO(
				"App config loaded from '{}'; backgroundColor=({:.2f}, {:.2f}, {:.2f}, {:.2f}), windows={}",
				configPath.string(),
				config.window.backgroundColor.r,
				config.window.backgroundColor.g,
				config.window.backgroundColor.b,
				config.window.backgroundColor.a,
				config.windows.size());
		}
		catch (const std::exception& ex)
		{
			NENE_LOG_WARN("App config: failed to parse '{}': {}. Using defaults", configPath.string(), ex.what());
		}

		return config;
	}
}
