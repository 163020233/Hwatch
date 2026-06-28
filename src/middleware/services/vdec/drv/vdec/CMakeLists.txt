#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(
    vdec_drv_src
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/drv_vdec_dev.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/drv_vdec.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/platform/osal/liteos/drv_vdec_osal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/platform/sdk/fbb/drv_vdec_sdk.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/vfmw_adpt/vfmw_v7.0/drv_vdec_vfmw.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/vfmw_adpt/vfmw_v7.0/drv_vdec_convert.c
)

set(
    vdec_drv_proc_src
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/modules/proc/drv_vdec_proc.c
)

set(
    vdec_drv_heif_src
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/modules/heif/drv_vdec_heif.c
)

set(VDEC_SRC_LIST "${VDEC_SRC_LIST}" "${vdec_drv_src}" CACHE INTERNAL "platform src obj list" FORCE)

if(${vdec_proc_support} STREQUAL "yes")
set(VDEC_SRC_LIST "${VDEC_SRC_LIST}" "${vdec_drv_proc_src}"  CACHE INTERNAL "platform src obj list" FORCE)
endif()

if(${vdec_heif_support} STREQUAL "yes")
set(VDEC_SRC_LIST "${VDEC_SRC_LIST}" "${vdec_drv_heif_src}"  CACHE INTERNAL "platform src obj list" FORCE)
endif()

set(
    vdec_drv_header
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/platform/osal/liteos
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/platform/sdk/fbb
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/vfmw_adpt/vfmw_v7.0
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/modules/proc
)

set(
    vdec_drv_heif_header
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/modules/heif
)

set(VDEC_INC_LIST "${VDEC_INC_LIST}" "${vdec_drv_header}" CACHE INTERNAL "platform src obj list" FORCE)

if(${vdec_heif_support} STREQUAL "yes")
set(VDEC_INC_LIST "${VDEC_INC_LIST}" "${vdec_drv_heif_header}"  CACHE INTERNAL "platform src obj list" FORCE)
endif()
