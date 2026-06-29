#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2026. All rights reserved.
#===============================================================================


set(COMPONENT_NAME "camera_at")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/at/at_camera.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_camera_cmd_register.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/at
    ${ROOT_DIR}/middleware/utils/at/at
    ${ROOT_DIR}/middleware/utils/at/at/include
    ${ROOT_DIR}/middleware/utils/at/at/src
    ${ROOT_DIR}/ohos/foundation/multimedia/camera_lite/sample
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    AT_COMMAND_CAMERA
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    true
)

build_component()
