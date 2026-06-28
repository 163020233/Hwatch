# Copyright HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.

# 配置nano_npu仓之外的头文件依赖
# ROOT_DEPTH是nano_npu仓映射目录的父目录

set(cfbb_root ${ROOT_DEPTH}/../..)

set(NPU_PATH ${cfbb_root}/protocol/npu)

set(TEST_LIB_PATH ${NPU_PATH}/npu_test/hlt/drv/drv_hlt_tsdrv/liteos/libs)
set(LITEOS_DIR ${cfbb_root}/kernel/liteos/LiteOS)
set(DRIVER_DIR ${cfbb_root}/drivers)
set(APPLICATION_DIR ${cfbb_root}/application)

set(EXTRA_HEADFILE
    ${LITEOS_DIR}/self_src/kernel_lite/include
    ${LITEOS_DIR}/self_src/drivers/timer/include
    ${LITEOS_DIR}/self_src/drivers/interrupt/include
    ${LITEOS_DIR}/self_src/targets/3322_app/include
    ${LITEOS_DIR}/self_src/targets/3322_app/include/asm
    ${LITEOS_DIR}/self_src/targets/3322_app/include/soc
    ${LITEOS_DIR}/self_src/arch_lite/riscv/include
    ${LITEOS_DIR}/self_src/arch_lite/riscv/include/arch
    ${LITEOS_DIR}/self_src/lib/liteos_libc/include
    ${cfbb_root}/open_source/musl/include
    ${cfbb_root}/middleware/utils/hwsec_c

    ${DRIVER_DIR}/chips/3322/include/acore
    ${DRIVER_DIR}/chips/3322/include/shared
    ${DRIVER_DIR}/chips/3322/porting/include
    ${DRIVER_DIR}/chips/3322/porting/uart
    ${DRIVER_DIR}/chips/3322/porting/systick
    ${DRIVER_DIR}/chips/3322/pmu/drivers
    ${DRIVER_DIR}/chips/3322/pmu/hal
    ${DRIVER_DIR}/chips/3322/clocks/hal
    ${DRIVER_DIR}/chips/3322/clocks/drivers
    ${DRIVER_DIR}/drivers/chips/3322/porting/arch/riscv
    ${DRIVER_DIR}/drivers/hal/psram_v2
    ${DRIVER_DIR}/drivers/hal/uart
    ${DRIVER_DIR}/boards/3322_evb/product
    ${DRIVER_DIR}/boards/3322_evb/memory_config/include
    ${DRIVER_DIR}/boards/3322_evb/memory_config/norflash/normal
    ${DRIVER_DIR}/boards/3322_evb/memory_config/psram/normal
    ${DRIVER_DIR}/boards/3322_evb/memory_config/norflash/custom_single_norflash
    ${DRIVER_DIR}/boards/3322_evb/memory_config/psram/custom_single_norflash
    ${cfbb_root}/include/driver
    ${cfbb_root}/include
    ${cfbb_root}/middleware/utils/common_headers
    ${cfbb_root}/middleware/utils/dfx/massdata
    ${cfbb_root}/middleware/chips/3322/pm/include
    ${cfbb_root}/middleware/chips/3322/pm/pm_clocks
    ${cfbb_root}/middleware/chips/3322/pm/pm_pmu
    ${cfbb_root}/middleware/chips/3322/pm/pm_sleep/acore
    ${cfbb_root}/middleware/chips/3322/pm/pm_veto
    ${cfbb_root}/middleware/chips/3322/include/shared
    ${cfbb_root}/middleware/chips/3322/dfx/include

    ${APPLICATION_DIR}/3322/3322_app_standard

    ${TEST_LIB_PATH}/shell
)
