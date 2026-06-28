#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory(source)
set(
    vdec_api_include_list
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set(VDEC_INC_LIST "${VDEC_INC_LIST}" "${vdec_api_include_list}" CACHE INTERNAL "platform src obj list" FORCE)
