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

#ifndef SENSOR_PARA_DECISION_H
#define SENSOR_PARA_DECISION_H

#include "sensor_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SENSOR_PARA_REALTIME_PERIOD_MIN 1
#define SENSOR_PARA_REALTIME_PERIOD_MAX 1000000
#define SENSOR_PARA_BATCH_MIN 1
#define SENSOR_PARA_BATCH_MAX 100

void SensorParaDefaultInit(SensorPara *sensorPara);
void SensorParaClear(SensorPara *para);
void SensorParaCopy(SensorPara *dest, const SensorPara *src);
int32_t SensorOpenParaDecision(
    enum SensorTypeTag type, const SensorPara *cur, const SensorPara *require, SensorPara *result);
int32_t SensorOptionDecision(enum SensorTypeTag type, int32_t cur, int32_t require);
bool SensorParaIsClear(const SensorPara *para);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SENSOR_PARA_DECISION_H */