// Application.cpp

#include "Application.h"
#include "Win32Window.h"

#include <stdexcept>

namespace NeneEngine
{
	
	Application::Application() = default;

	Application::~Application()
	{
		if (m_running) RequestShutdown();
	}

	bool Application::Init(uint32_t width, uint32_t height, const std::string& title) {
		try
		{
			m_window = eastl::make_unique<Win32Window>();
			if (!m_window->Create(width, height, title))
			{
				return false;
			}

			// spdlog::info("Application initialized ({0}x{1})", width, height);
			return true;
		}
		catch (const std::exception& e)
		{
			// spdlog::error("Init failed: {}", e.what());
			return false;
		}
	}


	inline std::wstring AnsiToWString(const std::string& str)
	{
		WCHAR buffer[512];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
		return std::wstring(buffer);
	}

	void Application::CalculateFrameStats()
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


	void Application::Run() 
	{
		m_running = true;
		m_timer.Reset();

		while (m_running && !m_window->ShouldClose())
		{
			m_window->PumpMessages();

			m_timer.Tick();
			if (!m_isPaused)
			{
				CalculateFrameStats();
				
			}
			else
			{
				Sleep(100);
			}

		}
	}

	void Application::RequestShutdown()
	{
		m_running = false;
	}

} // namespace NeneEngine