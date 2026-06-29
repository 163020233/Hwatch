/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: UIAMapView
 * Author:
 * Create: 2026-01
 */

#ifndef UI_AMAP_VIEW_H
#define UI_AMAP_VIEW_H

#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <components/ui_canvas_ext.h>
#include <components/ui_view_group.h>
#include "common/task_manager.h"
#include "gfx_utils/graphic_types.h"
#include "gfx_utils/list.h"
#include "graphic_mutex.h"
#include "imgdecode/hw_jpeg_decoder.h"
#include "map/map_render_interface.h"

namespace OHOS {

#define MAX_UUID_LENGTH 32

class UIAMapView : public UIViewGroup, public IMapRender, public Task {
public:
    UIAMapView(const UIAMapView &) = delete;
    UIAMapView &operator=(const UIAMapView &) = delete;
    UIAMapView();
    ~UIAMapView() override;

    void BeginDraw(int32_t width, int32_t height, Rect mapRect) override;
    void CommitDraw() override;
    void DrawPoints(Point *points, uint32_t pointNum, PointStyle *style) override;
    void DrawPolyline(Point *points, uint32_t pointNum, StrokeStyle *style) override;
    void DrawPolygon(Point *points, uint32_t pointNum, MapPaintStyle *style) override;
    void DrawRect(Rect rect, MapPaintStyle *style, float angle) override;
    void DrawBitmap(BitmapBufferInfo *buffer, Rect rect, float angle) override;
    void DrawImage(ImageBufferInfo *image, Rect rect, float angle) override;
    void DrawText(Point center, const char *text, TextStyle *style, float angle) override;
    bool MeasureText(const char *text, TextStyle *style, int32_t *width, int32_t *height) override;
    void DrawSvg(SvgBufferInfo *svg, Rect rect, float angle) override;
    void Callback() override;

private:
    enum class DrawStatus {
        DRAW_STATUS_BEGIN,
        DRAW_STATUS_END,
        DRAW_STATUS_MAX,
    };

    enum class CanvasStatus {
        CANVAS_STATUS_INIT,
        CANVAS_STATUS_USED,
        CANVAS_STATUS_MAX,
    };

    typedef struct DrawingData {
        UICanvasExt canvas;
        CanvasStatus status;
        List<std::string> svgKeys;
        List<std::string> imgKeys;
    } DrawingData;

    typedef struct SvgCacheNode {
        void *parser = nullptr;
        uint32_t cacheKey = 0;
        uint8_t refCnt = 0;
    } SvgCacheNode;

    typedef struct ImgCacheNode {
        ImageInfo img;
        uint8_t refCnt = 0;
    } ImgCacheNode;

    typedef struct TextSizeCacheNode {
        std::string uuid;
        Point size;
    } TextSizeCacheNode;

    DrawStatus drawStatus_ = DrawStatus::DRAW_STATUS_MAX;
    DrawingData *curDrawingData_ = nullptr;
    std::queue<DrawingData *> DrawingDataQueue_;
    DrawingData *releasingData_ = nullptr;
    GraphicMutex drawLock_;
    const char *fontName_ = DEFAULT_VECTOR_FONT_FILENAME;
    uint16_t fontId_ = 0xff;
    std::unordered_map<std::string, SvgCacheNode *> svgCache_;
    std::unordered_map<std::string, ImgCacheNode *> imageCache_;
    std::unordered_map<std::string, TextSizeCacheNode *> textSizeCacheMap_;
    List<TextSizeCacheNode *> textSizeCacheList_;
    std::shared_ptr<bool> exists_;
    bool cacheRenderSupport_ = false;

    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;
    bool HandlePaintStyle(MapPaintStyle *style, PaintExt &paint);
    bool CheckImage(ImageBufferInfo *image);
    bool CheckSvg(SvgBufferInfo *svg);
    SvgCacheNode *ParseSvg(SvgBufferInfo *svg);
    void DrawSvgByCanvas(void *svgParser, Rect rect, float angle);
    void DrawSvgToCache(uint32_t pathCacheKey, void *svgParser);
    BufferInfo InitFakeBuffer();
    void UpdateCacheRef(DrawingData *drawingData);
};
} // namespace OHOS
#endif // UI_AMAP_VIEW_H