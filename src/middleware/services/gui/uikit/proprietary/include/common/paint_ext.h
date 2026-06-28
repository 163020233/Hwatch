/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: extended paint
 * Author: Hisi Graphic Team
 * Created: 2025-9
 */
#ifndef GRAPHIC_PAINT_EXT_H
#define GRAPHIC_PAINT_EXT_H
#include "securec.h"
#include "gfx_utils/graphic_types.h"
#include "gfx_utils/vector.h"
#include "common/graphic_hardware_types.h"
#include "gfx_utils/diagram/common/paint.h"

namespace OHOS {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
/**
 *
 * @brief Defines PaintExt to extend Paint ability, that support hardware paint.
 *
 * @since 1.0
 * @version 1.0
 */
class PaintExt : public Paint {
public:
    /**
     * @brief A constructor used to create an <b>PaintExt</b> instance.
     * @since 1.0
     * @version 1.0
     */
    PaintExt()
        : fillRule_(FillRule::NON_ZERO), gradientSpread_(GradientSpread::SPREAD_PAD),
          paintType_(PaintType::PAINT_COLOR)
    {
        memset_s(&sweepGradient_, sizeof(sweepGradient_), 0, sizeof(sweepGradient_));
        memset_s(&pattern_.img, sizeof(pattern_.img), 0, sizeof(pattern_.img));
        pattern_.color = 0;
        pattern_.wrap = PatternType::PATTERN_DISABLE;
        SetMiterLimit(2); // 2:default value
    }

    /**
     * @brief A destructor used to delete the <b>PaintExt</b> instance.
     */
    ~PaintExt() override {}

    /**
     * @brief Set the linear gradient info.
     *
     */
    void SetLinearGradient(const LinearGradient& linear, const ColorStop* stops, uint16_t stopCount)
    {
        Paint::createLinearGradient(linear.startX, linear.startY, linear.endX, linear.endY);
        SetColorStops(stops, stopCount);
        paintType_ = PaintType::PAINT_GRADIENT;
    }

    /**
     * @brief Set the radial gradient info.
     *
     */
    void SetRadialGradient(const RadialGradient& radial, const ColorStop* stops, uint16_t stopCount)
    {
        Paint::createRadialGradient(radial.centerX, radial.centerY, radial.radius,
            radial.focalX, radial.focalY, radial.radius);
        SetColorStops(stops, stopCount);
        paintType_ = PaintType::PAINT_GRADIENT;
    }

    /**
     * @brief Set the sweep gradient info.
     *
     */
    void SetSweepGradient(const SweepGradient& sweep, const ColorStop* stops, uint16_t stopCount)
    {
        sweepGradient_ = sweep;
        SetColorStops(stops, stopCount);
        SetGradientFlag(Gradient::Sweep);
    }

    /**
     * @brief Set the fill rule object.
     *
     */
    void SetFillRule(const FillRule& fillRule)
    {
        fillRule_ = fillRule;
    }

    /**
     * @brief Set the gradient spread object.
     *
     */
    void SetGradientSpread(const GradientSpread& gradientSpread)
    {
        gradientSpread_ = gradientSpread;
    }

    /**
     * @brief Set the join Type.
     *
     */
    void SetJoinType(const JoinType& joinType)
    {
        if (joinType == JoinType::JOIN_MITER) {
            SetLineJoin(LineJoin::MITER_JOIN);
        } else if (joinType == JoinType::JOIN_ROUND) {
            SetLineJoin(LineJoin::ROUND_JOIN);
        } else if (joinType == JoinType::JOIN_BEVEL) {
            SetLineJoin(LineJoin::BEVEL_JOIN);
        }
    }

    /**
     * @brief Set the cap yype.
     *
     */
    void SetCapType(const CapType& capType)
    {
        if (capType == CapType::CAP_NONE) {
            SetLineCap(LineCap::BUTT_CAP);
        } else if (capType == CapType::CAP_ROUND) {
            SetLineCap(LineCap::ROUND_CAP);
        }
    }

    /**
     * @brief Set the antialiased flag.
     *
     */
    void SetAntialiased(bool antialiased)
    {
        antialiased_ = antialiased;
    }

    /**
     * @brief Set pattern params.
     * @param wrap means pattern fill type(fill, pad, repeat, reflect), color is used when wrap is fill mode,
              mat is used when pattern fill, img is image info for a picture.
     * @return No returns.
     */
    void SetWrapPattern(PatternType wrap, uint32_t color, Matrix3<float> mat, ImageInfo img)
    {
        pattern_.wrap = wrap;
        pattern_.color = color;
        pattern_.mat3 = mat;
        pattern_.img = img;
        paintType_ = PaintType::PAINT_PATTERN;
    }

    /**
     * @brief Set path data to the current path in the context.
     */
    void SetClipRect(Rect clipRect)
    {
        hasClipRect_ = true;
        clipRect_ = clipRect;
    }

    SweepGradient sweepGradient_;
    FillRule fillRule_;
    GradientSpread gradientSpread_;
    bool antialiased_ = true;
    PaintType paintType_;
    PatternInfo pattern_;
    Rect clipRect_;
    bool hasClipRect_ = false;

private:
    void SetColorStops(const ColorStop* stops, uint16_t stopCount)
    {
        if ((stops == nullptr) || (stopCount == 0)) {
            return;
        }
        Paint::StopAndColor* stopColors = Paint::GetStopAndColor();
        if (stopColors != nullptr) {
            UIFree(stopColors);
            stopColors = nullptr;
            Paint::SetStopAndColor(nullptr, 0);
        }

        stopColors = static_cast<Paint::StopAndColor*>(UIMalloc(sizeof(Paint::StopAndColor) * stopCount));
        if (stopColors == nullptr) {
            GRAPHIC_LOGE("UIMalloc for stops failed.");
            return;
        }

        if (memcpy_s(stopColors, sizeof(Paint::StopAndColor) * stopCount,
            stops, sizeof(Paint::StopAndColor) * stopCount) != EOK) {
            UIFree(stopColors);
        }
        Paint::SetStopAndColor(stopColors, stopCount);
    }
};
#endif
}
#endif
