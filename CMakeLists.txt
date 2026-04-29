# NeneEngine/CMakeList.txt : Top-level CMake project file, do global configuration

cmake_minimum_required(VERSION 3.19)

project(
    NeneEngine
    VERSION 0.2
    DESCRIPTION "Game engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

# Diligent Engine configs
set(DILIGENT_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(DILIGENT_BUILD_TOOLS   OFF CACHE BOOL "" FORCE)
set(DILIGENT_BUILD_FX      OFF CACHE BOOL "" FORCE)
set(DILIGENT_BUILD_TESTS   OFF CACHE BOOL "" FORCE)

set(DILIGENT_NO_DIRECT3D11 ON  CACHE BOOL "" FORCE)
set(DILIGENT_NO_VULKAN     ON  CACHE BOOL "" FORCE)
set(DILIGENT_NO_OPENGL     ON  CACHE BOOL "" FORCE)
set(DILIGENT_NO_GLES       ON  CACHE BOOL "" FORCE)
set(DILIGENT_NO_METAL      ON  CACHE BOOL "" FORCE)
set(DILIGENT_NO_DIRECT3D12 OFF CACHE BOOL "" FORCE)

add_subdirectory("external/DiligentEngine/DiligentCore")

foreach(target_name SPIRV-Tools-opt SPIRV-Tools SPIRV-Tools-core)
    if(TARGET ${target_name})
        get_target_property(aliased_target ${target_name} ALIASED_TARGET)
        if(aliased_target)
            target_compile_options(${aliased_target} PRIVATE /wd5232 /wd4717)
            set_target_properties(${aliased_target} PROPERTIES COMPILE_WARNING_AS_ERROR OFF)
            message(STATUS "[Diligent Fix] Applied to aliased target: ${aliased_target}")
        else()
            target_compile_options(${target_name} PRIVATE /wd5232 /wd4717)
            set_target_properties(${target_name} PROPERTIES COMPILE_WARNING_AS_ERROR OFF)
            message(STATUS "[Diligent Fix] Applied to target: ${target_name}")
        endif()
    endif()
endforeach()

add_subdirectory("src")
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT NeneEngine)
