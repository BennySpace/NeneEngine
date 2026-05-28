#pragma once

#include "ECS/World.h"

namespace NeneEngine
{
	class IRenderAdapter;

	void RunDemoBootstrap(ECS::World& world, IRenderAdapter* primaryRenderer);
} // namespace NeneEngine
