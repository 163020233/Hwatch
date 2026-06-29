#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(sle)
add_subdirectory_if_exist(port)
add_subdirectory_if_exist(ble)
add_subdirectory_if_exist(spp)
add_subdirectory_if_exist(generic)
add_subdirectory_if_exist(database)
add_subdirectory_if_exist(api)

if(NOT "DEMO_WATCH" IN_LIST DEFINES)
    add_subdirectory_if_exist(bt)
    add_subdirectory_if_exist(common)
endif()