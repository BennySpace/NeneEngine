// RenderSystem.cpp

#include "ECS/Components/MeshRenderer.h"
#include "ECS/Components/Transform.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/World.h"

#include <spdlog/spdlog.h>

namespace NeneEngine::ECS {

	void RenderSystem::Update(World& /*world*/, float /*deltaTime*/)
	{
		// TODO
	}

	void RenderSystem::Render(World& world)
	{
		auto view = world.GetRegistry().view<Transform, MeshRenderer>();

		spdlog::debug("RenderSystem: starting render pass");

		for (auto entity : view)
		{
			const auto& transform = view.get<Transform>(entity);
			const auto& mesh = view.get<MeshRenderer>(entity);

			const auto* tag = world.GetRegistry().try_get<Tag>(entity);
			std::string name = tag ? tag->name : "Unnamed";

			spdlog::debug("Entity '{}' | pos: ({:.2f}, {:.2f}, {:.2f}) | color: ({:.2f}, {:.2f}, {:.2f})",
				name,
				transform.position.x, transform.position.y, transform.position.z,
				mesh.color.r, mesh.color.g, mesh.color.b);
		}
	}

} // namespace NeneEngine::ECS
