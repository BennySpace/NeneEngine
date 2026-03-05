# NeneEngine/CMakeList.txt : Top-level CMake project file, do global configuration

cmake_minimum_required (VERSION 3.12)

project (
	NeneEngine
	VERSION 0.1
	DESCRIPTION "Game engine"
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)


add_subdirectory ("external/DiligentEngine")

foreach(target_name SPIRV-Tools-opt SPIRV-Tools SPIRV-Tools-core)
    if(TARGET ${target_name})
        get_target_property(aliased_target ${target_name} ALIASED_TARGET)
        if(aliased_target)
            target_compile_options(${aliased_target} PRIVATE /wd5232)
            message(STATUS "Applied /wd5232 to aliased target: ${aliased_target}")
        else()
            target_compile_options(${target_name} PRIVATE /wd5232)
            message(STATUS "Applied /wd5232 to target: ${target_name}")
        endif()
    endif()
endforeach()

add_subdirectory ("src")
# add_subdirectory(tests)