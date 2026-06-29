#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2026. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "camera_sample")
set(CAMERA_LITE_DIR ${ROOT_DIR}/ohos/foundation/multimedia/camera_lite)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/camera_sample.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/camera_test.cpp
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/ohos/foundation/multimedia/camera_lite/services/impl/include
    ${ROOT_DIR}/ohos/foundation/multimedia/camera_lite/services/server/include
    ${ROOT_DIR}/ohos/foundation/communication/ipc/interfaces/innerkits/c/ipc/include
    ${ROOT_DIR}/ohos/foundation/multimedia/camera_lite/frameworks/passthrough/include
    ${ROOT_DIR}/ohos/foundation/multimedia/camera_lite/services/include
    ${ROOT_DIR}/ohos/foundation/multimedia/camera_lite/interfaces/kits
    ${ROOT_DIR}/ohos/foundation/multimedia/camera_lite/frameworks
    ${ROOT_DIR}/ohos/foundation/multimedia/media_utils_lite/hals
    ${ROOT_DIR}/ohos/foundation/systemabilitymgr/samgr_lite/interfaces/kits/registry
    ${ROOT_DIR}/ohos/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${ROOT_DIR}/ohos/base/security/permission_lite/interfaces/kits
    ${ROOT_DIR}/ohos/base/global/resource_management_lite/frameworks/resmgr_lite/include/utils
    ${ROOT_DIR}/ohos/commonlibrary/utils_lite/include
    ${ROOT_DIR}/ohos/drivers/peripheral/codec/interfaces/include
)

# uikit
set(UIKIT_DIR ${ROOT_DIR}/middleware/services/gui/uikit)
list(APPEND PRIVATE_HEADER
    ${UIKIT_DIR}/ui/interfaces/innerkits
    ${UIKIT_DIR}/utils/interfaces/kits
    ${UIKIT_DIR}/utils/interfaces/innerkits
    ${UIKIT_DIR}/surface/interfaces/kits
    ${UIKIT_DIR}/ui/interfaces/kits
    ${UIKIT_DIR}/proprietary/src/utils/innerkits
    ${UIKIT_DIR}/proprietary/include
    ${UIKIT_DIR}/proprietary/include/common/
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_AT_CAMERA
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
    -Wno-missing-field-initializers
    -Wno-writable-strings
    -Wno-reorder-ctor
    -Wno-tautological-pointer-compare
    -Wno-unused-function
    -Werror=return-type
    -Wno-c++11-narrowing
    -Wno-error=unused-function
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(AUTO_ADD_PATH
    false
)

build_component()
