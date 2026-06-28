#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "init_utils")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/base/startup/init/services/log
)

set(SOURCES
    ${OHOS_DIR}/base/startup/init/services/utils/init_utils.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-typedef-redefinition
    -Wno-strict-prototypes
    -Wno-return-type
    -Wno-error=sign-compare
    -Wno-error=unused-but-set-variable
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    _GNU_SOURCE
    OHOS_LITE
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

