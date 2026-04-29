# NeneEngine/src/CMakeList.txt : CMake project for NeneEngine, include source and define project specific logic here.

add_executable(NeneEngine WIN32
    "${CMAKE_CURRENT_SOURCE_DIR}/../WinMain.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../NeneEngine.rc"
)

target_sources(NeneEngine PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/Resource.h"
)

add_subdirectory(App)
add_subdirectory(Core)
add_subdirectory(ECS)
add_subdirectory(Input)
add_subdirectory(Platform)
add_subdirectory(RenderAdapters)
add_subdirectory(Rendering)
add_subdirectory(Scene)
add_subdirectory(States)

target_include_directories(NeneEngine
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../external>
)

find_package(spdlog         CONFIG REQUIRED)
find_package(nlohmann_json  CONFIG REQUIRED)
find_package(EASTL          CONFIG REQUIRED)
find_package(EnTT           CONFIG REQUIRED)
find_package(glm            CONFIG REQUIRED)

target_link_libraries(NeneEngine
    PRIVATE
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        EASTL
        EnTT::EnTT
        glm::glm
        glm::glm-header-only
        Diligent-GraphicsEngineD3D12-shared
)

if(COMMAND copy_required_dlls)
    copy_required_dlls(NeneEngine)
else()
    message(WARNING "copy_required_dlls was not found; Diligent may not be configured correctly")
endif()

target_compile_features(NeneEngine PRIVATE cxx_std_20)
