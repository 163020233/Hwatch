#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "huks_sdk")

# cut abilities for authentication
set(disable_authenticate false)

# if HUKS use mbedtls engine
set(huks_use_mbedtls true)

# whether use lite storeage
set(huks_use_lite_storage false)

# whether use hardware root key
set(huks_use_hardware_root_key false)

# whether huks enable log
set(huks_enable_log false)

set(huks_sources
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/common/src/hks_ability.c
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/common/src/hks_base_check.c
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/common/src/hks_check_paramset.c
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/common/src/hks_common_check.c
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/common/src/hks_crypto_adapter.c
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/common/src/hks_param.c
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/core/src/hks_local_engine.c
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/os_dependency/ipc/src/hks_client_service_passthrough.c
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/os_dependency/posix/hks_mem.c
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/os_dependency/sysinfo/src/hks_get_process_info_passthrough.c
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/os_dependency/sysinfo/src/hks_get_udid.c
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_get_main_key.c
    ${OHOS_DIR}/base/security/huks/interfaces/inner_api/huks_standard/main/src/hks_api.c
    ${OHOS_DIR}/base/security/huks/interfaces/inner_api/huks_standard/main/src/hks_api_adapter.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_engine/main/core/src/hks_auth.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_engine/main/core/src/hks_core_interfaces.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_engine/main/core/src/hks_core_service.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_engine/main/core/src/hks_core_service_three_stage.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_engine/main/core/src/hks_keyblob.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_engine/main/core/src/hks_keynode.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_engine/main/core/src/hks_sm_import_wrap_key.c
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/rkc/src/hks_rkc.c    
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/rkc/src/hks_rkc_rw.c
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/rkc/src/hw_rkc_rmk.c
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/os_dependency/posix/hks_util.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_engine/main/core/src/hks_secure_access.c
    # ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_engine/main/core/src/hks_upgrade_key_info.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/core/src/hks_client_check.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/core/src/hks_client_service.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/core/src/hks_hitrace.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/core/src/hks_report.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/core/src/hks_session_manager.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/core/src/hks_storage_manager.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/core/src/hks_storage_utils.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/core/src/hks_client_service_util.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/plugin_proxy/src/hks_plugin_adapter_mock.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/os_dependency/idl/passthrough/huks_access.c
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/os_dependency/idl/passthrough/huks_core_static_hal.c

    ${OHOS_DIR}/base/security/huks/utils/crypto_adapter/hks_client_service_adapter_common.c
    ${OHOS_DIR}/base/security/huks/utils/crypto_adapter/hks_client_service_adapter_lite.c
    ${OHOS_DIR}/base/security/huks/utils/file_operator/hks_file_operator_lite.c
    ${OHOS_DIR}/base/security/huks/utils/list/hks_double_list.c
    ${OHOS_DIR}/base/security/huks/utils/mutex/hks_mutex.c
)

if(${huks_use_mbedtls} STREQUAL "true")
    set(huks_mbedtls_engine_sources
        ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_crypto_ed25519.c
        ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_ability.c
        ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_aes.c
        ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_bn.c
        ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_common.c
        ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_ecc.c
        ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_ecdh.c
        ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_ecdsa.c
        ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_engine.c
        ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_hash.c
        ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_hmac.c
        ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_kdf.c
        ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_rsa.c
        ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/mbedtls/src/hks_mbedtls_x25519.c
    )
endif()

if(${huks_use_lite_storage} STREQUAL "true")
    set(huks_storage_source
        ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/core/src/hks_storage_adapter.c
        ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/core/src/hks_storage_lite.c
        ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_engine/main/core/src/hks_keyblob_lite.c
    )
else()
    set(huks_storage_source
        ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/core/src/hks_storage.c
    )
endif()

set(SOURCES
    ${huks_sources}
    ${huks_mbedtls_engine_sources}
    ${OHOS_DIR}/base/security/huks/interfaces/inner_api/huks_lite/hks_tmp_client.c
    ${huks_storage_source}
)

set(PUBLIC_HEADER
    ${OHOS_DIR}/base/security/huks/interfaces/innerkits/huks_standard/main/include
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/base/security/huks/interfaces/inner_api/huks_standard/main/include
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/common/include
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/core/include
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/mbedtls/include
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/crypto_engine/rkc/include
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/os_dependency/ipc/include
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/os_dependency/sysinfo/include
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_engine/main/core/include
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_engine/main/core_dependency/include
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_engine/main/device_cert_manager/include
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/os_dependency/idl/ipc
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/os_dependency/idl/passthrough
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/systemapi_wrap/useridm/inc
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/systemapi_wrap/hitrace_meter_wrapper/include
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/core/include
    ${OHOS_DIR}/base/security/huks/services/huks_standard/huks_service/main/plugin_proxy/include
    ${OHOS_DIR}/base/security/huks/utils/crypto_adapter
    ${OHOS_DIR}/base/security/huks/utils/file_operator
    ${OHOS_DIR}/base/security/huks/utils/list
    ${OHOS_DIR}/base/security/huks/utils/mutex
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite/
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include/syspara
)

set(huks_defines)
if(${disable_authenticate} STREQUAL "true")
    set(huks_defines ${huks_defines} _CUT_AUTHENTICATE_)
endif()

if(${huks_use_hardware_root_key} STREQUAL "true")
    set(huks_defines ${huks_defines} _HARDWARE_ROOT_KEY_)
endif()

if(${huks_use_lite_storage} STREQUAL "true")
    set(huks_defines ${huks_defines} _STORAGE_LITE_)
endif()

if(${huks_enable_log} STREQUAL "true")
    set(huks_defines ${huks_defines} _HUKS_LOG_ENABLE_)
endif()

if(${CHIP} STREQUAL "brandy")
    set(huks_defines ${huks_defines} GET_DEV_UDID_ENABLE HARDWARE_UDID HKS_HARDWARE_ROOT_KEY)
elseif(${CHIP} STREQUAL "3322")
    set(huks_defines ${huks_defines} GET_DEV_UDID_ENABLE HKS_HARDWARE_ROOT_KEY)
endif()

set(PRIVATE_DEFINES
    _FORTIFY_SOURCE=2
    ${huks_defines}
    HKS_CONFIG_FILE="hks_config_lite.h"
    HKS_KEY_VERSION=1
    HKS_CONFIG_KEY_STORE_PATH="/user/app/user" 
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -fvisibility=hidden
    -Wall
    -Wno-error=unused-parameter
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
