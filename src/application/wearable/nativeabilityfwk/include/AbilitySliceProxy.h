/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AbilitySliceProxy
 * Create: 2025-11
 */

#ifndef NATIVEABILITYFWK_ABILITY_SLICE_PROXY_H
#define NATIVEABILITYFWK_ABILITY_SLICE_PROXY_H

#include "AbilitySlice.h"
#include "wearable_log.h"
#include "Presenter.h"

namespace OHOS {
template <class V, class P>
class AbilitySliceProxy : public SliceProxy {
public:
    Slice* CreateSlice(uint32_t targetId) override
    {
        V* v = new V();
        if (v == nullptr) {
            return nullptr;
        }
        P* p = new P();
        if (p == nullptr) {
            delete v;
            return nullptr;
        }
        return new AbilitySlice(v, p, targetId);
    }
};
} // namespace OHOS
#endif // NATIVEABILITYFWK_ABILITY_SLICE_PROXY_H
