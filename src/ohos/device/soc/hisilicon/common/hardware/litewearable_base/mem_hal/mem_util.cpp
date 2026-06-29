/*
 * Copyright (c) 2020-2023 Huawei Device Co., Ltd.
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

#include "mem_util.h"
#include "psram.h"
#include "log.h"
#include "hiview_log.h"
#include "ace_mem_base.h"
#include "los_memory_pri.h"

static void *g_psramMemory = nullptr;

static void InitPSRam(uintptr_t start, size_t length)
{
    if (g_psramMemory == nullptr) {
        g_psramMemory = reinterpret_cast<void *>(start);
        HILOG_DEBUG(HILOG_MODULE_AAFWK, "MemUtils::InitPSRam g_psramMemory = %x", g_psramMemory);
        OHOS::ACELite::ACEMemHooks memoryHooks = {PSRAM_Malloc, PSRAM_Free, PSRAM_Calloc};
        InitMemHooks(memoryHooks);
    }
}

void *PSRAM_Malloc(uint32_t size)
{
    if (g_psramMemory == nullptr) {
        InitPSRam(PSRAM_HM_APP_SYS_ADDR, PSRAM_HM_APP_SYS_LENGTH);
    }
    void *value = LOS_MemAllocAlign(g_psramMemory, size, 4); // 4字节对齐
    if (value == nullptr) {
        OsMemInfoPrint(g_psramMemory);
    }
    return value;
}

void *PSRAM_Calloc(uint32_t nitems, uint32_t size)
{
    if (g_psramMemory == nullptr) {
        InitPSRam(PSRAM_HM_APP_SYS_ADDR, PSRAM_HM_APP_SYS_LENGTH);
    }
    uint32_t real_size = nitems * size;
    void *psram = LOS_MemAllocAlign(g_psramMemory, real_size, 4); // 4字节对齐
    if (psram != nullptr) {
        (void)memset_s((void *)psram, real_size, 0, real_size);
    }
    if (psram == nullptr) {
        OsMemInfoPrint(g_psramMemory);
    }
    return psram;
}

void PSRAM_Free(void *buffer)
{
    if (g_psramMemory == nullptr) {
        return;
    }
    LOS_MemFree(g_psramMemory, const_cast<void *>(buffer));
}

void AceMemInfoPrint(void)
{
    if (g_psramMemory == nullptr) {
        return;
    }
    PRINTK("Ace Heap Info\n");
    OsMemInfoPrint(g_psramMemory);
}
