#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "device_msg")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/ui/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/ui/frameworks
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiomanager
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/frameworks
    ${ROOT_DIR}/middleware/services/gui/uikit/surface/interfaces/kits
    ${ROOT_DIR}/middleware/chips/brandy/nv/include
    ${ROOT_DIR}/middleware/chips/brandy/nv/nv_config
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativelauncher/include
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${ROOT_DIR}/middleware/utils/dfx/diag/
    ${OHOS_DIR}/third_party/cJSON
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/
    ${ROOT_DIR}/drivers/chips/3322/include/shared
    ${ROOT_DIR}/include
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/device_msg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/device_proxy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/msg_center_device.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
    -Wno-missing-field-initializers
)

set(WHOLE_LINK
    true
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    APP_PLATFORM_WATCHGT
    CONFIG_ACELITE_ENABLE
)

build_component()


