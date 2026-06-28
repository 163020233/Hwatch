#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================

if ("CONFIG_NON_TEE_SUPPORT" IN_LIST DEFINES)
    include(${CMAKE_CURRENT_SOURCE_DIR}/tee_mini.cmake)
else()
    include(${CMAKE_CURRENT_SOURCE_DIR}/tee.cmake)
endif()