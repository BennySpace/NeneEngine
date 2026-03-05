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
add_subdirectory ("src")
# add_subdirectory(tests)