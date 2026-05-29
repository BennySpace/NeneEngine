// KeyCodeStrings.h

#pragma once

#include "Input/KeyCode.h"

#include <optional>
#include <string_view>

namespace NeneEngine
{

	[[nodiscard]] std::string_view ToString(KeyCode keyCode);
	[[nodiscard]] std::optional<KeyCode> TryParseKeyCode(std::string_view value);

} // namespace NeneEngine
