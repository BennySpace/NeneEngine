#pragma once

#include "Rendering/RenderTypes.h"

#include <filesystem>

namespace NeneEngine
{
	class IRenderAdapter;

	namespace ECS
	{
		class World;
	}

	struct ModelSpawnResult
	{
		bool hideSceneTriangle = false;
	};

	[[nodiscard]] ShaderId CreateTexturedMeshShader(IRenderAdapter& renderer, const std::filesystem::path& shaderPath);
	[[nodiscard]] ModelSpawnResult SpawnModelsFromManifest(ECS::World& world, IRenderAdapter& renderer,
	                                                       ShaderId shaderId,
	                                                       const std::filesystem::path& manifestPath);

} // namespace NeneEngine
