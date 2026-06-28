#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "devattest_sdk")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/test/xts/device_attest_lite/common
    ${OHOS_DIR}/test/xts/device_attest_lite/common/mini
    ${OHOS_DIR}/test/xts/device_attest_lite/interfaces/innerkits
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/include
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/include/adapter
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/include/attest
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/include/dfx
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/include/security
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/include/network
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/include/utils
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include/syspara
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits
    ${OHOS_DIR}/third_party/cJSON
    ${OHOS_DIR}/base/startup/init/interfaces/hals
    ${OHOS_DIR}/developtools/syscap_codec/interfaces/inner_api
)

set(SOURCES
    ${OHOS_DIR}/test/xts/device_attest_lite/framework/mini/src/attest_framework_client_mini.c
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
    MBEDTLS_ALLOW_PRIVATE_ACCESS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

