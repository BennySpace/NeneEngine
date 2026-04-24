// DiligentDX12Adapter.h

#pragma once

#include "IRenderAdapter.h"

#include "../external/DiligentEngine/DiligentCore/Common/interface/RefCntAutoPtr.hpp"
#include "../external/DiligentEngine/DiligentCore/Graphics/GraphicsEngine/interface/Buffer.h"
#include "../external/DiligentEngine/DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "../external/DiligentEngine/DiligentCore/Graphics/GraphicsEngine/interface/PipelineState.h"
#include "../external/DiligentEngine/DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "../external/DiligentEngine/DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceBinding.h"
#include "../external/DiligentEngine/DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h"

#include <array>
#include <vector>

namespace NeneEngine {

	class DiligentDX12Adapter final : public IRenderAdapter {
	public:
		DiligentDX12Adapter();
		~DiligentDX12Adapter() override;

		bool Init(HWND hwnd, uint32_t width, uint32_t height) override;
		void Shutdown() override;

		void BeginFrame() override;
		void SubmitRenderItem(const RenderItem& item) override;
		void CreateResources();
		void EndFrame() override;
		void Present() override;
		void Resize(uint32_t width, uint32_t height) override;

	private:
		static constexpr size_t PrimitiveTypeCount = 4;

		struct PrimitiveDrawConstants
		{
			glm::mat4 modelViewProjectionMatrix = glm::mat4(1.0f);
			glm::vec4 tint = { 1.0f, 1.0f, 1.0f, 1.0f };
		};

		Diligent::RefCntAutoPtr<Diligent::IRenderDevice>     m_pDevice;
		Diligent::RefCntAutoPtr<Diligent::IDeviceContext>    m_pImmediateContext;
		Diligent::RefCntAutoPtr<Diligent::ISwapChain>        m_pSwapChain;

		std::array<Diligent::RefCntAutoPtr<Diligent::IPipelineState>, PrimitiveTypeCount> m_pPrimitivePSOs;
		std::array<Diligent::RefCntAutoPtr<Diligent::IBuffer>, PrimitiveTypeCount> m_pPrimitiveConstantBuffers;
		std::array<Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding>, PrimitiveTypeCount> m_pPrimitiveSRBs;
		std::vector<RenderItem> m_renderQueue;

		[[nodiscard]] Diligent::IPipelineState* GetPipelineState(PrimitiveType primitiveType) const;
		[[nodiscard]] uint32_t GetVertexCount(PrimitiveType primitiveType) const;
	};

} // namespace NeneEngine
