#include "Core/PathResolver.h"

#include <Windows.h>

namespace NeneEngine
{

	std::filesystem::path GetExecutableDirectory()
	{
		wchar_t modulePath[MAX_PATH]{};
		const DWORD pathLength = GetModuleFileNameW(nullptr, modulePath, MAX_PATH);
		if (pathLength == 0 || pathLength == MAX_PATH) return {};

		return std::filesystem::path(modulePath).parent_path();
	}

	std::filesystem::path ResolveFromAncestors(const std::filesystem::path& start,
	                                           const std::filesystem::path& relativePath, bool allowMissingLeaf)
	{
		std::error_code errorCode;
		auto current = start;
		std::filesystem::path resolvedPath;
		while (!current.empty())
		{
			const auto candidate = current / relativePath;
			if (std::filesystem::exists(candidate, errorCode))
				resolvedPath = candidate;
			else if (allowMissingLeaf && std::filesystem::exists(candidate.parent_path(), errorCode))
				resolvedPath = candidate;

			const auto parent = current.parent_path();
			if (parent == current) break;
			current = parent;
		}

		return resolvedPath;
	}

	std::filesystem::path ResolveFromExecutionRoots(const std::filesystem::path& relativePath, bool allowMissingLeaf)
	{
		if (const auto fromCurrentDirectory =
		        ResolveFromAncestors(std::filesystem::current_path(), relativePath, allowMissingLeaf);
		    !fromCurrentDirectory.empty())
			return fromCurrentDirectory;

		if (const auto executableDirectory = GetExecutableDirectory(); !executableDirectory.empty())
		{
			if (const auto fromExecutableDirectory =
			        ResolveFromAncestors(executableDirectory, relativePath, allowMissingLeaf);
			    !fromExecutableDirectory.empty())
				return fromExecutableDirectory;
		}

		return {};
	}

} // namespace NeneEngine
