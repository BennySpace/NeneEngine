target_sources(NeneEngine PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/Input/IInputHandler.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/Input/InputActions.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/Input/InputDevice.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/Input/InputManager.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/Input/KeyCode.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/InputDevice.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/InputManager.cpp"
)
