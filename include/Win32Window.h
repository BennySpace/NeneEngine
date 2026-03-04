// Win32Window.h

#pragma once

#include "WindowInterface.h"

#include <Windows.h>

namespace NeneEngine 
{

	class Win32Window final : public WindowInterface
	{
	public:
		Win32Window();
		~Win32Window() override;

		bool Create(uint32_t width, uint32_t height, const std::string& title) override;
		void Destroy() override;

		void PumpMessages() override;
		bool ShouldClose() const override { return m_shouldClose; }

		HWND GetHWND() const override { return m_hwnd; }
		uint32_t GetWidth() const override { return m_width; }
		uint32_t GetHeight() const override { return m_height; }

	private:
		static LRESULT CALLBACK WndProcStatic(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		HWND m_hwnd = nullptr;
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		bool m_shouldClose = false;
		std::string m_title;
	};

} // namespace NeneEngine