#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "appverify")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/include
    ${OHOS_DIR}/third_party/cJSON
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/products/default
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/products/ipcamera
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.5/include
    ${OHOS_DIR}/commonlibrary/utils_lite/memory/include
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include/syspara
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include/token
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include/param
    ${OHOS_DIR}/base/hiviewdfx/hiview_lite
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/base/hiviewdfx/hievent_lite/interfaces/native/innerkits
    ${OHOS_DIR}/device/soc/hisilicon/common/hardware/litewearable_base/mem_hal
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/products/ipcamera
)

set(SOURCES
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/src/app_centraldirectory.c
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/src/app_common.c
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/src/app_file.c
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/src/app_provision.c
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/src/app_verify.c
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/src/app_verify_hal.c
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/src/app_verify_hap.c
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/src/mbedtls_pkcs7.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-int-conversion
    -Wno-typedef-redefinition
    -Wno-strict-prototypes
    -Wno-constant-conversion
    -Wno-error=sign-compare
    -Wno-error=sometimes-uninitialized
    -Wno-error=unused-but-set-variable
    -Wno-error=unused-function
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    PARSE_PEM_FORMAT_SIGNED_DATA
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

