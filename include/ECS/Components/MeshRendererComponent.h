// MeshRendererComponent.h

#pragma once

#include "Rendering/RenderTypes.h"

namespace NeneEngine::ECS {
	
	struct MeshRendererComponent
	{
		PrimitiveType primitiveType = PrimitiveType::Triangle;
		bool          visible = true;
		MeshId        meshId{};
		Material      material{};
	};

} // namespace NeneEngine::ECS
