#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
set(
    hdr_list
        ${CMAKE_CURRENT_SOURCE_DIR}/vau_hdr_process.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vau_hdr_node.c
        ${CMAKE_CURRENT_SOURCE_DIR}/alg/hdr_algorithm.c
        ${CMAKE_CURRENT_SOURCE_DIR}/alg/cuva_api_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/alg/cuva_coef_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/alg/cuva_api.c
)
set(GPU_SRC_LIST "${GPU_SRC_LIST}" "${hdr_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    hdr_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/alg
)
set(GPU_INC_LIST "${GPU_INC_LIST}" "${hdr_header_list}" CACHE INTERNAL "platform src obj list" FORCE)
