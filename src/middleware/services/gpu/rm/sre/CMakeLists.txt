#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================

set(
    sre_src_list
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sre_api.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sre_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sre_process.c
)
set(GFX_RM_SRC_LIST "${GFX_RM_SRC_LIST}" "${sre_src_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    sre_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
set(GFX_RM_INC_LIST "${GFX_RM_INC_LIST}" "${sre_header_list}" CACHE INTERNAL "platform src obj list" FORCE)
