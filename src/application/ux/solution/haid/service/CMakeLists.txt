#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(
    hearing_comm_src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/haid_service_handle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/haid_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/haid_event_handle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/haid_diag.c
    ${CMAKE_CURRENT_SOURCE_DIR}/haid_tone.c
)
set(HEARING_SERVICE_LIST "${HEARING_SERVICE_LIST}" "${hearing_comm_src_list}" CACHE INTERNAL "hearing ram obj list" FORCE)