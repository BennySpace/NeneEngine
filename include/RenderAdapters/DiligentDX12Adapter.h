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
#include <unordered_map>
#include <vector>

namespace NeneEngine {

	class DiligentDX12Adapter final : public IRenderAdapter {
	public:
		DiligentDX12Adapter();
		~DiligentDX12Adapter() override;

		bool Init(HWND hwnd, uint32_t width, uint32_t height) override;
		void Shutdown() override;

		GPUMesh UploadMesh(const MeshData& meshData) override;
		void BeginFrame() override;
		void SubmitRenderItem(const RenderItem& item) override;
		void CreateResources();
		void EndFrame() override;
		void Present() override;
		void Resize(uint32_t width, uint32_t height) override;
		void SetClearColor(const glm::vec4& color) override;

	private:
		static constexpr size_t PrimitiveTypeCount = 4;

		struct PrimitiveDrawConstants
		{
			glm::mat4 modelViewProjectionMatrix = glm::mat4(1.0f);
			glm::vec4 tint = { 1.0f, 1.0f, 1.0f, 1.0f };
		};

		struct UploadedMeshBuffers
		{
			Diligent::RefCntAutoPtr<Diligent::IBuffer> vertexBuffer;
			Diligent::RefCntAutoPtr<Diligent::IBuffer> indexBuffer;
			uint32_t vertexCount = 0;
			uint32_t indexCount = 0;
		};

		Diligent::RefCntAutoPtr<Diligent::IRenderDevice>     m_pDevice;
		Diligent::RefCntAutoPtr<Diligent::IDeviceContext>    m_pImmediateContext;
		Diligent::RefCntAutoPtr<Diligent::ISwapChain>        m_pSwapChain;

		std::array<Diligent::RefCntAutoPtr<Diligent::IPipelineState>, PrimitiveTypeCount> m_pPrimitivePSOs;
		std::array<Diligent::RefCntAutoPtr<Diligent::IBuffer>, PrimitiveTypeCount> m_pPrimitiveConstantBuffers;
		std::array<Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding>, PrimitiveTypeCount> m_pPrimitiveSRBs;
		Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pMeshPSO;
		Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pMeshConstantBuffer;
		Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_pMeshSRB;
		std::vector<RenderItem> m_renderQueue;
		std::unordered_map<uint32_t, UploadedMeshBuffers> m_uploadedMeshes;
		glm::vec4 m_clearColor{ 0.1f, 0.1f, 0.2f, 1.0f };
		uint32_t m_nextMeshId = 1;

		[[nodiscard]] Diligent::IPipelineState* GetPipelineState(PrimitiveType primitiveType) const;
		[[nodiscard]] const UploadedMeshBuffers* GetUploadedMesh(MeshId meshId) const;
		[[nodiscard]] uint32_t GetVertexCount(PrimitiveType primitiveType) const;
	};

} // namespace NeneEngine
