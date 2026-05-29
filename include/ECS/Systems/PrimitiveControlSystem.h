// PrimitiveControlSystem.h

#pragma once

#include "ECS/Systems/ISystem.h"

namespace NeneEngine
{
	class IInputHandler;
}

namespace NeneEngine::ECS
{

	class PrimitiveControlSystem final : public ISystem
	{
	  public:
		explicit PrimitiveControlSystem(const IInputHandler& input) : m_input(input) {}

		void Update(World& world, float deltaTime) override;

	  private:
		const IInputHandler& m_input;
	};

} // namespace NeneEngine::ECS
