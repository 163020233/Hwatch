/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024. All rights reserved.
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

#include "notification_manager.h"
#include "graphic_service.h"

namespace OHOS {

bool NotificationManager::ShowNotify(const NewNotify& notify, uint16_t id, bool fullScreen, bool needPauseSlice)
{
    return false;
}

void NotificationManager::StartShowNotify()
{
}

void NotificationManager::RefreshNotify(const RefreshNotifyContent& refreshNotifyContent)
{
}

void NotificationManager::StartRefreshNotifyContent()
{
}

void NotificationManager::StopNotify()
{
}

void NotificationManager::StopShowNotify()
{
}

bool NotificationManager::HasNotifyShowing()
{
}

void NotificationManager::SetNotifyPriority(uint16_t prio)
{
}

/* 是否为音量界面 */
void NotificationManager::SetVolumeScreenStatus(bool status)
{
}

bool NotificationManager::GetVolumeScreenStatus()
{
    return false;
}

/* 是否为通话通知界面 */
void NotificationManager::SetWatchCallStatus(bool status)
{
}

bool NotificationManager::GetWatchCallStatus()
{
    return false;
}

void NotificationManager::SetNativeUIRunning(bool flag)
{
}

uint16_t NotificationManager::GetNotifyPriority()
{
}

uint16_t NotificationManager::GetShowingNotifyId()
{
}

void NotificationManager::Callback()
{
}

void NotificationManager::RegisterNotifyCleanupFunction(FreeNotifyfunc)
{
}

bool NotificationManager::IsCurNotifyNeedPause()
{
    return false;
}
}  // namespace OHOS
