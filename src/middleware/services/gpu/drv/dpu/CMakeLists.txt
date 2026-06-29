#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
if (PUBLIC_DEFINES MATCHES "GPU_3322")
    set(VERSION_PATH v2)
elseif (PUBLIC_DEFINES MATCHES "GPU_1A22")
    set(VERSION_PATH v3)
else()
    set(VERSION_PATH v1)
endif()

set(
    dpu_drv_list
        ${CMAKE_CURRENT_SOURCE_DIR}/intf/drv_dpu_intf.c
        ${CMAKE_CURRENT_SOURCE_DIR}/intf/drv_dpu_boot_logo.c
        ${CMAKE_CURRENT_SOURCE_DIR}/dfx/dpu_param_print.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ioctl/dpu_ioctl.c
        ${CMAKE_CURRENT_SOURCE_DIR}/layer/dpu_layer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/disp/dpu_disp.c
        ${CMAKE_CURRENT_SOURCE_DIR}/isr/dpu_isr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/dfx/dpu_dfx.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/${VERSION_PATH}/dpu_hal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/${VERSION_PATH}/dpu_hal_disp.c
        ${CMAKE_CURRENT_SOURCE_DIR}/qspi/dpu_qspi.c
        ${CMAKE_CURRENT_SOURCE_DIR}/affine/dpu_affine.c
)

set(
    dpu_drv_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/dfx
        ${CMAKE_CURRENT_SOURCE_DIR}/disp
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/${VERSION_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/hal
        ${CMAKE_CURRENT_SOURCE_DIR}/intf
        ${CMAKE_CURRENT_SOURCE_DIR}/ioctl
        ${CMAKE_CURRENT_SOURCE_DIR}/isr
        ${CMAKE_CURRENT_SOURCE_DIR}/layer
        ${CMAKE_CURRENT_SOURCE_DIR}/qspi
        ${CMAKE_CURRENT_SOURCE_DIR}/affine
        ${CMAKE_CURRENT_SOURCE_DIR}/sre
)

if (PUBLIC_DEFINES MATCHES "GPU_SRE_SUPPORT")
    list(APPEND dpu_drv_list
        ${CMAKE_CURRENT_SOURCE_DIR}/sre/dpu_sre.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/dpu_hal_acc.c)
    list(APPEND dpu_drv_header_list
        ${ROOT_DIR}/middleware/services/gpu/rm/sre/inc)
endif()

if (PUBLIC_DEFINES MATCHES "GPU_1A22")
    list(APPEND dpu_drv_list
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/${VERSION_PATH}/dpu_hal_layer.c)
else()
    list(APPEND dpu_drv_list
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/dpu_hal_layer.c)
endif()

set(GPU_SRC_LIST "${GPU_SRC_LIST}" "${dpu_drv_list}" CACHE INTERNAL "platform src obj list" FORCE)
set(GPU_INC_LIST "${GPU_INC_LIST}" "${dpu_drv_header_list}" CACHE INTERNAL "platform src obj list" FORCE)
