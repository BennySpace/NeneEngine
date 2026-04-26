// CameraControllerSystem.cpp

#include "ECS/Components/CameraComponent.h"
#include "ECS/Components/CameraControllerComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Systems/CameraControllerSystem.h"
#include "ECS/World.h"
#include "Input/InputDevice.h"

#include <algorithm>
#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>

namespace NeneEngine::ECS {

	void CameraControllerSystem::Update(World& world, float deltaTime)
	{
		auto view = world.GetRegistry().view<TransformComponent, const CameraComponent, CameraControllerComponent>();

		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (!camera.isPrimary)
				continue;

			auto& transform = view.get<TransformComponent>(entity);
			auto& controller = view.get<CameraControllerComponent>(entity);

			const bool canRotate = !controller.rotateWithRightMouse || m_input.IsKeyDown(KeyCode::MouseRight);
			const glm::vec2 mouseDelta = m_input.GetMouseDelta();

			if (canRotate && glm::dot(mouseDelta, mouseDelta) > 0.0f)
			{
				controller.yawRadians -= mouseDelta.x * controller.lookSensitivity;
				controller.pitchRadians -= mouseDelta.y * controller.lookSensitivity;
				controller.pitchRadians = std::clamp(
					controller.pitchRadians,
					-controller.maxPitchRadians,
					controller.maxPitchRadians);

				transform.rotation = glm::normalize(
					glm::angleAxis(controller.yawRadians, glm::vec3{ 0.0f, 1.0f, 0.0f })
					* glm::angleAxis(controller.pitchRadians, glm::vec3{ 1.0f, 0.0f, 0.0f }));
			}

			glm::vec3 direction{ 0.0f };

			if (m_input.IsKeyDown(KeyCode::W))
				direction.z -= 1.0f;
			if (m_input.IsKeyDown(KeyCode::S))
				direction.z += 1.0f;
			if (m_input.IsKeyDown(KeyCode::A))
				direction.x -= 1.0f;
			if (m_input.IsKeyDown(KeyCode::D))
				direction.x += 1.0f;
			if (m_input.IsKeyDown(KeyCode::Space))
				direction.y += 1.0f;
			if (m_input.IsKeyDown(KeyCode::LeftControl) || m_input.IsKeyDown(KeyCode::RightControl))
				direction.y -= 1.0f;

			if (glm::dot(direction, direction) > 0.0f)
			{
				const glm::vec3 worldDirection = transform.rotation * glm::normalize(direction);
				transform.position += worldDirection * controller.moveSpeed * deltaTime;
			}

			return;
		}
	}

} // namespace NeneEngine::ECS
