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

#include "sensor_sample.h"

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmsis_os.h>
#include "securec.h"
#include "sensor_agent.h"
#include "sensor_agent_type.h"
#include "sensor_agent_type_ext.h"
#include "sensor_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SUPPORT_SENSOR_CNT 7
#define SENSOR_CMD_LEN_MAX 64
#define SENSOR_SET_BATCH_PARAM_LEN 4
#define SENSOR_SET_MODE_PARAM_LEN 3
#define SENSOR_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
typedef void *ThreadId;

typedef struct {
    const char **argv;
    int32_t argc;
} Params;

static int32_t SensorGetAllSensors(Params *params);
static int32_t SensorSetBatch(Params *params);
static int32_t SensorSetMode(Params *params);
static int32_t SensorSubscribe(Params *params);
static int32_t SensorActivate(Params *params);
static int32_t SensorUnsubscribe(Params *params);
static int32_t SensorDeactivate(Params *params);
static int32_t SensorGetData(Params *params);

typedef struct {
    char cmd[SENSOR_CMD_LEN_MAX];
    int32_t (*func)(Params *params);
} FuncsMap;

static FuncsMap g_sensorFuncs[] = {
    {"getall", SensorGetAllSensors},
    {"setbatch", SensorSetBatch},
    {"setmode", SensorSetMode},
    {"subscribe", SensorSubscribe},
    {"activate", SensorActivate},
    {"unsubscribe", SensorUnsubscribe},
    {"deactivate", SensorDeactivate},
    {"getdata", SensorGetData},
};

typedef struct {
    char cmd[SENSOR_CMD_LEN_MAX];
    SensorTypeId sensorTypeId;
} SensorIdMap;

static SensorIdMap g_sensorIdMap[] = {
    {"accelerometer", SENSOR_TYPE_ID_ACCELEROMETER},
    {"barometer", SENSOR_TYPE_ID_BAROMETER},
    {"gyroscope", SENSOR_TYPE_ID_GYROSCOPE},
    {"heartrate", SENSOR_TYPE_ID_HEART_RATE},
    {"onbodystate", SENSOR_TYPE_ID_WEAR_DETECTION},
    {"stepcounter", SENSOR_TYPE_ID_PEDOMETER},
    {"accealgsimu", SENSOR_TYPE_ID_VSENSOR_ACCELEROMETER_ALG_SIMU},
};

void RecordAccelSensorCallback(SensorEvent *event)
{
    uint32_t accelDataLen = sizeof(AccelData);
    HILOG_INFO(HILOG_MODULE_SEN,
        "sensorTypeId:%d version:%d timestamp:%lld option:%u  mode:%d accelDataLen:%u dataLen:%u",
        event->sensorTypeId,
        event->version,
        event->timestamp,
        event->option,
        event->mode,
        accelDataLen,
        event->dataLen);
    if (event->data == NULL || event->dataLen == 0 || (event->dataLen % accelDataLen) != 0) {
        HILOG_ERROR(HILOG_MODULE_SEN, "SensorEvent data invalid\n");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_ACCELEROMETER) {
        HILOG_ERROR(HILOG_MODULE_SEN, "sensorTypeId %d is not ACCELEROMETER\n", event->sensorTypeId);
        return;
    }
    uint32_t batchCount = event->dataLen / accelDataLen;
    for (uint32_t i = 0; i < batchCount; i++) {
        AccelData *accelData = (AccelData *)event->data;
        HILOG_INFO(HILOG_MODULE_SEN,
            "batch :%u axisX:%d axisY:%d axisZ:%d",
            i,
            accelData->axisX,
            accelData->axisY,
            accelData->axisZ);
    }
}

void RecordBarSensorCallback(SensorEvent *event)
{
    HILOG_INFO(HILOG_MODULE_SEN,
        "sensorTypeId:%d version:%d timestamp:%lld option:%u  mode:%d \n",
        event->sensorTypeId,
        event->version,
        event->timestamp,
        event->option,
        event->mode);
    if (event->data == NULL || event->dataLen == 0 || event->dataLen != sizeof(PressureData)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "SensorEvent data invalid\n");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_BAROMETER) {
        HILOG_ERROR(HILOG_MODULE_SEN, "sensorTypeId %d is not BAROMETER\n", event->sensorTypeId);
        return;
    }
    PressureData *pressureData = (PressureData *)event->data;
    HILOG_INFO(HILOG_MODULE_SEN,
        "pressureData pressure:%u temperature:%u dataValid:%u \n",
        pressureData->pressure,
        pressureData->temperature,
        pressureData->dataValid);
}

