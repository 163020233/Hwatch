#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "nativeabilityfwk")
set(MODULE_NAME "app")
set(AUTO_DEF_FILE_ID TRUE)
add_compile_options(-D_FORTIFY_SOURCE=2 -O2)
add_compile_options(-ftrapv)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transition
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/application/wearable/nativeapp/nativelauncher/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/include
    ${OHOS_DIR}/foundation/ace/ace_engine_lite/interfaces/base
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/ui/frameworks/
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits/gfx_utils
    ${ROOT_DIR}/middleware/services/gui/uikit/surface/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/frameworks
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/font
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/ui/innerkits
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Ability.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AbilitySlice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NativeAbility.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StartNative.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RotationManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transition/ZoomOutTransition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transition/ZoomTransition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transition/ThreeDFoldTransition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transition/FadeInOutTransition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transition/HexagonsTransition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transition/BackToHexagonsTransition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transition/BackToWaterfallTransition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transition/WaterfallTransition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transition/EnterWaterfallTransition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transition/TransparentGradientTransition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transition/GaussianBlurTransition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transition/CrossShapedTransition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transition/BackToCrossShapedTransition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transition/CoverRight.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transition/SlideOutRight.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

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
)


set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    APP_PLATFORM_WATCHGT
)

build_component()

