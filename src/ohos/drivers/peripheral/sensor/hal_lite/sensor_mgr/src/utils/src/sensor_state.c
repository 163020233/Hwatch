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

#include "sensor_state.h"
#include "sensor_log.h"
#include "sensor_utils.h"
#include "sensor_common.h"

void SensorSetState(enum SensorTypeTag type, SensorState state)
{
    Sensor *sensor = NULL;

    sensor = SensorUtilsFindSensor(type);
    if (sensor == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[STATE] cannot get sensor:%d, sensor may be not registered\n", type);
        return;
    }

    sensor->sensorInfo.state = state;
}

void SensorSetPara(enum SensorTypeTag type, const SensorPara *para)
{
    Sensor *sensor = NULL;
    sensor = SensorUtilsFindSensor(type);
    if (sensor == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[STATE] cannot get sensor:%d, sensor may be not registered\n", type);
        return;
    }

    if (para == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[ATASK] sensor open para NULL!");
        return;
    }
    sensor->sensorInfo.openPara.period = para->period;
    sensor->sensorInfo.openPara.batch = para->batch;
    sensor->sensorInfo.openPara.mode = para->mode;
    sensor->sensorInfo.openPara.option = para->option;

    HILOG_INFO(HILOG_MODULE_SEN,
        "[UTIL][storge msg] period:%d batch:%d mode:%d option:%d\n",
        sensor->sensorInfo.openPara.period,
        sensor->sensorInfo.openPara.batch,
        sensor->sensorInfo.openPara.mode,
        sensor->sensorInfo.openPara.option);
}