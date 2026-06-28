/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: agnss sample
 * Author:
 * Create:
 */
#ifndef GNSS_COMMON_PROC_H
#define GNSS_COMMON_PROC_H

#include <stdio.h>
#include <stdint.h>
#include "errcode.h"
#include "securec.h"
#include "gnss_datatypes.h"

void update_binary_save_flag(uint8_t flag);
void update_nmea_save_flag(uint8_t flag);
void gnss_proc_binary_msg(uint8_t *buff, uint32_t len);
void gnss_proc_nmea_msg(uint8_t *buff, uint32_t len);
errcode_t gnss_init(void);
errcode_t gnss_start(void);
errcode_t gnss_stop(void);
errcode_t gnss_deinit(void);
errcode_t gnss_inject_time(GnssUtcTime *utcTime);
errcode_t encode_and_inject_cmd(GnssCmdType cmdType, uint8_t* inBuff, uint32_t inLen);
void update_gnss_nmea_print_flag(uint8_t index);
errcode_t gnss_board_config(uint8_t posType, uint8_t xgnssType);

#endif