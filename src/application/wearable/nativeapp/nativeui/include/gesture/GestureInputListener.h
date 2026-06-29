/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: GestureInputListener.h
 * Author: Hisi Graphic Team
 * Created: 2025-10
 */

#ifndef GESTURE_INPUT_LISTENER_H
#define GESTURE_INPUT_LISTENER_H

#include "components/root_view.h"
#include "events/gesture_event.h"
#include "ohos_types.h"

namespace OHOS {
class GestureInputListener : public RootView::OnGestureListener {
public:
    static GestureInputListener* GetInstance();
    bool OnGesture(UIView& view, const GestureEvent& event) override;
private:
    GestureInputListener() {}
    ~GestureInputListener() {}
    virtual void OnGestureFlipWristOnce(void);
    virtual void OnGestureFlipWristTwice(void);
    virtual void OnGestureFistOnce(void);
    virtual void OnGestureFistTwice(void);
};
}
#endif // GESTURE_INPUT_LISTENER_H