// WindowInterface.h

#pragma once

#include <string>
#include <cstdint>
#include <wtypes.h>

namespace NeneEngine
{

	class WindowInterface 
	{
	public:
		virtual ~WindowInterface() = default;

		virtual bool Create(uint32_t width, uint32_t height, const std::string& title) = 0;
		virtual void Destroy() = 0;

		virtual void PumpMessages() = 0;
		virtual bool ShouldClose() const = 0;

		virtual HWND GetHWND() const = 0;
		virtual std::string GetTitle() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
	};

} // namespace NeneEngine