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

#include "vsensor_task_comm.h"
#include "sensor_log.h"
#include "sensor_memory.h"
#include "sensor_common.h"
#include "securec.h"
#include "sensor_tmsg.h"
#include "sensor_para_decision.h"
#include "sensor_state.h"

int32_t VsensorTaskCheckCommComponents(const VsensorTaskComm *component)
{
    if (component == NULL || component->taskPrefix == NULL) {
        return SENSOR_TASK_CHECK_COMM_COMPONENT_NULL;
    }
    return SENSOR_OK;
}

static VsensorTaskHlistener *VsensorTaskFindHlistener(
    const VsensorTaskHlistener *hlistenerPool, uint16_t hlistenerPoolLen, enum SensorTypeTag type)
{
    CHECK_SENSOR_TYPE_ILLEGAL_RETURN_RET(type, NULL);

    for (uint32_t i = 0; i < VSENSOR_TASK_HLISTENER_MAX_NUM; i++) {
        VsensorTaskHlistener *hlistener = (VsensorTaskHlistener *)(&(hlistenerPool[i]));
        if (hlistener->type == type) {
            return hlistener;
        }
    }

    return NULL;
}

static VsensorTaskHlistener *VsensorTaskRequestHlistener(
    const VsensorTaskHlistener *hlistenerPool, uint16_t hlistenerPoolLen, enum SensorTypeTag type)
{
    VsensorTaskHlistener *hlistener = VsensorTaskFindHlistener(hlistenerPool, hlistenerPoolLen, type);
    if (hlistener != NULL) {
        return hlistener;
    }

    for (uint32_t i = 0; i < VSENSOR_TASK_HLISTENER_MAX_NUM; i++) {
        hlistener = (VsensorTaskHlistener *)(&(hlistenerPool[i]));
        if (hlistener->type == SENSOR_TYPE_NONE) {
            return hlistener;
        }
    }

    return NULL;
}

static int32_t VsensorTaskDataUpdateCheck(const SensorNotifyInfo *events, const void *notifyArg)
{
    if ((events == NULL) || (events->data == NULL) || (notifyArg == NULL)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]dataupdate para null");
        return SENSOR_TASK_HSDATA_UPDATE_DATA_NULL;
    }

    if (events->type >= SENSOR_TYPE_MAX) {
        HILOG_ERROR(HILOG_MODULE_SEN,
            "[VTASK]dataupdate sensor type or len err,type:%d, len:%u\r\n",
            events->type,
            events->dataLen);
        return SENSOR_TASK_HSDATA_UPDATE_DATA_NULL;
    }

    return SENSOR_OK;
}

static void VsensorTaskSensorDataUpdate(const SensorNotifyInfo *events, void *notifyArg)
{
    int32_t ret;
    ret = VsensorTaskDataUpdateCheck(events, notifyArg);
    if (ret != SENSOR_OK) {
        return;
    }

    VsensorTaskComm *component = (VsensorTaskComm *)notifyArg;

    /* 计算实际需要的存储空间 */
    uint32_t dataSize = (uint32_t)(sizeof(VsensorProcessData)) + events->dataLen;
    VsensorProcessData *processData = (VsensorProcessData *)SensorMalloc(dataSize);
    if (processData == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN,
            "[VTASK]cannot malloc for hsensor:%d data!, taskId:%d\n",
            events->type,
            component->taskId);
        return;
    }
    int32_t err = memset_s(processData, dataSize, 0, dataSize);
    if (err != EOK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]dataupdate: memset fail\r\n");
    }

    processData->timestamp = events->timestamp;
    processData->sensor = events->type;
    processData->dataLen = events->dataLen;

    uint32_t i = 0;
    while (i < events->dataLen) {
        processData->sensordata[i] = events->data[i];
        i++;
    }
    SensorTmsg tmsgSend = {
        .id = SENSOR_TMSG_HSENSOR_DATA_UPDATE, .value = 0, .ts = 0, .info = processData, .infoSize = dataSize};

    ret = SensorTmsgSend(component->taskId, &tmsgSend);
    if (ret != SENSOR_OK) {
        SensorFree((void *)processData);
        processData = NULL;
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]dataupdate send msg error 0x%x\r\n", ret);
    }

    return;
}

