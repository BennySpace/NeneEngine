// CameraControllerSystem.h

#pragma once

#include "ECS/Entity.h"
#include "ECS/Systems/ISystem.h"

namespace NeneEngine {
	class InputDevice;
}

namespace NeneEngine::ECS {

	class CameraControllerSystem : public ISystem
	{
	public:
		CameraControllerSystem(const InputDevice& input, Entity controlledCamera = NullEntity) :
			m_input(input),
			m_controlledCamera(controlledCamera)
		{}

		void Update(World& world, float deltaTime) override;

	private:
		const InputDevice& m_input;
		Entity m_controlledCamera = NullEntity;
	};

} // namespace NeneEngine::ECS
