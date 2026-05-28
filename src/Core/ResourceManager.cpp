// ResourceManager.cpp

#include "Core/ResourceManager.h"

#include "Rendering/MeshLoader.h"
#include "Rendering/ShaderLoader.h"
#include "Rendering/TextureLoader.h"

namespace NeneEngine
{

	ResourceManager& ResourceManager::GetInstance()
	{
		static ResourceManager instance;
		return instance;
	}

	void ResourceManager::RegisterDefaultLoaders()
	{
		RegisterLoader<Mesh>([](const std::string& path) { return LoadMeshFromFile(path); });
		RegisterLoader<TextureResource>([](const std::string& path) { return LoadTextureResourceFromFile(path); });
		RegisterLoader<ShaderProgramResource>([](const std::string& path)
		                                      { return LoadShaderProgramResourceFromFile(path); });
	}

	void ResourceManager::Clear()
	{
		std::scoped_lock lock(m_mutex);
		m_resourceCaches.clear();
		NENE_LOG_INFO("ResourceManager: cleared all cached resources");
	}

} // namespace NeneEngine
