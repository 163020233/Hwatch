/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ChangeSliceListener
 * Create: 2025-11
 */

#ifndef CHANGE_SLICE_LISTENER_H
#define CHANGE_SLICE_LISTENER_H
#include <cstdint>

#include "AppViewIDs.h"
#include "components/ui_view.h"
#include "NativeAbility.h"

namespace OHOS {
class ChangeSliceListener : public UIView::OnTouchListener {
public:
    ChangeSliceListener(AppViewId nextAppId)
    {
        nextAppId_ = nextAppId;
    }
    ~ChangeSliceListener() {}

    bool OnPress(UIView &view, const PressEvent &event)
    {
        (void)view;
        (void)event;
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ChangeSliceListener, nextAppId_=%d", nextAppId_);
        NativeAbility::GetInstance().ChangeSlice(nextAppId_);
        return true;
    }
    bool OnRelease(UIView &view, const ReleaseEvent &event)
    {
        (void)view;
        (void)event;
        return true;
    }
    bool OnCancel(UIView &view, const CancelEvent &event)
    {
        (void)view;
        (void)event;
        return true;
    }

private:
     uint16_t nextAppId_;
};
}
#endif
