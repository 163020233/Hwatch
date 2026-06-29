#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
if("DEMO_WATCH" IN_LIST DEFINES)
    add_subdirectory_if_exist(watch)
else()
    add_subdirectory_if_exist(melody)
endif()