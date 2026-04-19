// Entity.h

#pragma once

#include <entt/entt.hpp>

namespace NeneEngine::ECS {

	using Entity = entt::entity;

	inline constexpr Entity NullEntity = entt::null;

} // namespace NeneEngine::ECS
