/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "sensor_service_impl.h"

#include <securec.h>
#include <stdlib.h>
#include <string.h>
#include "ipc_skeleton.h"
#include "sensor_service.h"
#include "sensor_type.h"

static struct SensorInformation *g_sensorLists = NULL;
static int32_t g_sensorListsLength = 0;
static SensorInfo *g_sensorInfos = NULL;
const struct SensorInterface *g_sensorDevice;
static Identity g_svcIdentity;

static int32_t InitSensorList(void)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "[SERVICE:%s]: %s begin", SENSOR_SERVICE, __func__);
#ifdef HAS_HDI_SENSOR_LITE_PRAT
    if (g_sensorDevice == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s g_sensorDevice is NULL",
            SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    int32_t ret = g_sensorDevice->GetAllSensors(&g_sensorLists, &g_sensorListsLength);
    if ((ret != 0) || (g_sensorLists == NULL) || (g_sensorListsLength == 0)) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s getAllSensors failed, ret: %d",
            SENSOR_SERVICE, __func__, ret);
        return SENSOR_ERROR_INVALID_PARAM;
    }
#endif // HAS_HDI_SENSOR_LITE_PRAT
    // ohos-hisi begin
    HILOG_DEBUG(HILOG_MODULE_SEN, "[SERVICE:%s]: sensorListsLength:%d", SENSOR_SERVICE, g_sensorListsLength);
    if (g_sensorInfos != NULL) {
        return SENSOR_OK;
    }
    g_sensorInfos = malloc(g_sensorListsLength * sizeof(SensorInfo));
    if (g_sensorInfos == NULL) {
        return SENSOR_ERROR_INVALID_PARAM;
    }
    for (int32_t i = 0; i < g_sensorListsLength; i++) {
        HILOG_DEBUG(HILOG_MODULE_SEN,
            "[SERVICE:%s]: index:%d  sensorName:%s, sensorTypeId:%d",
            SENSOR_SERVICE,
            i,
            g_sensorLists[i].sensorName,
            g_sensorLists[i].sensorTypeId);
        g_sensorInfos[i].sensorTypeId = g_sensorLists[i].sensorTypeId;
    }
    // ohos-hisi end
    return SENSOR_OK;
}

const char *SENSOR_GetName(Service *service)
{
    (void)service;
    HILOG_DEBUG(HILOG_MODULE_APP, "[SERVICE:%s]: %s begin",
        SENSOR_SERVICE, __func__);
    return SENSOR_SERVICE;
}

// ohos-hisi begin
#ifdef SUPPORT_SMALL
// ohos-hisi end
static int SensorDataCallback(const struct SensorEvent *event)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "[SERVICE:%s]: %s begin", SENSOR_SERVICE, __func__);
    if ((event == NULL) || (event->dataLen == 0)) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s event is NULL",
            SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_ID;
    }
    IpcIo io;
    uint8_t data[MAX_IO_SIZE];
    IpcIoInit(&io, data, MAX_IO_SIZE, IPC_MAX_OBJECTS);

    WriteUint32(&io, (uint32_t)(sizeof(struct SensorEvent)));
    WriteBuffer(&io, (void *)(event), (uint32_t)(sizeof(struct SensorEvent)));
    WriteUint32(&io, (uint32_t)(event->dataLen));
    WriteBuffer(&io, (void *)(event->data), (uint32_t)(event->dataLen));
    MessageOption option;
    MessageOptionInit(&option);
    option.flags = TF_OP_ASYNC;
    SendRequest(g_svcIdentity, 0, &io, NULL, option, NULL);
    return SENSOR_OK;
}

void SetSvcIdentity(IpcIo *req, const IpcIo *reply)
{
    SvcIdentity sid;
    bool ret = ReadRemoteObject(req, &sid);
    if (!ret) {
        HILOG_ERROR(HILOG_MODULE_APP, "%s ReadRemoteObject failed ", __func__);
        return;
    }
    g_svcIdentity.handle = sid.handle;
    g_svcIdentity.token = sid.token;
}
// ohos-hisi begin
#endif
// ohos-hisi end

BOOL Initialize(Service *service, Identity identity)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "[SERVICE:%s]: %s begin", SENSOR_SERVICE, __func__);
    // ohos-hisi begin
    g_svcIdentity = identity;
    // ohos-hisi end
#ifdef HAS_HDI_SENSOR_LITE_PRAT
    g_sensorDevice = NewSensorInterfaceInstance();
    if (g_sensorDevice == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s g_sensorDevice is NULL",
            SENSOR_SERVICE, __func__);
    }
