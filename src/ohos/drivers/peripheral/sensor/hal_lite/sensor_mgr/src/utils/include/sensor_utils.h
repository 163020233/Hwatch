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

#ifndef SENSOR_HAL_UTILS_H
#define SENSOR_HAL_UTILS_H

#include "sensor_typedef.h"
#include "hsensormgr_service.h"
#include "vsensormgr_service.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define RELY_SENSOR_CNT_MAX 10
#define SENSOR_MAX_NUM 10       /* 支持的最大的传感器数量 */
#define VSENSOR_MAX_NUM 10      /* 虚拟传感器的最大数目 */

/* 初始化util模块 */
void SensorUtilsInit(void);
Sensor *SensorUtilsFindSensor(enum SensorTypeTag type);
Sensor *SensorUtilsRequestSensor(enum SensorTypeTag type);
char *SensorUtilsGetSimpleName(enum SensorTypeTag type);
HsensorListener *SensorUtilsRequestListener(void);
void SensorUtilsFreeListener(HsensorListener *listener);
int32_t SensorUtilsAddHsensorList(HsensorScb *hsensorScb);
int32_t SensorUtilsAddVsensorList(enum SensorTypeTag type);
int32_t SensorUtilsHsensorCheck(const Hsensor *hsensor);
int32_t SensorUtilsVsensorCheck(const Vsensor *vsensor);
bool SensorUtilsCheckHSensorType(enum SensorTypeTag type);
void SensorUtilsRegisterVsensorType(enum SensorTypeTag type);
bool SensorUtilsCheckVSensorType(enum SensorTypeTag type);
bool SensorUtilsIsVSensor(enum SensorTypeTag type);
int32_t SensorUtilsUserInfoCheck(SensorPersonInfo *info);
/* 获取全部sensor */
int32_t SensorUtilsGetAllSensors(struct SensorInformation **sensorInfo, int32_t *count);

void SensorUtilsRegisterTaskTable(enum SensorTypeTag type, uint32_t taskId);
void SensorUtilsUnRegisterTaskTable(enum SensorTypeTag type, uint32_t taskId);
uint32_t SensorUtilsGetRegisteredTaskId(enum SensorTypeTag type);
enum SensorTypeTag SensorUtilsGetRegisteredType(uint32_t taskId);
/* 判断sensor有没有被注册 */
bool SensorUtilsSensorIsRegistered(enum SensorTypeTag type);
uint64_t SensorUtilsGetRtcTimeMs(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
