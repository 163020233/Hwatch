#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "samgr_broadcast")

set(PUBLIC_HEADER
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/communication/broadcast/source
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/communication/broadcast
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/adapter
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
)

set(SOURCES
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/communication/broadcast/source/broadcast_service.c
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/communication/broadcast/source/pub_sub_feature.c
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/communication/broadcast/source/pub_sub_implement.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-missing-field-initializers
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
    -Wno-error=unused-function
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
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

