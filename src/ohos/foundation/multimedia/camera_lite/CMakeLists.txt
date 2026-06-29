#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2026. All rights reserved.
#===============================================================================
enable_language(CXX)
set(COMPONENT_NAME "camera_lite")

set(CAMERA_LITE_DIR ${ROOT_DIR}/ohos/foundation/multimedia/camera_lite)

set(CAMERA_LITE_SOURCES
)

set(SOURCES
    ${CAMERA_LITE_DIR}/frameworks/camera_kit.cpp
    ${CAMERA_LITE_DIR}/frameworks/camera_ability.cpp
    ${CAMERA_LITE_DIR}/frameworks/camera_info_impl.cpp
    ${CAMERA_LITE_DIR}/frameworks/camera_manager.cpp
    ${CAMERA_LITE_DIR}/frameworks/camera_client.cpp
    ${CAMERA_LITE_DIR}/frameworks/event_handler.cpp
    ${CAMERA_LITE_DIR}/frameworks/camera_impl.cpp
    ${CAMERA_LITE_DIR}/frameworks/frame_config.cpp

    ${CAMERA_LITE_DIR}/frameworks/camera_config.cpp
    ${CAMERA_LITE_DIR}/frameworks/camera_ability_impl.cpp
    ${CAMERA_LITE_DIR}/frameworks/passthrough/src/camera_service_client.cpp
    ${CAMERA_LITE_DIR}/frameworks/passthrough/src/camera_device_client.cpp

    ${CAMERA_LITE_DIR}/services/impl/src/camera_service.cpp
    ${CAMERA_LITE_DIR}/services/impl/src/camera_device.cpp
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

if ("ENABLE_UIKIT" IN_LIST DEFINES)
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
endif()

set(PRIVATE_DEFINES
    ENABLE_PASSTHROUGH_MODE
)

set(PUBLIC_DEFINES
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

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/ohos/camera/camera_lite)

build_component()

add_subdirectory_with_alias_if_exist(sample ${PROJECT_BINARY_DIR}/ohos/camera/camera_lite/sample)
