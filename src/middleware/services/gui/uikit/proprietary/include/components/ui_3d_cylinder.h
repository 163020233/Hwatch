/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UI3DCylinder
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#ifndef UI_3D_PLANE_H
#define UI_3D_PLANE_H

#include "components/ui_3d_view.h"

namespace OHOS {

class UI3DCylinder : public UI3DView {
public:
    /**
     * @brief  Constructor of UI3DCylinder. Particle animation runs reapeatedly.
     */
    UI3DCylinder();

    /**
     * @brief  Destructor of UI3DCylinder.
     */
    virtual ~UI3DCylinder() override;

    /**
     * @brief  Set start Y, which decides the part of texture that will be rendered. It defaults to 0.
     */
    void SetStartY(int16_t y);

    /**
     * @brief  Get start Y.
     */
    int16_t GetStartY() const;

    /**
     * @brief  Set camera angle.
     */
    void SetCameraAngle(float angle);

    /**
     * @brief  Set camera distance.
     */
    void SetCameraVFov(float distance);

    /**
     * @brief  Set camera aspect ratio.
     */
    void SetCameraAspectRatio(float ratio);

    /**
     * @brief  Set camera near plane clipping.
     */
    void SetCameraNearPlane(float value);

    /**
     * @brief  Set camera far plane clipping.
     */
    void SetCameraFarPlane(float value);

    /**
     * @brief  Set camera axis.
     */
    void SetCameraAxis(Vector3<float> axis);

    /**
     * @brief  Set camera scale.
     */
    void SetCameraScale(Vector3<float> scale);

    /**
     * @brief  Set camera translation.
     */
    void SetCameraTranslation(Vector3<float> translation);

    /**
     * @brief  Set camera position.
     */
    void SetCameraPosition(Vector3<float> position);

    bool SetTexture(const ImageInfo* info) override;

    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    UIViewType GetViewType() const override
    {
        return UI_3D_CYLINDER;
    }

private:
    void CalcSubRect();
    VGUCameraAttr cam_ = {0};
    Rect subRect_ = {0, 0, 1, 1};
    uint16_t startY_ = 0;
};
}
#endif