static int32_t VsensorTaskHlistenerInit(
    VsensorTaskComm *component, VsensorTaskHlistener *hlistener, enum SensorTypeTag type)
{
    HsensorListener *listener = &(hlistener->hlistener);
    Sensor *sensor = SensorUtilsFindSensor(type);
    if (sensor == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]init sensor NULL! type:%d", type);
        return SENSOR_TASK_SENSOR_NULL;
    }

    errno_t err = memset_s(listener->name, HSENSOR_LISTENER_NAME_MAX_LEN, 0, HSENSOR_LISTENER_NAME_MAX_LEN);
    if (err != EOK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "hlistener init name memset failed! type:%d", type);
        return SENSOR_SENSOR_SECUREC_FUNC_ERR;
    }
    err = strcat_s(listener->name, sizeof(listener->name), component->taskPrefix);
    if (err != EOK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "taskPrefix strcat failed! type:%d", type);
        return SENSOR_SENSOR_SECUREC_FUNC_ERR;
    }
    const char *vSensorName = component->vsensorInfo.vsensor->name;
    err = strcat_s(listener->name, sizeof(listener->name), vSensorName);
    if (err != EOK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "vSensorName strcat failed! type:%d", type);
        return SENSOR_SENSOR_SECUREC_FUNC_ERR;
    }
    const char *relyOn = "_RELY_ON_";
    err = strcat_s(listener->name, sizeof(listener->name), relyOn);
    if (err != EOK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "RELY_ON_strcat failed! type:%d", type);
        return SENSOR_SENSOR_SECUREC_FUNC_ERR;
    }
    err = strcat_s(listener->name, sizeof(listener->name), sensor->sensorInfo.name);
    if (err != EOK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "hSensorName strcat failed! type:%d", type);
        return SENSOR_SENSOR_SECUREC_FUNC_ERR;
    }

    listener->type = type;
    hlistener->type = type;

    /* 如果没有指定物理传感器数据更新函数，默认直接采用 标准通道函数 */
    if (component->hdataCb == NULL) {
        listener->notifyCallback = VsensorTaskSensorDataUpdate;
    } else {
        listener->notifyCallback = component->hdataCb;
    }
    listener->notifyArg = (void *)component;
    HILOG_DEBUG(HILOG_MODULE_SEN, "[VTASK]Hlistener init type :%d", type);
    return SENSOR_OK;
}

static int32_t VsensorTaskHlistenerRegister(HsensorListener *listener)
{
    int32_t ret = HsensorMgrRegisterListener(listener);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]hsensormgr reg listener:%d return FAIL!\n", listener->type);
        return ret;
    }

    return SENSOR_OK;
}

static int32_t VsensorTaskVsensorReg(VsensorTaskComm *component, Vsensor *vsensor)
{
    component->vsensorInfo.type = vsensor->type;
    component->vsensorInfo.vsensor = vsensor;
    HILOG_DEBUG(HILOG_MODULE_SEN, "[VTASK]Reg type:%d", vsensor->type);
    return SENSOR_OK;
}

static int32_t VsensorTaskOpenRelyHSensor(VsensorTaskComm *component, enum SensorTypeTag type, const SensorPara *para)
{
    VsensorTaskHlistener *hlistenerPool = component->hlistenersList;
    HILOG_DEBUG(HILOG_MODULE_SEN, "[VTASK]Open Rely hsensor:%d ", type);
    VsensorTaskHlistener *var = VsensorTaskFindHlistener(hlistenerPool, VSENSOR_TASK_HLISTENER_MAX_NUM, type);
    if ((var == NULL) || (var->type == SENSOR_TYPE_NONE)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]rely hlistener NULL! type:%d", type);
        return SENSOR_TASK_LISTENER_FIND_NULL;
    }

    SensorPara curDecisionPara = var->hlistener.openParam;
    int32_t ret = SensorOpenParaDecision(type, &curDecisionPara, para, &(var->hlistener.openParam));
    if (ret != SENSOR_OK) {
        return ret;
    }

    if (SensorParaIsClear(&(var->hlistener.openParam))) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]sensor decision para err! type:%d", type);
        return SENSOR_TASK_SENSOR_DECISION_PARA_ERROR;
    }

    ret = HsensorMgrOpenListener(&(var->hlistener));
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]hsensor:%d enable err 0x%04x!\n", type, ret);
        return ret;
    }
    HILOG_DEBUG(HILOG_MODULE_SEN, "[VTASK]Open Rely hsensor:%d sucess", type);
    return SENSOR_OK;
}

