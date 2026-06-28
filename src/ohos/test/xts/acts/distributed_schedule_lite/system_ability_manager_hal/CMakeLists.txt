#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "module_ActsSamgrTest")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/third_party/unity/src
    ${OHOS_DIR}/kernel/liteos_m/kal
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/communication/broadcast
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/adapter
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/source
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/src
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/utils
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/test/xts/tools/lite/hctest/include
)

set(SOURCES
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/src/broadcast_publish_func_test.c
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/src/broadcast_subscribe_func_test.c
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/src/common_func_test.c
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/src/defaultfeatureapi_func_test.c
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/src/feature_func_test.c
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/src/featureapi_func_test.c
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/src/iunknown_func_test.c
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/src/sendrequest_func_test.c
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/src/sendresponse_func_test.c
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/src/sendsharerequest_func_test.c
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/src/service_func_test.c
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/src/taskpool_notask_func_test.c
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/src/taskpool_sharedtask_func_test.c
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/src/taskpool_singletask_func_test.c
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/src/taskpool_specifiedtask_func_test.c
    ${OHOS_DIR}/test/xts/acts/distributed_schedule_lite/system_ability_manager_hal/utils/samgr_maintenance.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error
    -fno-function-sections
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    UNITY_INCLUDE_CONFIG_H
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

