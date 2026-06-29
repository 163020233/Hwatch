#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "graphic_utils")

set(UIKIT_ROOT_DIR ${ROOT_DIR}/middleware/services/gui/uikit)
set(HAL_ROOT_DIR ${ROOT_DIR}/middleware/services/gui/hal)
set(DFX_LOG_NATIVE ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native)

set(PUBLIC_HEADER
    ${UIKIT_ROOT_DIR}/proprietary/include
    ${UIKIT_ROOT_DIR}/utils/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/utils/interfaces/kits
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${UIKIT_ROOT_DIR}/ui/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/ui/interfaces/kits
    ${UIKIT_ROOT_DIR}/ui/frameworks
    ${UIKIT_ROOT_DIR}/surface/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/surface/interfaces/kits
    ${UIKIT_ROOT_DIR}/proprietary/include/service
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits/hals
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/innerkits
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/frameworks
    ${HAL_ROOT_DIR}/input/include
    ${HAL_ROOT_DIR}/common/include
    ${DFX_LOG_NATIVE}/innerkits/hilog
    ${DFX_LOG_NATIVE}/innerkits
)

set(SOURCES
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/frameworks/color.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/frameworks/geometry2d.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/frameworks/graphic_math.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/frameworks/graphic_performance.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/frameworks/graphic_timer.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/frameworks/hal_cpu.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/frameworks/hal_tick.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/frameworks/mem_api.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/frameworks/pixel_format_utils.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/frameworks/style.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/frameworks/trans_affine.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/frameworks/transform.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/frameworks/version.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/frameworks/diagram/common/paint.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/frameworks/hals/display_dev.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/frameworks/hals/gralloc_engines.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/frameworks/hals/lite_input_distributer.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/frameworks/hals/lite_input_hal.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/frameworks/mem_check.cpp
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/frameworks/mem_custom.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
    -Wno-writable-strings
    -Wno-tautological-pointer-compare
    -Wno-unused-function
    -Werror=return-type
    -Wno-missing-field-initializers
    -Wno-macro-redefined
    -Wno-unused-parameter
    -Wno-float-to-int-overflow
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
    __LITEOS_M__
    ENABLE_OHOS_GRAPHIC_UTILS_PRODUCT_CONFIG=1
)

set(PRIVATE_DEFINES
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/" "CMakeLists.txt")