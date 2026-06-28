#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

if (PUBLIC_DEFINES MATCHES "GPU_1A21")
    set(VERSION_PATH v2)
elseif (PUBLIC_DEFINES MATCHES "GPU_3322")
    set(VERSION_PATH v3)
elseif (PUBLIC_DEFINES MATCHES "GPU_1A22")
    set(VERSION_PATH v4)
else()
    set(VERSION_PATH v1)
endif()

set(
    comm_drv_list
        ${CMAKE_CURRENT_SOURCE_DIR}/crg/gpu_crg_intf.c
        ${CMAKE_CURRENT_SOURCE_DIR}/crg/gpu_crg_comm.c
        ${CMAKE_CURRENT_SOURCE_DIR}/crg/${VERSION_PATH}/gpu_crg.c
)

set(GPU_SRC_LIST "${GPU_SRC_LIST}" "${comm_drv_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    comm_drv_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/crg
        ${CMAKE_CURRENT_SOURCE_DIR}/crg/${VERSION_PATH}
)

set(GPU_INC_LIST "${GPU_INC_LIST}" "${comm_drv_header_list}" CACHE INTERNAL "platform src obj list" FORCE)