#endif // HAS_HDI_SENSOR_LITE_PRAT
    return TRUE;
}

BOOL MessageHandle(Service *service, Request *msg)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "[SERVICE:%s]: %s begin",
        SENSOR_SERVICE, __func__);
    return TRUE;
}

TaskConfig GetTaskConfig(Service *service)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "[SERVICE:%s]: %s begin", SENSOR_SERVICE, __func__);
    TaskConfig config = {LEVEL_HIGH, PRI_BELOW_NORMAL, TASK_CONFIG_STACK_SIZE, TASK_CONFIG_QUEUE_SIZE, SHARED_TASK};

#if defined(MELODY_DEMO_EARPHONE)
    config.taskFlags = SINGLE_TASK;
#endif

    return config;
}

int32_t GetAllSensorsImpl(SensorInfo **sensorInfo, int32_t *count)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "[SERVICE:%s]: %s begin", SENSOR_SERVICE, __func__);
    if ((sensorInfo == NULL) || (count == NULL)) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s sensorInfo or count is null",
            SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    if ((g_sensorLists == NULL) || (g_sensorListsLength <= 0)) {
        if (InitSensorList() != 0) {
            HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s initSensorList failed!",
                SENSOR_SERVICE, __func__);
            return SENSOR_ERROR_INVALID_PARAM;
        }
    }
    *sensorInfo = (SensorInfo *)g_sensorInfos;
    *count = g_sensorListsLength;
    return SENSOR_OK;
}

// ohos-hisi begin
bool CheckSensorTypeId(int32_t sensorId)
{
    if ((sensorId >= SENSOR_TYPE_ID_MAX) || (sensorId < SENSOR_TYPE_ID_NONE)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: sensorId: %d is invalid", SENSOR_SERVICE, sensorId);
        return false;
    }
    if ((g_sensorInfos == NULL) || (g_sensorListsLength <= 0)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: SensorList invalid!", SENSOR_SERVICE);
        return false;
    }
    for (int32_t i = 0; i < g_sensorListsLength; i++) {
        if (sensorId == g_sensorInfos[i].sensorTypeId) {
            return true;
        }
    }
    return false;
}
// ohos-hisi end

int32_t ActivateSensorImpl(int32_t sensorId, const SensorUser *user)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "[SERVICE:%s]: %s begin", SENSOR_SERVICE, __func__);
    // ohos-hisi begin
    if (!CheckSensorTypeId(sensorId)) {
        return SENSOR_ERROR_INVALID_ID;
    }
    // ohos-hisi end
    if (user == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s user is NULL",
            SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
#ifdef HAS_HDI_SENSOR_LITE_PRAT
    if (g_sensorDevice == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s g_sensorDevice is NULL",
            SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    int32_t ret = g_sensorDevice->Enable(sensorId);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s ActivateSensor sensor failed, ret: %d",
            SENSOR_SERVICE, __func__, ret);
        return ret;
    }
#endif // HAS_HDI_SENSOR_LITE_PRAT
    return SENSOR_OK;
}

int32_t DeactivateSensorImpl(int32_t sensorId, const SensorUser *user)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "[SERVICE:%s]: %s begin",
        SENSOR_SERVICE, __func__);
    // ohos-hisi begin
    if (!CheckSensorTypeId(sensorId)) {
        return SENSOR_ERROR_INVALID_ID;
    }
    // ohos-hisi end
    if (user == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s user is NULL",
            SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
#ifdef HAS_HDI_SENSOR_LITE_PRAT
    if (g_sensorDevice == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s g_sensorDevice is NULL",
            SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    int32_t ret = g_sensorDevice->Disable(sensorId);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s DeactivateSensor sensor failed, ret: %d",
            SENSOR_SERVICE, __func__, ret);
        return ret;
    }
#endif // HAS_HDI_SENSOR_LITE_PRAT
    return SENSOR_OK;
}

int32_t SetBatchImpl(int32_t sensorId, const SensorUser *user, int64_t samplingInterval, int64_t reportInterval)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "[SERVICE:%s]: %s begin",
        SENSOR_SERVICE, __func__);
    // ohos-hisi begin
    if (!CheckSensorTypeId(sensorId)) {
        return SENSOR_ERROR_INVALID_ID;
    }
    // ohos-hisi end
    if ((samplingInterval < 0) || (reportInterval < 0)) {
        HILOG_ERROR(HILOG_MODULE_APP,
            "[SERVICE:%s]: %s samplingInterval: %lld or reportInterval: %lld is invalid",
            SENSOR_SERVICE, __func__, samplingInterval, reportInterval);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    // ohos-hisi begin
#ifdef HAS_HDI_SENSOR_LITE_PRAT
    if (g_sensorDevice == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s g_sensorDevice is NULL",
            SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    int32_t ret = g_sensorDevice->SetBatch(sensorId, samplingInterval, reportInterval);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s SetBatch failed, ret: %d",
            SENSOR_SERVICE, __func__, ret);
        return SENSOR_ERROR_INVALID_PARAM;
    }
