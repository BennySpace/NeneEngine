target_sources(NeneEngine PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/States/IGameState.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/States/MenuState.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/States/PauseState.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/States/PlayState.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/MenuState.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PauseState.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/PlayState.cpp"
)
