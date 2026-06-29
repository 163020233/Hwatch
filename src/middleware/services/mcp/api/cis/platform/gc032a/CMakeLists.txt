#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================

set(PRIVATE_DEFINES
)

set(
    gc032a_api_list
        ${CMAKE_CURRENT_SOURCE_DIR}/gc032a.c
)
set(MCP_SRC_LIST "${MCP_SRC_LIST}" "${gc032a_api_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    gc032a_api_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}
)
set(MCP_INC_LIST "${MCP_INC_LIST}" "${gc032a_api_header_list}" CACHE INTERNAL "platform src obj list" FORCE)