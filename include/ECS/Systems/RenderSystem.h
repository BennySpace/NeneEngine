// RenderSystem.h

#pragma once

#include "ECS/Systems/ISystem.h"
#include "RenderAdapters/IRenderAdapter.h"

namespace NeneEngine::ECS {

	class RenderSystem final : public ISystem
	{
	public:
		explicit RenderSystem(IRenderAdapter* adapter) : m_renderer(adapter) {}

		void Update(World& world, float deltaTime) override;

		void Render(World& world) override;

	private:
		IRenderAdapter* m_renderer;
	};

} // namespace NeneEngine::ECS
