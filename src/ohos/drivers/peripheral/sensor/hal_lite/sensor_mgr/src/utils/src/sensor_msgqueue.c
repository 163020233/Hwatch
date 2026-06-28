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

#include "sensor_msgqueue.h"
#include "sensor_log.h"

int32_t SensorMsgQueueCreate(SensorMsgQueueCfg* cfg, uint32_t taskId)
{
    if (cfg == NULL) {
        return 0;
    }
    HILOG_INFO(HILOG_MODULE_SEN, "task:%d create msgqueue.\n", taskId);
    return osal_msg_queue_create(cfg->name, cfg->queueLen, &(cfg->queueId), cfg->flags, cfg->maxMsgSize);
}

int32_t SensorMsgQueueWriteCopy(unsigned int queueId, void *bufferAddr, unsigned int bufferSize, unsigned int timeout)
{
    return osal_msg_queue_write_copy(queueId, bufferAddr, bufferSize, timeout);
}

int32_t SensorMsgQueueReadCopy(unsigned int queueId, void *bufferAddr, unsigned int *bufferSize, unsigned int timeout)
{
    return osal_msg_queue_read_copy(queueId, bufferAddr, bufferSize, timeout);
}

void SensorMsgQueueDelete(unsigned int queueId)
{
    osal_msg_queue_delete(queueId);
}