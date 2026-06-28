#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================

add_subdirectory_if_exist(gnss_at_samples)
add_subdirectory_if_exist(gnss_at_samples20)
add_subdirectory_if_exist(gnss_sample)
add_subdirectory_if_exist(realtime_pdr_sample)
add_subdirectory_if_exist(agnss_sample)
add_subdirectory_if_exist(pgnss_sample)
add_subdirectory_if_exist(replay_pdr_sample)
if (DEFINED CONFIG_SUPPORT_GNSS_REPLAY OR "CONFIG_SUPPORT_GNSS_REPLAY" IN_LIST DEFINES)
add_subdirectory_if_exist(replay_rtk_sample)
endif()
