#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ohos_drv")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/iot_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/iot_i2c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/iot_pwm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/iot_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/iot_watchdog.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lowpower.c
    ${CMAKE_CURRENT_SOURCE_DIR}/reset.c
)

if(DEFINED CONFIG_DRIVER_SUPPORT_SFC)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/iot_sfc.c
    )
elseif(DEFINED CONFIG_DRIVER_SUPPORT_SFC_V2)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/iot_sfc_v2.c
    )
else()
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/iot_flash.c
    )
endif()

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

if(DEFINED CONFIG_SUPPORT_OH)
    list(APPEND PUBLIC_HEADER
        ${ROOT_DIR}/ohos/base/iothardware/peripheral/interfaces/inner_api
    )

else()
    list(APPEND PUBLIC_HEADER
        ${ROOT_DIR}/ohos/base/iothardware/peripheral/interfaces/inner_api
        ${OHOS_DIR}/iothardware/peripheral/interfaces/kits
    )
endif()

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
