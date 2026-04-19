// NeneEngineApp.cpp

#include "CustomLogger.h"
#include "ECS/Components/MeshRenderer.h"
#include "ECS/Components/Tag.h"
#include "ECS/Components/Transform.h"
#include "ECS/Systems/RenderSystem.h"
#include "NeneEngineApp.h"
#include "RenderAdapters/DiligentDX12Adapter.h"
#include "States/PlayState.h"
#include "Win32Window.h"

#include <stdexcept>

namespace NeneEngine
{
	
	NeneEngineApp::NeneEngineApp() = default;

	NeneEngineApp::~NeneEngineApp()
	{
		if (m_running) RequestShutdown();
	}

	bool NeneEngineApp::Init(uint32_t width, uint32_t height, const std::string& title) 
	{
		try
		{
			// 1. Logger
			CustomLogger::GetInstance().Initialize("../../../../logs/nene_engine.log", true, spdlog::level::info, true);
			spdlog::info("===== NeneEngine v0.2 starting =====");

			// 2. Window
			m_window = eastl::make_unique<Win32Window>();
			if (!m_window->Create(width, height, title))
				return false;

			// 3. Renderer
			m_renderer = eastl::make_unique<DiligentDX12Adapter>();
			if (!m_renderer->Init(m_window->GetHWND(), width, height))
				return false;
			
			// 4. States
			m_gameStateMachine.PushState(eastl::make_unique<PlayState>());

			// 5. ECS
			m_world.AddSystem(std::make_unique<ECS::RenderSystem>(m_renderer.get()));

			spdlog::info("Application initialized successfully ({}x{})", width, height);

			return true;
		}
		catch (const std::exception& e)
		{
			spdlog::error("Init failed: {}", e.what());

			return false;
		}
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

				m_renderer->BeginFrame();
				m_renderer->EndFrame();
				m_renderer->Present();

				CalculateFrameStats();
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
