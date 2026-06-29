#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(PLATFORM_PATH wear)

if (DEFINES MATCHES "HI3322_CHIP_V100" OR DEFINES MATCHES "HI3322_PRODUCT_FPGA" )
set(
    base_drv_list
        ${CMAKE_CURRENT_SOURCE_DIR}/osal/${PLATFORM_PATH}/drv_mcp_dependency.c
)
elseif(PRIVATE_DEFINES MATCHES "VB_SUPPORT_EXTERNAL_QUEUE")
set(
    base_drv_list
        ${CMAKE_CURRENT_SOURCE_DIR}/osal/${PLATFORM_PATH}/drv_mcp_dependency.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vb/vb_core.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vb/vb_queue.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vb/vb_supplement.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vb/vb_utils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vb/comm/vb_compress.c
)
else()

set(BASE_CHIP_NAME 1A22)
set(
    base_drv_list
        ${CMAKE_CURRENT_SOURCE_DIR}/osal/${PLATFORM_PATH}/drv_mcp_dependency.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vb/vb_core.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vb/vb_supplement.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vb/vb_utils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vb/comm/vb_compress.c
)
endif()

set(MCP_SRC_LIST "${MCP_SRC_LIST}" "${base_drv_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    base_drv_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}/vb
        ${CMAKE_CURRENT_SOURCE_DIR}/osal/${PLATFORM_PATH}
)
set(MCP_INC_LIST "${MCP_INC_LIST}" "${base_drv_header_list}" CACHE INTERNAL "platform src obj list" FORCE)


