// Application.h

#pragma once

#include "GameStateMachine.h"
#include "GameTimer.h"
#include "IWindow.h"
#include "RenderAdapters/IRenderAdapter.h"

#include <atomic>
#include <EASTL/unique_ptr.h>

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
		eastl::unique_ptr<IWindow>			m_window;
		eastl::unique_ptr<IRenderAdapter>	m_renderer;
		
		GameTimer m_timer;
		GameStateMachine m_gameStateMachine;

		std::atomic<bool> m_running{ false };
		std::atomic<bool> m_isPaused{ false };
	};

} // namespace NeneEngine
