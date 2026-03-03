// NeneEngineApp.cpp : Defines the entry point for the application.
//

#include "NeneEngineApp.h"
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>

struct Position { float x = 0, y = 0; };
struct Velocity { float x = 0, y = 0; };
struct Name { eastl::string value; };

int main()
{
    spdlog::info("NeneEngine + EnTT + EASTL запущен!");

    entt::registry registry;

    for (int i = 0; i < 10; ++i) {
        auto entity = registry.create();
        registry.emplace<Name>(entity, eastl::string{ "Entity_" } + eastl::to_string(i));
        registry.emplace<Position>(entity, static_cast<float>(i * 10), 0.0f);
        registry.emplace<Velocity>(entity, 1.0f, 2.0f);
    }

    registry.view<Position, Velocity>().each([](auto& pos, auto& vel) {
        pos.x += vel.x * 0.016f;
        pos.y += vel.y * 0.016f;
        });

    for (auto [entity, name, pos] : registry.view<Name, Position>().each()) {
        spdlog::info("{} находится в ({:.1f}, {:.1f})", name.value.c_str(), pos.x, pos.y);
    }

    spdlog::info("EnTT работает идеально! Готов к полноценному ECS.");
    return 0;
}