void RecordCyroSensorCallback(SensorEvent *event)
{
    HILOG_INFO(HILOG_MODULE_SEN,
        "sensorTypeId:%d version:%d timestamp:%lld option:%u  mode:%d \n",
        event->sensorTypeId,
        event->version,
        event->timestamp,
        event->option,
        event->mode);
    if (event->data == NULL || event->dataLen == 0 || event->dataLen != sizeof(GyroData)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "SensorEvent data invalid\n");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_GYROSCOPE) {
        HILOG_ERROR(HILOG_MODULE_SEN, "sensorTypeId %d is not GYROSCOPE\n", event->sensorTypeId);
        return;
    }
    GyroData *gyroData = (GyroData *)event->data;
    HILOG_INFO(HILOG_MODULE_SEN,
        "gyroData axisX:%d axisY:%d axisZ:%d \n",
        gyroData->axisX,
        gyroData->axisY,
        gyroData->axisZ);
}

void RecordHrSensorCallback(SensorEvent *event)
{
    HILOG_INFO(HILOG_MODULE_SEN,
        "sensorTypeId:%d version:%d timestamp:%lld option:%u  mode:%d \n",
        event->sensorTypeId,
        event->version,
        event->timestamp,
        event->option,
        event->mode);
    if (event->data == NULL || event->dataLen == 0 || event->dataLen != sizeof(uint32_t)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "SensorEvent data invalid\n");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_HEART_RATE) {
        HILOG_ERROR(HILOG_MODULE_SEN, "sensorTypeId %d is not HEART_RATE\n", event->sensorTypeId);
        return;
    }
    uint32_t heartRate = *(uint32_t *)event->data;
    HILOG_INFO(HILOG_MODULE_SEN, "heartRate:%u", heartRate);
}

void RecordObsSensorCallback(SensorEvent *event)
{
    HILOG_INFO(HILOG_MODULE_SEN,
        "sensorTypeId:%d version:%d timestamp:%lld option:%u  mode:%d \n",
        event->sensorTypeId,
        event->version,
        event->timestamp,
        event->option,
        event->mode);
    if (event->data == NULL || event->dataLen == 0 || event->dataLen != sizeof(uint8_t)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "SensorEvent data invalid\n");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_WEAR_DETECTION) {
        HILOG_ERROR(HILOG_MODULE_SEN, "sensorTypeId %d is not WEAR_DETECTION\n", event->sensorTypeId);
        return;
    }
    uint8_t isonbody = *event->data;
    HILOG_INFO(HILOG_MODULE_SEN, "isonbody:%u", isonbody);
}

void RecordScSensorCallback(SensorEvent *event)
{
    HILOG_INFO(HILOG_MODULE_SEN,
        "sensorTypeId:%d version:%d timestamp:%lld option:%u  mode:%d \n",
        event->sensorTypeId,
        event->version,
        event->timestamp,
        event->option,
        event->mode);
    if (event->data == NULL || event->dataLen == 0 || event->dataLen != sizeof(uint32_t)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "SensorEvent data invalid\n");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_PEDOMETER) {
        HILOG_ERROR(HILOG_MODULE_SEN, "sensorTypeId %d is not PEDOMETER\n", event->sensorTypeId);
        return;
    }
    uint32_t steps = *(uint32_t *)event->data;
    HILOG_INFO(HILOG_MODULE_SEN, "steps:%u", steps);
}

typedef struct {
    SensorTypeId sensorTypeId;
    SensorUser user;
    bool activated;
    bool subscribed;
} SensorDev;

typedef struct {
    bool isStarted;
    SensorInfo *sensorInfo;
    int32_t snsCnt;
    SensorDev sensorDev[SUPPORT_SENSOR_CNT];
} SensorSampleContext;

