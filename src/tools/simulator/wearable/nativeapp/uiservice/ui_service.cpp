/*
 * Copyright (c) 2024-2024 HiSilicon (Shanghai) Technologies Co., Ltd..
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

#include "uiservice/ui_service.h"
#include "wearable_log.h"
#include "samgr_lite.h"
#include "main/UiServersMsg.h"
#include "ohos_init.h"
#include "memory_adapter.h"
#include <cstdio>
#include "bundle_install_msg.h"
#include "graphic_service.h"
#include "applist/ApplistModel.h"
#include "screennotify/ui_screennotify.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

static const char *GetServiceName(Service *service);
static BOOL ServiceInitialize(Service *service, Identity identity);
static TaskConfig GetServiceTaskConfig(Service *service);
static BOOL ServiceMessageHandle(Service *service, Request *request);
static void ServiceMsgProcess(Request *request);
static int32_t SendMsg(int32_t msgId, uint32_t msgValue, void *data, int32_t dataLen);

const int DATA_DIST_STACK_SIZE = 0xC00;
const int DATA_DIST_MAIL_LENGTH = 10;
const int BYTE_OFFSET = 8;
static uint32_t g_interval = 5;

void ScreenTurnOn(int32_t timeout)
{
}

void ScreenTurnOff(void)
{
}

void ScreenOffEnable(int32_t enable)
{
}

static void Init()
{
}

const char *GetServiceName(Service *service)
{
    return UI_SERVICE;
}

BOOL ServiceInitialize(Service *service, Identity identity)
{
    return TRUE;
}

void ServiceMsgProcess(Request *request)
{
}

BOOL ServiceMessageHandle(Service *service, Request *request)
{
    return TRUE;
}

TaskConfig GetServiceTaskConfig(Service *service)
{
    TaskConfig config = {LEVEL_HIGH, PRI_NORMAL, DATA_DIST_STACK_SIZE, DATA_DIST_MAIL_LENGTH, SINGLE_TASK};
    return config;
}

static int32_t SendMsg(int32_t msgId, uint32_t msgValue, void *data, int32_t dataLen)
{
    return TRUE;
}

void SendMsgToUIService(int32_t msgId, uint32_t msgValue, void *data, int32_t dataLen)
{
}

#ifdef __cplusplus
}
#endif
