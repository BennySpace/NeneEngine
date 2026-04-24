// CameraComponent.h

#pragma once

namespace NeneEngine::ECS {

	struct CameraComponent
	{
		float fovDegrees = 60.0f;
		float nearPlane = 0.1f;
		float farPlane = 100.0f;
		float aspectRatio = 16.0f / 9.0f;
		bool  isPrimary = true;
	};

} // namespace NeneEngine::ECS