static SensorSampleContext g_sensorContext = {
    .isStarted = false,
    .sensorInfo = NULL,
    .snsCnt = 0,
    .sensorDev =
        {
            {SENSOR_TYPE_ID_ACCELEROMETER, {"AT_Test", RecordAccelSensorCallback, NULL}, false, false},
            {SENSOR_TYPE_ID_BAROMETER, {"AT_Test", RecordBarSensorCallback, NULL}, false, false},
            {SENSOR_TYPE_ID_GYROSCOPE, {"AT_Test", RecordCyroSensorCallback, NULL}, false, false},
            {SENSOR_TYPE_ID_HEART_RATE, {"AT_Test", RecordHrSensorCallback, NULL}, false, false},
            {SENSOR_TYPE_ID_WEAR_DETECTION, {"AT_Test", RecordObsSensorCallback, NULL}, false, false},
            {SENSOR_TYPE_ID_PEDOMETER, {"AT_Test", RecordScSensorCallback, NULL}, false, false},
            {SENSOR_TYPE_ID_VSENSOR_ACCELEROMETER_ALG_SIMU, {"AT_Test", RecordAccelSensorCallback, NULL}, false, false},
        },
};

static SensorDev *GetSensorIdBySensorDevIndex(SensorTypeId sensorTypeId)
{
    if (g_sensorContext.snsCnt <= 0 || g_sensorContext.sensorInfo == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "must GetAllSensors first\n");
        return NULL;
    }
    bool found = false;
    for (int32_t index = 0; index < g_sensorContext.snsCnt; index++) {
        if (g_sensorContext.sensorInfo[index].sensorTypeId == sensorTypeId) {
            found = true;
            break;
        }
    }
    if (!found) {
        HILOG_ERROR(HILOG_MODULE_SEN, "not found sensorTypeId:%d\n", sensorTypeId);
        return NULL;
    }
    for (uint32_t index = 0; index < SENSOR_ARRAY_SIZE(g_sensorContext.sensorDev); index++) {
        if (g_sensorContext.sensorDev[index].sensorTypeId == sensorTypeId) {
            return &g_sensorContext.sensorDev[index];
        }
    }
    return NULL;
}

static SensorTypeId GetSensorIdByParam(const char *argv)
{
    if (argv == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "argv invalid\n");
        return SENSOR_TYPE_ID_MAX;
    }
    for (uint32_t index = 0; index < SENSOR_ARRAY_SIZE(g_sensorIdMap); index++) {
        if (strcmp(argv, g_sensorIdMap[index].cmd) == 0) {
            return g_sensorIdMap[index].sensorTypeId;
        }
    }
    return SENSOR_TYPE_ID_MAX;
}

static int32_t SensorGetAllSensors(Params *params)
{
    HILOG_INFO(HILOG_MODULE_SEN, "SensorGetAllSensors\n");
    (void)params;
    if (g_sensorContext.snsCnt != 0 || g_sensorContext.sensorInfo != NULL) {
        HILOG_INFO(HILOG_MODULE_SEN, "sensorInfo is Got\n");
        return SENSOR_OK;
    }
    int32_t errCode = GetAllSensors(&g_sensorContext.sensorInfo, &g_sensorContext.snsCnt);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "sensorInfo failed:%d\n", errCode);
        return errCode;
    }
    return SENSOR_OK;
}

