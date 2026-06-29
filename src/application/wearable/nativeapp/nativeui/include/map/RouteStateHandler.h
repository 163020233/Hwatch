/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: RouteStateHandler - Route planning state UI management
 * Created: 2026-01-14
 */
#ifndef ROUTE_STATE_HANDLER_H
#define ROUTE_STATE_HANDLER_H

#include <memory>
#include <vector>
#include <string>
#include "components/ui_label_button.h"
#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "IMapStateHandler.h"
#include "MapPageContext.h"
#include "IMapPageListener.h"

namespace OHOS {

class RouteStateHandler : public IMapStateHandler, public UIView::OnClickListener, public IRouteStateListener {
public:
    explicit RouteStateHandler(MapPageContext &ctx);
    ~RouteStateHandler() override = default;

    void Enter() override;
    void Exit() override;

    // UIView::OnClickListener
    bool OnClick(UIView &view, const ClickEvent &event) override;

    // IRouteStateListener 回调
    void OnRoutePlanSuccess(int routeCount) override;
    void OnRoutePlanFailed() override;

private:
    void SetUpBackButton();
    void SetUpNaviButton();
    void SetUpRouteInfoPanel();
    void SetUpRouteListPanel();
    void SetUpZoomButtons();

    void CreateRouteListUI();
    void ClearRouteListUI();
    void OnRouteItemSelected(int index);
    void DrawRouteOnMap(int routeIndex);
    void ClearRouteOnMap();

    // 布局常量
    static constexpr int16_t BACK_X = 140;
    static constexpr int16_t BACK_Y = 15;
    static constexpr int16_t NAVI_BTN_X = 246;
    static constexpr int16_t NAVI_BTN_Y = 15;
    static constexpr int16_t INFO_X = 40;
    static constexpr int16_t INFO_Y = 55;
    static constexpr uint16_t INFO_W = 386;
    static constexpr uint16_t INFO_H = 50;
    static constexpr int16_t MAP_X = 23;
    static constexpr int16_t MAP_Y = 110;
    static constexpr uint16_t MAP_W = 420;
    static constexpr uint16_t MAP_H = 200;
    static constexpr int16_t ZOOM_IN_X = 350;
    static constexpr int16_t ZOOM_IN_Y = 130;
    static constexpr int16_t ZOOM_OUT_X = 350;
    static constexpr int16_t ZOOM_OUT_Y = 220;
    static constexpr int16_t LIST_X = 60;
    static constexpr int16_t LIST_Y = 320;
    static constexpr uint16_t LIST_W = 346;
    static constexpr uint16_t LIST_H = 120;
    static constexpr int16_t TOP_PANEL_H = 110;
    static constexpr int16_t BOTTOM_PANEL_Y = 310;
    static constexpr uint16_t BOTTOM_PANEL_H = 156;  // 466 - 310
    static constexpr int16_t LIST_REL_Y = 10;        // 320 - 310
    static constexpr int FONT_SIZE_SMALL = 14;
    static constexpr int FONT_SIZE_LARGE = 28;
    static constexpr int MARGIN_SMALL = 5;
    static constexpr int MARGIN_NORMAL = 10;
    static constexpr int RADIUS_SMALL = 6;
    static constexpr int RADIUS_NORMAL = 8;
    static constexpr int BORDER_WIDTH_THIN = 1;
    static constexpr int ROUTE_ITEM_HEIGHT = 32;
    static constexpr int INFO_TEXT_X = 50;
    static constexpr int INFO_START_Y = 2;
    static constexpr int INFO_END_Y = 26;
    static constexpr int INFO_TEXT_HEIGHT = 22;
    static constexpr int INFO_TEXT_RIGHT_MARGIN = 60;
    static constexpr int NAVI_BUTTON_WIDTH = 80;
    static constexpr int NAVI_BUTTON_HEIGHT = 36;

    static constexpr const char *START_NAVI_ID = "StartNaviButton";
    static constexpr const char *ROUTE_ITEM_PREFIX = "RouteItem_";

    MapPageContext &ctx_;

    std::unique_ptr<UILabelButton> backButton_;
    std::unique_ptr<UILabelButton> startNaviButton_;
    std::unique_ptr<UIViewGroup> routeInfoPanel_;
    std::unique_ptr<UILabel> startPointLabel_;
    std::unique_ptr<UILabel> endPointLabel_;
    std::unique_ptr<UIViewGroup> routeListPanel_;
    std::unique_ptr<UILabelButton> zoomInButton_;
    std::unique_ptr<UILabelButton> zoomOutButton_;
    std::unique_ptr<UIViewGroup> topPanel_;
    std::unique_ptr<UIViewGroup> bottomPanel_;

    std::vector<std::unique_ptr<UILabelButton>> routeButtons_;
    std::vector<std::string> routeButtonIds_;
    int32_t selectedRouteIndex_ = -1;
    int32_t routeOverlayId_ = -1;
    bool isRouteDrawn = false;
};

}  // namespace OHOS
#endif  // ROUTE_STATE_HANDLER_H
