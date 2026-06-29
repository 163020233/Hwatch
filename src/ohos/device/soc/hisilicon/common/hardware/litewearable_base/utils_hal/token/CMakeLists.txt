#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_token_static")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/base/startup/init/interfaces/hals
    ${OHOS_DIR}/commonlibrary/utils_lite/include
)

set(SOURCES
    ${OHOS_DIR}/base/startup/init/interfaces/hals/utils/token/hal_token.c
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
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
install_sdk(${CMAKE_CURRENT_SOURCE_DIR} "*")
