/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SliceProxy
 * Create: 2025-11
 */

#ifndef SLICE_PROXY_H
#define SLICE_PROXY_H
#include "Slice.h"

namespace OHOS {
class SliceProxy {
public:
    virtual Slice* CreateSlice(uint32_t targetId) = 0;
    SliceProxy() = default;
    virtual ~SliceProxy() = default;
    SliceProxy(const SliceProxy&) = delete;
    SliceProxy& operator=(const SliceProxy&) = delete;
    SliceProxy(SliceProxy&&) = delete;
    SliceProxy& operator=(SliceProxy&&) = delete;
};
} // namespace OHOS
#endif // SLICE_PROXY_H
