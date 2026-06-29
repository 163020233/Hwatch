#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "cjson_static")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/third_party/cJSON
)

set(SOURCES
    ${OHOS_DIR}/third_party/cJSON/cJSON.c
    ${OHOS_DIR}/third_party/cJSON/cJSON_Utils.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
    CJSON_NESTING_LIMIT=128
    CJSON_SUPPORT
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

