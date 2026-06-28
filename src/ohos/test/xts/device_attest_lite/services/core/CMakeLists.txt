#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "devattest_core")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/include
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/include/adapter
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/include/attest
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/include/dfx
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/include/security
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/include/network
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/include/utils
    ${OHOS_DIR}/test/xts/device_attest_lite/interfaces/innerkits
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include/syspara
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits
    ${OHOS_DIR}/third_party/cJSON
    ${OHOS_DIR}/base/startup/init/interfaces/hals
    ${OHOS_DIR}/developtools/syscap_codec/interfaces/inner_api
    ${OHOS_DIR}/test/xts/device_attest_lite/common/mini
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.5/include
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.5/library
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.5/include/mbedtls
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.5/tests/include
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.5/port/include
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.5/port/config/compat_posix
)

set(SOURCES
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/attest_entry.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/adapter/attest_adapter.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/adapter/attest_adapter_os.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/adapter/attest_adapter_oem.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/adapter/attest_adapter_mock.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/attest/attest_service.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/attest/attest_service_auth.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/attest/attest_service_active.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/attest/attest_service_challenge.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/attest/attest_service_reset.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/attest/attest_service_device.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/network/attest_network.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/security/attest_security.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/security/attest_security_token.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/security/attest_security_ticket.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/utils/attest_utils_file.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/utils/attest_utils_json.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/utils/attest_utils_log.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/utils/attest_utils_timer.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/utils/attest_utils_list.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/utils/attest_utils.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/network/attest_coap.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/network/attest_tls.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/adapter/attest_adapter_hal.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/adapter/attest_adapter_property.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/network/attest_channel.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/mini/adapter/attest_adapter_network_config.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/mini/attest/attest_service_pcid.c
    ${OHOS_DIR}/test/xts/device_attest_lite/services/core/mini/utils/attest_utils_file_detail.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-unused-function
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