static int32_t SensorSetBatch(Params *params)
{
    if (params->argc < SENSOR_SET_BATCH_PARAM_LEN) {
        HILOG_ERROR(HILOG_MODULE_SEN, "argv is invalid\n");
        return -1;
    }
    SensorTypeId sensorTypeId = GetSensorIdByParam(params->argv[0x1]);
    SensorDev *sensorDev = GetSensorIdBySensorDevIndex(sensorTypeId);
    if (sensorDev == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "GetSensorIdBySensorDevIndex failed\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    char *endptr = NULL;
    int64_t samplingInterval = strtol(params->argv[0x2], &endptr, 10);
    if (*endptr != '\0') {
        HILOG_ERROR(HILOG_MODULE_SEN, "samplingInterval  invalid\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    endptr = NULL;
    int64_t reportInterval = strtol(params->argv[0x3], &endptr, 10);
    if (*endptr != '\0') {
        HILOG_ERROR(HILOG_MODULE_SEN, "samplingInterval  invalid\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    HILOG_INFO(HILOG_MODULE_SEN, "sensorTypeId:%d SetBatch samplingInterval :%lld reportInterval :%lld\n",
        sensorTypeId, samplingInterval, reportInterval);
    int32_t errCode = SetBatch(sensorTypeId, &sensorDev->user, samplingInterval, reportInterval);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "SetBatch failed:%d\n", errCode);
        return errCode;
    }
    return SENSOR_OK;
}

static int32_t SensorSetMode(Params *params)
{
    if (params->argc < SENSOR_SET_MODE_PARAM_LEN) {
        HILOG_ERROR(HILOG_MODULE_SEN, "argv is invalid\n");
        return -1;
    }
    SensorTypeId sensorTypeId = GetSensorIdByParam(params->argv[1]);
    HILOG_INFO(HILOG_MODULE_SEN, "SetMode :%d\n", sensorTypeId);
    SensorDev *sensorDev = GetSensorIdBySensorDevIndex(sensorTypeId);
    if (sensorDev == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "GetSensorIdBySensorDevIndex failed\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    char *endptr = NULL;
    int32_t mode = strtol(params->argv[2], &endptr, 10);
    if (*endptr != '\0') {
        HILOG_ERROR(HILOG_MODULE_SEN, "mode  invalid\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    int32_t errCode = SetMode(sensorTypeId, &sensorDev->user, mode);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "SetBatch failed:%d\n", errCode);
        return errCode;
    }
    return SENSOR_OK;
}

static int32_t SensorSubscribe(Params *params)
{
    SensorTypeId sensorTypeId = GetSensorIdByParam(params->argv[1]);
    HILOG_INFO(HILOG_MODULE_SEN, "SensorSubscribe :%d\n", sensorTypeId);
    SensorDev *sensorDev = GetSensorIdBySensorDevIndex(sensorTypeId);
    if (sensorDev == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "GetSensorIdBySensorDevIndex failed\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    if (sensorDev->subscribed) {
        HILOG_WARN(HILOG_MODULE_SEN, " sensorTypeId:%d subscribed\n", sensorTypeId);
        return SENSOR_OK;
    }
    int32_t errCode = SubscribeSensor(sensorTypeId, &sensorDev->user);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "SubscribeSensor failed:%d\n", errCode);
        return errCode;
    }
    sensorDev->subscribed = true;
    HILOG_INFO(HILOG_MODULE_SEN, "SensorSubscribe end\n");
    return SENSOR_OK;
}

static int32_t SensorActivate(Params *params)
{
    SensorTypeId sensorTypeId = GetSensorIdByParam(params->argv[1]);
    HILOG_INFO(HILOG_MODULE_SEN, "SensorActivate :%d\n", sensorTypeId);
    SensorDev *sensorDev = GetSensorIdBySensorDevIndex(sensorTypeId);
    if (sensorDev == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "GetSensorIdBySensorDevIndex failed\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    if (sensorDev->activated) {
        HILOG_WARN(HILOG_MODULE_SEN, "already activated\n");
        return SENSOR_OK;
    }
    HILOG_INFO(HILOG_MODULE_SEN, "ActivateSensor \n");
    int32_t errCode = ActivateSensor(sensorTypeId, &sensorDev->user);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "ActivateSensor failed:%d\n", errCode);
        return errCode;
    }
    sensorDev->activated = true;
    return SENSOR_OK;
}

static int32_t SensorUnsubscribe(Params *params)
{
    SensorTypeId sensorTypeId = GetSensorIdByParam(params->argv[1]);
    HILOG_INFO(HILOG_MODULE_SEN, "SensorUnsubscribe :%d\n", sensorTypeId);
    SensorDev *sensorDev = GetSensorIdBySensorDevIndex(sensorTypeId);
    if (sensorDev == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "GetSensorIdBySensorDevIndex failed\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    if (!sensorDev->subscribed) {
        HILOG_WARN(HILOG_MODULE_SEN, "not subscribed\n");
        return SENSOR_OK;
    }
    int32_t errCode = UnsubscribeSensor(sensorTypeId, &sensorDev->user);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "UnsubscribeSensor failed:%d\n", errCode);
        return errCode;
    }
    sensorDev->subscribed = false;
    return SENSOR_OK;
}

