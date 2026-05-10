// ExternalLibrarySmokeTest.cpp

#include "Core/ExternalLibrarySmokeTest.h"

#include "Core/CustomLogger.h"
#include "Rendering/MeshLoader.h"

#include <Windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <filesystem>
#include <string>

namespace NeneEngine
{
	namespace
	{
		std::filesystem::path ResolveFrom(const std::filesystem::path& start, const std::filesystem::path& relativePath)
		{
			std::error_code errorCode;
			auto current = start;
			while (!current.empty())
			{
				const auto candidate = current / relativePath;
				if (std::filesystem::exists(candidate, errorCode))
					return candidate;

				const auto parent = current.parent_path();
				if (parent == current)
					break;
				current = parent;
			}

			return {};
		}

		std::filesystem::path ResolveAssetPath(const std::filesystem::path& relativePath)
		{
			if (const auto fromCurrentDirectory = ResolveFrom(std::filesystem::current_path(), relativePath); !fromCurrentDirectory.empty())
				return fromCurrentDirectory;

			wchar_t modulePath[MAX_PATH]{};
			const DWORD length = GetModuleFileNameW(nullptr, modulePath, static_cast<DWORD>(std::size(modulePath)));
			if (length > 0)
			{
				const std::filesystem::path executableDirectory = std::filesystem::path(modulePath).parent_path();
				if (const auto fromExecutableDirectory = ResolveFrom(executableDirectory, relativePath); !fromExecutableDirectory.empty())
					return fromExecutableDirectory;
			}

			return {};
		}

		void RunAssimpSmokeTest()
		{
			const auto modelPath = ResolveAssetPath(std::filesystem::path{ "assets" } / "models" / "test_triangle.obj");
			if (modelPath.empty())
			{
				NENE_LOG_ERROR("Assimp smoke test: model file was not found");
				return;
			}

			try
			{
				const MeshData meshData = LoadMeshDataFromFile(modelPath.string());
				NENE_LOG_INFO(
					"Assimp smoke test: loaded '{}' | vertices={} indices={}",
					modelPath.string(),
					meshData.vertices.size(),
					meshData.indices.size());
			}
			catch (const std::exception& exception)
			{
				NENE_LOG_ERROR("Assimp smoke test failed for '{}': {}", modelPath.string(), exception.what());
			}
		}

		void RunStbImageSmokeTest()
		{
			const auto texturePath = ResolveAssetPath(std::filesystem::path{ "assets" } / "readme" / "banner.jpg");
			if (texturePath.empty())
			{
				NENE_LOG_ERROR("stb_image smoke test: texture file was not found");
				return;
			}

			int width = 0;
			int height = 0;
			int channels = 0;
			stbi_uc* pixels = stbi_load(texturePath.string().c_str(), &width, &height, &channels, 0);
			if (pixels == nullptr)
			{
				NENE_LOG_ERROR("stb_image smoke test failed for '{}': {}", texturePath.string(), stbi_failure_reason());
				return;
			}

			NENE_LOG_INFO(
				"stb_image smoke test: loaded '{}' | width={} height={} channels={}",
				texturePath.string(),
				width,
				height,
				channels);

			stbi_image_free(pixels);
		}
	}

	void RunExternalLibrarySmokeTests()
	{
		NENE_LOG_INFO("External library smoke tests: started");
		RunAssimpSmokeTest();
		RunStbImageSmokeTest();
		NENE_LOG_INFO("External library smoke tests: finished");
	}

} // namespace NeneEngine