int32_t VsensorTaskOpenRelySensors(VsensorTaskComm *component, const RelySensorInfo *relySensors, uint8_t relyCnt)
{
    int32_t ret = SENSOR_OK;
    if ((relySensors == NULL) || (relyCnt <= 0)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[RELY] rely sensor is NULL!\n");
        return SENSOR_OPEN_RELY_SENSORS_CHECK_NULL;
    }

    for (uint8_t i = 0; i < relyCnt; i++) {
        enum SensorTypeTag type = relySensors[i].type;

        if (!SensorUtilsSensorIsRegistered(type)) {
            HILOG_ERROR(HILOG_MODULE_SEN, "[RELY] rely sensor:%d not registed!\n", type);
            return SENSOR_TASK_RELY_SENSOR_NOT_REGISTERED;
        }

        uint32_t taskId = SensorUtilsGetRegisteredTaskId(type);
        bool isVsensor = SensorUtilsIsVSensor(type);
        if (isVsensor == true) {
            // 打开虚拟sensor
            SensorTmsg tmsgSend = {.id = SENSOR_TMSG_VSENSOR_RELY_OPEN_EVENT,
                .value = (uintptr_t)type,
                .ts = 0,
                .info = NULL,
                .infoSize = 0};

            ret = SensorTmsgSend(taskId, &tmsgSend);
            if (ret != SENSOR_OK) {
                HILOG_ERROR(HILOG_MODULE_SEN, "vsensor:%d send open msg FAIL!\r\n", type);
                return ret;
            }
        } else {
            /* 打开物理sensor, type 为物理sensor的类型 */
            ret = VsensorTaskOpenRelyHSensor(component, type, (SensorPara *)&(relySensors[i].param));
            if (ret != SENSOR_OK) {
                return ret;
            }
        }
    }
    HILOG_DEBUG(HILOG_MODULE_SEN, "Open Rely Sensors sucess");
    return SENSOR_OK;
}

static int32_t SensorCloseParaDecision(enum SensorTypeTag type, VsensorTaskComm *component, SensorPara *result)
{
    SensorPara *temp = NULL;
    VsensorTaskInfo *vsensorInfo = NULL;
    Vsensor *vsensor = NULL;

    if (component == NULL || result == NULL) {
        return SENSOR_TASK_CHECK_COMM_COMPONENT_NULL;
    }

    SensorParaClear(result);

    for (uint32_t i = 0; i < VSENSOR_MAX_NUM; i++) {
        vsensorInfo = &component->vsensorInfo;
        if (vsensorInfo->type == (enum SensorTypeTag)0 || vsensorInfo->vsensor == NULL) {
            continue;
        }

        vsensor = vsensorInfo->vsensor;
        temp = NULL;
        for (uint32_t cnt = 0; cnt < vsensor->relySensors->relySensorCnt; cnt++) {
            if (type == vsensor->relySensors->relySensorArray[cnt].type && vsensorInfo->state == SENSOR_STATE_OPEN) {
                temp = &(vsensor->relySensors->relySensorArray[cnt].param);
                HILOG_INFO(
                    HILOG_MODULE_SEN, "close sensor:%d decision, vsensor:%d still running\n", type, vsensor->type);
            }
        }

        if (temp == NULL) {
            continue;
        }

        if (SensorParaIsClear(result)) {
            SensorParaCopy(result, temp);
        } else {
            SensorPara cur = *result;
            result->period = SENSOR_MIN(cur.period, temp->period);
            if (result->period == 0) {
                return SENSOR_ERROR;
            }
            result->batch = SENSOR_MIN(cur.batch * cur.period, temp->batch * temp->period) / result->period;
            result->mode = SENSOR_MIN(cur.mode, temp->mode);
            result->option = SensorOptionDecision(type, cur.option, temp->option);
        }
    }

    return SENSOR_OK;
}

