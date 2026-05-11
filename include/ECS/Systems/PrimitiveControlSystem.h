// PrimitiveControlSystem.h

#pragma once

#include "ECS/Systems/ISystem.h"

namespace NeneEngine
{
	class InputDevice;
}

namespace NeneEngine::ECS
{

	class PrimitiveControlSystem final : public ISystem
	{
	  public:
		explicit PrimitiveControlSystem(InputDevice& input) : m_input(input) {}

		void Update(World& world, float deltaTime) override;

	  private:
		InputDevice& m_input;
	};

} // namespace NeneEngine::ECS
