#===============================================================================
# @brief    Native UI page component — out-of-tree project template
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2026. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "samplepage")

set(NativeApp    ${ROOT_DIR}/application/wearable/nativeapp)
set(NativeAblFwk ${ROOT_DIR}/application/wearable/nativeabilityfwk)
set(UIKit        ${ROOT_DIR}/middleware/services/gui/uikit)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SamplePage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SamplePresenter.cpp
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_HEADER
    ${NativeApp}/nativelauncher/include
    ${NativeApp}/nativeui/include
    ${NativeAblFwk}/include
    ${UIKit}/proprietary/include/components
    ${UIKit}/ui/interfaces/kits
    ${UIKit}/ui/interfaces/kits/components
    ${UIKit}/ui/interfaces/kits/events
    ${UIKit}/utils/interfaces/kits
    ${UIKit}/utils/interfaces/kits/gfx_utils
    ${ROOT_DIR}/application/wearable/ohos_deps
)

set(PRIVATE_DEFINES)
set(PUBLIC_DEFINES)
set(COMPONENT_CCFLAGS
    -std=c++11
    -Wno-error
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
    -nostdinc
    -nostdlib
    -Wno-c++11-narrowing
    -Wno-error=unused-function
    -Wno-missing-field-initializers
    -Werror=return-type
)
set(WHOLE_LINK true)
set(BUILD_AS_OBJ false)
set(MAIN_COMPONENT true)
set(APP_TARGETS "diting-community")
build_component()
