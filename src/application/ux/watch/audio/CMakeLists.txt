#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
if("CONFIG_SUPPORT_HFP_HF" IN_LIST DEFINES)
    add_subdirectory_if_exist(sco)
endif()

if("CONFIG_SUPPORT_A2DP_SINK" IN_LIST DEFINES)
    add_subdirectory_if_exist(a2dp)
endif()