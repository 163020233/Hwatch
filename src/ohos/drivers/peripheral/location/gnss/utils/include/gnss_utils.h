/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef GNSS_UTILS_H
#define GNSS_UTILS_H
#include <stdint.h>
#include "gnss_nmea_parse.h"
#include "location_vendor_lib.h"

#pragma pack(1)

typedef struct {
    uint16_t year;   // YYYY
    uint8_t month;   // MM, 1~12
    uint8_t day;     // DD, 1~31
    uint8_t hour;    // hh, 0~23
    uint8_t minute;  // mm, 0~59
    uint8_t second;  // ss, 0~59
    uint16_t ms;     // ms, 0~999
    uint8_t valid;   // 0: invalid, 1: valid
} GnssUtcTime;

#pragma pack()

int32_t LocationGnssUtilsUtc2UnixTime(const GnssUtcTime *utcTime, uint32_t *unixTime);

int64_t LocationGnssUtilsGetTimestampMS(void);

int32_t LocationGnssUtilsFillGnssLocation(const GnssInfo *gnssInfo, OHOS::HDI::Location::GnssLocation *location);

int32_t LocationGnssUtilsFillAllSatelliteStatus(const GnssInfo *gnssInfo,
                                                OHOS::HDI::Location::GnssSatelliteStatus *status);

#endif