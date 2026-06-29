/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef SENSOR_TMSG_H
#define SENSOR_TMSG_H

#include "sensor_typedef.h"
#include "sensor_task.h"

/* FA线程间消息定义 */
typedef enum {
    SENSOR_TMSG_INVALID = 0,

    /* 虚拟vsensor管理消息 */
    SENSOR_TMSG_HSENSOR_DATA_UPDATE,
    SENSOR_TMSG_VSENSOR_DATA_UPDATE,
    SENSOR_TMSG_VSENSOR_REG_EVENT,
    SENSOR_TMSG_VSENSOR_OPEN_EVENT,
    SENSOR_TMSG_VSENSOR_CLOSE_EVENT,
    SENSOR_TMSG_VSENSOR_RELY_OPEN_EVENT,
    SENSOR_TMSG_VSENSOR_RELY_CLOSE_EVENT,

    SENSOR_TMSG_MAX_ID
} SensorTmsgId;

typedef struct {
    SensorTmsgId id;
    uintptr_t value;
    uint64_t ts;
    void *info;
    uint32_t infoSize;
} SensorTmsg;

typedef struct {
    SensorTmsgId tmsgId;
    int32_t (*tmsgProc)(const SensorTmsg* tmsg);
} SensorTmsgProcMap;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int32_t SensorTmsgSend(uint32_t taskId, SensorTmsg* tmsg);
int32_t SensorTmsgRecv(uint32_t taskId, SensorTmsg* tmsg);
int32_t SensorTmsgFree(SensorTmsg* tmsg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif