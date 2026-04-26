// TagComponent.h

#pragma once

#include <string>

namespace NeneEngine::ECS {

	struct TagComponent
	{
		std::string name;

		TagComponent() = default;
		explicit TagComponent(std::string_view n) : name(n) {}
	};

} // namespace NeneEngine::ECS
