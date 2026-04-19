// IRenderAdapter.h

#pragma once

#include <cstdint>
#include <Windows.h>

namespace NeneEngine {

    class IRenderAdapter {
    public:
        virtual ~IRenderAdapter() = default;

        virtual bool Init(HWND hwnd, uint32_t width, uint32_t height) = 0;
        virtual void Shutdown() = 0;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Present() = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;
    };

} // namespace NeneEngine
