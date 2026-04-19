// Tag.h

#pragma once

#include <string>

namespace NeneEngine::ECS {

	struct Tag
	{
		std::string name;

		Tag() = default;
		explicit Tag(std::string_view n) : name(n) {}
	};

} // namespace NeneEngine::ECS
