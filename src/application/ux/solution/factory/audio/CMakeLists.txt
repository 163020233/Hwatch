#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(
    audio_fac_common_src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/audio_calib_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/audio_calib_service_handle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/audio_loop.c
)

set(UX_FACTORY_LIST "${UX_FACTORY_LIST}" "${audio_fac_common_src_list}" CACHE INTERNAL "factory ram obj list" FORCE)