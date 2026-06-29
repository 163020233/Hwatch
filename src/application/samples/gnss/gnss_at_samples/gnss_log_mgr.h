/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: gnss sample log mgr
 */

#ifndef GNSS_LOG_MGR_H
#define GNSS_LOG_MGR_H

#include <stdint.h>
#include <stddef.h>
#include "errcode.h"

void init_gnss_log_ctrl_info(void);
errcode_t update_gnss_log_ctrl_info(uint32_t logNumLimit, uint32_t logSizeLimit);
void gnss_save_data(uint8_t *buff, uint32_t size);
void output_log(const char* logLevel, const char* fmt, ...);
void flash_gnss_log(void);

#endif