static int32_t VsensorTaskCloseRelyHsensor(VsensorTaskComm *component, enum SensorTypeTag type)
{
    VsensorTaskHlistener *hlistenerPool = component->hlistenersList;

    VsensorTaskHlistener *var = VsensorTaskFindHlistener(hlistenerPool, VSENSOR_TASK_HLISTENER_MAX_NUM, type);
    if ((var == NULL) || (var->type == (enum SensorTypeTag)0)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]rely hlistenerInfo NULL! type:%d", type);
        return SENSOR_TASK_LISTENER_FIND_NULL;
    }

    int32_t ret = SensorCloseParaDecision(type, component, &(var->hlistener.openParam));
    if (ret != SENSOR_OK) {
        return ret;
    }

    if (!SensorParaIsClear(&(var->hlistener.openParam))) {
        HILOG_INFO(HILOG_MODULE_SEN,
            "[VTASK]do not close rely sensor:%d, decision para,period:%d, batch:%d, mode:%d, option:%d\n",
            type,
            var->hlistener.openParam.period,
            var->hlistener.openParam.batch,
            var->hlistener.openParam.mode,
            var->hlistener.openParam.option);
        ret = HsensorMgrOpenListener(&(var->hlistener));
        if (ret != SENSOR_OK) {
            HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]hsensor:%d enable err %d!\n", type, ret);
            return ret;
        }
        return SENSOR_OK;
    }

    ret = HsensorMgrCloseListener(&(var->hlistener));
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]hsensor:%d close err!\n", type);
        return ret;
    }

    return SENSOR_OK;
}

static int32_t VsensorTaskCloseRelySensors(
    VsensorTaskComm *component, const RelySensorInfo *relySensors, uint8_t relyCnt)
{
    int32_t ret = SENSOR_OK;

    if ((relySensors == NULL) || (relyCnt <= 0)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[RELY] rely sensor is NULL!\n");
        return SENSOR_OPEN_RELY_SENSORS_CHECK_NULL;
    }

    for (uint8_t i = 0; i < relyCnt; i++) {
        enum SensorTypeTag type = relySensors[i].type;

        if (!SensorUtilsSensorIsRegistered(type)) {
            HILOG_ERROR(HILOG_MODULE_SEN, "[RELY] rely sensor:%d not registed!\n", type);
            return SENSOR_TASK_RELY_SENSOR_NOT_REGISTERED;
        }

        uint32_t taskId = SensorUtilsGetRegisteredTaskId(type);
        bool isVsensor = SensorUtilsIsVSensor(type);
        if (isVsensor == true) {
            // 关闭虚拟sensor
            SensorTmsg tmsgSend = {.id = SENSOR_TMSG_VSENSOR_RELY_CLOSE_EVENT,
                .value = (uintptr_t)type,
                .ts = 0,
                .info = NULL,
                .infoSize = 0};

            ret = SensorTmsgSend(taskId, &tmsgSend);
            if (ret != SENSOR_OK) {
                HILOG_ERROR(HILOG_MODULE_SEN, "[UTIL] fa vsensor:%d send open msg FAIL!\r\n", type);
                return ret;
            }
        } else {
            /* 关闭物理sensor */
            ret = VsensorTaskCloseRelyHsensor(component, type);
            if (ret != SENSOR_OK) {
                return ret;
            }
        }
    }

    return SENSOR_OK;
}

