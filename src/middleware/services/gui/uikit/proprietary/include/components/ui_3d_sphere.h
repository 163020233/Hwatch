/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UI3DSphere
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#ifndef UI_3D_SPHERE_H
#define UI_3D_SPHERE_H

#include "gfx_utils/image_info.h"
#include "components/ui_3d_view.h"
#include "imgdecode/image_load.h"

namespace OHOS {

class UI3DSphere : public UI3DView {
public:
    /**
     * @brief  Constructor of UI3DSphere.
     */
    UI3DSphere();

    /**
     * @brief  Destructor of UI3DSphere.
     */
    virtual ~UI3DSphere() override;

    /**
     * @brief  Rotate around X Axis.
     */
    void RotateAroundXAxis(float angle);

    /**
     * @brief  Rotate around Y Axis.
     */
    void RotateAroundYAxis(float angle);

    /**
     * @brief  Rotate around Z Axis.
     */
    void RotateAroundZAxis(float angle);

    /**
     * @brief  Get rotation angle around X Axis.
     */
    float GetAngleAroundXAxis();

    /**
     * @brief  Get rotation angle around Y Axis.
     */
    float GetAngleAroundYAxis();

    /**
     * @brief  Get rotation angle around Z Axis.
     */
    float GetAngleAroundZAxis();

    bool SetModel(std::string file) override;

    bool SetModel(FILE *fp, uint32_t offset, uint32_t len) override;

    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    UIViewType GetViewType() const override
    {
        return UI_3D_SPHERE;
    }

private:
    bool redrawMask_ = false;
    ImageInfo mask_ = {0};
    Path maskPath_ = {0};
    VGUSphereAttr sphereAttr_ = {0};

    bool PrepareMask();
};
}
#endif