#endif // HAS_HDI_SENSOR_LITE_PRAT
    // ohos-hisi end
    return SENSOR_OK;
}

int32_t SubscribeSensorImpl(int32_t sensorId, const SensorUser *user)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "[SERVICE:%s]: %s begin",
        SENSOR_SERVICE, __func__);
    // ohos-hisi begin
    if (!CheckSensorTypeId(sensorId)) {
        return SENSOR_ERROR_INVALID_ID;
    }
    // ohos-hisi end
    if (user == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s user is NULL",
            SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
#ifdef HAS_HDI_SENSOR_LITE_PRAT
    if (g_sensorDevice == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s g_sensorDevice is NULL",
            SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
// ohos-hisi begin
#ifdef SUPPORT_SMALL
    int32_t ret = g_sensorDevice->Register(sensorId, (RecordDataCallback)SensorDataCallback);
#else
    int32_t ret = g_sensorDevice->Register(sensorId, (RecordDataCallback)user->callback);
#endif
// ohos-hisi end
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s register sensor user failed, ret: %d",
            SENSOR_SERVICE, __func__, ret);
        return SENSOR_ERROR_INVALID_PARAM;
    }
#endif // HAS_HDI_SENSOR_LITE_PRAT
    return SENSOR_OK;
}

int32_t UnsubscribeSensorImpl(int32_t sensorId, const SensorUser *user)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "[SERVICE:%s]: %s begin",
        SENSOR_SERVICE, __func__);
    // ohos-hisi begin
    if (!CheckSensorTypeId(sensorId)) {
        return SENSOR_ERROR_INVALID_ID;
    }
    // ohos-hisi end
    if (user == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s user is NULL",
            SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
#ifdef HAS_HDI_SENSOR_LITE_PRAT
    if (g_sensorDevice == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s g_sensorDevice is NULL",
            SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    // ohos-hisi begin
#ifdef SUPPORT_SMALL
    int32_t ret = g_sensorDevice->Unregister(sensorId, (RecordDataCallback)SensorDataCallback);
#else
    int32_t ret = g_sensorDevice->Unregister(sensorId, (RecordDataCallback)user->callback);
#endif
// ohos-hisi end
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s unregister sensor failed, ret: %d",
            SENSOR_SERVICE, __func__, ret);
        return SENSOR_ERROR_INVALID_PARAM;
    }
#endif // HAS_HDI_SENSOR_LITE_PRAT
    return SENSOR_OK;
}

int32_t SetModeImpl(int32_t sensorId, const SensorUser *user, int32_t mode)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "[SERVICE:%s]: %s begin",
        SENSOR_SERVICE, __func__);
    // ohos-hisi begin
    if (!CheckSensorTypeId(sensorId)) {
        return SENSOR_ERROR_INVALID_ID;
    }
#ifdef HAS_HDI_SENSOR_LITE_PRAT
    if (g_sensorDevice == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s g_sensorDevice is NULL",
            SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    int32_t ret = g_sensorDevice->SetMode(sensorId, mode);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_APP, "[SERVICE:%s]: %s setmode failed, ret: %d",
            SENSOR_SERVICE, __func__, ret);
        return SENSOR_ERROR_INVALID_PARAM;
    }
#endif // HAS_HDI_SENSOR_LITE_PRAT
    // ohos-hisi end
    return SENSOR_OK;
}

int32_t SetOptionImpl(int32_t sensorId, const SensorUser *user, int32_t option)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "[SERVICE:%s]: %s begin",
        SENSOR_SERVICE, __func__);
    // ohos-hisi begin
    if (!CheckSensorTypeId(sensorId)) {
        return SENSOR_ERROR_INVALID_ID;
    }
    // ohos-hisi end
    return SENSOR_OK;
}

// ohos-hisi begin
int32_t GetSensorDataImpl(int32_t sensorId, const SensorUser *user, void *sensorData)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "[SERVICE:%s]: %s begin",
        SENSOR_SERVICE, __func__);
    if (!CheckSensorTypeId(sensorId)) {
        return SENSOR_ERROR_INVALID_ID;
    }
    return g_sensorDevice->GetSensorData(sensorId, (SensorData *)sensorData);
}
// ohos-hisi end