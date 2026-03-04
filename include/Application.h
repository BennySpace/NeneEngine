// Application.h

#pragma once

#include "WindowInterface.h"

#include <memory>
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

	private:
		std::unique_ptr<WindowInterface> m_window;
		std::atomic<bool> m_running{ false };
	};

} // namespace NeneEngine