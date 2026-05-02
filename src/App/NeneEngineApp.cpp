// NeneEngineApp.cpp

#include "App/AppConfig.h"
#include "ECS/Systems/CameraControllerSystem.h"
#include "ECS/Systems/MovementSystem.h"
#include "ECS/Systems/RenderSystem.h"
#include "App/NeneEngineApp.h"
#include "RenderAdapters/DiligentDX12Adapter.h"
#include "Rendering/RenderResizeHandler.h"
#include "Scene/TestScene.h"
#include "States/PlayState.h"
#include "Platform/Windows32/Windows32Window.h"
#include "Core/CustomLogger.h"

#include <stdexcept>

namespace NeneEngine
{
	namespace
	{
		constexpr float kConfigReloadIntervalSeconds = 0.5f;
	}

	NeneEngineApp::NeneEngineApp() = default;

	NeneEngineApp::~NeneEngineApp()
	{
		if (m_running) RequestShutdown();

		if (m_window && m_windowResizeHandle.IsValid())
		{
			m_window->OnResized().Remove(m_windowResizeHandle);
			m_windowResizeHandle.Reset();
		}

		if (spdlog::default_logger())
			spdlog::default_logger()->flush();

		spdlog::shutdown();
	}

	bool NeneEngineApp::Init(uint32_t width, uint32_t height, const std::string& title) 
	{
		try
		{
			// 1. Logger
			CustomLogger::GetInstance().Initialize("../../../../logs/nene_engine.log", true, spdlog::level::info, true);
			LOG_INFO("===== NeneEngine v0.2 starting =====");

			m_appConfigPath = DefaultAppConfigPath();
			const AppConfig appConfig = LoadAppConfig(m_appConfigPath);
			if (std::filesystem::exists(m_appConfigPath))
				m_appConfigLastWriteTime = std::filesystem::last_write_time(m_appConfigPath);

			// 2. Window
			m_window = eastl::make_unique<Windows32Window>();
			if (!m_window->Create(width, height, title))
				return false;

			// 3. Renderer
			m_renderer = eastl::make_unique<DiligentDX12Adapter>();
			if (!m_renderer->Init(m_window->GetHWND(), width, height))
				return false;
			ApplyAppConfig(appConfig);
			m_windowResizeHandle = m_window->OnResized().AddLambda([this](uint32_t newWidth, uint32_t newHeight) {
				HandleWindowResize(newWidth, newHeight);
			});
			
			// 4. States
			m_gameStateMachine.PushState(eastl::make_unique<PlayState>());

			// 5. ECS
			m_world.AddSystem(std::make_unique<ECS::CameraControllerSystem>(m_window->GetInput()));
			m_world.AddSystem(std::make_unique<ECS::MovementSystem>());
			m_world.AddSystem(std::make_unique<ECS::RenderSystem>(m_renderer.get()));
			TestScene::LoadOrCreate(m_world, width, height);
			LOG_INFO("Test scene loaded from {}", TestScene::DefaultScenePath().string());

			LOG_INFO("Application initialized successfully ({}x{})", width, height);

			return true;
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Init failed: {}", e.what());

			return false;
		}
	}

	void NeneEngineApp::ApplyAppConfig(const AppConfig& config)
	{
		if (!m_renderer)
			return;

		m_renderer->SetClearColor(config.window.backgroundColor);
	}

	void NeneEngineApp::HandleWindowResize(uint32_t width, uint32_t height)
	{
		if (!m_renderer)
			return;

		ResizeRenderResources(*m_renderer, m_world, width, height);
		LOG_INFO("Application resized to {}x{}", width, height);
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

			std::wstring windowText = AnsiToWString(m_window->GetTitle()) +
				L"   fps: " + fpsStr +
				L"   mspf: " + mspfStr;

			SetWindowTextW(m_window->GetHWND(), windowText.c_str());

			// Reset for next average.
			frameCnt = 0;
			timeElapsed += 1.0f;
		}
	}

	void NeneEngineApp::ReloadAppConfigIfChanged(float deltaTime)
	{
		m_configReloadAccumulator += deltaTime;
		if (m_configReloadAccumulator < kConfigReloadIntervalSeconds)
			return;

		m_configReloadAccumulator = 0.0f;

		const std::filesystem::path resolvedConfigPath = DefaultAppConfigPath();
		const bool pathChanged = resolvedConfigPath != m_appConfigPath;

		if (pathChanged)
		{
			LOG_INFO("App config path updated to '{}'", resolvedConfigPath.string());
			m_appConfigPath = resolvedConfigPath;
		}

		if (!std::filesystem::exists(m_appConfigPath))
			return;

		const auto currentWriteTime = std::filesystem::last_write_time(m_appConfigPath);
		if (!pathChanged && currentWriteTime == m_appConfigLastWriteTime)
			return;

		const AppConfig updatedConfig = LoadAppConfig(m_appConfigPath);
		ApplyAppConfig(updatedConfig);
		m_appConfigLastWriteTime = currentWriteTime;

		LOG_INFO("App config hot-reloaded from '{}'", m_appConfigPath.string());
	}


	void NeneEngineApp::Run() 
	{
		m_running = true;
		m_timer.Reset();

		while (m_running && !m_window->ShouldClose())
		{
			m_window->PumpMessages();

			m_timer.Tick();
			float deltaTime = m_timer.DeltaTime();

			if (!m_isPaused)
			{
				m_gameStateMachine.HandleInput();
				m_gameStateMachine.Update(deltaTime);
				m_world.Update(deltaTime);
				ReloadAppConfigIfChanged(deltaTime);

				m_renderer->BeginFrame();
				m_world.Render();
				m_renderer->EndFrame();
				m_renderer->Present();

				CalculateFrameStats();
				m_window->GetInput().EndFrame();
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

} // namespace NeneEngine
