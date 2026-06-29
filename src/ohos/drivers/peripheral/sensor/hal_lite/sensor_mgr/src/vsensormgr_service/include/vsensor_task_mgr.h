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

#ifndef VENSOR_TASK_MGR_H
#define VENSOR_TASK_MGR_H

#include <stdbool.h>
#include "sensor_typedef.h"
#include "sensor_tmsg.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int32_t VsensorTaskMgrInit(uint32_t taskId);
void VsensorTaskMgrDeInit(uint32_t taskId);
int32_t VsensorTaskMgrMsgProcess(uint32_t taskId, const SensorTmsg *tmsg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
