#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "sysparam_simulator")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.5/include
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/base/startup/init/simulator/parameter/src
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include/syspara
    ${OHOS_DIR}/base/startup/init/interfaces/hals
)

set(SOURCES
    ${OHOS_DIR}/base/startup/init/simulator/parameter/src/param_impl_posix/param_impl_posix.c
    ${OHOS_DIR}/base/startup/init/simulator/parameter/src/parameter_common.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error
    -D_INC_STDIO_S
    -D_INC_STDLIB_S
    -D_INC_MEMORY_S
    -D_INC_STRING_S
    -D_INC_WCHAR_S
    -D_SECTMP=//
    -D_STDIO_S_DEFINED
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    INCREMENTAL_VERSION=""
    BUILD_TYPE=""
    BUILD_USER=""
    BUILD_TIME=""
    BUILD_HOST=""
    BUILD_ROOTHASH=""
    USE_MBEDTLS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

