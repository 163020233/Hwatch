#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================


set(COMPONENT_NAME "ohos_at")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_ohos_cmd_register.c
    ${CMAKE_CURRENT_SOURCE_DIR}/at/at_ohos.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/at
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/at
    ${ROOT_DIR}/middleware/utils/at/at
    ${ROOT_DIR}/middleware/utils/at/at/include
    ${ROOT_DIR}/middleware/utils/at/at/src
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/sample
    ${ROOT_DIR}/ohos/base/location/location_lite/sample
    ${ROOT_DIR}/ohos/base/sensors/sensor_lite/sample
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    AT_COMMAND_OHOS
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wincompatible-function-pointer-types
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    true
)

build_component()

install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/at "*.h")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/src "*.h")
