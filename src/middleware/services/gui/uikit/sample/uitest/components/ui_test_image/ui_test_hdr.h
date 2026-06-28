/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UITestHDR
 * Create: 2025-12
 */

#ifndef UI_TEST_HDR_H
#define UI_TEST_HDR_H

#include "ui_test.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "core/render_manager.h"
#include "components/ui_image_view.h"
#include "components/ui_scroll_view.h"
#include "graphic_config.h"

#if CAPABILITY_HW_JPEG_DECODE
namespace OHOS {
class UITestHDR : public UITest {
public:
    UITestHDR() {}
    ~UITestHDR() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
private:
    void SetBackgroundColor();
    void TestHDR();
    void CreateLabel(const char* text);
    UIScrollView* container_ = nullptr;
    int16_t defaultBrightness_ = -1;
};
}
#endif
#endif