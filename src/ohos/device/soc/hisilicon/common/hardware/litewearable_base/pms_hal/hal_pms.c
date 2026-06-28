/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// ohos begin
// Implement PMS Hi layer interface
#include "hal_pms.h"
#include "mem_util.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "cmsis_os2.h"

#define P_DIR_APP_BASE_DIR "/user/app/user/ace/etc/permissions/"
#define LOS_WAIT_FOREVER 0xFFFFFFFF
// Supported permissions
static const PermissionDef g_permissions[] = {
    {"ohos.permission.LOCATION",                   SYSTEM_GRANT},
    {"ohos.permission.ACTIVITY_MOTION",            SYSTEM_GRANT},
    {"ohos.permission.READ_HEALTH_DATA",           SYSTEM_GRANT},
    {"ohos.permission.ACCESS_DISTRIBUTED_ABILITY", SYSTEM_GRANT},
    {"ohos.permission.MODIFY_AUDIO_SETTINGS",      SYSTEM_GRANT},
    {"ohos.permission.VIBRATE",                    SYSTEM_GRANT},
    {"ohos.permission.ACCELEROMETER",              SYSTEM_GRANT},
    {"ohos.permission.GYROSCOPE",                  SYSTEM_GRANT},
};

static unsigned int g_permissionSize = sizeof(g_permissions) / sizeof(PermissionDef);
osMutexId_t mutex_locked = NULL;
osMutexAttr_t g_mutexAttr;

const PermissionDef* HalGetPermissionList(unsigned int *length)
{
    *length = g_permissionSize;
    return g_permissions;
}

const char *HalGetPermissionPath(void)
{
    return P_DIR_APP_BASE_DIR;
}

int HalGetMaxPermissionSize(void)
{
    return g_permissionSize;
}

void* HalMalloc(unsigned int size)
{
    return PSRAM_Malloc(size);
}

void HalFree(void *ptr)
{
    return PSRAM_Free(ptr);
}

int HalAccess(const char *pathname)
{
    int ret = open(pathname, O_RDONLY, S_IREAD | S_IWRITE);
    if (ret < 0) {
        return PERM_ERRORCODE_FILE_NOT_EXIST;
    }
    close(ret);
    return PERM_ERRORCODE_SUCCESS;
}

void HalMutexLock(void)
{
    if (mutex_locked == NULL) {
        mutex_locked = osMutexNew(&g_mutexAttr);
    }
    osStatus_t uwRet = osMutexAcquire(mutex_locked, LOS_WAIT_FOREVER);
    if (uwRet != 0) {
        osMutexDelete(mutex_locked);
        mutex_locked = NULL;
    }
}
void HalMutexUnlock(void)
{
    if (mutex_locked == NULL) {
        return;
    }
    osMutexRelease(mutex_locked);
}

bool HalIsValidPath(const char *path)
{
    return true;
}

// ohos end