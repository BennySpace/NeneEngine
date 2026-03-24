// Application.h

#pragma once

#include "WindowInterface.h"
#include "GameTimer.h"
#include "RenderAdapter.h"

#include <EASTL/unique_ptr.h>
#include <atomic>

namespace NeneEngine 
{

	class Application 
	{
	public:
		Application();
		~Application();

		bool Init(uint32_t width = 1280, uint32_t height = 720, const std::string& title = "NeneEngine");
		void Run();
		void RequestShutdown();
		void CalculateFrameStats();

	private:
		eastl::unique_ptr<WindowInterface> m_window;
		eastl::unique_ptr<RenderAdapter> m_renderer;

		GameTimer m_timer;
		std::atomic<bool> m_running{ false };
		std::atomic<bool> m_isPaused{ false };
	};

} // namespace NeneEngine