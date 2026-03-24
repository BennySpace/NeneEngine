#include "DiligentDX12Adapter.h"

#include "../external/DiligentEngine/DiligentCore/Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
#include <spdlog/spdlog.h>

namespace NeneEngine {

    using namespace Diligent;

    DiligentDX12Adapter::DiligentDX12Adapter() = default;

    DiligentDX12Adapter::~DiligentDX12Adapter()
    {
        Shutdown();
    }

    bool DiligentDX12Adapter::Init(HWND hwnd, uint32_t width, uint32_t height)
    {
        IEngineFactoryD3D12* pFactory = LoadAndGetEngineFactoryD3D12();
        if (!pFactory)
        {
            spdlog::error("Failed to load D3D12 Engine Factory");
            return false;
        }

        EngineD3D12CreateInfo EngineCI{};
        EngineCI.EnableValidation = true;

        SwapChainDesc SCDesc{};
        SCDesc.Width = width;
        SCDesc.Height = height;
        SCDesc.BufferCount = 2;                     // Double buffering

        pFactory->CreateDeviceAndContextsD3D12(EngineCI, &m_pDevice, &m_pImmediateContext);

        if (!m_pDevice || !m_pImmediateContext)
        {
            spdlog::error("Failed to create D3D12 device and immediate context");
            return false;
        }

        // Swap Chain
        Win32NativeWindow Window{ hwnd };
        FullScreenModeDesc FSDesc{};

        pFactory->CreateSwapChainD3D12(m_pDevice, m_pImmediateContext, SCDesc, FSDesc, Window, &m_pSwapChain);

        if (!m_pSwapChain)
        {
            spdlog::error("Failed to create D3D12 swap chain");
            return false;
        }

        CreateResources();

        spdlog::info("Diligent DX12 Adapter initialized successfully ({}x{})", width, height);
        return true;
    }

    void DiligentDX12Adapter::Shutdown()
    {
        if (m_pImmediateContext)
            m_pImmediateContext->Flush();

        m_pPSO.Release();
        m_pSwapChain.Release();
        m_pImmediateContext.Release();
        m_pDevice.Release();
    }

    void DiligentDX12Adapter::BeginFrame()
    {
        if (!m_pSwapChain || !m_pImmediateContext) return;

        ITextureView* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
        ITextureView* pDSV = m_pSwapChain->GetDepthBufferDSV();

        m_pImmediateContext->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        const float ClearColor[] = { 0.1f, 0.1f, 0.2f, 1.0f };
        m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }

    void DiligentDX12Adapter::EndFrame()
    {
        if (!m_pImmediateContext || !m_pPSO) return;

        m_pImmediateContext->SetPipelineState(m_pPSO);

        DrawAttribs drawAttrs{};
        drawAttrs.NumVertices = 3;
        drawAttrs.StartVertexLocation = 0;

        m_pImmediateContext->Draw(drawAttrs);
    }

    void DiligentDX12Adapter::Present()
    {
        if (m_pSwapChain)
            m_pSwapChain->Present();
    }

    void DiligentDX12Adapter::Resize(uint32_t width, uint32_t height)
    {
        if (m_pSwapChain)
            m_pSwapChain->Resize(width, height);
    }

    // Create all resources for app
    void DiligentDX12Adapter::CreateResources()
    {
        // Basic vertex shader and pixel shaders
        static const char* VSSource = R"raw(
            struct PSInput
            {
                float4 Pos   : SV_POSITION;
                float3 Color : COLOR;
            };

            void main(in uint VertId : SV_VertexID, out PSInput PSIn)
            {
                float4 Pos[3] = {
                    float4(-0.5f, -0.5f, 0.0f, 1.0f),
                    float4( 0.0f,  0.5f, 0.0f, 1.0f),
                    float4( 0.5f, -0.5f, 0.0f, 1.0f)
                };
                float3 Col[3] = {
                    float3(1.0f, 0.0f, 0.0f),
                    float3(0.0f, 1.0f, 0.0f),
                    float3(0.0f, 0.0f, 1.0f)
                };

                PSIn.Pos   = Pos[VertId];
                PSIn.Color = Col[VertId];
            }
        )raw";

        static const char* PSSource = R"raw(
            struct PSInput
            {
                float4 Pos   : SV_POSITION;
                float3 Color : COLOR;
            };

            struct PSOutput
            {
                float4 Color : SV_TARGET;
            };

            void main(in PSInput PSIn, out PSOutput PSOut)
            {
                PSOut.Color = float4(PSIn.Color, 1.0f);
            }
        )raw";

        GraphicsPipelineStateCreateInfo PSOCreateInfo{};
        PSOCreateInfo.PSODesc.Name = "Simple Colored Triangle PSO";
        PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

        PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
        PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = m_pSwapChain->GetDesc().ColorBufferFormat;
        PSOCreateInfo.GraphicsPipeline.DSVFormat = m_pSwapChain->GetDesc().DepthBufferFormat;
        PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_NONE;
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = false;

        ShaderCreateInfo ShaderCI{};
        ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
        ShaderCI.Desc.UseCombinedTextureSamplers = true;

        RefCntAutoPtr<IShader> pVS, pPS;

        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.Desc.Name = "Triangle VS";
        ShaderCI.Source = VSSource;
        m_pDevice->CreateShader(ShaderCI, &pVS);

        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.Desc.Name = "Triangle PS";
        ShaderCI.Source = PSSource;
        m_pDevice->CreateShader(ShaderCI, &pPS);

        if (!pVS || !pPS)
        {
            spdlog::error("Failed to create shaders");
            return;
        }

        PSOCreateInfo.pVS = pVS;
        PSOCreateInfo.pPS = pPS;

        m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pPSO);

        if (!m_pPSO)
            spdlog::error("Failed to create Graphics Pipeline State");
    }

} // namespace NeneEngine