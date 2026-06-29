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

#include "sensor_tmsg.h"
#include "sensor_typedef.h"
#include "sensor_msgqueue.h"
#include "sensor_task.h"
#include "securec.h"
#include "sensor_memory.h"
#include "sensor_common.h"
#include "sensor_log.h"

#define FTMSG_WAIT_FOREVER                  0xFFFFFFFF  /* 接收消息永久等待 */

int32_t SensorTmsgSend(uint32_t taskId, SensorTmsg *tmsg)
{
    uint32_t tmsgSize = (uint32_t)sizeof(SensorTmsg);

    if (tmsg == NULL) {
        return SENSOR_SEND_TMSG_NULL;
    }

    if (SensorTaskGetRunningFlag(taskId) == false) {
        HILOG_ERROR(HILOG_MODULE_SEN, "taskId :%u NOT RUNNING", taskId);
        return SENSOR_SEND_TMSG_DEST_NOT_RUNNING;
    }

    SensorMsgQueueCfg *msgqueueHandle = SensorTaskGetTaskMsgqueueHandle(taskId);
    if (msgqueueHandle == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "msgqueueHandle null");
        return SENSOR_SEND_TMSG_MSG_QUEUE_NULL;
    }

    int32_t ret = SensorMsgQueueWriteCopy(msgqueueHandle->queueId, tmsg, tmsgSize, 0);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_SEN, "SensorMsgQueueWriteCopy error ret:0x%X\n", ret);
        return ret;
    }

    return SENSOR_OK;
}

int32_t SensorTmsgRecv(uint32_t taskId, SensorTmsg *tmsg)
{
    uint32_t tmsgSize = (uint32_t)sizeof(SensorTmsg);

    if (tmsg == NULL) {
        return SENSOR_RECV_TMSG_NULL;
    }
    SensorMsgQueueCfg *msgqueueHandle = SensorTaskGetTaskMsgqueueHandle(taskId);
    if (msgqueueHandle == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "SensorTmsgRecv msgqueueHandle NULL");
        return SENSOR_RECV_TMSG_MSG_QUEUE_NULL;
    }
    int32_t ret = SensorMsgQueueReadCopy(msgqueueHandle->queueId, tmsg, &tmsgSize, FTMSG_WAIT_FOREVER);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_SEN, "SensorTmsgRecv error ret:0x%X\n", ret);
        return SENSOR_RECV_TMSG_ERR;
    }
    return SENSOR_OK;
}

int32_t SensorTmsgFree(SensorTmsg *tmsg)
{
    if (tmsg == NULL) {
        return SENSOR_FREE_TMSG_NULL;
    }

    if (tmsg->info != NULL) {
        SensorFree(tmsg->info);
    }

    memset_s(tmsg, sizeof(SensorTmsg), 0, sizeof(SensorTmsg));

    return SENSOR_OK;
}
