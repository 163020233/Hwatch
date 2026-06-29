#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "module_ActsWifiIotTest")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/test/xts/acts/iothardware_lite/peripheral_hal/src
    ${OHOS_DIR}/base/iothardware/peripheral/interfaces/inner_api
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/test/xts/tools/lite/hctest/include
    ${OHOS_DIR}/third_party/unity/src
    ${OHOS_DIR}/kernel/liteos_m/kal
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
)

set(SOURCES
    ${OHOS_DIR}/test/xts/acts/iothardware_lite/peripheral_hal/src/iot_flash_test.c
    ${OHOS_DIR}/test/xts/acts/iothardware_lite/peripheral_hal/src/iot_gpio_test.c
    ${OHOS_DIR}/test/xts/acts/iothardware_lite/peripheral_hal/src/iot_i2c_test.c
    ${OHOS_DIR}/test/xts/acts/iothardware_lite/peripheral_hal/src/iot_pwm_test.c
    ${OHOS_DIR}/test/xts/acts/iothardware_lite/peripheral_hal/src/iot_uart_test.c
    ${OHOS_DIR}/test/xts/acts/iothardware_lite/peripheral_hal/src/iot_watchdog_test.c
    ${OHOS_DIR}/test/xts/acts/iothardware_lite/peripheral_hal/src/lowpower_test.c
    ${OHOS_DIR}/test/xts/acts/iothardware_lite/peripheral_hal/src/reset_test.c
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
    CONFIG_I2C_SUPPORT
    CONFIG_PWM_SUPPORT
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

