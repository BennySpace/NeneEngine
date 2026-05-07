// RenderSystem.cpp

#include "ECS/Components/CameraComponent.h"
#include "ECS/Components/HierarchyComponent.h"
#include "ECS/Components/MeshRendererComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/World.h"
#include "Core/CustomLogger.h"

#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#include <unordered_set>

namespace NeneEngine::ECS {
	namespace
	{
		uint32_t ToEntityId(Entity entity)
		{
			return static_cast<uint32_t>(entt::to_integral(entity));
		}

		glm::mat4 ComputeWorldMatrix(
			World& world,
			Entity entity,
			std::unordered_map<uint32_t, glm::mat4>& cache,
			std::unordered_set<uint32_t>& recursionStack)
		{
			const uint32_t entityId = ToEntityId(entity);
			if (const auto cached = cache.find(entityId); cached != cache.end())
				return cached->second;

			const auto* transform = world.GetComponent<TransformComponent>(entity);
			if (transform == nullptr)
				return glm::mat4(1.0f);

			const glm::mat4 localMatrix = transform->GetModelMatrix();
			const auto* hierarchy = world.GetComponent<HierarchyComponent>(entity);
			if (hierarchy == nullptr || hierarchy->parent == NullEntity)
			{
				cache[entityId] = localMatrix;
				return localMatrix;
			}

			if (!recursionStack.insert(entityId).second)
			{
				LOG_WARN("RenderSystem: hierarchy cycle detected for entity {}", entityId);
				cache[entityId] = localMatrix;
				return localMatrix;
			}

			const glm::mat4 parentWorldMatrix = ComputeWorldMatrix(world, hierarchy->parent, cache, recursionStack);
			recursionStack.erase(entityId);

			const glm::mat4 worldMatrix = parentWorldMatrix * localMatrix;
			cache[entityId] = worldMatrix;
			return worldMatrix;
		}
	}

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

		Entity activeCameraEntity = NullEntity;
		const CameraComponent* activeCamera = nullptr;

		for (auto entity : cameraView)
		{
			if (m_cameraEntity != NullEntity && entity != m_cameraEntity)
				continue;

			const auto& camera = cameraView.get<CameraComponent>(entity);
			if (m_cameraEntity == NullEntity && !camera.isPrimary)
				continue;

			activeCameraEntity = entity;
			activeCamera = &camera;
			break;
		}

		if (activeCameraEntity == NullEntity || activeCamera == nullptr)
		{
			LOG_WARN("RenderSystem: no primary camera found");
			return;
		}

		std::unordered_map<uint32_t, glm::mat4> worldMatrixCache;
		std::unordered_set<uint32_t> recursionStack;

		const glm::mat4 cameraWorldMatrix = ComputeWorldMatrix(world, activeCameraEntity, worldMatrixCache, recursionStack);
		const glm::mat4 viewMatrix = glm::inverse(cameraWorldMatrix);
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
			item.modelMatrix = ComputeWorldMatrix(world, entity, worldMatrixCache, recursionStack);
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
