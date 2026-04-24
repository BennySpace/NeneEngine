// MeshRenderer.h

#pragma once

#include "Rendering/RenderTypes.h"

namespace NeneEngine::ECS {
	
	struct MeshRenderer
	{
		PrimitiveType primitiveType = PrimitiveType::Triangle;
		bool          visible = true;
		MeshId        meshId{};
		Material      material{};
	};

} // namespace NeneEngine::ECS
