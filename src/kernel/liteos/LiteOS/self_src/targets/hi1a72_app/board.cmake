# C sources

list(APPEND LOCAL_SRCS_y
    # ${LITEOS_PLATFORM}/vendor/src/hal_uart_v151.c
    # ${LITEOS_PLATFORM}/vendor/src/uart_porting.c
    # ${LITEOS_PLATFORM}/vendor/src/uart.c
    # ${LITEOS_PLATFORM}/vendor/src/tick_timer.c
    # ${LITEOS_PLATFORM}/vendor/src/startup.S
    # ${LITEOS_PLATFORM}/vendor/src/hal_uart.c
    # ${LITEOS_PLATFORM}/vendor/src/osal_interrupt.c
    # ${LITEOS_PLATFORM}/vendor/src/sfc.c
    # ${LITEOS_PLATFORM}/vendor/src/clocks/clocks_config_priv.c
    # ${LITEOS_PLATFORM}/vendor/src/clocks/clocks.c
    # ${LITEOS_PLATFORM}/vendor/src/clocks/hal_clocks_app.c
    # ${LITEOS_PLATFORM}/vendor/src/clocks/hal_clocks_app_priv.c
    # ${LITEOS_PLATFORM}/vendor/src/clocks/hal_clocks_glb.c
    # ${LITEOS_PLATFORM}/vendor/src/clocks/hal_clocks_sdio.c
    # ${LITEOS_PLATFORM}/vendor/src/clocks/hal_clocks.c
    # ${LITEOS_PLATFORM}/vendor/src/clocks/hal_cpu_clocks.c
    # ${LITEOS_PLATFORM}/vendor/src/clocks_core/clocks_core.c
    # ${LITEOS_PLATFORM}/vendor/src/clocks_core/clocks_switch_priv.c
    # ${LITEOS_PLATFORM}/vendor/src/clocks_core/clocks_switch.c
    # ${LITEOS_PLATFORM}/vendor/src/clocks_core/clock_calibration.c
    # ${LITEOS_PLATFORM}/vendor/src/clocks_core/hal_32k_clock.c
    # ${LITEOS_PLATFORM}/vendor/src/tcxo/tcxo.c
    # ${LITEOS_PLATFORM}/vendor/src/tcxo/tcxo_porting.c
    # ${LITEOS_PLATFORM}/vendor/src/gpio/hal_gpio_v150_comm.c
    # ${LITEOS_PLATFORM}/vendor/src/gpio/hal_gpio_v150_regs_op.c
    # ${LITEOS_PLATFORM}/vendor/src/gpio/hal_gpio_v150.c
    # ${LITEOS_PLATFORM}/vendor/src/gpio/gpio.c
    # ${LITEOS_PLATFORM}/vendor/src/gpio/hal_gpio.c
    # ${LITEOS_PLATFORM}/vendor/src/gpio/gpio_porting.c
    # ${LITEOS_PLATFORM}/vendor/src/pmu/pmu_cmu.c
    # ${LITEOS_PLATFORM}/vendor/src/pmu/pmu_ldo.c
    # ${LITEOS_PLATFORM}/vendor/src/pmu/pmu_pg.c
    # ${LITEOS_PLATFORM}/vendor/src/pmu/pmu_vdd.c
    # ${LITEOS_PLATFORM}/vendor/src/pmu/pmu.c
    # ${LITEOS_PLATFORM}/vendor/src/pmu/hal_pmu_cmu.c
    # ${LITEOS_PLATFORM}/vendor/src/pmu/hal_pmu_peripheral_app.c
    # ${LITEOS_PLATFORM}/vendor/src/pmu/hal_pmu_ldo.c
    # ${LITEOS_PLATFORM}/vendor/src/pmu/hal_pmu_pg.c
    # ${LITEOS_PLATFORM}/vendor/src/pmu/hal_pmu_vdd.c
    # ${LITEOS_PLATFORM}/vendor/src/pinmux/pinctrl.c
    # ${LITEOS_PLATFORM}/vendor/src/pinmux/hal_pinctrl_1a72.c
    # ${LITEOS_PLATFORM}/vendor/src/pinmux/hal_pinctrl.c
    # ${LITEOS_PLATFORM}/vendor/src/pinmux/pinctrl_porting_1a72.c
    # ${LITEOS_PLATFORM}/vendor/src/reg_config/hal_reg_config.c
    # ${ROOT_DIR}/drivers/chips/${CHIP}/include/
    # ${ROOT_DIR}/drivers/chips/${CHIP}/porting/include/
)

