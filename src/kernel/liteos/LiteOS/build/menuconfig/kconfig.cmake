# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
if(LOSCFG_SAFETY_KERNEL)
    set(KCONFIG_KERNEL_PATH kernel_s)
    set(KCONFIG_ARCH_PATH arch_s)
    set(KCONFIG_TEST_PATH test_s)
else()
    set(KCONFIG_KERNEL_PATH kernel_lite)
    set(KCONFIG_ARCH_PATH arch_lite)
    set(KCONFIG_TEST_PATH test)
endif()

add_custom_target(
    menuconfig
    COMMAND export KCONFIG_KERNEL_PATH=${KCONFIG_KERNEL_PATH} &&
            export KCONFIG_ARCH_PATH=${KCONFIG_ARCH_PATH} &&
            export KCONFIG_TEST_PATH=${KCONFIG_TEST_PATH} &&
            export KCONFIG_OUTPUT_MENUCONFIG_PATH=${LITEOS_CHECK_MENUCONFIG_H} &&
            python3 ${RTOSLITETOPDIR}/build/menuconfig/usr_config.py
    WORKING_DIRECTORY ${RTOS_LITE_TOP_DIR}
)

add_custom_target(
    savemenuconfig
    COMMAND export KCONFIG_KERNEL_PATH=${KCONFIG_KERNEL_PATH} &&
            export KCONFIG_ARCH_PATH=${KCONFIG_ARCH_PATH} &&
            export KCONFIG_TEST_PATH=${KCONFIG_TEST_PATH} &&
            export KCONFIG_OUTPUT_MENUCONFIG_PATH=${LITEOS_CHECK_MENUCONFIG_H} &&
            python3 ${RTOSLITETOPDIR}/build/menuconfig/usr_config.py savemenuconfig
    BYPRODUCTS ${LITEOS_CHECK_MENUCONFIG_H}
    WORKING_DIRECTORY ${RTOS_LITE_TOP_DIR}
)
set_property(TARGET menuconfig PROPERTY STATUS AVAILABLE)
set_property(TARGET savemenuconfig PROPERTY STATUS AVAILABLE)
