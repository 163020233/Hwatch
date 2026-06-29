#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
if (DEFINES MATCHES "HI3322_CHIP_V100" OR DEFINES MATCHES "HI3322_PRODUCT_FPGA" )
    add_subdirectory(viu_v1.0)
else()
    add_subdirectory(viu_v2.0)
endif()
