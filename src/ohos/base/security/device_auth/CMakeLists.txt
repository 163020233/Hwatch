#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "deviceauth")

set(SOURCES
    #hal_common_files
    ${OHOS_DIR}/base/security/device_auth/common_lib/impl/src/hc_parcel.c
    ${OHOS_DIR}/base/security/device_auth/common_lib/impl/src/hc_string.c
    ${OHOS_DIR}/base/security/device_auth/common_lib/impl/src/hc_string_vector.c
    ${OHOS_DIR}/base/security/device_auth/common_lib/impl/src/hc_tlv_parser.c
    ${OHOS_DIR}/base/security/device_auth/common_lib/impl/src/json_utils.c
    ${OHOS_DIR}/base/security/device_auth/common_lib/impl/src/string_util.c
    ${OHOS_DIR}/base/security/device_auth/deps_adapter/key_management_adapter/impl/src/alg_loader.c
    ${OHOS_DIR}/base/security/device_auth/deps_adapter/os_adapter/impl/src/hc_mutex.c
    ${OHOS_DIR}/base/security/device_auth/deps_adapter/os_adapter/impl/src/hc_task_thread.c
    ${OHOS_DIR}/base/security/device_auth/deps_adapter/os_adapter/impl/src/hc_time.c

    #mini
    ${OHOS_DIR}/base/security/device_auth/deps_adapter/key_management_adapter/impl/src/huks_adapter.c
    ${OHOS_DIR}/base/security/device_auth/deps_adapter/os_adapter/impl/src/liteos/hc_condition.c
    ${OHOS_DIR}/base/security/device_auth/deps_adapter/os_adapter/impl/src/liteos/hc_dev_info.c
    ${OHOS_DIR}/base/security/device_auth/deps_adapter/os_adapter/impl/src/liteos/hc_thread.c
    ${OHOS_DIR}/base/security/device_auth/deps_adapter/os_adapter/impl/src/liteos/hc_types.c
    ${OHOS_DIR}/base/security/device_auth/deps_adapter/os_adapter/impl/src/liteos/mini/hc_init_protection.c

    #POSIX OFF
    ${OHOS_DIR}/base/security/device_auth/deps_adapter/os_adapter/impl/src/liteos/mini/hc_file.c

    #device_auth
    #dev_frameworks_files
    ${OHOS_DIR}/base/security/device_auth/services/frameworks/src/module/dev_auth_module_manager.c
    ${OHOS_DIR}/base/security/device_auth/services/frameworks/src/task_manager/task_manager.c
    #deviceauth_common_files
    ${OHOS_DIR}/base/security/device_auth/services/device_auth.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/src/group_manager.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/src/channel_manager/channel_manager.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/src/callback_manager/callback_manager.c
    ${OHOS_DIR}/base/security/device_auth/services/protocol/src/protocol_common.c
    ${OHOS_DIR}/base/security/device_auth/services/protocol/src/iso_protocol/iso_protocol_common.c
    ${OHOS_DIR}/base/security/device_auth/services/protocol/src/pake_protocol/pake_common.c
    ${OHOS_DIR}/base/security/device_auth/services/protocol/src/pake_protocol/pake_v1_protocol/pake_v1_protocol_common.c
    ${OHOS_DIR}/base/security/device_auth/services/protocol/src/pake_protocol/pake_protocol_dl_common/pake_protocol_dl_common.c
    ${OHOS_DIR}/base/security/device_auth/services/protocol/src/pake_protocol/pake_protocol_ec_common/pake_protocol_ec_common.c
    ${OHOS_DIR}/base/security/device_auth/services/protocol/src/pake_protocol/pake_v2_protocol/pake_v2_protocol_common.c
    #group_auth_files
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_auth/src/group_auth_manager/group_auth_common/group_auth_data_operation.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_auth/src/group_auth_manager/group_auth_manager.c
    #group_manager_files
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/src/group_operation/group_operation.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/src/group_operation/group_operation_common/group_operation_common.c
    #database_manager_files
    ${OHOS_DIR}/base/security/device_auth/services/data_manager/src/data_manager.c
    #broadcast_manager_files
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/src/broadcast_manager/broadcast_manager.c
    #hiview_adapter_mock_files
    #os_account_adapter_lite_files
    ${OHOS_DIR}/base/security/device_auth/test/unittest/deviceauth/source/os_account_adapter_mock.c

    #group_auth_account_unrelated_files
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_auth/src/group_auth_manager/account_unrelated_group_auth/account_unrelated_group_auth.c

    #group_manager_peer_to_peer_files
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/src/group_operation/peer_to_peer_group/peer_to_peer_group.c

    #authenticators_p2p_files
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/common/das_task_common.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/das_module.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/common/das_version_util.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/das_task_main.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/pake_task/pake_task_common.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/pake_task/pake_message_util.c

    #authenticators_p2p_iso_files
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/iso_task/iso_client_task.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/iso_task/iso_server_task.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/iso_task/iso_task_common/iso_task_common.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/iso_task/iso_task_main.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/iso_task/iso_protocol_task/iso_client_protocol_task.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/iso_task/iso_protocol_task/iso_server_protocol_task.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/creds_manager/das_lite_token_manager.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/iso_task/lite_exchange_task/iso_client_bind_exchange_task.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/iso_task/lite_exchange_task/iso_server_bind_exchange_task.c

    #authenticators_p2p_pake_mock_files
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/pake_task/pake_v1_task_mock/pake_v1_task_main_mock.c
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/pake_task/pake_v2_task_mock/pake_v2_task_main_mock.c

    #authenticators_standard_exchange_task_mock_files
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_unrelated/pake_task/standard_exchange_task_mock/standard_exchange_task_mock.c

    #group_auth_account_related_mock_files
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_auth/src/group_auth_manager/account_related_group_auth_mock/account_related_group_auth_mock.c

    #group_manager_identical_account_mock_files
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/src/group_operation/identical_account_group_mock/identical_account_group_mock.c

    #group_manager_across_account_mock_files
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/src/group_operation/across_account_group_mock/across_account_group_mock.c

    #authenticators_account_related_mock_files
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/src/account_related_mock/account_module_mock.c

    #soft_bus_channel_files
    #${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/src/channel_manager/soft_bus_channel/soft_bus_channel.c

    #soft_bus_channel_mock_files
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/src/channel_manager/soft_bus_channel_mock/soft_bus_channel_mock.c

    ${OHOS_DIR}/base/security/device_auth/frameworks/src/ipc_service.c
)

