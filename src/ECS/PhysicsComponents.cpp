// PhysicsComponents.cpp

#include "ECS/Components/ColliderComponent.h"
#include "ECS/Components/RigidbodyComponent.h"

namespace NeneEngine::ECS
{

	namespace
	{
		static_assert(sizeof(RigidbodyComponent) > 0);
		static_assert(sizeof(ColliderComponent) > 0);
	} // namespace

} // namespace NeneEngine::ECS
