/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: header of log memory section
 * Author: audio
 * Create: 2022-10-15
 */

#ifndef LOG_MEMORY_SECTION_H
#define LOG_MEMORY_SECTION_H

#include "mem_addr.h"
#include "core_interface.h"

typedef struct {
    volatile td_u32 read;
    volatile td_u32 write;
} log_memory_section_control;

typedef struct {
    td_u32 offset[LOG_MEMORY_REGION_MAX_NUMBER];
    td_u32 length;
    log_memory_section_control section_control[LOG_MEMORY_REGION_MAX_NUMBER];
} log_memory_region_control;

typedef struct {
    td_u8 *start;
    td_u32 length;
} log_memory_section_params;

void log_memory_section_get(log_memory_region_section section, log_memory_section_params *section_data);

log_memory_section_control *log_memory_section_get_control(log_memory_region_section section);

#define pointer_in_log_memory_region(x) (((td_u32)(x) >= (SAP_LOGGING_REGION_START)) && \
                                         ((td_u32)(x) <= ((SAP_LOGGING_REGION_START) + (SAP_LOGGING_REGION_LENGTH))))

#endif
