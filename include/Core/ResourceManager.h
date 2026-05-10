// ResourceManager.h

#pragma once

#include "Core/CustomLogger.h"
#include "Core/Resource.h"

#include <functional>
#include <memory>
#include <mutex>
#include <string>

namespace NeneEngine
{

	class ResourceManager final
	{
	public:
		template<typename T>
		using ResourcePtr = std::shared_ptr<Resource<T>>;

		template<typename T>
		using LoaderFn = std::function<T(const std::string&)>;

		static ResourceManager& GetInstance();

		void Clear();

	private:
		ResourceManager() = default;

		std::mutex m_mutex;
	};

} // namespace NeneEngine
