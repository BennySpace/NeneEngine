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
find_package(assimp         CONFIG REQUIRED)
find_package(nlohmann_json  CONFIG REQUIRED)
find_package(EASTL          CONFIG REQUIRED)
find_package(EnTT           CONFIG REQUIRED)
find_package(glm            CONFIG REQUIRED)
find_package(Stb            MODULE REQUIRED)

target_include_directories(NeneEngine PRIVATE ${Stb_INCLUDE_DIR})

target_link_libraries(NeneEngine
    PRIVATE
        assimp::assimp
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        EASTL
        EnTT::EnTT
        glm::glm
        Diligent-Win32Platform
        Diligent-GraphicsEngineD3D12-shared
        Diligent-TextureLoader
)

if(EXISTS "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/debug/bin")
    file(GLOB NENEENGINE_VCPKG_DEBUG_DLLS "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/debug/bin/*.dll")
    if(NENEENGINE_VCPKG_DEBUG_DLLS)
        add_custom_command(TARGET NeneEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${NENEENGINE_VCPKG_DEBUG_DLLS}
                $<TARGET_FILE_DIR:NeneEngine>
            COMMAND_EXPAND_LISTS
        )
    endif()
endif()

if(COMMAND copy_required_dlls)
    copy_required_dlls(NeneEngine)
else()
    message(WARNING "copy_required_dlls was not found; Diligent may not be configured correctly")
endif()

target_compile_features(NeneEngine PRIVATE cxx_std_20)