static int32_t SensorDeactivate(Params *params)
{
    SensorTypeId sensorTypeId = GetSensorIdByParam(params->argv[1]);
    HILOG_INFO(HILOG_MODULE_SEN, "SensorDeactivate :%d\n", sensorTypeId);
    SensorDev *sensorDev = GetSensorIdBySensorDevIndex(sensorTypeId);
    if (sensorDev == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "GetSensorIdBySensorDevIndex failed\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    if (sensorDev->subscribed) {
        UnsubscribeSensor(sensorTypeId, &sensorDev->user);
        sensorDev->subscribed = false;
    }
    if (!sensorDev->activated) {
        HILOG_WARN(HILOG_MODULE_SEN, "not activated\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    int32_t errCode = DeactivateSensor(sensorTypeId, &sensorDev->user);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "DeactivateSensor failed:%d\n", errCode);
        return errCode;
    }
    sensorDev->activated = false;
    return SENSOR_OK;
}

static int32_t SensorGetData(Params *params)
{
    SensorTypeId sensorTypeId = GetSensorIdByParam(params->argv[1]);
    HILOG_INFO(HILOG_MODULE_SEN, "getData :%d\n", sensorTypeId);
    SensorDev *sensorDev = GetSensorIdBySensorDevIndex(sensorTypeId);
    if (sensorDev == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "GetSensorIdBySensorDevIndex failed\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    SensorData *sensorData = (SensorData *)malloc(sizeof(SensorData));
    if (sensorData == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "malloc failed\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    int32_t errCode = GetSensorData(sensorTypeId, &sensorDev->user, sensorData);
    if (errCode != SENSOR_OK) {
        free(sensorData);
        HILOG_ERROR(HILOG_MODULE_SEN, "GetSensorData failed:%d\n", errCode);
        return errCode;
    }
    AccelData *accData = (AccelData *)sensorData->data;
    for (uint32_t i = 0; i < sensorData->batchCnt; i++) {
        HILOG_INFO(HILOG_MODULE_SEN,
            "SensorData dataX:%x, datY:%x, dataZ:%x\n",
            accData[i].axisX,
            accData[i].axisY,
            accData[i].axisZ);
    }
    HILOG_INFO(HILOG_MODULE_SEN, "SensorData batchCnt:%u timestamp:%lld\n",
        sensorData->batchCnt, sensorData->timestamp);
    free(sensorData);
    return SENSOR_OK;
}

static void SensorTaskFree(Params *params)
{
    for (int32_t i = 0; i < params->argc; i++) {
        if (params->argv[i] != NULL) {
            free(params->argv[i]);
        }
    }
    free(params->argv);
    free(params);
}

static void *SensorTaskEntry(void *argv)
{
    Params *params = (Params *)argv;
    HILOG_INFO(HILOG_MODULE_SEN, "enter SensorTaskEntry :argc:%d argv:%s\n", params->argc, params->argv[0]);
    for (uint32_t i = 0; i < SENSOR_ARRAY_SIZE(g_sensorFuncs); i++) {
        if (strcmp(params->argv[0], g_sensorFuncs[i].cmd) == 0) {
            HILOG_INFO(HILOG_MODULE_SEN, "execute argv :%s\n", params->argv[0]);
            int32_t ret = g_sensorFuncs[i].func(params);
            HILOG_INFO(
                HILOG_MODULE_SEN, "%s execute %s\n", g_sensorFuncs[i].cmd, (ret != 0) ? "failed" : "success");
            break;
        }
    }
    osDelay(300);
    HILOG_INFO(HILOG_MODULE_SEN, "SensorSample end\n");
    SensorTaskFree(params);
    return NULL;
}

uint32_t SensorSample(uint8_t argc, char *argv[])
{
    if (argv == NULL || argc < 1) {
        HILOG_ERROR(HILOG_MODULE_SEN, "argv is invalid\n");
        return -1;
    }
    Params *params = (Params *)malloc(sizeof(Params));
    if (params == NULL) {
        return -1;
    }
    params->argv = (char **)malloc(argc * sizeof(char *));
    if (params->argv == NULL) {
        free(params);
        return -1;
    }
    for (int32_t i = 0; i < argc; i++) {
        params->argv[i] = strdup(argv[i]);
        if (params->argv[i] == NULL) {
            SensorTaskFree(params);
            return -1;
        }
    }
    params->argc = argc;
    osThreadAttr_t taskAttr = {"SensorTaskEntry", 0, NULL, 0, NULL, 0x2000, (osPriority_t)(osPriorityHigh), 0, 0};

    osThreadId_t taskHandle = osThreadNew((osThreadFunc_t)SensorTaskEntry, params, &taskAttr);
    if (taskHandle == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "osThreadNew failed!\n");
        SensorTaskFree(params);
        return -1;
    }
    HILOG_INFO(HILOG_MODULE_SEN, "SensorSample exit\n");
    return 0;
}

#ifdef __cplusplus
};
#endif
