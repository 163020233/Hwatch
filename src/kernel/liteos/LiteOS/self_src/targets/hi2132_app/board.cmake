# C sources

list(APPEND LOCAL_SRCS_y
#    ${LITEOS_PLATFORM}/vendor/src/hal_pinctrl_cat1.c
#    ${LITEOS_PLATFORM}/vendor/src/hal_uart_v151.c
#    ${LITEOS_PLATFORM}/vendor/src/pinctrl.c
#    ${LITEOS_PLATFORM}/vendor/src/uart_porting.c
#    ${LITEOS_PLATFORM}/vendor/src/uart.c
    ${LITEOS_PLATFORM}/../hi2132_ccore/tick_timer.c
#    ${LITEOS_PLATFORM}/vendor/src/startup.S
#    ${LITEOS_PLATFORM}/vendor/src/mpu.c
#    ${LITEOS_PLATFORM}/vendor/src/hal_watchdog_v151.c
#    ${LITEOS_PLATFORM}/vendor/src/hal_watchdog.c
#    ${LITEOS_PLATFORM}/vendor/src/watchdog_porting.c
#    ${LITEOS_PLATFORM}/vendor/src/watchdog.c
)

list(APPEND MODULE_INCLUDE_PUB ${CMAKE_CURRENT_SOURCE_DIR}/hi2131/include)
list(APPEND MODULE_INCLUDE_PUB ${CMAKE_SOURCE_DIR}/drivers/chips/hi2132/board/linker/app_core)
list(APPEND MODULE_INCLUDE_PUB ${CMAKE_SOURCE_DIR}/drivers/chips/hi2132/include)
list(APPEND MODULE_INCLUDE_PUB ${CMAKE_SOURCE_DIR}/drivers/chips/hi2132/board/memory_config/include/product)
list(APPEND MODULE_INCLUDE_PUB ${CMAKE_SOURCE_DIR}/middleware/utils/common_headers)
list(APPEND MODULE_INCLUDE_PUB ${LITEOS_PLATFORM}/vendor/include)
list(APPEND MODULE_INCLUDE_PUB ${CMAKE_SOURCE_DIR}/drivers/chips/hi2132/board/memory_config/include)
list(APPEND MODULE_INCLUDE_PUB ${CMAKE_SOURCE_DIR}/kernel/liteos/LiteOS/self_src/targets/hi2132_ccore/include)
list(APPEND MODULE_INCLUDE_PUB ${CMAKE_SOURCE_DIR}/kernel/non_os/non_os)
list(APPEND MODULE_INCLUDE_PUB ${CMAKE_SOURCE_DIR}/drivers/chips/hi2132/arch/include)
