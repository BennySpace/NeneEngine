// World.h

#pragma once

#include "Components/CameraComponent.h"
#include "Components/MeshRenderer.h"
#include "Components/Tag.h"
#include "Components/Transform.h"
#include "Entity.h"
#include "Systems/ISystem.h"

#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace NeneEngine::ECS {

	class World {
	public:
		World();
		~World() = default;

		// ===== Entity =====
		Entity CreateEntity(const std::string& name = "Entity");
		void DestroyEntity(Entity entity);

		// ===== Components =====
		template<typename Component, typename... Args>
		Component& AddComponent(Entity entity, Args&&... args)
		{
			return m_registry.emplace<Component>(entity, std::forward<Args>(args)...);
		}

		template<typename Component>
		Component* GetComponent(Entity entity)
		{
			return m_registry.try_get<Component>(entity);
		}

		template<typename Component>
		bool HasComponent(Entity entity) const
		{
			return m_registry.any_of<Component>(entity);
		}

		template<typename Component>
		void RemoveComponent(Entity entity)
		{
			m_registry.remove<Component>(entity);
		}

		// ===== Systems =====
		void AddSystem(std::unique_ptr<ISystem> system);

		// ===== Update / Render =====
		void Update(float deltaTime);
		void Render();

		entt::registry& GetRegistry() { return m_registry; }
		const entt::registry& GetRegistry() const { return m_registry; }

	private:
		entt::registry m_registry;
		std::vector<std::unique_ptr<ISystem>> m_systems;
	};

} // namespace NeneEngine::ECS
