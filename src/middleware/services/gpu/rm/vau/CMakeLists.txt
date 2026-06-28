#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================

set(
    vau_src_list
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vau_rm_api.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/comm/rm_comm.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/blur/rm_blur.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mapping/rm_mapping.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/math/rm_math.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/math/float_t.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vau_stroke_path.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vau_stroke_dash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vau_arc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/alg_3d/rm_mesh_3d_alg.c
)
set(GFX_RM_SRC_LIST "${GFX_RM_SRC_LIST}" "${vau_src_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    vau_header_list
        ${ROOT_DIR}/middleware/utils/common_headers/native
        ${ROOT_DIR}/middleware/utils/common_headers/gpu/external
        ${ROOT_DIR}/middleware/utils/common_headers/gpu/internal
        ${ROOT_DIR}/middleware/services/gpu/dfx
        ${ROOT_DIR}/middleware/services/gpu/api/vau/debug
        ${ROOT_DIR}/middleware/services/gpu/drv/vau/mem
        ${ROOT_DIR}/middleware/services/gpu/drv/include
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/comm
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/blur
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/mapping
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/math
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/3d
)
set(GFX_RM_INC_LIST "${GFX_RM_INC_LIST}" "${vau_header_list}" CACHE INTERNAL "platform src obj list" FORCE)