// CameraControllerComponent.h

#pragma once

namespace NeneEngine::ECS {

	struct CameraControllerComponent
	{
		float moveSpeed = 4.0f;
		float lookSensitivity = 0.003f;
		float yawRadians = 0.0f;
		float pitchRadians = 0.0f;
		float maxPitchRadians = 1.5f;
		bool rotateWithRightMouse = true;
	};

} // namespace NeneEngine::ECS
