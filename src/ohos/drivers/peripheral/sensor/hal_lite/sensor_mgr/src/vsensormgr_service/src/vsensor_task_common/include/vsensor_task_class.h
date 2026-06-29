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

#ifndef VSENSOR_TASK_CLASS_H
#define VSENSOR_TASK_CLASS_H

#include "vsensor_task_comm.h"

typedef struct {
    VsensorTaskComm component;
    SensorTmsgProcMap *tmsgProcMap;
    uint16_t tmsgProcMapLen;
} VsensorTaskClass;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* aclass对象创建 */
VsensorTaskClass *VsensorTaskClassInit(uint32_t taskId, const char *taskPrefix, NotifyCallback hdataCb,
    const SensorTmsgProcMap *tmsgProcMap, uint16_t tmsgProcMapLen);

/* aclass对象销毁 */
void VsensorTaskClassDestruct(VsensorTaskClass *vSensorClass);

/* 通过 taskid 查找 aclass 对象 */
VsensorTaskClass *VsensorTaskClassGetByTaskid(uint32_t taskId);

/* aclass类公用的消息处理函数 */
int32_t VsensorTaskClassMsgProcess(VsensorTaskClass *vSensorClass, const SensorTmsg *tmsg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif