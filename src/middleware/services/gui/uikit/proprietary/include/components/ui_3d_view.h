/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UI3DView
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#ifndef UI_3D_VIEW_H
#define UI_3D_VIEW_H

#include <cstring>
#include "components/ui_view.h"
#include "display_vgu.h"

namespace OHOS {

class UI3DView : public UIView {
public:
    /**
     * @brief  Constructor of UI3DView. Particle animation runs reapeatedly.
     */
    UI3DView();

    /**
     * @brief  Destructor of UI3DView.
     */
    virtual ~UI3DView() override;

    /**
     * @brief  Set 3D model file.
     */
    virtual bool SetModel(std::string file);

    /**
     * @brief  Set 3D model file by file pointer.
     */
    virtual bool SetModel(FILE *fp, uint32_t offset, uint32_t len);

    /**
     * @brief  Clear 3D model file.
     */
    void ClearModel();

    /**
     * @brief  Set 3D texture image.
     */
    virtual bool SetTexture(const ImageInfo* info);

    /**
     * @brief  Clear 3D texture image.
     */
    void ClearTexture();

    /**
     * @brief  Get 3D texture image.
     */
    const ImageInfo* GetTexture() const;

    void ReMeasure() override;

    UIViewType GetViewType() const override
    {
        return UI_3D_VIEW;
    }

protected:
    bool ConvertModel(std::string modelPath);
    bool Check3DValidity();

    ImageInfo* texture_ = nullptr;
    VGUModelSurface model_ = {0};
};
}
#endif