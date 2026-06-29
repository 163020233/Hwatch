#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "syspara")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/base/startup/init/interfaces/hals
    ${OHOS_DIR}/commonlibrary/utils_lite/include
)

set(SOURCES
    ${OHOS_DIR}/base/startup/init/interfaces/hals/utils/sys_param/hal_sys_param.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    INCREMENTAL_VERSION="OpenHarmony\ 5.1\ Release"
    BUILD_TYPE="debug"
    BUILD_USER="jenkins"
    BUILD_TIME="BUILD_TIME"
    BUILD_HOST="linux"
    BUILD_ROOTHASH=""
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

