// DiligentDX12Adapter.cpp

#include "RenderAdapters/DiligentDX12Adapter.h"

#include "../external/DiligentEngine/DiligentCore/Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
#include "../external/DiligentEngine/DiligentCore/Graphics/GraphicsEngine/interface/Shader.h"

#include "Core/CustomLogger.h"

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
            LOG_ERROR("Failed to load D3D12 Engine Factory");
            return false;
        }

        EngineD3D12CreateInfo EngineCI{};
        EngineCI.EnableValidation = true;

        SwapChainDesc SCDesc{};
        SCDesc.Width = width;
        SCDesc.Height = height;
        SCDesc.BufferCount = 2;                     // Double buffering
        SCDesc.DepthBufferFormat = TEX_FORMAT_D32_FLOAT;

        pFactory->CreateDeviceAndContextsD3D12(EngineCI, &m_pDevice, &m_pImmediateContext);

        if (!m_pDevice || !m_pImmediateContext)
        {
            LOG_ERROR("Failed to create D3D12 device and immediate context");
            return false;
        }

        // Swap Chain
        Win32NativeWindow Window{ hwnd };
        FullScreenModeDesc FSDesc{};

        pFactory->CreateSwapChainD3D12(m_pDevice, m_pImmediateContext, SCDesc, FSDesc, Window, &m_pSwapChain);

        if (!m_pSwapChain)
        {
            LOG_ERROR("Failed to create D3D12 swap chain");
            return false;
        }

        CreateResources();

        LOG_INFO("Diligent DX12 Adapter initialized successfully ({}x{})", width, height);

        return true;
    }

    void DiligentDX12Adapter::Shutdown()
    {
        if (m_pImmediateContext)
            m_pImmediateContext->Flush();

        for (auto& pso : m_pPrimitivePSOs)
            pso.Release();
        for (auto& buffer : m_pPrimitiveConstantBuffers)
            buffer.Release();
        for (auto& srb : m_pPrimitiveSRBs)
            srb.Release();

        m_renderQueue.clear();
        m_pSwapChain.Release();
        m_pImmediateContext.Release();
        m_pDevice.Release();
    }

    void DiligentDX12Adapter::BeginFrame()
    {
        if (!m_pSwapChain || !m_pImmediateContext) return;

        ITextureView* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
        ITextureView* pDSV = m_pSwapChain->GetDepthBufferDSV();
        if (pRTV == nullptr)
        {
            LOG_WARN("DiligentDX12Adapter: current back buffer RTV is missing");
            return;
        }

        m_pImmediateContext->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        const float ClearColor[] = { 0.1f, 0.1f, 0.2f, 1.0f };
        m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        if (pDSV != nullptr)
            m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        else
            LOG_WARN("DiligentDX12Adapter: depth buffer DSV is missing");

        m_renderQueue.clear();
    }

    void DiligentDX12Adapter::SubmitRenderItem(const RenderItem& item)
    {
        m_renderQueue.push_back(item);
    }

    void DiligentDX12Adapter::EndFrame()
    {
        if (!m_pImmediateContext || !m_pSwapChain)
            return;

        const auto& swapChainDesc = m_pSwapChain->GetDesc();
        Viewport viewport{};
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = static_cast<float>(swapChainDesc.Width);
        viewport.Height = static_cast<float>(swapChainDesc.Height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        m_pImmediateContext->SetViewports(1, &viewport, swapChainDesc.Width, swapChainDesc.Height);

        for (const auto& item : m_renderQueue)
        {
            const size_t primitiveIndex = static_cast<size_t>(item.primitiveType);
            auto* pipelineState = GetPipelineState(item.primitiveType);
            auto* constantBuffer = m_pPrimitiveConstantBuffers[primitiveIndex].RawPtr();
            auto* srb = m_pPrimitiveSRBs[primitiveIndex].RawPtr();

            if (pipelineState == nullptr || constantBuffer == nullptr)
            {
                LOG_WARN("DiligentDX12Adapter: resources are missing for primitive type {}", static_cast<int>(item.primitiveType));
                continue;
            }

            PVoid mappedData = nullptr;
            m_pImmediateContext->MapBuffer(constantBuffer, MAP_WRITE, MAP_FLAG_DISCARD, mappedData);
            if (mappedData == nullptr)
            {
                LOG_WARN("DiligentDX12Adapter: failed to map constant buffer");
                continue;
            }

            auto* drawConstants = static_cast<PrimitiveDrawConstants*>(mappedData);
            drawConstants->modelViewProjectionMatrix = item.modelViewProjectionMatrix;
            drawConstants->tint = item.tint;
            m_pImmediateContext->UnmapBuffer(constantBuffer, MAP_WRITE);

            m_pImmediateContext->SetPipelineState(pipelineState);
            if (srb != nullptr)
                m_pImmediateContext->CommitShaderResources(srb, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

            DrawAttribs drawAttrs{};
            drawAttrs.NumVertices = GetVertexCount(item.primitiveType);
            drawAttrs.StartVertexLocation = 0;

            m_pImmediateContext->Draw(drawAttrs);

            LOG_DEBUG(
                "DiligentDX12Adapter: drew primitive={} mesh={} material={} shader={} tint=({:.2f}, {:.2f}, {:.2f}, {:.2f})",
                static_cast<int>(item.primitiveType),
                item.meshId.value,
                item.materialId.value,
                item.shaderId.value,
                item.tint.r,
                item.tint.g,
                item.tint.b,
                item.tint.a);
        }
    }

    void DiligentDX12Adapter::Present()
    {
        if (m_pSwapChain)
            m_pSwapChain->Present();
    }

    void DiligentDX12Adapter::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
            return;

        if (m_pImmediateContext)
            m_pImmediateContext->Flush();

        if (m_pSwapChain)
        {
            m_pSwapChain->Resize(width, height);
            LOG_INFO("DiligentDX12Adapter: swap chain resized to {}x{}", width, height);
        }
    }

    // Create all resources for app
    void DiligentDX12Adapter::CreateResources()
    {
        const auto createPipelineState = [this](PrimitiveType primitiveType, const char* name, const char* vertexShaderSource)
        {
            static const char* PSSource = R"raw(
                cbuffer Constants
                {
                    float4x4 ModelViewProjection;
                    float4 Tint;
                };

                struct PSInput
                {
                    float4 Pos   : SV_POSITION;
                    float4 Color : COLOR;
                };

                struct PSOutput
                {
                    float4 Color : SV_TARGET;
                };

                void main(in PSInput PSIn, out PSOutput PSOut)
                {
                    PSOut.Color = PSIn.Color;
                }
            )raw";

            GraphicsPipelineStateCreateInfo PSOCreateInfo{};
            PSOCreateInfo.PSODesc.Name = name;
            PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

            PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
            PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = m_pSwapChain->GetDesc().ColorBufferFormat;
            PSOCreateInfo.GraphicsPipeline.DSVFormat = m_pSwapChain->GetDesc().DepthBufferFormat;
            PSOCreateInfo.GraphicsPipeline.PrimitiveTopology =
                primitiveType == PrimitiveType::Line ? PRIMITIVE_TOPOLOGY_LINE_LIST : PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_NONE;
            PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;
            PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable = true;
            PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_LESS;

            ShaderResourceVariableDesc variables[] =
            {
                { SHADER_TYPE_VERTEX, "Constants", SHADER_RESOURCE_VARIABLE_TYPE_STATIC }
            };

            PSOCreateInfo.PSODesc.ResourceLayout.Variables = variables;
            PSOCreateInfo.PSODesc.ResourceLayout.NumVariables = 1;

            ShaderCreateInfo ShaderCI{};
            ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
            ShaderCI.Desc.UseCombinedTextureSamplers = true;

            RefCntAutoPtr<IShader> pVS;
            RefCntAutoPtr<IShader> pPS;

            ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
            ShaderCI.Desc.Name = name;
            ShaderCI.Source = vertexShaderSource;
            m_pDevice->CreateShader(ShaderCI, &pVS);

            ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
            ShaderCI.Desc.Name = "Primitive PS";
            ShaderCI.Source = PSSource;
            m_pDevice->CreateShader(ShaderCI, &pPS);

            if (!pVS || !pPS)
            {
                LOG_ERROR("Failed to create shaders for primitive pipeline '{}'", name);
                return;
            }

            PSOCreateInfo.pVS = pVS;
            PSOCreateInfo.pPS = pPS;

            const size_t primitiveIndex = static_cast<size_t>(primitiveType);

            m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pPrimitivePSOs[primitiveIndex]);

            if (!m_pPrimitivePSOs[primitiveIndex])
            {
                LOG_ERROR("Failed to create Graphics Pipeline State '{}'", name);
                return;
            }

            BufferDesc constantBufferDesc{};
            constantBufferDesc.Name = "Primitive Draw Constants";
            constantBufferDesc.Size = sizeof(PrimitiveDrawConstants);
            constantBufferDesc.BindFlags = BIND_UNIFORM_BUFFER;
            constantBufferDesc.Usage = USAGE_DYNAMIC;
            constantBufferDesc.CPUAccessFlags = CPU_ACCESS_WRITE;

            m_pDevice->CreateBuffer(constantBufferDesc, nullptr, &m_pPrimitiveConstantBuffers[primitiveIndex]);
            if (!m_pPrimitiveConstantBuffers[primitiveIndex])
            {
                LOG_ERROR("Failed to create constant buffer for '{}'", name);
                return;
            }

            auto* constantsVariable = m_pPrimitivePSOs[primitiveIndex]->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants");
            if (constantsVariable == nullptr)
            {
                LOG_ERROR("Failed to get shader constant variable for '{}'", name);
                return;
            }

            constantsVariable->Set(m_pPrimitiveConstantBuffers[primitiveIndex]);
            m_pPrimitivePSOs[primitiveIndex]->CreateShaderResourceBinding(&m_pPrimitiveSRBs[primitiveIndex], true);
        };

        static const char* LineVSSource = R"raw(
            cbuffer Constants
            {
                float4x4 ModelViewProjection;
                float4 Tint;
            };

            struct PSInput
            {
                float4 Pos   : SV_POSITION;
                float4 Color : COLOR;
            };

            void main(in uint VertId : SV_VertexID, out PSInput PSIn)
            {
                float4 Pos[2] = {
                    float4(-0.5f, 0.0f, 0.0f, 1.0f),
                    float4( 0.5f, 0.0f, 0.0f, 1.0f)
                };

                PSIn.Pos = mul(ModelViewProjection, Pos[VertId]);
                PSIn.Color = float4(1.0f, 1.0f, 1.0f, 1.0f) * Tint;
            }
        )raw";

        static const char* TriangleVSSource = R"raw(
            cbuffer Constants
            {
                float4x4 ModelViewProjection;
                float4 Tint;
            };

            struct PSInput
            {
                float4 Pos   : SV_POSITION;
                float4 Color : COLOR;
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

                PSIn.Pos = mul(ModelViewProjection, Pos[VertId]);
                PSIn.Color = float4(Col[VertId], 1.0f) * Tint;
            }
        )raw";

        static const char* QuadVSSource = R"raw(
            cbuffer Constants
            {
                float4x4 ModelViewProjection;
                float4 Tint;
            };

            struct PSInput
            {
                float4 Pos   : SV_POSITION;
                float4 Color : COLOR;
            };

            void main(in uint VertId : SV_VertexID, out PSInput PSIn)
            {
                float4 Pos[6] = {
                    float4(-0.5f, -0.5f, 0.0f, 1.0f),
                    float4(-0.5f,  0.5f, 0.0f, 1.0f),
                    float4( 0.5f,  0.5f, 0.0f, 1.0f),
                    float4(-0.5f, -0.5f, 0.0f, 1.0f),
                    float4( 0.5f,  0.5f, 0.0f, 1.0f),
                    float4( 0.5f, -0.5f, 0.0f, 1.0f)
                };

                PSIn.Pos = mul(ModelViewProjection, Pos[VertId]);
                PSIn.Color = float4(0.9f, 0.8f, 0.2f, 1.0f) * Tint;
            }
        )raw";

        static const char* CubeVSSource = R"raw(
            cbuffer Constants
            {
                float4x4 ModelViewProjection;
                float4 Tint;
            };

            struct PSInput
            {
                float4 Pos   : SV_POSITION;
                float4 Color : COLOR;
            };

            void main(in uint VertId : SV_VertexID, out PSInput PSIn)
            {
                float4 Pos[36] = {
                    float4(-0.4f, -0.4f,  0.4f, 1.0f), float4(-0.4f,  0.4f,  0.4f, 1.0f), float4( 0.4f,  0.4f,  0.4f, 1.0f),
                    float4(-0.4f, -0.4f,  0.4f, 1.0f), float4( 0.4f,  0.4f,  0.4f, 1.0f), float4( 0.4f, -0.4f,  0.4f, 1.0f),

                    float4(-0.4f, -0.4f, -0.4f, 1.0f), float4( 0.4f,  0.4f, -0.4f, 1.0f), float4(-0.4f,  0.4f, -0.4f, 1.0f),
                    float4(-0.4f, -0.4f, -0.4f, 1.0f), float4( 0.4f, -0.4f, -0.4f, 1.0f), float4( 0.4f,  0.4f, -0.4f, 1.0f),

                    float4(-0.4f, -0.4f, -0.4f, 1.0f), float4(-0.4f,  0.4f,  0.4f, 1.0f), float4(-0.4f,  0.4f, -0.4f, 1.0f),
                    float4(-0.4f, -0.4f, -0.4f, 1.0f), float4(-0.4f, -0.4f,  0.4f, 1.0f), float4(-0.4f,  0.4f,  0.4f, 1.0f),

                    float4( 0.4f, -0.4f, -0.4f, 1.0f), float4( 0.4f,  0.4f, -0.4f, 1.0f), float4( 0.4f,  0.4f,  0.4f, 1.0f),
                    float4( 0.4f, -0.4f, -0.4f, 1.0f), float4( 0.4f,  0.4f,  0.4f, 1.0f), float4( 0.4f, -0.4f,  0.4f, 1.0f),

                    float4(-0.4f,  0.4f, -0.4f, 1.0f), float4(-0.4f,  0.4f,  0.4f, 1.0f), float4( 0.4f,  0.4f,  0.4f, 1.0f),
                    float4(-0.4f,  0.4f, -0.4f, 1.0f), float4( 0.4f,  0.4f,  0.4f, 1.0f), float4( 0.4f,  0.4f, -0.4f, 1.0f),

                    float4(-0.4f, -0.4f, -0.4f, 1.0f), float4( 0.4f, -0.4f,  0.4f, 1.0f), float4(-0.4f, -0.4f,  0.4f, 1.0f),
                    float4(-0.4f, -0.4f, -0.4f, 1.0f), float4( 0.4f, -0.4f, -0.4f, 1.0f), float4( 0.4f, -0.4f,  0.4f, 1.0f)
                };

                PSIn.Pos = mul(ModelViewProjection, Pos[VertId]);
                PSIn.Color = float4(0.6f, 0.8f, 1.0f, 1.0f) * Tint;
            }
        )raw";

        createPipelineState(PrimitiveType::Line, "Simple Line PSO", LineVSSource);
        createPipelineState(PrimitiveType::Triangle, "Simple Triangle PSO", TriangleVSSource);
        createPipelineState(PrimitiveType::Quad, "Simple Quad PSO", QuadVSSource);
        createPipelineState(PrimitiveType::Cube, "Simple Cube PSO", CubeVSSource);
    }

    IPipelineState* DiligentDX12Adapter::GetPipelineState(PrimitiveType primitiveType) const
    {
        return m_pPrimitivePSOs[static_cast<size_t>(primitiveType)];
    }

    uint32_t DiligentDX12Adapter::GetVertexCount(PrimitiveType primitiveType) const
    {
        switch (primitiveType)
        {
        case PrimitiveType::Line:
            return 2;
        case PrimitiveType::Triangle:
            return 3;
        case PrimitiveType::Quad:
            return 6;
        case PrimitiveType::Cube:
            return 36;
        default:
            return 3;
        }
    }

} // namespace NeneEngine
