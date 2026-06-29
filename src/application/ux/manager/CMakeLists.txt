#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

add_subdirectory_if_exist(protocol)
add_subdirectory_if_exist(include)
add_subdirectory_if_exist(inputmgr)
add_subdirectory_if_exist(devicemgr)
add_subdirectory_if_exist(communication)
add_subdirectory_if_exist(sysmgr)
add_subdirectory_if_exist(powermgr)
add_subdirectory_if_exist(sourcemgr)