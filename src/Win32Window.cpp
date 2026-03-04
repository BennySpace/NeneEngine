// Win32Window.cpp

#include "../include/Win32Window.h"

#include <stdexcept>

namespace NeneEngine
{
	Win32Window::Win32Window() = default;

	Win32Window::~Win32Window() {
		Destroy();
	}

	bool Win32Window::Create(uint32_t width, uint32_t height, const std::string& title) {
		m_width = width;
		m_height = height;
		m_title = title;

		HINSTANCE hInstance = GetModuleHandle(nullptr);

		WNDCLASSEX wc{};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProcStatic;
		wc.hInstance = hInstance;
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.lpszClassName = "NeneEngineWindowClass";

		if (!RegisterClassEx(&wc)) {
			throw std::runtime_error("Failed to register window class");
		}

		RECT rect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

		m_hwnd = CreateWindowEx(
			0,
			wc.lpszClassName,
			title.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			rect.right - rect.left,
			rect.bottom - rect.top,
			nullptr, nullptr, hInstance, this
		);

		if (!m_hwnd) {
			throw std::runtime_error("Failed to create window");
		}

		ShowWindow(m_hwnd, SW_SHOW);
		UpdateWindow(m_hwnd);

		return true;
	}

	void Win32Window::Destroy() {
		if (m_hwnd) {
			DestroyWindow(m_hwnd);
			m_hwnd = nullptr;
		}
	}

	void Win32Window::PumpMessages() {
		MSG msg{};
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				m_shouldClose = true;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	LRESULT CALLBACK Win32Window::WndProcStatic(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		if (msg == WM_NCCREATE) {
			auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
			return TRUE;
		}

		auto* window = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (window) {
			return window->WndProc(hwnd, msg, wParam, lParam);
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	LRESULT Win32Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			m_shouldClose = true;
			return 0;

		case WM_SIZE:
			m_width = LOWORD(lParam);
			m_height = HIWORD(lParam);

			return 0;
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

} // namespace NeneEngine