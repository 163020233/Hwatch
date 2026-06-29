#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "samgr_adapter")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/adapter
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
)

set(SOURCES
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/adapter/cmsis/memory_adapter.c
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/adapter/cmsis/queue_adapter.c
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/adapter/cmsis/thread_adapter.c
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/adapter/cmsis/time_adapter.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
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

