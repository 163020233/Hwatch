/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: GestureInputListener.cpp
 * Author: Hisi Graphic Team
 * Created: 2025-10
 */
#include "gesture/GestureInputListener.h"
#include "events/gesture_event.h"
#include "main/MainViewSample.h"
#include "stdio.h"
namespace OHOS {
GestureInputListener *GestureInputListener::GetInstance()
{
    static GestureInputListener instance;
    return &instance;
}

void GestureInputListener::OnGestureFlipWristOnce(void)
{}

void GestureInputListener::OnGestureFlipWristTwice(void)
{
    NativeAbility::GetInstance().ChangeSlice(VIEW_APPLIST, TransitionType::TRANSITION_ZOOM_OUT);
}

void GestureInputListener::OnGestureFistOnce(void)
{}

void GestureInputListener::OnGestureFistTwice(void)
{}

bool GestureInputListener::OnGesture(UIView &view, const GestureEvent &event)
{
    if (event.GetEvent() == GestureType::FLIP_WRIST_ONCE) {
        OnGestureFlipWristOnce();
    } else if (event.GetEvent() == GestureType::FLIP_WRIST_TWICE) {
        OnGestureFlipWristTwice();
    } else if (event.GetEvent() == GestureType::FIST_ONCE) {
        OnGestureFistOnce();
    } else if (event.GetEvent() == GestureType::FIST_TWICE) {
        OnGestureFistTwice();
    }
    return true;
}

}  // namespace OHOS