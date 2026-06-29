/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: gnss common
 */

#ifndef GNSS_COMMON_H
#define GNSS_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include "errcode.h"
#include "gnss_datatypes.h"

uint16_t get_check_sum(uint8_t *buff, uint16_t len);
void get_sys_utc_time(GnssUtcTime* utcTime);
int32_t utc_to_unixtime(GnssUtcTime utcTime, uint32_t *unixTime);
void extra_time_from_nmea(const char* nmea, GpsTime *gps_time);

#endif