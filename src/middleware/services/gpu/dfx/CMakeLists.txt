#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================

if (PUBLIC_DEFINES MATCHES "GPU_SUPPORT_CHR")
    add_subdirectory(chr)
endif()

set(
    dfx_header_list
        ${CMAKE_CURRENT_SOURCE_DIR}
)
set(GPU_INC_LIST "${GPU_INC_LIST}" "${dfx_header_list}" CACHE INTERNAL "platform src obj list" FORCE)
