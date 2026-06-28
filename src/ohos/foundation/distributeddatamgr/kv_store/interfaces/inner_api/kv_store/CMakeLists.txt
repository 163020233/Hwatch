#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "utils_kv_store")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/foundation/distributeddatamgr/kv_store/frameworks/native/kv_store/src/kvstore_common
    ${OHOS_DIR}/foundation/distributeddatamgr/kv_store/interfaces/inner_api/kv_store/include
)

set(SOURCES
    ${OHOS_DIR}/foundation/distributeddatamgr/kv_store/frameworks/native/kv_store/src/kvstore_common/kvstore_common.c
    ${OHOS_DIR}/foundation/distributeddatamgr/kv_store/frameworks/native/kv_store/src/kvstore_impl_posix/kv_store.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-typedef-redefinition
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

