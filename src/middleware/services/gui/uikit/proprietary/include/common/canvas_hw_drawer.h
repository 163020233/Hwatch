/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: canvas hardware draw
 * Author: Hisi Graphic Team
 * Created: 2025-9
 */
#ifndef GRAPHIC_CANVAS_HARDWARE_DRAW_H
#define GRAPHIC_CANVAS_HARDWARE_DRAW_H
#include "common/graphic_hardware_types.h"
#include "components/ui_view.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "gfx_utils/diagram/common/paint.h"
#include "gfx_utils/rect.h"

namespace OHOS {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
enum CanvasHwDrawType {
    PARAM_TYPE_PATH,
    PARAM_TYPE_IMAGE,
    PARAM_TYPE_LABEL,
    PARAM_TYPE_BITMAP,
    PARAM_TYPE_RENDER_CACHE
};

union CanvasHwDrawObject {
    Path* path;
    UIImageView* imageView;
    UILabel* label;
    BufferInfo *bitmap;
    uint32_t cacheKey;
};

struct CanvasDrawParam {
    CanvasHwDrawType type;
    CanvasHwDrawObject object;
    int16_t index;
    Matrix3<float> matrix;
    Paint* paint;
    UIView* canvasView;
    uint8_t mixOpa;
};

class CanvasHwDrawer : public HeapBase {
public:
    static CanvasDrawParam* CreateCanvasDrawParam(CanvasHwDrawType type, const Paint* paint, UIView* canvas);
    static void DestoryCanvasDrawParam(CanvasDrawParam* param);
    static void DoDraw(BufferInfo& gfxDstBuffer, void* param, const Paint& paint,
        const Rect& rect, const Rect& invalidatedArea, const Style& style);
    static void DeleteParam(void* param);
private:
    CanvasHwDrawer() {}
    ~CanvasHwDrawer() {}
};
#endif
}
#endif // GRAPHIC_CANVAS_HARDWARE_DRAW_H