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

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "securec.h"
#include "sensor_thread.h"
#include "sensor_common.h"
#include "sensor_if.h"
#include "sensor_manager.h"

struct SensorInterfaceContext {
    bool inited;
    MutexId mutex;
};

struct SensorInterfaceContext g_sensorInterfaceCtx = {
    .inited = false,
    .mutex = NULL,
};

static void GetSensorDeviceMethods(struct SensorInterface *device);

const struct SensorInterface *NewSensorInterfaceInstance(void)
{
    g_sensorInterfaceCtx.mutex = SensorMutexCreate(NULL);
    if (g_sensorInterfaceCtx.mutex == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "mutex is null");
        return NULL;
    }
    SensorMutexLock(g_sensorInterfaceCtx.mutex);
    static struct SensorInterface sensorDevInstance;
    if (g_sensorInterfaceCtx.inited) {
        SensorMutexUnLock(g_sensorInterfaceCtx.mutex);
        return &sensorDevInstance;
    }
    int32_t ret = SensorMgrInit();
    if (ret != SENSOR_OK) {
        SensorMutexUnLock(g_sensorInterfaceCtx.mutex);
        HILOG_ERROR(HILOG_MODULE_SEN, "SensorMgrInit failed");
        return NULL;
    }

    // Construct device interface instance
    GetSensorDeviceMethods(&sensorDevInstance);

    g_sensorInterfaceCtx.inited = true;
    SensorMutexUnLock(g_sensorInterfaceCtx.mutex);
    HILOG_DEBUG(HILOG_MODULE_APP, "Get sensor instance success");
    return &sensorDevInstance;
}

int32_t FreeSensorInterfaceInstance(void)
{
    SensorMutexLock(g_sensorInterfaceCtx.mutex);
    if (!g_sensorInterfaceCtx.inited) {
        SensorMutexUnLock(g_sensorInterfaceCtx.mutex);
        return SENSOR_OK;
    }
    SensorMgrDeInit();
    g_sensorInterfaceCtx.inited = false;
    SensorMutexUnLock(g_sensorInterfaceCtx.mutex);
    SensorMutexDestroy(&g_sensorInterfaceCtx.mutex);
    return SENSOR_OK;
}

static int32_t GetSensorInfo(struct SensorInformation **sensorInfo, int32_t *count)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "enter");
    SensorMutexLock(g_sensorInterfaceCtx.mutex);
    int32_t ret = SensorMgrGetAllSensors(sensorInfo, count);
    SensorMutexUnLock(g_sensorInterfaceCtx.mutex);
    return ret;
}

static int32_t EnableSensor(int32_t sensorTypeId)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "enter");
    SensorMutexLock(g_sensorInterfaceCtx.mutex);
    int32_t ret = SensorMgrEnable(sensorTypeId);
    SensorMutexUnLock(g_sensorInterfaceCtx.mutex);
    return ret;
}

static int32_t DisableSensor(int32_t sensorTypeId)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "enter");
    SensorMutexLock(g_sensorInterfaceCtx.mutex);
    int32_t ret = SensorMgrDisable(sensorTypeId);
    SensorMutexUnLock(g_sensorInterfaceCtx.mutex);
    return ret;
}

static int32_t RegisterSensorDataCb(int32_t sensorTypeId, RecordDataCallback cb)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "enter sensorId:%d", sensorTypeId);
    SensorMutexLock(g_sensorInterfaceCtx.mutex);
    int32_t ret = SensorMgrRegistDataCallback(sensorTypeId, cb);
    SensorMutexUnLock(g_sensorInterfaceCtx.mutex);
    return ret;
}

static int32_t UnregisterSensorDataCb(int32_t sensorTypeId, RecordDataCallback cb)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "enter sensorId:%d", sensorTypeId);
    SensorMutexLock(g_sensorInterfaceCtx.mutex);
    int32_t ret = SensorMgrUnregistDataCallback(sensorTypeId, cb);
    SensorMutexUnLock(g_sensorInterfaceCtx.mutex);
    return ret;
}

static int32_t SetSensorBatch(int32_t sensorTypeId, int64_t samplingInterval, int64_t interval)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "enter");
    SensorMutexLock(g_sensorInterfaceCtx.mutex);
    int32_t ret = SensorMgrSetBatch(sensorTypeId, samplingInterval, interval);
    SensorMutexUnLock(g_sensorInterfaceCtx.mutex);
    return ret;
}

static int32_t SetSensorMode(int32_t sensorTypeId, int32_t mode)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "enter");
    SensorMutexLock(g_sensorInterfaceCtx.mutex);
    int32_t ret = SensorMgrSetMode(sensorTypeId, mode);
    SensorMutexUnLock(g_sensorInterfaceCtx.mutex);
    return ret;
}

static int32_t SetSensorOption(int32_t sensorTypeId, uint32_t option)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "enter");
    SensorMutexLock(g_sensorInterfaceCtx.mutex);
    int32_t ret = SensorMgrSetOption(sensorTypeId, option);
    SensorMutexUnLock(g_sensorInterfaceCtx.mutex);
    return ret;
}

static int32_t SensorIfGetData(int32_t sensorTypeId, SensorData *sensorData)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "enter");
    return SensorMgrGetData(sensorTypeId, sensorData);
}

void GetSensorDeviceMethods(struct SensorInterface *device)
{
    CHECK_NULL_PTR_RETURN(device);
    device->GetAllSensors = GetSensorInfo;
    device->Enable = EnableSensor;
    device->Disable = DisableSensor;
    device->SetBatch = SetSensorBatch;
    device->SetMode = SetSensorMode;
    device->SetOption = SetSensorOption;
    device->Register = RegisterSensorDataCb;
    device->Unregister = UnregisterSensorDataCb;
    device->GetSensorData = SensorIfGetData;
}
