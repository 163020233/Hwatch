/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UIBoxProgressExt
 * Author: Hisi Graphic Team
 * Created: 2026-01
 */

#ifndef UI_BOX_PROGRESS_EXT_H
#define UI_BOX_PROGRESS_EXT_H

#include "components/ui_box_progress.h"

namespace OHOS {
/**
 * @brief Represents a custom linear progress bar.
 *        Compared to UIBoxProgress, it considers the length of cap when calculating the progress value.
 *        Besides, it doesn't support set image as background or foreground
 * @see UIBoxProgressExt
 */
class UIBoxProgressExt : public UIBoxProgress {
public:
    UIBoxProgressExt();
    virtual ~UIBoxProgressExt();
    UIBoxProgressExt(const UIBoxProgressExt &) = delete;
    UIBoxProgressExt &operator=(const UIBoxProgressExt &) = delete;

    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

protected:
    void GetBackgroundParam(Point& startPoint, int16_t& width, int16_t& height, uint16_t& radius, const Style& style);
    void DrawBackground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);
    void DrawForeground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);
    void GenerateFgPath();
    void GenerateL2RBgPath(Point startPoint, int16_t progressWidth, int16_t progressHeight, uint16_t radius);
    void GenerateL2RFgPath(Point startPoint, int16_t progressWidth, int16_t progressHeight, uint16_t radius);
    void GenerateR2LBgPath(Point startPoint, int16_t progressWidth, int16_t progressHeight, uint16_t radius);
    void GenerateR2LFgPath(Point startPoint, int16_t progressWidth, int16_t progressHeight, uint16_t radius);
    void GenerateT2BBgPath(Point startPoint, int16_t progressWidth, int16_t progressHeight, uint16_t radius);
    void GenerateT2BFgPath(Point startPoint, int16_t progressWidth, int16_t progressHeight, uint16_t radius);
    void GenerateB2TBgPath(Point startPoint, int16_t progressWidth, int16_t progressHeight, uint16_t radius);
    void GenerateB2TFgPath(Point startPoint, int16_t progressWidth, int16_t progressHeight, uint16_t radius);

    Path path_;
    Path pathBg_;
};
} // namespace OHOS
#endif // UI_BOX_PROGRESS_EXT_H
