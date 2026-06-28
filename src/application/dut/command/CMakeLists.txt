#===============================================================================
# @brief    cmake entry
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

add_subdirectory_if_exist(audio)
add_subdirectory_if_exist(media)
add_subdirectory_if_exist(bt)
add_subdirectory_if_exist(ohosfwk)
add_subdirectory_if_exist(plt)

if(${CHIP} STREQUAL "socmn1")
    add_subdirectory_if_exist(${CHIP})
endif()