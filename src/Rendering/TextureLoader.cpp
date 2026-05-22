// TextureLoader.cpp

#include "Rendering/TextureLoader.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

namespace NeneEngine
{
	namespace
	{
		TextureFilterMode ReadFilterMode(const std::filesystem::path& metadataPath)
		{
			if (!std::filesystem::exists(metadataPath)) return TextureFilterMode::Linear;

			std::ifstream file(metadataPath);
			if (!file) return TextureFilterMode::Linear;

			const nlohmann::json metadata = nlohmann::json::parse(file);
			const std::string filter = metadata.value("filter", "linear");
			return filter == "nearest" || filter == "point" ? TextureFilterMode::Nearest : TextureFilterMode::Linear;
		}
	} // namespace

	TextureResource LoadTextureResourceFromFile(const std::string& path)
	{
		if (!std::filesystem::exists(path)) throw std::runtime_error("Texture file does not exist: " + path);

		const std::filesystem::path texturePath{path};
		// The loader keeps CPU metadata only; backend adapters perform the actual GPU upload.
		const std::filesystem::path metadataPath = texturePath.parent_path() / (texturePath.stem().string() + ".texture.json");
		return TextureResource{path, true, ReadFilterMode(metadataPath)};
	}

} // namespace NeneEngine
