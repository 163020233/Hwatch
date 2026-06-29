/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: amap controller
 * Created: 2026-01-14
 */

#include "gralloc_c_api.h"
#include "hals/gralloc_engines.h"

namespace OHOS {

extern "C" void *GrallocAllocBufferBySize(size_t size, unsigned int flag)
{
    if (size == 0) {
        return nullptr;
    }

    return (void *)GrallocEngines::GetInstance()->AllocBufferBySize(size, flag);
}

extern "C" void GrallocFreeBuffer(void *ptr)
{
    if (ptr == nullptr) {
        return;
    }
    auto *buffer = static_cast<uint8_t *>(ptr);
    GrallocEngines::GetInstance()->FreeBuffer(buffer);
}

};  // namespace OHOS