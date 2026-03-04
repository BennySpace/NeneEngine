// Application.cpp

#include "../include/Application.h"
#include "../include/Win32Window.h"

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
			m_window = std::make_unique<Win32Window>();
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

	void Application::Run() 
	{
		m_running = true;

		while (m_running && !m_window->ShouldClose())
		{
			m_window->PumpMessages();
		}
	}

	void Application::RequestShutdown()
	{
		m_running = false;
	}

} // namespace NeneEngine