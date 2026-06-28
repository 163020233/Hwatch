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

#ifndef SENSOR_TASK_COMM_H
#define SENSOR_TASK_COMM_H

#include <stdbool.h>
#include "sensor_typedef.h"
#include "hsensormgr_service.h"
#include "sensor_task.h"
#include "sensor_tmsg.h"
#include "sensor_utils.h"

#define VSENSOR_TASK_HLISTENER_MAX_NUM 10    /* FA算法线程支持的物理传感器listener的最大数量设置 */
#define FA_VSENSOR_BE_RELYED_NUM 10      /* 虚拟sensor被依赖的任务最大个数 */
#define VSENSOR_RESULT_TS_OFFSET 1000 /* 虚拟传感器上报的时间戳与获取的时间戳的矫正间隔值 */

enum {
    APP_CTRL = 0,
    RELY_CTRL,
};

typedef struct {
    bool appOpenFlag;
    uint32_t relyedCnt; /* 被其他算法依赖的次数 */
} VsensorTaskRelyedInfo;

typedef struct {
    enum SensorTypeTag type;
    SensorState state;
    Vsensor *vsensor;
    VsensorTaskRelyedInfo relyedInfo;
} VsensorTaskInfo;

typedef struct {
    enum SensorTypeTag type;
    HsensorListener hlistener;
} VsensorTaskHlistener;

typedef struct {
    enum SensorTypeTag sensor;
    uint32_t dataLen;
    uint64_t timestamp;
    uint8_t sensordata[];
} VsensorProcessData;

typedef struct {
    uint32_t taskId;
    char *taskPrefix;
    NotifyCallback hdataCb; /* hlistener中的数据回调函数，若为NULL，将直接使用atask类默认的数据回调函数 */
    VsensorTaskHlistener hlistenersList[VSENSOR_TASK_HLISTENER_MAX_NUM];
    VsensorTaskInfo vsensorInfo;
} VsensorTaskComm;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int32_t VsensorTaskCheckCommComponents(const VsensorTaskComm *component);
bool VsensorTaskVsensorIsRunning(const VsensorTaskInfo *vsensorInfo);
void VsensorTaskCommInit(VsensorTaskComm *component);
int32_t VsensorTaskVsensorRegEvent(VsensorTaskComm *component, Vsensor *vsensor);
int32_t VsensorTaskSensorDataUpdateEvent(VsensorTaskComm *component, VsensorProcessData *data);

int32_t VsensorTaskVsensorOpenEvent(
    VsensorTaskComm *component, enum SensorTypeTag type, SensorPara *para, uint32_t openType);
int32_t VsensorTaskVsensorCloseEvent(VsensorTaskComm *component, enum SensorTypeTag type, uint32_t closeType);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* VSENSOR_TASK_COMM_H */