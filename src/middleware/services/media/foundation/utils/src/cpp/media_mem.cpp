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

#include "media_mem.h"
#include <malloc.h>
#include "securec.h"
#include "media_log.h"
#ifdef HI3322_CHIP_V100
#include "osal_addr.h"
#include "chip_init.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void *MediaMemMalloc(const size_t bytesNum)
{
    if (bytesNum == 0) {
        return nullptr;
    }
#ifdef HI3322_CHIP_V100
    return osal_pool_mem_alloc(OS_EXTEND_MEM_ADDR, bytesNum);
#else
    return malloc(bytesNum);
#endif
}

void *MediaMemCalloc(const size_t bytesNum)
{
#ifdef HI3322_CHIP_V100
    void *tmp = osal_pool_mem_alloc(OS_EXTEND_MEM_ADDR, bytesNum);
#else
    void *tmp = malloc(bytesNum);
#endif
    if (tmp == nullptr) {
        MEDIA_ERR_LOG("%s", "malloc failed!");
        return nullptr;
    }

    if (memset_s(tmp, bytesNum, 0, bytesNum) != EOK) {
        MEDIA_ERR_LOG("%s", "memset_s failed!");
        MediaMemFree(tmp);
        return nullptr;
    } else {
        return tmp;
    }
}

void MediaMemFree(const void* src)
{
    if (src == nullptr) {
        MEDIA_ERR_LOG("%s", "don't need to free!");
        return;
    }
#ifdef HI3322_CHIP_V100
    osal_pool_mem_free(OS_EXTEND_MEM_ADDR, src);
#else
    free(const_cast<void*>(src));
#endif
}
#ifdef __cplusplus
};
#endif
