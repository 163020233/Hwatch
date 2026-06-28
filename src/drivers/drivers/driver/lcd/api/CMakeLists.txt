#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "lcd")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/lcd_bus.c
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/lcd_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/lcd_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/lcd_private.c
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/lcd_mcu_int.c
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/lcd_dfx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/soc_lcd_api.c
)

if("SUPPORT_GPU_QSPI" IN_LIST DEFINES)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/qspi/lcd_qspi.c)
else()
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/mipi/lcd_mipi.c)
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/comm
    ${CMAKE_CURRENT_SOURCE_DIR}/mipi
    ${CMAKE_CURRENT_SOURCE_DIR}/qspi
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
    CONFIG_LCD_DFX_ERROR
)

set(PUBLIC_DEFINES
)

if (DEFINES MATCHES "SW21_CHIP_V100" OR DEFINES MATCHES "SW21_PRODUCT_FPGA" )
    list(APPEND PUBLIC_DEFINES "GPU_1A21")
endif()

if (DEFINES MATCHES "HI3322_CHIP_V100" OR DEFINES MATCHES "HI3322_PRODUCT_FPGA" )
    list(APPEND PUBLIC_DEFINES "GPU_3322")
endif()

if (DEFINES MATCHES "SW22_CHIP_V100" OR DEFINES MATCHES "SW22_PRODUCT_FPGA" )
    list(APPEND PUBLIC_DEFINES "GPU_1A22")
endif()

if (DEFINES MATCHES "SW21_CHIP_V100")
    list(APPEND PUBLIC_DEFINES "GPU_ASIC_1A21")
elseif (DEFINES MATCHES "SW21_PRODUCT_FPGA")
    list(APPEND PUBLIC_DEFINES "GPU_FPGA_1A21")
elseif (DEFINES MATCHES "SUQIN_PRODUCT_FPGA")
    list(APPEND PUBLIC_DEFINES "GPU_FPGA_3321")
elseif (DEFINES MATCHES "HI3322_CHIP_V100")
    list(APPEND PUBLIC_DEFINES "GPU_ASIC_3322")
elseif (DEFINES MATCHES "HI3322_PRODUCT_FPGA")
    list(APPEND PUBLIC_DEFINES "GPU_FPGA_3322")
elseif (DEFINES MATCHES "SW22_CHIP_V100")
    list(APPEND PUBLIC_DEFINES "GPU_ASIC_1A22")
elseif (DEFINES MATCHES "SW22_PRODUCT_FPGA")
    list(APPEND PUBLIC_DEFINES "GPU_FPGA_1A22")
else()
    list(APPEND PUBLIC_DEFINES "GPU_FOR_ASIC")
endif()

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    "-Werror=out-of-line-declaration"
    "-Werror=uninitialized"
    "-Werror=int-conversion"
    "-Werror=enum-conversion"
    "-Werror=long-long"
    "-Werror=unused-variable"
    "-Werror=unused-value"
    "-Werror=unused-function"
    "-Werror=unused-but-set-variable"
    "-Werror=sign-compare"
    "-Werror=unused-parameter"
    "-Werror=tautological-compare"
    "-Werror=bad-function-cast"
    "-Werror=incompatible-pointer-types"
    "-Werror=implicit-function-declaration"
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/qspi/ "*.*")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/mipi/ "*.*")

build_component()
