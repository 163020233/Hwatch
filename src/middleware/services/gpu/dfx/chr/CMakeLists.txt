#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(
    chr_list 
        ${CMAKE_CURRENT_SOURCE_DIR}/gpu_chr.c
)
set(GPU_SRC_LIST "${GPU_SRC_LIST}" "${chr_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    chr_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}
)
set(GPU_INC_LIST "${GPU_INC_LIST}" "${chr_header_list}" CACHE INTERNAL "platform src obj list" FORCE)
