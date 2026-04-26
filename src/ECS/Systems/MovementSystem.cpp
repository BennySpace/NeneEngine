// MovementSystem.cpp

#include "ECS/Components/MovementComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Systems/MovementSystem.h"
#include "ECS/World.h"

#include <cmath>

namespace NeneEngine::ECS {

	void MovementSystem::Update(World& world, float deltaTime)
	{
		auto view = world.GetRegistry().view<TransformComponent, MovementComponent>();

		for (auto entity : view)
		{
			auto& transform = view.get<TransformComponent>(entity);
			auto& movement = view.get<MovementComponent>(entity);

			if (movement.useOscillation && movement.oscillationAmplitude > 0.0f)
			{
				const float axisLengthSquared = glm::dot(movement.oscillationAxis, movement.oscillationAxis);
				if (axisLengthSquared > 0.0001f)
				{
					movement.elapsedTime += deltaTime;
					const glm::vec3 axis = movement.oscillationAxis / std::sqrt(axisLengthSquared);
					const float offset = std::sin(movement.elapsedTime * movement.oscillationSpeed) * movement.oscillationAmplitude;
					transform.position = movement.origin + axis * offset;
					continue;
				}
			}

			transform.position += movement.velocity * deltaTime;
		}
	}

} // namespace NeneEngine::ECS
