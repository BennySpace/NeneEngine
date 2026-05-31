#pragma once

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace NeneEngine::ECS
{

	class EventBus
	{
	  public:
		template <typename Event, typename Handler> void Subscribe(Handler&& handler)
		{
			auto& subscribers = m_subscribers[std::type_index(typeid(Event))];
			subscribers.push_back(
			    [callback = std::forward<Handler>(handler)](const void* eventData) { callback(*static_cast<const Event*>(eventData)); });
		}

		template <typename Event> void Publish(const Event& event) const
		{
			const auto iterator = m_subscribers.find(std::type_index(typeid(Event)));
			if (iterator == m_subscribers.end()) return;

			for (const auto& subscriber : iterator->second)
			{
				subscriber(&event);
			}
		}

		void Clear() { m_subscribers.clear(); }

	  private:
		using UntypedSubscriber = std::function<void(const void*)>;

		std::unordered_map<std::type_index, std::vector<UntypedSubscriber>> m_subscribers;
	};

} // namespace NeneEngine::ECS
