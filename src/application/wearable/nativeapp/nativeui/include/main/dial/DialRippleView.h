/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: DialRippleView
 * Author:
 * Create: 2025-09-23
 */

#ifndef DIAL_RIPPLE_VIEW_H
#define DIAL_RIPPLE_VIEW_H
#include "components/ui_3d_mesh.h"

namespace OHOS {
class DialRippleView : public UI3DMesh {
public:
    DialRippleView();
    ~DialRippleView() override;
    bool OnClickEvent(const ClickEvent& event) override;
}; // DialRippleView
}
#endif // DIAL_RIPPLE_VIEW_H