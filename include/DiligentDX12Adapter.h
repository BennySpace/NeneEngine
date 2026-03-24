#pragma once
#include "RenderAdapter.h"

#include "../external/DiligentEngine/DiligentCore/Common/interface/RefCntAutoPtr.hpp"
#include "../external/DiligentEngine/DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "../external/DiligentEngine/DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "../external/DiligentEngine/DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h"

namespace NeneEngine {

    class DiligentDX12Adapter final : public RenderAdapter {
    public:
        DiligentDX12Adapter();
        ~DiligentDX12Adapter() override;

        bool Init(HWND hwnd, uint32_t width, uint32_t height) override;
        void Shutdown() override;

        void BeginFrame() override;
        void CreateResources();
        void EndFrame() override;
        void Present() override;
        void Resize(uint32_t width, uint32_t height) override;

    private:
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice>     m_pDevice;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext>    m_pImmediateContext;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain>        m_pSwapChain;

        Diligent::RefCntAutoPtr<Diligent::IPipelineState>    m_pPSO;
    };

} // namespace NeneEngine