target_sources(NeneEngine PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/Core/CustomLogger.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/Core/Delegate.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/Core/GameTimer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/CustomLogger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/EASTLAllocator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/GameTimer.cpp"
)
