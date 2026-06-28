/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: DialRippleView
 * Author:
 * Create: 2025-09-23
 */

#include "main/dial/DialRippleView.h"

namespace OHOS {
DialRippleView::DialRippleView()
{
    SetMeshType(VGU_MESH_RIPPLE);
    SetTouchable(true);
    SetRealTimeScreencap(true);
}

DialRippleView::~DialRippleView()
{
}

bool DialRippleView::OnClickEvent(const ClickEvent& event)
{
    SetMeshPosition(event.GetCurrentPos().x, event.GetCurrentPos().y);
    Start();
    return true;
}
} // namespace OHOS
