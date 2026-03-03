# CMakeList.txt : CMake project for NeneEngine, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (NeneEngine
	"NeneEngineApp.cpp"
	"NeneEngineApp.h"
	"EASTLAllocator.cpp"
)

find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(EASTL CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)

target_link_libraries(NeneEngine PRIVATE
	spdlog::spdlog
	nlohmann_json::nlohmann_json
	EASTL
	EnTT::EnTT
)

set_property(TARGET NeneEngine PROPERTY CXX_STANDARD 23)
set_property(TARGET NeneEngine PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET NeneEngine PROPERTY CXX_EXTENSIONS OFF)

# TODO: Add tests and install targets if needed.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT NeneEngine)