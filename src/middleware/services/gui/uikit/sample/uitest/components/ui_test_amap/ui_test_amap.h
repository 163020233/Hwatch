/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: UIAMapView
 * Author:
 * Create: 2026-01
 */

#ifndef UI_TEST_AMAP_H
#define UI_TEST_AMAP_H

#include "ui_test.h"
#include "components/ui_label_button.h"
#include "components/ui_amap_view.h"
#include "components/ui_scroll_view.h"

namespace OHOS {
class UITestAMap : public UITest, public UIView::OnClickListener {
public:
    UITestAMap() {}
    ~UITestAMap();

    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    void ShowAmapByIndex(int16_t index);

private:
    UITestAMap(const UITestAMap&) = delete;
    UITestAMap& operator=(const UITestAMap&) = delete;

    class AmapAnimator : public Animator, public AnimatorCallback {
    public:
        explicit AmapAnimator(UITestAMap* testAmap) : Animator(this, nullptr, 0, true)
        {
            testAmap_ = testAmap;
        }

        virtual ~AmapAnimator() {}
        void Callback(UIView* view) override;
        void SetChangeMapRes(bool isChangeMapRes)
        {
            isChangeMapRes_ = isChangeMapRes;
        }
    private:
        UITestAMap* testAmap_ = nullptr;
        bool isChangeMapRes_ = false;
        uint32_t lastTime_ = 0;
    };
    friend class AmapAnimator;

    void AMapTestDrawSvg(UIAMapView *amap, const char *svgFile, Rect rect);
    void AMapTestDrawImage(UIAMapView *amap, const char *jpegFile, Rect rect);
    void AMapTestDrawPoints(UIAMapView *amap);
    void AMapTestDrawPolyline(UIAMapView *amap);
    void AMapTestDrawPolygon(UIAMapView *amap);
    void AMapTestDrawText(UIAMapView *amap);
    void AMapTestMeasureText(UIAMapView *amap);
    void AMapTestDrawBitmap(UIAMapView *amap);
    void AMapTestDrawColor(UIAMapView *amap);
    void SetUpAmap();
    void SetUpControlBtn();

    UIScrollView *container_ = nullptr;
    UIAMapView *amapSvg_ = nullptr;
    UIAMapView *amapImage_ = nullptr;

    UILabelButton* changeSvgBtn_ = nullptr;
    UILabelButton* startAniBtn_ = nullptr;
    UILabelButton* startAniBtn2_ = nullptr;

    enum class ControlIndex {
        CHANGE_TILE_MAP = 1,
        START_TILE_MAP_ANIMAT,
        START_TILE_MAP_ANIMAT_NO_CHANGE,
    };

    uint8_t curIndex_ = 0;
    AmapAnimator* animator_ = nullptr;
};
}
#endif