# if(LOSCFG_DRIVERS_MMC)
#     list(APPEND LOCAL_SRCS_y
#         #${LITEOS_PLATFORM}/vendor/src/mmc/mmc_porting.c
#     )
# endif()

# list(APPEND MODULE_INCLUDE_PUB ${LITEOS_PLATFORM}/vendor/include)
# list(APPEND MODULE_INCLUDE_PUB ${LITEOS_PLATFORM}/vendor/include/clocks)
# list(APPEND MODULE_INCLUDE_PUB ${LITEOS_PLATFORM}/vendor/include/clocks_core)
# list(APPEND MODULE_INCLUDE_PUB ${LITEOS_PLATFORM}/vendor/include/shared)
# list(APPEND MODULE_INCLUDE_PUB ${LITEOS_PLATFORM}/vendor/include/tcxo)
# list(APPEND MODULE_INCLUDE_PUB ${LITEOS_PLATFORM}/vendor/include/porting)
# list(APPEND MODULE_INCLUDE_PUB ${LITEOS_PLATFORM}/vendor/include/pmu)
# list(APPEND MODULE_INCLUDE_PUB ${LITEOS_PLATFORM}/vendor/include/product)
# list(APPEND MODULE_INCLUDE_PUB ${LITEOS_PLATFORM}/vendor/include/gpio)
# list(APPEND MODULE_INCLUDE_PUB ${LITEOS_PLATFORM}/vendor/include/pinctrl)

# list(APPEND MODULE_COPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
# list(APPEND MODULE_ASOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
# list(APPEND MODULE_CXXOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})

# C sources
set(CFBB_DRV_INCLUDE
    ${ROOT_DIR}/drivers/chips/${CHIP}/clocks/drivers/
    ${ROOT_DIR}/drivers/chips/${CHIP}/clocks/hal/
    ${ROOT_DIR}/drivers/chips/${CHIP}/include/acore/
    ${ROOT_DIR}/drivers/chips/${CHIP}/porting/include/
    ${ROOT_DIR}/drivers/chips/${CHIP}/porting/uart/
    ${ROOT_DIR}/drivers/chips/${CHIP}/pmu/drivers/acore/
    ${ROOT_DIR}/drivers/chips/${CHIP}/include/shared/
    ${ROOT_DIR}/drivers/chips/${CHIP}/pmu/hal/
    ${ROOT_DIR}/drivers/chips/${CHIP}/pmu/drivers/shared/
    ${ROOT_DIR}/drivers/chips/${CHIP}/arch/include/
    ${ROOT_DIR}/drivers/chips/${CHIP}/porting/arch/riscv
    ${ROOT_DIR}/drivers/chips/${CHIP}/interrupt/acore/
    ${ROOT_DIR}/middleware/utils/common_headers/
    ${ROOT_DIR}/include/driver/
    ${ROOT_DIR}/include/
    ${ROOT_DIR}/drivers/boards/${CHIP}/${CHIP}_${BOARD}/product/
    ${ROOT_DIR}/drivers/boards/${CHIP}/${CHIP}_${BOARD}/linker/standard/
    ${ROOT_DIR}/drivers/boards/${CHIP}/${CHIP}_${BOARD}/memory_config/include/
    ${ROOT_DIR}/drivers/drivers/hal/uart/
    ${ROOT_DIR}/drivers/drivers/hal/gpio/
    ${ROOT_DIR}/drivers/drivers/hal/gpio/v150/
)

list(APPEND USER_SRC_TEMP ${CMAKE_CURRENT_SOURCE_DIR}/hi1a72_app/os_adapt/os_adapt.c)
list(APPEND LOCAL_SRCS_y ${USER_SRC_TEMP})
list(APPEND MODULE_INCLUDE_PUB ${CMAKE_CURRENT_SOURCE_DIR}/include)
list(APPEND MODULE_INCLUDE_PUB ${CFBB_DRV_INCLUDE})

# list(APPEND MODULE_INCLUDE_PUB ${LITEOS_OPENSOURCE_PATHNAME}/freebsd/liteos/drivers/mmc/core)
# list(APPEND MODULE_INCLUDE_PUB ${LITEOS_OPENSOURCE_PATHNAME}/FatFs/source)

# list(APPEND MODULE_CMACRO_PUB CHIP_1A72)
# list(APPEND MODULE_CMACRO_PUB CONFIG_UART_SUPPORT_TX)
