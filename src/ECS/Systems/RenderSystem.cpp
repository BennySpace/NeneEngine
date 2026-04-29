// RenderSystem.cpp

#include "ECS/Components/CameraComponent.h"
#include "ECS/Components/MeshRendererComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/World.h"
#include "Core/CustomLogger.h"

#include <glm/gtc/matrix_transform.hpp>

namespace NeneEngine::ECS {

	void RenderSystem::Update(World& /*world*/, float /*deltaTime*/)
	{
		
	}

	void RenderSystem::Render(World& world)
	{
		if (m_renderer == nullptr)
		{
			LOG_WARN("RenderSystem: render adapter is null");
			return;
		}

		const auto cameraView = world.GetRegistry().view<const TransformComponent, const CameraComponent>();

		const TransformComponent* activeCameraTransform = nullptr;
		const CameraComponent* activeCamera = nullptr;

		for (auto entity : cameraView)
		{
			const auto& camera = cameraView.get<CameraComponent>(entity);
			if (!camera.isPrimary)
				continue;

			activeCameraTransform = &cameraView.get<TransformComponent>(entity);
			activeCamera = &camera;
			break;
		}

		if (activeCameraTransform == nullptr || activeCamera == nullptr)
		{
			LOG_WARN("RenderSystem: no primary camera found");
			return;
		}

		const glm::mat4 viewMatrix = glm::inverse(activeCameraTransform->GetModelMatrix());
		const glm::mat4 projectionMatrix = glm::perspective(
			glm::radians(activeCamera->fovDegrees),
			activeCamera->aspectRatio,
			activeCamera->nearPlane,
			activeCamera->farPlane);
		const glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

		auto view = world.GetRegistry().view<const TransformComponent, const MeshRendererComponent>();

		LOG_DEBUG("RenderSystem: starting render pass");

		for (auto entity : view)
		{
			const auto& transform = view.get<TransformComponent>(entity);
			const auto& meshRenderer = view.get<MeshRendererComponent>(entity);

			if (!meshRenderer.visible)
				continue;

			RenderItem item{};
			item.modelMatrix = transform.GetModelMatrix();
			item.viewMatrix = viewMatrix;
			item.projectionMatrix = projectionMatrix;
			item.viewProjectionMatrix = viewProjectionMatrix;
			item.modelViewProjectionMatrix = viewProjectionMatrix * item.modelMatrix;
			item.primitiveType = meshRenderer.primitiveType;
			item.meshId = meshRenderer.meshId;
			item.materialId = meshRenderer.material.materialId;
			item.shaderId = meshRenderer.material.shaderId;
			item.tint = meshRenderer.material.tint;

			m_renderer->SubmitRenderItem(item);

			LOG_DEBUG("RenderSystem: submitted entity {} | primitive={} | mesh={} | material={} | shader={}",
				static_cast<uint32_t>(entt::to_integral(entity)),
				static_cast<int>(item.primitiveType),
				item.meshId.value,
				item.materialId.value,
				item.shaderId.value);
		}
	}

} // namespace NeneEngine::ECS
