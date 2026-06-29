#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================

set(PRIVATE_DEFINES
    SOCT_VIU_SEND_YUV_SUPPORT
    SOCT_VIU_3DNRX_SUPPORT
    SOCT_ISP_SUPPORT
	CONFIG_CHIP_PLATFORM_WEAR
#   SOCT_VIU_ONLY_VICAP
)

set(
    viu_api_list
        ${CMAKE_CURRENT_SOURCE_DIR}/mpi_viu.c        
)
set(MCP_SRC_LIST "${MCP_SRC_LIST}" "${viu_api_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    viu_api_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}
)
set(MCP_INC_LIST "${MCP_INC_LIST}" "${viu_api_header_list}" CACHE INTERNAL "platform src obj list" FORCE)