static bool VsensorTaskSensorBeRelied(enum SensorTypeTag type, const RelySensorInfo *relySensors, uint8_t relyCnt)
{
    if ((relySensors == NULL) || (relyCnt <= 0)) {
        return false;
    }

    for (uint8_t i = 0; i < relyCnt; i++) {
        if (relySensors[i].type == type) {
            return true;
        }
    }

    return false;
}

static int32_t VsensorTaskHlistenerRegEvent(VsensorTaskComm *component, enum SensorTypeTag type)
{
    HILOG_INFO(HILOG_MODULE_SEN, "[VTASK]type:%d Hlistener RegEvent ", type);

    VsensorTaskHlistener *var =
        VsensorTaskRequestHlistener(component->hlistenersList, VSENSOR_TASK_HLISTENER_MAX_NUM, type);
    if (var == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]VsensorTaskHlistenerRegEvent find hlistener NULL! type:%d", type);
        return SENSOR_TASK_LISTENER_FIND_NULL;
    }
    int32_t ret = VsensorTaskHlistenerInit(component, var, type);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]hlistener:%d init FAIL!\n", type);
        return ret;
    }

    ret = VsensorTaskHlistenerRegister(&(var->hlistener));
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]hlistener:%d reg FAIL!\n", type);
        return ret;
    }
    return SENSOR_OK;
}

static int32_t VsensorTaskVsensorRelyReg(VsensorTaskComm *component, enum SensorTypeTag type)
{
    CHECK_SENSOR_TYPE_ILLEGAL_RETURN_RET(type, SENSOR_TASK_HLISTENER_REG_TYPE_ERR);
    if (SensorUtilsIsVSensor(type)) {
        /* 依赖虚拟传感器，do nothing */
        HILOG_INFO(HILOG_MODULE_SEN, "[VTASK]type:%d IsVSensor do nothing", type);
        return SENSOR_OK;
    }
    /* 依赖物理传感器 */
    int32_t ret = VsensorTaskHlistenerRegEvent(component, type);
    if (ret != SENSOR_OK) {
        return ret;
    }
    return SENSOR_OK;
}

bool VsensorTaskVsensorIsRunning(const VsensorTaskInfo *vsensorInfo)
{
    if (vsensorInfo == NULL || vsensorInfo->vsensor == NULL) {
        return false;
    }
    if (vsensorInfo->type == SENSOR_TYPE_NONE) {
        return false;
    }
    if (vsensorInfo->state == SENSOR_STATE_OPEN) {
        return true;
    }
    return false;
}

void VsensorTaskCommInit(VsensorTaskComm *component)
{
    int32_t ret = VsensorTaskCheckCommComponents(component);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]init :component is NULL!\n");
        return;
    }

    for (uint32_t i = 0; i < VSENSOR_TASK_HLISTENER_MAX_NUM; i++) {
        ret = memset_s(&component->hlistenersList[i].hlistener, sizeof(HsensorListener), 0, sizeof(HsensorListener));
        if (ret != EOK) {
            HILOG_WARN(HILOG_MODULE_SEN, "init :component memset_s hlistener failed!\n");
        }
        SensorPara *openParam = &(component->hlistenersList[i].hlistener.openParam);
        SensorParaClear(openParam);
    }
    return;
}

