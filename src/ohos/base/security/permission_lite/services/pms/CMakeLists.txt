#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "permission")

set(PUBLIC_HEADER
    ${OHOS_DIR}/base/security/permission_lite/services/pms/include
    ${OHOS_DIR}/base/security/permission_lite/interfaces/kits
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/base/security/permission_lite/interfaces/kits
    ${OHOS_DIR}/base/security/permission_lite/services/pms/include
    ${OHOS_DIR}/base/security/permission_lite/services/pms/include/hals
    ${OHOS_DIR}/third_party/cJSON
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/commonlibrary/utils_lite/include
)

set(SOURCES
    ${OHOS_DIR}/base/security/permission_lite/services/pms/src/perm_operate.c
    ${OHOS_DIR}/base/security/permission_lite/services/pms/src/pms_impl.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-sign-compare
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

