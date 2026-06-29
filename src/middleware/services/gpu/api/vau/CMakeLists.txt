#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================

if (PUBLIC_DEFINES MATCHES "GPU_SDF_SUPPORT")
    add_subdirectory_if_exist(sdf)
endif()

set(
    vau_api_list
        ${CMAKE_CURRENT_SOURCE_DIR}/soc_vau_api.c
        ${CMAKE_CURRENT_SOURCE_DIR}/blur/vau_blur.c
        ${CMAKE_CURRENT_SOURCE_DIR}/draw/vau_draw.c
        ${CMAKE_CURRENT_SOURCE_DIR}/matrix/vau_matrix.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compress/vau_compress.c
        ${CMAKE_CURRENT_SOURCE_DIR}/debug/vau_debug.c
)
set(GPU_SRC_LIST "${GPU_SRC_LIST}" "${vau_api_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    vau_api_header_list
        ${ROOT_DIR}/middleware/services/gpu/rm/vau/inc
        ${ROOT_DIR}/middleware/services/gpu/rm/vau/inc/math
        ${ROOT_DIR}/middleware/services/gpu/rm/vau/include
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/blur
        ${CMAKE_CURRENT_SOURCE_DIR}/draw
        ${CMAKE_CURRENT_SOURCE_DIR}/matrix
        ${CMAKE_CURRENT_SOURCE_DIR}/compress
        ${CMAKE_CURRENT_SOURCE_DIR}/debug
)
set(GPU_INC_LIST "${GPU_INC_LIST}" "${vau_api_header_list}" CACHE INTERNAL "platform src obj list" FORCE)