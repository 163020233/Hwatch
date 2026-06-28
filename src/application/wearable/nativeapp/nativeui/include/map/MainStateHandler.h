/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: MainStateHandler - Main/home state UI management
 * Created: 2026-01-14
 */
#ifndef MAIN_STATE_HANDLER_H
#define MAIN_STATE_HANDLER_H

#include <memory>
#include "components/ui_label_button.h"
#include "IMapStateHandler.h"
#include "MapPageContext.h"

namespace OHOS {

class MainStateHandler : public IMapStateHandler, public UIView::OnClickListener {
public:
    explicit MainStateHandler(MapPageContext &ctx);
    ~MainStateHandler() override = default;

    void Enter() override;
    void Exit() override;

    // UIView::OnClickListener
    bool OnClick(UIView &view, const ClickEvent &event) override;

    // 静态访问点,供 MapService 回调通知
    static MainStateHandler *GetActive() { return s_active_; }
    
    // 下载完成回调
    void OnOfflineMapDownloadFinished(bool success);

private:
    void SetUpButtons();
    void ShowLoadingMask(const char *text);
    void HideLoadingMask();

#ifdef AMAP_DRAW_OVERLAY_TEST
    ACELite::map_polygon_overlay *polygonOverlay_ = nullptr;
    void TestPolygonOverlay();
#endif

    static constexpr int16_t BACK_X = 60;
    static constexpr int16_t BACK_Y = 60;
    static constexpr int16_t ZOOM_IN_X = 350;
    static constexpr int16_t ZOOM_IN_Y = 103;
    static constexpr int16_t ZOOM_OUT_X = 350;
    static constexpr int16_t ZOOM_OUT_Y = 193;
    static constexpr int16_t POI_X = 350;
    static constexpr int16_t POI_Y = 283;
    static constexpr int16_t DOWNLOAD_X = 60;
    static constexpr int16_t DOWNLOAD_Y = 370;
    static constexpr uint8_t LOADING_FONT_SIZE = 28;
    static constexpr int16_t LOADING_LABEL_W = 300;
    static constexpr int16_t LOADING_LABEL_H = 60;

    MapPageContext &ctx_;
    std::unique_ptr<UILabelButton> backButton_;
    std::unique_ptr<UILabelButton> zoomInButton_;
    std::unique_ptr<UILabelButton> zoomOutButton_;
    std::unique_ptr<UILabelButton> poiButton_;
    std::unique_ptr<UILabelButton> downloadButton_;

    // Loading 浮层
    std::unique_ptr<UIViewGroup> loadingMask_;
    std::unique_ptr<UILabel> loadingLabel_;

    // 下载状态(防止重入)
    volatile bool downloadInProgress_ = false;
    static MainStateHandler *s_active_;
};

}  // namespace OHOS
#endif  // MAIN_STATE_HANDLER_H