int32_t VsensorTaskVsensorRegEvent(VsensorTaskComm *component, Vsensor *vsensor)
{
    int32_t ret = VsensorTaskCheckCommComponents(component);
    SENSOR_CHECK_OK(ret);
    CHECK_NULL_PTR_RETURN_VALUE(vsensor, SENSOR_VSENSOR_TASK_REG_VENSOR_NULL);

    HILOG_DEBUG(HILOG_MODULE_SEN, "[VTASK]vsensor:%d reg task:%d start", vsensor->type, component->taskId);

    ret = VsensorTaskVsensorReg(component, vsensor);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN,
            "[VTASK]vsensor:%d reg task:%d FAIL!, ret:0x%04x",
            vsensor->type,
            component->taskId,
            ret);
        return ret;
    }

    RelySensorsInfo *relySensors = vsensor->relySensors;
    CHECK_NULL_PTR_RETURN_VALUE(relySensors, SENSOR_OPEN_RELY_SENSORS_CHECK_NULL);
    HILOG_DEBUG(
        HILOG_MODULE_SEN, "[VTASK]vsensor:%d relySensorCnt:%u", vsensor->type, relySensors->relySensorCnt);
    for (uint8_t i = 0; i < relySensors->relySensorCnt; i++) {
        enum SensorTypeTag type = relySensors->relySensorArray[i].type;
        HILOG_DEBUG(HILOG_MODULE_SEN, "[VTASK]vsensor:%d relySensor :%u", vsensor->type, type);
        ret = VsensorTaskVsensorRelyReg(component, type);
        if (ret != SENSOR_OK) {
            HILOG_ERROR(HILOG_MODULE_SEN,
                "[VTASK]vsensor:%d rely sensor:%d FAIL!, ret:0x%04x\n",
                vsensor->type,
                type,
                ret);
            return ret;
        }
    }
    HILOG_DEBUG(HILOG_MODULE_SEN, "[VTASK]vsensor:%d reg task:%d SUCCESS", vsensor->type, component->taskId);
    return SENSOR_OK;
}

int32_t VsensorTaskSensorDataUpdateEvent(VsensorTaskComm *component, VsensorProcessData *data)
{
    int32_t ret;
    enum SensorTypeTag type;
    Vsensor *vsensor = NULL;
    VsensorTaskInfo *vsensorInfo = NULL;

    ret = VsensorTaskCheckCommComponents(component);
    SENSOR_CHECK_OK(ret);
    CHECK_NULL_PTR_RETURN_VALUE(data, SENSOR_TASK_HSDATA_UPDATE_DATA_NULL);

    type = data->sensor;

    vsensorInfo = &component->vsensorInfo;
    vsensor = vsensorInfo->vsensor;

    /* 判断算法是否在运行 */
    if (!VsensorTaskVsensorIsRunning(vsensorInfo)) {
        return SENSOR_SEND_TMSG_DEST_NOT_RUNNING;
    }

    /* 检查数据依赖 */
    if (!VsensorTaskSensorBeRelied(type, vsensor->relySensors->relySensorArray,
        vsensor->relySensors->relySensorCnt)) {
        return SENSOR_CHECK_RELY_SENSORS_SENSORIL;
    }

    if (vsensor->process == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]vsensor_process NULL! type:%d", vsensor->type);
        return SENSOR_TASK_VSENSOR_PROCESS_NULL;
    }
        ret = vsensor->process(type, data->sensordata, data->timestamp, data->dataLen);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]alg:%d process err:%x\n", vsensorInfo->type, ret);
        return SENSOR_CHECK_PROCESS_DATA_TS_ERR;
    }

    return SENSOR_OK;
}

static int32_t VsensorTaskVsensorOpenRely(
    VsensorTaskComm *component, VsensorTaskInfo *vsensorInfo, SensorPara *para, uint32_t openType)
{
    /* 在前面的处理中，不可能出现空指针现象 */
    Vsensor *vsensor = vsensorInfo->vsensor;
    if (vsensor == NULL) {
        return SENSOR_TASK_VSENSOR_INFO_FIND_NULL;
    }

    if (vsensor->open == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]vsensor_open NULL! type:%d", vsensorInfo->type);
        return SENSOR_TASK_VSENSOR_OPEN_NULL;
    }
    int32_t ret = vsensor->open(para);
    if (ret != SENSOR_OK) {
        return SENSOR_TASK_VSENSOR_OPEN_FAIL;
    }
    vsensorInfo->state = SENSOR_STATE_OPEN;
    if (openType == APP_CTRL) {
        SensorSetState(vsensor->type, SENSOR_STATE_OPEN);
        if (para != NULL) {
            SensorSetPara(vsensor->type, para);
        }
    }

    RelySensorsInfo *relySensors = vsensor->relySensors;
    ret = VsensorTaskOpenRelySensors(component, relySensors->relySensorArray, relySensors->relySensorCnt);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(
            HILOG_MODULE_SEN, "[VTASK]vsensor:%d open rely sensor FAIL 0x%04x!\n", vsensorInfo->type, ret);
        return ret;
    }
    HILOG_DEBUG(
        HILOG_MODULE_SEN, "[VTASK]VsensorOpenRely type:%d openType:%u SUCESS", vsensorInfo->type, openType);
    return SENSOR_OK;
}

