#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "storage_jsapi")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/commonlibrary/utils_lite/js/builtin/filekit/include
    ${OHOS_DIR}/commonlibrary/utils_lite/js/builtin/common/include
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/base
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/async
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/ability_lite
    ${OHOS_DIR}/commonlibrary/utils_lite/include
)

set(SOURCES
    ${OHOS_DIR}/commonlibrary/utils_lite/js/builtin/filekit/src/nativeapi_fs.cpp
    ${OHOS_DIR}/commonlibrary/utils_lite/js/builtin/filekit/src/nativeapi_fs_impl.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-missing-field-initializers
    -Wno-error=sign-compare
)

set(COMPONENT_CXXCFLAGS
    -std=c++11
)

set(PUBLIC_DEFINES
    FEATURE_MODULE_STORAGE
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

