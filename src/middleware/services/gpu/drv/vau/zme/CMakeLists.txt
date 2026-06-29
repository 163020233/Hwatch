#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
set(
    zme_list
        ${CMAKE_CURRENT_SOURCE_DIR}/alg/vau_zme_alg.c
)
set(GPU_SRC_LIST "${GPU_SRC_LIST}" "${zme_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    zme_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/alg
)
set(GPU_INC_LIST "${GPU_INC_LIST}" "${zme_header_list}" CACHE INTERNAL "platform src obj list" FORCE)