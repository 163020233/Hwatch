#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "bootstrap")

set(PUBLIC_HEADER
    ${OHOS_DIR}/base/startup/bootstrap_lite/services/source
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/base/startup/bootstrap_lite/services/source
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include/syspara
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include/token
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include/param
)

set(SOURCES
    ${OHOS_DIR}/base/startup/bootstrap_lite/services/source/bootstrap_service.c
    ${OHOS_DIR}/base/startup/bootstrap_lite/services/source/system_init.c
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
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
)
if ("CFG_NORFLASH_ONLY" IN_LIST DEFINES)
set(PRIVATE_DEFINES
    LITEWEARABLE_SUPPORT_ENABLED
)
else()
set(PRIVATE_DEFINES
    LITE_NATIVE_ABILITY_FWK_SUPPORT
    LITEWEARABLE_SUPPORT_ENABLED
)
endif()

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

