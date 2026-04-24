// Win32Window.cpp

#include "Win32Window.h"
#include "CustomLogger.h"

#include <array>
#include <stdexcept>
#include <vector>

namespace NeneEngine
{
	namespace {

		glm::vec2 GetCurrentClientMousePosition(HWND hwnd)
		{
			POINT point{};
			GetCursorPos(&point);
			ScreenToClient(hwnd, &point);

			return { static_cast<float>(point.x), static_cast<float>(point.y) };
		}

		KeyCode TranslateVirtualKey(UINT virtualKey, UINT scanCode, bool isExtended)
		{
			switch (virtualKey)
			{
			case VK_SHIFT:
				virtualKey = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
				break;
			case VK_CONTROL:
				virtualKey = isExtended ? VK_RCONTROL : VK_LCONTROL;
				break;
			case VK_MENU:
				virtualKey = isExtended ? VK_RMENU : VK_LMENU;
				break;
			default:
				break;
			}

			return static_cast<KeyCode>(virtualKey);
		}

		KeyCode TranslateRawVirtualKey(const RAWKEYBOARD& keyboard)
		{
			const bool isExtended = (keyboard.Flags & RI_KEY_E0) != 0;
			return TranslateVirtualKey(keyboard.VKey, keyboard.MakeCode, isExtended);
		}

		void RegisterRawInput(HWND hwnd)
		{
			std::array<RAWINPUTDEVICE, 2> devices{};

			devices[0].usUsagePage = 0x01;
			devices[0].usUsage = 0x02;
			// Keep legacy messages enabled so Windows can still handle the title bar, close button, and Alt+F4.
			devices[0].dwFlags = 0;
			devices[0].hwndTarget = hwnd;

			devices[1].usUsagePage = 0x01;
			devices[1].usUsage = 0x06;
			devices[1].dwFlags = 0;
			devices[1].hwndTarget = hwnd;

			if (!RegisterRawInputDevices(devices.data(), static_cast<UINT>(devices.size()), sizeof(RAWINPUTDEVICE)))
				throw std::runtime_error("Failed to register raw input devices");
		}

	} // namespace

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
		RegisterRawInput(m_hwnd);

		return true;
	}

	void Win32Window::Destroy() {
		if (m_hwnd) {
			DestroyWindow(m_hwnd);
			m_hwnd = nullptr;
		}
	}

	void Win32Window::PumpMessages() {
		m_input.EndFrame();

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
		case WM_INPUT:
		{
			UINT dataSize = 0;
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &dataSize, sizeof(RAWINPUTHEADER)) != 0)
			{
				LOG_WARN("Win32Window: failed to query raw input data size");
				return 0;
			}

			std::vector<uint8_t> rawData(dataSize);
			const UINT bytesRead = GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				rawData.data(),
				&dataSize,
				sizeof(RAWINPUTHEADER));

			if (bytesRead != dataSize)
			{
				LOG_WARN("Win32Window: failed to read raw input data");
				return 0;
			}

			const auto* rawInput = reinterpret_cast<const RAWINPUT*>(rawData.data());

			if (rawInput->header.dwType == RIM_TYPEKEYBOARD)
			{
				const RAWKEYBOARD& keyboard = rawInput->data.keyboard;
				if (keyboard.VKey == 0xFF)
					return 0;

				const KeyCode key = TranslateRawVirtualKey(keyboard);
				const bool isKeyUp = (keyboard.Flags & RI_KEY_BREAK) != 0;

				if (isKeyUp)
					m_input.NotifyKeyUp(key);
				else
					m_input.NotifyKeyDown(key);

				return 0;
			}

			if (rawInput->header.dwType == RIM_TYPEMOUSE)
			{
				const RAWMOUSE& mouse = rawInput->data.mouse;
				const USHORT flags = mouse.usButtonFlags;

				if (flags & RI_MOUSE_LEFT_BUTTON_DOWN)
					m_input.NotifyKeyDown(KeyCode::MouseLeft);
				if (flags & RI_MOUSE_LEFT_BUTTON_UP)
					m_input.NotifyKeyUp(KeyCode::MouseLeft);
				if (flags & RI_MOUSE_RIGHT_BUTTON_DOWN)
					m_input.NotifyKeyDown(KeyCode::MouseRight);
				if (flags & RI_MOUSE_RIGHT_BUTTON_UP)
					m_input.NotifyKeyUp(KeyCode::MouseRight);
				if (flags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
					m_input.NotifyKeyDown(KeyCode::MouseMiddle);
				if (flags & RI_MOUSE_MIDDLE_BUTTON_UP)
					m_input.NotifyKeyUp(KeyCode::MouseMiddle);
				if (flags & RI_MOUSE_BUTTON_4_DOWN)
					m_input.NotifyKeyDown(KeyCode::MouseX1);
				if (flags & RI_MOUSE_BUTTON_4_UP)
					m_input.NotifyKeyUp(KeyCode::MouseX1);
				if (flags & RI_MOUSE_BUTTON_5_DOWN)
					m_input.NotifyKeyDown(KeyCode::MouseX2);
				if (flags & RI_MOUSE_BUTTON_5_UP)
					m_input.NotifyKeyUp(KeyCode::MouseX2);
				if (flags & RI_MOUSE_WHEEL)
				{
					const auto wheelDelta = static_cast<float>(static_cast<SHORT>(mouse.usButtonData)) / static_cast<float>(WHEEL_DELTA);
					m_input.NotifyMouseWheel(wheelDelta);
				}

				const glm::vec2 position = GetCurrentClientMousePosition(hwnd);
				if ((mouse.usFlags & MOUSE_MOVE_ABSOLUTE) != 0)
				{
					m_input.NotifyMouseMove(position);
				}
				else if (mouse.lLastX != 0 || mouse.lLastY != 0)
				{
					m_input.NotifyMouseDelta(
						{ static_cast<float>(mouse.lLastX), static_cast<float>(mouse.lLastY) },
						position);
				}

				return 0;
			}

			return 0;
		}

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
