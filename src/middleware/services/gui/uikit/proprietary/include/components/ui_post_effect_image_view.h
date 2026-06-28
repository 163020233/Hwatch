/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UIPostEffectImageView
 * Author: Hisi Graphic Team
 * Created: 2025-5
 */

/**
 * @file post_effect_image_view.h
 *
 * @brief Declares an post effect image view.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef UI_POST_EFFECT_IMAGE_VIEW_H
#define UI_POST_EFFECT_IMAGE_VIEW_H

#include "components/ui_image_view.h"
#include "imgdecode/cache_manager.h"
#include "engines/gfx/lite_m_gfx_engine.h"

namespace OHOS {
/**
 * @brief Defines the functions related to an image view.
 *
 * @since 1.0
 * @version 1.0
 */
class UIPostEffectImageView : public UIImageView {
public:
    /**
     * @brief A default constructor used to create a <b>UIPostEffectImageView</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    UIPostEffectImageView() {}

    /**
     * @brief A destructor used to delete the <b>UIPostEffectImageView</b> instance.
     * @since 1.0
     * @version 1.0
     */
    ~UIPostEffectImageView() override;

    /**
     * @brief Draws this image view.
     *
     * @param invalidatedArea Indicates the area to draw.
     * @since 1.0
     * @version 1.0
     */
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    /**
     * @brief Enable colorize.
     *
     * @param enable Indicates state of colorize.
     * @since 1.0
     * @version 1.0
     */
    void EnableColorize(bool enable)
    {
        isEnableColorize_ = enable;
    }

    /**
     * @brief Gets the state of colorize.
     *
     * @since 1.0
     * @version 1.0
     */
    bool IsEnableColorize()
    {
        return isEnableColorize_;
    }

    /**
     * @brief Sets the color to infect image.
     *
     * @param color Indicates the color to infect image.
     * @since 1.0
     * @version 1.0
     */
    void SetColorize(ColorType color)
    {
        colorize_ = color;
    }

    /**
     * @brief Gets the color to infect image.
     *
     * @since 1.0
     * @version 1.0
     */
    ColorType GetColorize()
    {
        return colorize_;
    }

    /**
     * @brief Enable HSV adjust.
     *
     * @param enable Indicates state of HSV adjust.
     * @since 1.0
     * @version 1.0
     */
    void EnableHSVAdjust(bool enable)
    {
        isEnableHSVAdjust_ = enable;
    }

    /**
     * @brief Gets the state of HSV adjust.
     *
     * @since 1.0
     * @version 1.0
     */
    bool IsEnableHSVAdjust()
    {
        return isEnableHSVAdjust_;
    }

    /**
     * @brief Sets the HSV adjust info.
     *
     * @param hAdjust Indicates the adjust value of Hue: [-359, 359].
     * @param sAdjust Indicates the adjust value of Saturation: [-100, 100].
     * @param vAdjust Indicates the adjust value of Value: [-100, 100].
     * @param threshold Indicates the threshold of mask value: [0, 255].
     * @since 1.0
     * @version 1.0
     */
    void SetHSVInfo(int16_t hAdjust, int8_t sAdjust, int8_t vAdjust, int16_t threshold);

    /**
     * @brief Sets the HSV adjust mask.
     *
     * @param mask Indicates the path of mask.
     * @since 1.0
     * @version 1.0
     */
    void SetHSVMask(const std::string& mask);

    UIViewType GetViewType() const override
    {
        return UI_POST_EFFECT_IMAGE_VIEW;
    }

private:
    /**
     * @brief Represents the color to infect the image.
     */
    ColorType colorize_ = Color::White();
    bool isEnableColorize_ = false;
    bool isEnableHSVAdjust_ = false;

    struct HSVInfo {
        int16_t hAdjust;
        int8_t sAdjust;
        int8_t vAdjust;
        int16_t threshold;
    };

    static constexpr int16_t MIN_HADJ_VALUE = -359;
    static constexpr int16_t MAX_HADJ_VALUE = 359;
    static constexpr int8_t MIN_SADJ_VALUE = -100;
    static constexpr int8_t MAX_SADJ_VALUE = 100;
    static constexpr int8_t MIN_VADJ_VALUE = -100;
    static constexpr int8_t MAX_VADJ_VALUE = 100;
    static constexpr int16_t MIN_THRESHOLD_VALUE = 0;
    static constexpr int16_t MAX_THRESHOLD_VALUE = 255;

    HSVInfo hsvInfo_ = {0};
    ImageInfo* maskInfo_ = nullptr;
    std::string maskPath_;

    void DrawColorize(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);
    void DrawHSVAdjust(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);
};
} // namespace OHOS
#endif // GRAPHIC_LITE_POST_EFFECT_IMAGE_VIEW_H
/**
 * @}
 */