int32_t VsensorTaskVsensorOpenEvent(
    VsensorTaskComm *component, enum SensorTypeTag type, SensorPara *para, uint32_t openType)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "[VTASK]OpenEvent type:%d openType:%u", type, openType);
    int32_t ret = VsensorTaskCheckCommComponents(component);
    SENSOR_CHECK_OK(ret);

    VsensorTaskInfo *vsensorInfo = &component->vsensorInfo;

    if (openType == APP_CTRL) {
        vsensorInfo->relyedInfo.appOpenFlag = true;
    } else { /* VSENSOR_CTRL */
        vsensorInfo->relyedInfo.relyedCnt++;
    }

    if (vsensorInfo->state == SENSOR_STATE_OPEN) {
        /* 已经开启vsensor，不再重复打开 */
        if (openType == APP_CTRL) {
            /* APP_CTRL 将状态刷新为OPEN */
            Vsensor *vsensor = vsensorInfo->vsensor;
            if (vsensor == NULL) {
                return SENSOR_TASK_VSENSOR_INFO_FIND_NULL;
            }
            SensorSetState(vsensor->type, SENSOR_STATE_OPEN);
        }
        return SENSOR_OK;
    }

    return VsensorTaskVsensorOpenRely(component, vsensorInfo, para, openType);
}

int32_t VsensorTaskVsensorCloseEvent(VsensorTaskComm *component, enum SensorTypeTag type, uint32_t closeType)
{
    int32_t ret = VsensorTaskCheckCommComponents(component);
    SENSOR_CHECK_OK(ret);

    VsensorTaskInfo *vsensorInfo = &component->vsensorInfo;
    if (closeType == APP_CTRL) {
        vsensorInfo->relyedInfo.appOpenFlag = false;
    } else {
        vsensorInfo->relyedInfo.relyedCnt--;
    }

    if (vsensorInfo->relyedInfo.relyedCnt > 0 || vsensorInfo->relyedInfo.appOpenFlag ||
        (vsensorInfo->state == SENSOR_STATE_CLOSE)) {
        /* （1）用户控制开和其他算法依赖开，都不能关闭虚拟传感器；（2）已经关闭的情况下无需再关闭一次 */
        if (closeType == APP_CTRL) {  // 关闭vsensor,但还存在依赖不能关闭，需将状态更新为close
            SensorSetState(type, SENSOR_STATE_CLOSE);
        }
        return SENSOR_OK;
    }

    /* 在前面的处理中，不可能出现空指针现象 */
    Vsensor *vsensor = vsensorInfo->vsensor;
    if (vsensor == NULL) {
        return SENSOR_TASK_VSENSOR_INFO_FIND_NULL;
    }

    if (vsensor->close == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]vsensor_close NULL! type:%d", type);
        return SENSOR_TASK_VSENSOR_CLOSE_NULL;
    }
    ret = vsensor->close();
    if (ret != SENSOR_OK) {
        return SENSOR_TASK_VSENSOR_CLOSE_FAIL;
    }
    vsensorInfo->state = SENSOR_STATE_CLOSE;
    if (closeType == APP_CTRL) {
        SensorSetState(vsensor->type, SENSOR_STATE_CLOSE);
    }

    RelySensorsInfo *relySensors = vsensor->relySensors;
    ret = VsensorTaskCloseRelySensors(component, relySensors->relySensorArray, relySensors->relySensorCnt);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[VTASK]vsensor:%d close rely sensor FAIL!, ret:0x%04x\n", type, ret);
        return ret;
    }

    return SENSOR_OK;
}
