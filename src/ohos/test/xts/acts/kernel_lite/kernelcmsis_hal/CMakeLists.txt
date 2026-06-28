#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "module_ActsCMSISTest")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/third_party/unity/src
    ${OHOS_DIR}/test/xts/acts/kernel_lite/kernelcmsis_hal/src
    ${OHOS_DIR}/kernel/liteos_m/kal
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/test/xts/tools/lite/hctest/include
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
)

set(SOURCES
    ${OHOS_DIR}/test/xts/acts/kernel_lite/kernelcmsis_hal/src/cmsis_event_func_test.c
    ${OHOS_DIR}/test/xts/acts/kernel_lite/kernelcmsis_hal/src/cmsis_msg_func_test.c
    ${OHOS_DIR}/test/xts/acts/kernel_lite/kernelcmsis_hal/src/cmsis_mutex_func_test.c
    ${OHOS_DIR}/test/xts/acts/kernel_lite/kernelcmsis_hal/src/cmsis_sem_func_test.c
    ${OHOS_DIR}/test/xts/acts/kernel_lite/kernelcmsis_hal/src/cmsis_task_func_test.c
    ${OHOS_DIR}/test/xts/acts/kernel_lite/kernelcmsis_hal/src/cmsis_task_pri_func_test.c
    ${OHOS_DIR}/test/xts/acts/kernel_lite/kernelcmsis_hal/src/cmsis_timer_func_test.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error
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

