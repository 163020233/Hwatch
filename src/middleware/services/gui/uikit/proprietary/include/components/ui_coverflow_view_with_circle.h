/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: Implements the UICoverflowView and provides the selection circle function.
 * Author: Hisi Graphic Team
 * Created: 2026-01
 */
#ifndef UI_COVERFLOW_VIEW_WITH_CIRCLE_H
#define UI_COVERFLOW_VIEW_WITH_CIRCLE_H
#include "common/graphic_hardware_types.h"
#include "components/ui_coverflow_view.h"
#include "components/ui_swipe_view.h"
#include "components/ui_view.h"
#include "display_vgu.h"
#include "gfx_utils/color.h"

namespace OHOS {
class CoverflowViewWithCircle : public UICoverflowView,
                                public UIView::OnDragListener,
                                public UISwipeView::OnSwipeListener {
public:
    CoverflowViewWithCircle()
    {
        SetOnDragListener(this);
        SetOnSwipeListener(this);
    }

    ~CoverflowViewWithCircle()
    {
        DeInit();
    }

    CoverflowViewWithCircle(const CoverflowViewWithCircle &) = delete;
    CoverflowViewWithCircle &operator=(const CoverflowViewWithCircle &) = delete;

    /**
     * @brief This is an initialization resource function, which must be called after the object is created.
     *
     */
    void Init();

    /**
     * @brief Clear resources
     *
     */
    void DeInit();

    /**
     * @brief Set the width of the selection circle
     *
     * @param width selection circle width
     */
    void SetSelectCircleWidth(float width);

    /**
     * @brief Sets the color of the selection circle.
     *
     * @param color The color of the selection circle.
     */
    void SetSelectCircleColor(ColorType color);

    /**
     * @brief Sets the radial gradient color stops.
     *
     * @param stops Array of color stops.
     * @param stopCount Number of color stops.
     */
    void SetRadialGradientStops(const ColorStop* stops, uint16_t stopCount);
protected:
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    void OnSwipe(UISwipeView& view) override;
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

private:
    void HandleStyleAndGradient(VGUPaintStyle& style, VGUGradient& gradient, int16_t& centerX,
        int16_t& centerY, uint16_t radius);
    float selectWidth_ = 0.0f;
    Path* selectCircle_ = nullptr;
    ColorType circleColor_;
    Paint::StopAndColor* stopColors_ = nullptr;
    uint8_t stopCount_ = 0;
    bool isRenderGradient_ = false;
    bool isNeedDrawSelectCircle_ = true;
};
} // namespace OHOS
#endif // UI_COVERFLOW_VIEW_WITH_CIRCLE_H