set(PUBLIC_HEADER
    ${OHOS_DIR}/base/security/device_auth/interfaces/innerkits/
)

# 包含目录
set(PRIVATE_HEADER
    #mini
    ${OHOS_DIR}/base/security/device_auth/interfaces/innerkits
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include/syspara
    ${OHOS_DIR}/commonlibrary/utils_lite/include
     
    ${OHOS_DIR}/commonlibrary/utils_lite/memory/include
    ${OHOS_DIR}/base/security/huks/interfaces/inner_api/huks_standard/main/include
    ${OHOS_DIR}/ohos/foundation/communication/ipc/interfaces/innerkits/c/ipc/include
    
    #hals_inc_path
    ${OHOS_DIR}/base/security/device_auth/common_lib/interfaces
    ${OHOS_DIR}/base/security/device_auth/deps_adapter/os_adapter/interfaces
    ${OHOS_DIR}/base/security/device_auth/deps_adapter/os_adapter/interfaces/liteos
    
    ${OHOS_DIR}/third_party/bounds_checking_function/include

    ${OHOS_DIR}/third_party/cJSON

    ${OHOS_DIR}/foundation/communication/dsoftbus/interfaces/kits/common
    ${OHOS_DIR}/foundation/communication/dsoftbus/interfaces/kits/transport
    ${OHOS_DIR}/foundation/communication/dsoftbus/interfaces/inner_kits/transport
    ${OHOS_DIR}/base/security/device_auth/services/frameworks/inc/hiview_adapter
    ${OHOS_DIR}/base/security/device_auth/frameworks/inc/lite
    ${OHOS_DIR}/base/security/device_auth/interfaces/inner_api

    #inc_path
    ${OHOS_DIR}/base/security/device_auth/frameworks/inc
    ${OHOS_DIR}/base/security/device_auth/services/data_manager/inc
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_auth/inc
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_auth/inc/account_unrelated_group_auth
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_auth/inc/account_related_group_auth
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_auth/inc/session
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_auth/inc/session/auth_session
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_auth/inc/session/auth_session_lite
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/inc
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/inc/broadcast_manager
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/inc/callback_manager
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/inc/channel_manager
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/inc/channel_manager/soft_bus_channel
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/inc/session
    ${OHOS_DIR}/base/security/device_auth/services/legacy/group_manager/inc/session/bind_session
    ${OHOS_DIR}/base/security/device_auth/services/frameworks/inc
    ${OHOS_DIR}/base/security/device_auth/services/frameworks/inc/plugin_adapter
    ${OHOS_DIR}/base/security/device_auth/services/frameworks/inc/module
    ${OHOS_DIR}/base/security/device_auth/services/frameworks/inc/session
    ${OHOS_DIR}/base/security/device_auth/services/frameworks/inc/os_account_adapter
    ${OHOS_DIR}/base/security/device_auth/services/frameworks/inc/task_manager
    ${OHOS_DIR}/base/security/device_auth/services/frameworks/inc/security_label_adapter
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/inc/account_related
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/inc/account_unrelated
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/inc/account_unrelated/iso_task
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/inc/account_unrelated/iso_task/iso_protocol_task
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/inc/account_unrelated/iso_task/lite_exchange_task
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/inc/account_unrelated/pake_task
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/inc/account_unrelated/pake_task/pake_v1_task
    ${OHOS_DIR}/base/security/device_auth/services/legacy/authenticators/inc/account_unrelated/pake_task/pake_v2_task
    ${OHOS_DIR}/base/security/device_auth/services/protocol/inc
    ${OHOS_DIR}/base/security/device_auth/services/protocol/inc/pake_protocol
    ${OHOS_DIR}/base/security/device_auth/services/protocol/inc/pake_protocol/pake_v1_protocol
    ${OHOS_DIR}/base/security/device_auth/services/protocol/inc/pake_protocol/pake_v2_protocol
    ${OHOS_DIR}/base/security/device_auth/services/protocol/inc/pake_protocol/pake_protocol_dl_common
    ${OHOS_DIR}/base/security/device_auth/services/protocol/inc/pake_protocol/pake_protocol_ec_common
    ${OHOS_DIR}/base/security/device_auth/services/protocol/inc/iso_protocol
    ${OHOS_DIR}/base/security/device_auth/services/session_manager/inc/session/v1/compatible_auth_sub_session
    ${OHOS_DIR}/base/security/device_auth/services/session_manager/inc/session/v1
    ${OHOS_DIR}/base/security/device_auth/services/session_manager/inc
    ${OHOS_DIR}/base/security/device_auth/services/session_manager/inc/session
    ${OHOS_DIR}/base/security/device_auth/services/cred_manager/inc
    ${OHOS_DIR}/base/security/device_auth/services/mk_agree/inc
    ${OHOS_DIR}/base/security/device_auth/services/identity_manager/inc
    ${OHOS_DIR}/base/security/device_auth/services/privacy_enhancement/inc
    ${OHOS_DIR}/base/security/device_auth/deps_adapter/key_management_adapter/interfaces
    ${OHOS_DIR}/base/security/device_auth/deps_adapter/key_management_adapter/impl/inc
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Werror
    -Wno-error=missing-field-initializers
    -Wno-typedef-redefinition
    -Wno-error=strict-prototypes
    -Wno-error=unused-but-set-variable
)

set(PRIVATE_DEFINES
    DEV_AUTH_FUZZ_TEST
    _SCANTY_MEMORY_
    _CUT_STS_CLIENT_
    _CUT_REMOVE_
    _CUT_ADD_
    _CUT_LOG_
    HILOG_ENABLE
    DISABLE_HIVIEW
    P2P_PAKE_DL_PRIME_LEN_384
    AUTH_STORAGE_PATH=\"${deviceauth_storage_path}\"
    HICHAIN_THREAD_STACK_SIZE=8192
    DEV_AUTH_WORK_THREAD_STACK_SIZE=8192
)
set(PUBLIC_DEFINES
)
set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()