#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "resmgr")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/base/global/resource_management_lite/frameworks/resmgr_lite/include
    ${OHOS_DIR}/base/global/resource_management_lite/interfaces/inner_api/include
)

set(SOURCES
    ${OHOS_DIR}/base/global/resource_management_lite/frameworks/resmgr_lite/src/global.c
    ${OHOS_DIR}/base/global/resource_management_lite/frameworks/resmgr_lite/src/global_utils.c
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

