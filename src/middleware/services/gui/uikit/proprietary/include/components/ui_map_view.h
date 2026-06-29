/*
 * Copyright (c) 2024 HiSilicon (Shanghai) Technologies Co., Ltd. All rights reserved
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file ui_map_view.h
 *
 * @brief Defines the class to extend canvas/paint ability.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_MAP_VIEW_H
#define GRAPHIC_LITE_UI_MAP_VIEW_H
#include "components_ext/map_svg_parser.h"
#include "components/ui_view_group.h"
#include "draw/draw_utils.h"

namespace OHOS {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH

struct POIInfo {
    ImageDetail* img = nullptr;
    TextDetail* text[2] = {nullptr, nullptr};
    uint8_t tag = 0;
    uint16_t rank = 0;
    uint16_t id = 0;
    Rect rect;
    Point orignPos = {0, 0};
    bool visible = false;
    bool needUpdateVisible = false;
};

struct RoadInfo {
    TextDetail* text = nullptr;
    Rect rect;
    Point orignPos = {0, 0};
    bool visible = false;
};

class OnPOIClickListener : public HeapBase {
public:
    /* *
     * @brief Called when a POI is clicked.
     * @since 1.0
     * @version 1.0
     */
    virtual bool OnClick(uint32_t index)
    {
        return false;
    }

    /* *
     * @brief A destructor used to delete the <b>OnPOIClickListener</b> instance.
     * @since 1.0
     * @version 1.0
     */
    virtual ~OnPOIClickListener() {}
};

typedef enum {
    MAP_CACHE_GROUP_BACKGROUND,
    MAP_CACHE_GROUP_PATH,
    MAP_CACHE_GROUP_ROAD,
    MAP_CACHE_GROUP_POI
} MapCacheGroup;

class UIMapView : public UIViewGroup, public UIView::OnClickListener {
public:
    UIMapView(const UIMapView &) = delete;
    UIMapView &operator=(const UIMapView &) = delete;

    /* *
     * @brief A constructor used to create an <b>UIMapView</b> instance.
     *
     */
    UIMapView();

    /* *
     * @brief A destructor used to delete the <b>UIMapView</b> instance.
     *
     */
    ~UIMapView() override;

    /* *
     * @brief Get current view type.
     *
     */
    UIViewType GetViewType() const override
    {
        return UI_MAP_VIEW;
    }

    /* *
     * @brief Set SVG src with file path.
     *
     */
    bool SetSVG(const char* path);

    /* *
     * @brief Set SVG src with file buffer.
     *
     */
    bool SetSVG(uint8_t* data, uint32_t length);

    /* *
     * @brief Set POI on cick listener.
     *
     */
    void SetOnPOIClickListener(OnPOIClickListener* listener);

    /* *
     * @brief Enable cache. It will cache this tile immediately when enable is true,
     *        and free cache buffer immediately. We need to enable cache when setting svg,
     *        or after scale map finished, and disable cache before scale map.
     *
     */
    void EnableCache(bool enable);

    /* *
     * @brief Used to scale map base the target point.
     *
     */
    void ScaleMap(float scale, const Vector2<float>& pivot);

    /* *
     * @brief Hide the POI by tag.
     *
     */
    void HidePOI(uint16_t tag, bool isHide = true);
    void SetX(int16_t x) override;
    void SetY(int16_t y) override;

private:
    POIInfo* pois_ = nullptr;
    RoadInfo* roads_ = nullptr;
    OnPOIClickListener* poiListener_ = nullptr;
    bool imageCacheEnable_ = false;
    uint32_t pathCacheKey_ = 0;
    bool cacheRenderSupport_ = false;
    bool renderCacheValid_ = false;
    bool needUpdate_ = false;
    ImageInfo cacheInfo_ = {{0}};
    MapSVGParser parser_;
    PathDetail* pathArray_ = nullptr;
    uint8_t pathCount_ = 0;
    RoadDetail* roadArray_ = nullptr;
    uint8_t roadCount_ = 0;
    POIDetail* poiArray_ = nullptr;
    uint8_t poiCount_ = 0;
    const char* fontName_ = DEFAULT_VECTOR_FONT_FILENAME;
    uint16_t fontId_ = 0xff;
    uint8_t fontSize_ = 0;
    uint32_t poiMask_ = 0;
    float scale_ = 1.0f;
    bool isSetSVG_ = false;
    uint16_t pathCacheCnt_ = 0;
    uint16_t *roadCacheCnts_ = NULL;
    uint16_t *poiCacheCnts_ = NULL;
    uint16_t roadCacheStartId_ = 0;
    uint16_t poiCacheStartId_ = 0;

    void InitMapInfo();
    void DrawMapByRenderCache(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);
    void DrawPath(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);
    void DrawText(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, TextDetail& info, Point offset = {0, 0},
        bool isPOIText = false);
    Rect GetImageRect(ImageInfo& info, MapRect& rect);
    void DrawImage(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea,
        ImageInfo& info, MapRect& rect, Rect &imgRect);
    void DrawPOIs(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);
    void CachePOIsText(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);
    void DrawPOIsImg(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);
    void GetNormalTextOffset(TextDetail& info, int16_t &offsetX, int16_t &offsetY);
    void UpdatePOIsText();
    void SetPOIVisible(uint8_t id, bool visible);
    void DrawRoads(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);
    void UpdateRoadsText();
    bool RenderMapToCache(void);
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    void HandlePOICollision();
    void HandleRoadCollision();
    bool InitPOIInfo(int num);
    bool InitRoadInfo(int num);
    void ClearSVG();
    int32_t GetRenderCacheNum();
    bool DrawLetterVectorInner(BufferInfo& gfxDstBuffer, const LabelLetterInfo& letterInfo) const;
};
#endif
}
#endif
/**
 * @}
 */
