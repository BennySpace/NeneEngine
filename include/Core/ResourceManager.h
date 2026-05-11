// ResourceManager.h

#pragma once

#include "Core/CustomLogger.h"
#include "Core/Resource.h"

#include <any>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace NeneEngine
{

	class ResourceManager final
	{
	  public:
		template <typename T> using ResourcePtr = std::shared_ptr<Resource<T>>;

		template <typename T> using LoaderFn = std::function<T(const std::string&)>;

		static ResourceManager& GetInstance();

		template <typename T> void RegisterLoader(LoaderFn<T> loader)
		{
			std::scoped_lock lock(m_mutex);
			m_loaders[std::type_index(typeid(T))] = std::move(loader);
			NENE_LOG_INFO("ResourceManager: registered loader for type '{}'", typeid(T).name());
		}

		template <typename T> ResourcePtr<T> Load(const std::string& path)
		{
			std::scoped_lock lock(m_mutex);

			auto& cache = GetOrCreateCache<T>();
			if (const auto cached = cache.find(path); cached != cache.end())
			{
				NENE_LOG_DEBUG("ResourceManager: cache hit for '{}' ({})", path, typeid(T).name());
				return cached->second;
			}

			LoaderFn<T>* loader = GetLoader<T>();
			if (loader == nullptr)
			{
				NENE_LOG_ERROR("ResourceManager: no loader registered for '{}' ({})", path, typeid(T).name());
				return nullptr;
			}

			try
			{
				auto resource = std::make_shared<Resource<T>>(path, (*loader)(path));
				cache.emplace(path, resource);
				NENE_LOG_INFO("ResourceManager: loaded '{}' ({})", path, typeid(T).name());
				return resource;
			}
			catch (const std::exception& exception)
			{
				NENE_LOG_ERROR("ResourceManager: failed to load '{}' ({}): {}", path, typeid(T).name(),
				               exception.what());
			}
			catch (...)
			{
				NENE_LOG_ERROR("ResourceManager: failed to load '{}' ({}) with unknown error", path, typeid(T).name());
			}

			return nullptr;
		}

		void RegisterDefaultLoaders();
		void Clear();

	  private:
		ResourceManager() = default;

		template <typename T> using CacheMap = std::unordered_map<std::string, ResourcePtr<T>>;

		template <typename T> CacheMap<T>& GetOrCreateCache()
		{
			const std::type_index type = std::type_index(typeid(T));
			auto cacheIt = m_resourceCaches.find(type);
			if (cacheIt == m_resourceCaches.end()) cacheIt = m_resourceCaches.emplace(type, CacheMap<T>{}).first;

			return *std::any_cast<CacheMap<T>>(&cacheIt->second);
		}

		template <typename T> LoaderFn<T>* GetLoader()
		{
			const auto loaderIt = m_loaders.find(std::type_index(typeid(T)));
			if (loaderIt == m_loaders.end()) return nullptr;

			return std::any_cast<LoaderFn<T>>(&loaderIt->second);
		}

		std::mutex m_mutex;
		std::unordered_map<std::type_index, std::any> m_loaders;
		std::unordered_map<std::type_index, std::any> m_resourceCaches;
	};

} // namespace NeneEngine
