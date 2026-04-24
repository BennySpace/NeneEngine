// CameraControllerSystem.h

#pragma once

#include "ECS/Systems/ISystem.h"

namespace NeneEngine {
	class InputDevice;
}

namespace NeneEngine::ECS {

	class CameraControllerSystem : public ISystem
	{
	public:
		CameraControllerSystem(const InputDevice& input) : m_input(input) {}

		void Update(World& world, float deltaTime) override;

	private:
		const InputDevice& m_input;
	};

} // namespace NeneEngine::ECS
