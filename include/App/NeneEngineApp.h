// Application.h

#pragma once

#include "App/GameStateMachine.h"
#include "Core/Delegate.h"
#include "Core/GameTimer.h"
#include "ECS/World.h"
#include "Platform/IWindow.h"
#include "RenderAdapters/IRenderAdapter.h"

#include <atomic>
#include <EASTL/unique_ptr.h>

namespace NeneEngine 
{

	class NeneEngineApp 
	{
	public:
		NeneEngineApp();
		~NeneEngineApp();

		bool Init(uint32_t width = 1280, uint32_t height = 720, const std::string& title = "NeneEngine");
		void Run();
		void RequestShutdown();

	private:
		eastl::unique_ptr<IWindow>			m_window;
		eastl::unique_ptr<IRenderAdapter>	m_renderer;
		
		GameTimer							m_timer;
		GameStateMachine					m_gameStateMachine;
		ECS::World							m_world;

		std::atomic<bool>					m_running{ false };
		std::atomic<bool>					m_isPaused{ false };
		DelegateHandle						m_windowResizeHandle;

		void CalculateFrameStats();
		void HandleWindowResize(uint32_t width, uint32_t height);
	};

} // namespace NeneEngine
