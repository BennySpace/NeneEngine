// NeneEngineApp.cpp

#include "App/NeneEngineApp.h"
#include "App/AppConfig.h"
#include "Core/CustomLogger.h"
#include "Core/ExternalLibrarySmokeTest.h"
#include "Core/ResourceManager.h"
#include "ECS/Components/CameraComponent.h"
#include "ECS/Components/CameraControllerComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Systems/CameraControllerSystem.h"
#include "ECS/Systems/MovementSystem.h"
#include "ECS/Systems/PrimitiveControlSystem.h"
#include "Platform/Windows32/Windows32Window.h"
#include "RenderAdapters/DiligentDX12Adapter.h"
#include "Rendering/ModelSpawner.h"
#include "Scene/TestScene.h"
#include "States/PlayState.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace NeneEngine
{
	namespace
	{
		constexpr float kConfigReloadIntervalSeconds = 0.5f;

		bool WindowDefinitionsEqual(const WindowDefinitionConfig& lhs, const WindowDefinitionConfig& rhs)
		{
			return lhs.title == rhs.title && lhs.width == rhs.width && lhs.height == rhs.height &&
			       lhs.isMain == rhs.isMain;
		}

		bool WindowListsEqual(const std::vector<WindowDefinitionConfig>& lhs, const std::vector<WindowDefinitionConfig>& rhs)
		{
			if (lhs.size() != rhs.size()) return false;

			for (size_t index = 0; index < lhs.size(); ++index)
			{
				if (!WindowDefinitionsEqual(lhs[index], rhs[index])) return false;
			}

			return true;
		}

		std::filesystem::path ResolveFrom(const std::filesystem::path& start, const std::filesystem::path& relativePath)
		{
			// The app may run from the repo root or a build folder, so asset lookup walks upward from both anchors.
			std::error_code errorCode;
			auto current = start;
			std::filesystem::path resolvedPath;
			while (!current.empty())
			{
				const auto candidate = current / relativePath;
				if (std::filesystem::exists(candidate, errorCode)) resolvedPath = candidate;

				const auto parent = current.parent_path();
				if (parent == current) break;
				current = parent;
			}

			return resolvedPath;
		}

		std::filesystem::path ResolveAssetPath(const std::filesystem::path& relativePath)
		{
			if (const auto fromCurrentDirectory = ResolveFrom(std::filesystem::current_path(), relativePath);
			    !fromCurrentDirectory.empty())
				return fromCurrentDirectory;

			wchar_t modulePath[MAX_PATH]{};
			const DWORD length = GetModuleFileNameW(nullptr, modulePath, static_cast<DWORD>(std::size(modulePath)));
			if (length > 0)
			{
				const std::filesystem::path executableDirectory = std::filesystem::path(modulePath).parent_path();
				if (const auto fromExecutableDirectory = ResolveFrom(executableDirectory, relativePath);
				    !fromExecutableDirectory.empty())
					return fromExecutableDirectory;
			}

			return {};
		}

		std::filesystem::path ResolveAssetDirectory(const std::filesystem::path& relativePath)
		{
			if (const auto fromCurrentDirectory = ResolveFrom(std::filesystem::current_path(), relativePath);
			    !fromCurrentDirectory.empty() && std::filesystem::is_directory(fromCurrentDirectory))
				return fromCurrentDirectory;

			wchar_t modulePath[MAX_PATH]{};
			const DWORD length = GetModuleFileNameW(nullptr, modulePath, static_cast<DWORD>(std::size(modulePath)));
			if (length > 0)
			{
				const std::filesystem::path executableDirectory = std::filesystem::path(modulePath).parent_path();
				if (const auto fromExecutableDirectory = ResolveFrom(executableDirectory, relativePath);
				    !fromExecutableDirectory.empty() && std::filesystem::is_directory(fromExecutableDirectory))
					return fromExecutableDirectory;
			}

			return {};
		}

	} // namespace

	NeneEngineApp::NeneEngineApp() = default;

	NeneEngineApp::~NeneEngineApp()
	{
		if (m_running) RequestShutdown();
		m_gameStateMachine.Clear();

		for (auto& windowContext : m_windows)
		{
			if (windowContext.window && windowContext.resizeHandle.IsValid())
			{
				windowContext.window->OnResized().Remove(windowContext.resizeHandle);
				windowContext.resizeHandle.Reset();
			}
		}

		m_windows.clear();
		CustomLogger::GetInstance().Shutdown();
	}

	bool NeneEngineApp::Init(uint32_t width, uint32_t height, const std::string& title)
	{
		try
		{
			// 1. Logger
			CustomLogger::GetInstance().Initialize("../../../../logs/nene_engine.log", false, spdlog::level::info,
			                                       true);
			NENE_LOG_INFO("===== NeneEngine v0.3 starting =====");
			ResourceManager::GetInstance().RegisterDefaultLoaders();
			RunExternalLibrarySmokeTests();

			m_appConfigPath = DefaultAppConfigPath();
			const AppConfig appConfig = LoadAppConfig(m_appConfigPath);
			m_loadedAppConfig = appConfig;
			if (std::filesystem::exists(m_appConfigPath))
				m_appConfigLastWriteTime = std::filesystem::last_write_time(m_appConfigPath);

			// 2. States
			AppStateContext stateContext{*this, m_world, m_gameStateMachine};
			m_gameStateMachine.PushState(eastl::make_unique<PlayState>(stateContext));

			// 3. ECS
			m_world.AddSystem(std::make_unique<ECS::MovementSystem>());
			TestScene::LoadOrCreate(m_world, width, height);
			NENE_LOG_INFO("Test scene loaded from {}", TestScene::DefaultScenePath().string());

			const ECS::Entity primaryCameraEntity = FindPrimaryCameraEntity();
			if (primaryCameraEntity == ECS::NullEntity)
			{
				NENE_LOG_ERROR("Init failed: no primary camera found after loading scene");
				return false;
			}

			if (appConfig.windows.empty())
			{
				NENE_LOG_ERROR("Init failed: app config does not contain any windows");
				return false;
			}

			size_t mainWindowCount = 0;
			size_t secondaryWindowCount = 0;
			for (const auto& windowConfig : appConfig.windows)
			{
				if (windowConfig.isMain)
					++mainWindowCount;
				else
					++secondaryWindowCount;
			}

			if (mainWindowCount == 0)
			{
				NENE_LOG_ERROR("Init failed: no main window defined in config");
				return false;
			}

			if (mainWindowCount > 1)
				NENE_LOG_WARN(
				    "App config: multiple windows marked as main, only the first one will control the primary camera");

			const auto secondaryCameraEntities = CreateAdditionalWindowCameras(secondaryWindowCount, width, height);
			size_t secondaryCameraIndex = 0;
			bool mainWindowCreated = false;

			for (const auto& windowConfig : appConfig.windows)
			{
				ECS::Entity cameraEntity = primaryCameraEntity;
				if (windowConfig.isMain && !mainWindowCreated)
				{
					mainWindowCreated = true;
				}
				else
				{
					if (secondaryCameraIndex >= secondaryCameraEntities.size())
					{
						NENE_LOG_ERROR("Init failed: not enough secondary cameras for configured windows");
						return false;
					}

					cameraEntity = secondaryCameraEntities[secondaryCameraIndex++];
				}

				if (!CreateWindowContext(windowConfig.width, windowConfig.height, windowConfig.title, cameraEntity))
					return false;
			}

			if (!m_windows.empty() && m_windows.front().renderer)
			{
				IRenderAdapter& renderer = *m_windows.front().renderer;
				// Demo resource spawn still happens at startup until scene files own persistent asset references.
				const auto shaderPath =
				    ResolveAssetPath(std::filesystem::path{"assets"} / "shaders" / "textured_mesh.shader");
				const ShaderId shaderId = CreateTexturedMeshShader(renderer, shaderPath);
				const auto manifestPath =
				    ResolveAssetPath(std::filesystem::path{"assets"} / "models" / "spawn_manifest.json");
				SpawnModelsFromManifest(m_world, renderer, shaderId, manifestPath);
			}

			ApplyAppConfig(appConfig);

			NENE_LOG_INFO("Application initialized successfully ({}x{})", width, height);

			return true;
		}
		catch (const std::exception& e)
		{
			NENE_LOG_ERROR("Init failed: {}", e.what());

			return false;
		}
	}

	bool NeneEngineApp::CreateWindowContext(uint32_t width, uint32_t height, const std::string& title,
	                                        ECS::Entity cameraEntity)
	{
		WindowContext windowContext{};
		windowContext.title = title;
		windowContext.cameraEntity = cameraEntity;
		windowContext.window = eastl::make_unique<Windows32Window>();
		if (!windowContext.window->Create(width, height, title))
		{
			NENE_LOG_ERROR("Failed to create window '{}'", title);
			return false;
		}

		windowContext.renderer = eastl::make_unique<DiligentDX12Adapter>();
		if (!windowContext.renderer->Init(windowContext.window->GetHWND(), width, height))
		{
			NENE_LOG_ERROR("Failed to initialize renderer for window '{}'", title);
			return false;
		}

		windowContext.renderSystem = std::make_unique<ECS::RenderSystem>(windowContext.renderer.get(), cameraEntity);
		AddAppSystem(std::make_unique<ECS::CameraControllerSystem>(windowContext.window->GetInput(), cameraEntity));
		AddAppSystem(std::make_unique<ECS::PrimitiveControlSystem>(windowContext.window->GetInput()));

		const size_t windowIndex = m_windows.size();
		windowContext.resizeHandle =
		    windowContext.window->OnResized().AddLambda([this, windowIndex](uint32_t newWidth, uint32_t newHeight)
		                                                { HandleWindowResize(windowIndex, newWidth, newHeight); });

		m_windows.push_back(std::move(windowContext));
		HandleWindowResize(windowIndex, width, height);
		return true;
	}

	void NeneEngineApp::AddAppSystem(std::unique_ptr<ECS::ISystem> system)
	{
		m_appSystems.push_back(std::move(system));
	}

	std::vector<ECS::Entity> NeneEngineApp::CreateAdditionalWindowCameras(size_t count, uint32_t width, uint32_t height)
	{
		std::vector<ECS::Entity> secondaryCameraEntities;
		secondaryCameraEntities.reserve(count);

		const ECS::Entity primaryCameraEntity = FindPrimaryCameraEntity();
		if (primaryCameraEntity == ECS::NullEntity) return secondaryCameraEntities;

		const auto* primaryTransform = m_world.GetComponent<ECS::TransformComponent>(primaryCameraEntity);
		const auto* primaryCamera = m_world.GetComponent<ECS::CameraComponent>(primaryCameraEntity);
		const auto* primaryController = m_world.GetComponent<ECS::CameraControllerComponent>(primaryCameraEntity);
		if (primaryTransform == nullptr || primaryCamera == nullptr || primaryController == nullptr)
			return secondaryCameraEntities;

		for (size_t cameraIndex = 0; cameraIndex < count; ++cameraIndex)
		{
			const ECS::Entity secondaryCameraEntity =
			    m_world.CreateEntity("SecondaryCamera" + std::to_string(cameraIndex + 1));
			auto& secondaryTransform = m_world.AddComponent<ECS::TransformComponent>(secondaryCameraEntity);
			secondaryTransform = *primaryTransform;
			secondaryTransform.position.x += 2.0f * static_cast<float>(cameraIndex + 1);

			auto& secondaryCamera = m_world.AddComponent<ECS::CameraComponent>(secondaryCameraEntity);
			secondaryCamera = *primaryCamera;
			secondaryCamera.aspectRatio = height == 0 ? 1.0f : static_cast<float>(width) / static_cast<float>(height);
			secondaryCamera.isPrimary = false;

			auto& secondaryController = m_world.AddComponent<ECS::CameraControllerComponent>(secondaryCameraEntity);
			secondaryController = *primaryController;

			secondaryCameraEntities.push_back(secondaryCameraEntity);
		}

		return secondaryCameraEntities;
	}

	ECS::Entity NeneEngineApp::FindPrimaryCameraEntity() const
	{
		const auto cameraView = m_world.GetRegistry().view<const ECS::CameraComponent>();
		for (auto entity : cameraView)
		{
			const auto& camera = cameraView.get<ECS::CameraComponent>(entity);
			if (camera.isPrimary) return entity;
		}

		return ECS::NullEntity;
	}

	bool NeneEngineApp::AreAllWindowsClosed() const
	{
		if (m_windows.empty()) return true;

		for (const auto& windowContext : m_windows)
		{
			if (windowContext.window && !windowContext.window->ShouldClose()) return false;
		}

		return true;
	}

	void NeneEngineApp::ApplyAppConfig(const AppConfig& config)
	{
		for (auto& windowContext : m_windows)
		{
			if (windowContext.renderer) windowContext.renderer->SetClearColor(config.window.backgroundColor);
		}
	}

	void NeneEngineApp::HandleWindowResize(size_t windowIndex, uint32_t width, uint32_t height)
	{
		if (windowIndex >= m_windows.size()) return;

		if (width == 0 || height == 0)
		{
			NENE_LOG_WARN("Application resize ignored for window {} with invalid size {}x{}", windowIndex, width,
			              height);
			return;
		}

		auto& windowContext = m_windows[windowIndex];
		if (windowContext.renderer) windowContext.renderer->Resize(width, height);

		if (auto* camera = m_world.GetComponent<ECS::CameraComponent>(windowContext.cameraEntity))
			camera->aspectRatio = static_cast<float>(width) / static_cast<float>(height);

		NENE_LOG_INFO("Window '{}' resized to {}x{}", windowContext.title, width, height);
	}

	inline std::wstring AnsiToWString(const std::string& str)
	{
		WCHAR buffer[512];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
		return std::wstring(buffer);
	}

	void NeneEngineApp::CalculateFrameStats()
	{
		// Code computes the average frames per second, and also the
		// average time it takes to render one frame.  These stats
		// are appended to the window caption bar.

		static int frameCnt = 0;
		static float timeElapsed = 0.0f;

		frameCnt++;
		// Compute averages over one-second period.
		if ((m_timer.TotalTime() - timeElapsed) >= 1.0f)
		{
			float fps = (float)frameCnt; // fps = frameCnt / 1
			float mspf = 1000.0f / fps;

			std::wstringstream wss;
			wss << std::fixed << std::setprecision(0);
			wss << fps;
			std::wstring fpsStr = wss.str();
			wss.str(L""); // Reset wstringstream
			wss << std::setprecision(6);
			wss << mspf;
			std::wstring mspfStr = wss.str();

			for (const auto& windowContext : m_windows)
			{
				if (!windowContext.window || windowContext.window->ShouldClose()) continue;

				std::wstring windowText =
				    AnsiToWString(windowContext.window->GetTitle()) + L" | fps: " + fpsStr + L" | mspf: " + mspfStr;

				SetWindowTextW(windowContext.window->GetHWND(), windowText.c_str());
			}

			// Reset for next average.
			frameCnt = 0;
			timeElapsed += 1.0f;
		}
	}

	void NeneEngineApp::LogDeltaTimeStats(float deltaTime)
	{
		static int sampleCount = 0;
		static float accumulatedDeltaTime = 0.0f;
		static float timeElapsed = 0.0f;
		static float lastDeltaTime = 0.0f;

		++sampleCount;
		accumulatedDeltaTime += deltaTime;
		lastDeltaTime = deltaTime;

		if ((m_timer.TotalTime() - timeElapsed) < 1.0f) return;

		const float averageDeltaTime = sampleCount > 0 ? accumulatedDeltaTime / static_cast<float>(sampleCount) : 0.0f;

		NENE_LOG_INFO("deltaTime: last={:.6f} s ({:.3f} ms), avg={:.6f} s ({:.3f} ms), samples={}", lastDeltaTime,
		              lastDeltaTime * 1000.0f, averageDeltaTime, averageDeltaTime * 1000.0f, sampleCount);

		sampleCount = 0;
		accumulatedDeltaTime = 0.0f;
		timeElapsed += 1.0f;
	}

	void NeneEngineApp::ReloadAppConfigIfChanged(float deltaTime)
	{
		m_configReloadAccumulator += deltaTime;
		if (m_configReloadAccumulator < kConfigReloadIntervalSeconds) return;

		m_configReloadAccumulator = 0.0f;

		const std::filesystem::path resolvedConfigPath = DefaultAppConfigPath();
		const bool pathChanged = resolvedConfigPath != m_appConfigPath;

		if (pathChanged)
		{
			NENE_LOG_INFO("App config path updated to '{}'", resolvedConfigPath.string());
			m_appConfigPath = resolvedConfigPath;
		}

		if (!std::filesystem::exists(m_appConfigPath)) return;

		const auto currentWriteTime = std::filesystem::last_write_time(m_appConfigPath);
		if (!pathChanged && currentWriteTime == m_appConfigLastWriteTime) return;

		const AppConfig updatedConfig = LoadAppConfig(m_appConfigPath);
		if (!WindowListsEqual(updatedConfig.windows, m_loadedAppConfig.windows))
		{
			NENE_LOG_WARN("App config hot-reload: window definitions changed, but window creation, resizing, titles, "
			              "and main-window reassignment require application restart");
		}

		ApplyAppConfig(updatedConfig);
		m_loadedAppConfig.window = updatedConfig.window;
		m_loadedAppConfig.windows = updatedConfig.windows;
		m_appConfigLastWriteTime = currentWriteTime;

		NENE_LOG_INFO("App config hot-reloaded from '{}'; applied runtime-supported changes only", m_appConfigPath.string());
	}

	void NeneEngineApp::UpdateAppSystems(float deltaTime)
	{
		// Window-bound input systems stay app-owned so World contains gameplay logic only.
		for (auto& system : m_appSystems) system->Update(m_world, deltaTime);
	}

	void NeneEngineApp::Run()
	{
		m_running = true;
		m_timer.Reset();

		while (m_running && !AreAllWindowsClosed())
		{
			for (auto& windowContext : m_windows)
			{
				if (windowContext.window && !windowContext.window->ShouldClose()) windowContext.window->PumpMessages();
			}

			m_timer.Tick();
			float deltaTime = m_timer.DeltaTime();

			if (!m_isPaused)
			{
				m_gameStateMachine.HandleInput();
				m_gameStateMachine.Update(deltaTime);
				LogDeltaTimeStats(deltaTime);
				ReloadAppConfigIfChanged(deltaTime);

				for (auto& windowContext : m_windows)
				{
					if (!windowContext.window || windowContext.window->ShouldClose()) continue;
					if (!windowContext.renderer || !windowContext.renderSystem) continue;

					// Render systems are per-window so the same world can be drawn from different cameras.
					windowContext.renderer->BeginFrame();
					windowContext.renderSystem->Render(m_world);
					windowContext.renderer->EndFrame();
					windowContext.renderer->Present();
				}

				CalculateFrameStats();
				for (auto& windowContext : m_windows)
				{
					if (windowContext.window) windowContext.window->GetInput().EndFrame();
				}
			}
			else
			{
				Sleep(100);
			}
		}
	}

	void NeneEngineApp::RequestShutdown()
	{
		m_running = false;
	}

	InputDevice* NeneEngineApp::GetFocusedInput()
	{
		for (auto& windowContext : m_windows)
		{
			if (!windowContext.window || windowContext.window->ShouldClose()) continue;

			InputDevice& input = windowContext.window->GetInput();
			if (input.IsFocused()) return &input;
		}

		return nullptr;
	}

	const InputDevice* NeneEngineApp::GetFocusedInput() const
	{
		for (const auto& windowContext : m_windows)
		{
			if (!windowContext.window || windowContext.window->ShouldClose()) continue;

			const InputDevice& input = windowContext.window->GetInput();
			if (input.IsFocused()) return &input;
		}

		return nullptr;
	}

} // namespace NeneEngine
