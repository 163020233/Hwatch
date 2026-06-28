#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================

if (DEFINES MATCHES "HI3322_CHIP_V100" OR DEFINES MATCHES "HI3322_PRODUCT_FPGA" )
    add_subdirectory(gc032a)
    add_subdirectory(bf20a6)
endif()

if (DEFINES MATCHES "SW22_CHIP_V100" OR DEFINES MATCHES "SW22_PRODUCT_FPGA")
    add_subdirectory_if_exist(galaxycore_gc02m1)
    add_subdirectory_if_exist(galaxycore_gc05a2)
    add_subdirectory_if_exist(smart_sc820cs)
    add_subdirectory_if_exist(galaxycore_gc13a0)
    # add_subdirectory_if_exist(omnivision_os08a20)
    # add_subdirectory_if_exist(sony_imx290)
    # add_subdirectory_if_exist(sony_imx327)
endif()