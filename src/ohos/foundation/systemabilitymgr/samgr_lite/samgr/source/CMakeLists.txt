#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "samgr_source")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/adapter
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/registry
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/registry
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
)

set(SOURCES
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/source/common.c
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/source/feature.c
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/source/iunknown.c
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/source/message.c
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/source/service.c
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/source/task_manager.c
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
    -Wno-int-conversion
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

