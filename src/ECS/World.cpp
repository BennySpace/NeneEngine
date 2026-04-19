// World.cpp

#include "ECS/World.h"
#include "ECS/Components/Tag.h"

#include <spdlog/spdlog.h>

namespace NeneEngine::ECS {

	World::World() = default;

	Entity World::CreateEntity(const std::string& name)
	{
		Entity entity = m_registry.create();

		if (!name.empty())
			m_registry.emplace<Tag>(entity, name);

		return entity;
	}

	void World::DestroyEntity(Entity entity)
	{
		if (m_registry.valid(entity))
			m_registry.destroy(entity);
	}

	// ===== Templates =====
	template<typename Component, typename... Args>
	Component& World::AddComponent(Entity entity, Args&&... args)
	{
		return m_registry.emplace<Component>(entity, std::forward<Args>(args)...);
	}

	template<typename Component>
	Component* World::GetComponent(Entity entity)
	{
		return m_registry.try_get<Component>(entity);
	}

	template<typename Component>
	bool World::HasComponent(Entity entity) const
	{
		return m_registry.any_of<Component>(entity);
	}

	template<typename Component>
	void World::RemoveComponent(Entity entity)
	{
		m_registry.remove<Component>(entity);
	}

	void World::AddSystem(std::unique_ptr<ISystem> system)
	{
		m_systems.push_back(std::move(system));
	}

	void World::Update(float deltaTime)
	{
		for (auto& system : m_systems)
			system->Update(*this, deltaTime);
	}

	void World::Render()
	{
		for (auto& system : m_systems)
			system->Render(*this);
	}

} // namespace NeneEngine::ECS
