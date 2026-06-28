#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================

set(PRIVATE_DEFINES
)

set(
    bf20a6_api_list
        ${CMAKE_CURRENT_SOURCE_DIR}/bf20a6.c
)
set(MCP_SRC_LIST "${MCP_SRC_LIST}" "${bf20a6_api_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    bf20a6_api_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}
)
set(MCP_INC_LIST "${MCP_INC_LIST}" "${bf20a6_api_header_list}" CACHE INTERNAL "platform src obj list" FORCE)