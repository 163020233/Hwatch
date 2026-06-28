#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(
    vdec_api_list
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/soc_vdec_api.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/decoder_store/hardware/vdec_decoder_hw.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/decoder_store/vdec_hal/vdec_hal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/decoder_store/vdec_store.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/platform/osal/liteos/mpi_vdec_osal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/platform/sdk/fbb/mpi_vdec_sdk.c
)

if(${vdec_mjpeg_support} STREQUAL "yes")
set(
    vdec_mjpeg_list
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/decoder_store/mjpeg/vdec_decoder_mjpeg.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/decoder_store/mjpeg/vdec_jpeg_hal.c
)
endif()

set(VDEC_SRC_LIST "${VDEC_SRC_LIST}" "${vdec_api_list}" "${vdec_mjpeg_list}" CACHE INTERNAL "platform src obj list" FORCE)
set(
    vdec_api_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/decoder_store
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/decoder_store/hardware
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/decoder_store/vdec_hal
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/platform/osal/liteos
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/platform/sdk/fbb
        ${CMAKE_CURRENT_SOURCE_DIR}/vdec_v7.0/decoder_store/mjpeg
)
set(VDEC_INC_LIST "${VDEC_INC_LIST}" "${vdec_api_header_list}" CACHE INTERNAL "platform src obj list" FORCE)
