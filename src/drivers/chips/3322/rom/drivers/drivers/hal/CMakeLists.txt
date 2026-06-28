#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================

add_subdirectory_if_exist(systick)
add_subdirectory_if_exist(tcxo)
add_subdirectory_if_exist(watchdog)
add_subdirectory_if_exist(pinmux)
add_subdirectory_if_exist(rtc_unified)
add_subdirectory_if_exist(pmp)
