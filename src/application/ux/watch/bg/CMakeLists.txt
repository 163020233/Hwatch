#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(bt)

if("CONFIG_SUPPORT_HFP_HF" IN_LIST DEFINES)
    add_subdirectory_if_exist(hfp)
endif()

if("CONFIG_SUPPORT_A2DP_SINK" IN_LIST DEFINES OR "CONFIG_SUPPORT_A2DP_SRC" IN_LIST DEFINES)
    add_subdirectory_if_exist(a2dp)
endif()

if("CONFIG_SUPPORT_AVRCP" IN_LIST DEFINES)
    add_subdirectory_if_exist(avrcp)
endif()

if("CONFIG_SUPPORT_PBAP" IN_LIST DEFINES)
    add_subdirectory_if_exist(pbap)
endif()

if("CONFIG_SUPPORT_BR" IN_LIST DEFINES OR "CONFIG_SUPPORT_BLE" IN_LIST DEFINES)
    add_subdirectory_if_exist(gap)
endif()

add_subdirectory_if_exist(interface)