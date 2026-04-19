# NeneEngine/src/CMakeList.txt : CMake project for NeneEngine, include source and define project specific logic here.

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

list(FILTER SOURCES EXCLUDE REGEX 
    "(.*_test\\.cpp$|.*Test\\.cpp$|.*\\.t\\.cpp$|.*main_test\\.cpp$)"
)

add_executable(NeneEngine WIN32 ${SOURCES} "../WinMain.cpp")

target_include_directories(NeneEngine 
	PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../external>
)

find_package(spdlog			CONFIG REQUIRED)
find_package(nlohmann_json	CONFIG REQUIRED)
find_package(EASTL			CONFIG REQUIRED)
find_package(EnTT			CONFIG REQUIRED)
find_package(glm			CONFIG REQUIRED)

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
    message(WARNING "copy_required_dlls не найдена — возможно, Diligent не подгрузился")
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT NeneEngine)

target_compile_features(NeneEngine PUBLIC cxx_std_20)