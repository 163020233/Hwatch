/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: NaviStateHandler - Navigation state UI and coordination
 * Created: 2026-01-14
 */

#ifndef NAVI_STATE_HANDLER_H
#define NAVI_STATE_HANDLER_H

#include <memory>
#include "components/ui_label_button.h"
#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "IMapStateHandler.h"
#include "MapPageContext.h"
#include "IMapPageListener.h"
#include "NaviRouteRenderer.h"
#include "NaviSimulator.h"

namespace OHOS {

class NaviStateHandler : public IMapStateHandler, public UIView::OnClickListener, public INaviStateListener {
public:
    explicit NaviStateHandler(MapPageContext &ctx);
    ~NaviStateHandler() override = default;

    void Enter() override;
    void Exit() override;

    // UIView::OnClickListener
    bool OnClick(UIView &view, const ClickEvent &event) override;

    // INaviStateListener
    void OnLocationUpdated() override;
    void OnNaviStepChanged(int32_t stepIndex) override;
    void OnNaviArrived() override;
    void OnTurnApproaching(const char *message, double distance) override;
    void OnPassPoint(int32_t pointIndex) override;

private:
    // UI 搭建
    void SetUpStepPanel();
    void SetUpBottomBar();
    void SetUpButton(std::unique_ptr<UILabelButton> &btn, int16_t x, uint16_t w, const char *viewId,
                     const char *text, ColorType released, ColorType pressed, bool visible = true);
    void SetUpZoomButtons();
    void InitFirstStepUI();

    // UI 刷新
    void UpdateRemainingUI(int32_t remainDist, int32_t remainTime);
    void UpdateStepInfo(int32_t stepIndex, int32_t distToNextTurn);
    int32_t FindStepIndexByPolylineIndex(const RouteInfo *route, int32_t pointIndex) const;

    // 退出确认
    void ShowConfirmDialog();
    void HideConfirmDialog();

    // 重置地图朝北
    void ResetMapRotation();

    MapPageContext &ctx_;

    // 子模块
    std::unique_ptr<NaviRouteRenderer> renderer_;
    std::unique_ptr<NaviSimulator> simulator_;

    // 顶部步骤面板
    std::unique_ptr<UIViewGroup> stepPanel_;
    std::unique_ptr<UILabel> stepInstructionLabel_;
    std::unique_ptr<UILabel> stepActionLabel_;

    // 底部按钮栏
    std::unique_ptr<UIViewGroup> bottomPanel_;
    std::unique_ptr<UILabelButton> leaveButton_;
    std::unique_ptr<UILabel> distanceInfo_;
    std::unique_ptr<UILabelButton> simulateButton_;
    std::unique_ptr<UILabelButton> confirmButton_;
    std::unique_ptr<UILabelButton> cancelButton_;

    // 缩放
    std::unique_ptr<UILabelButton> zoomInButton_;
    std::unique_ptr<UILabelButton> zoomOutButton_;

    int32_t lastStepIndex_ = -1;
};

}  // namespace OHOS
#endif  // NAVI_STATE_HANDLER_H
