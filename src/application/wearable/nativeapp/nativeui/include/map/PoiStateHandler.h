/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: PoiStateHandler - POI search state UI management
 * Created: 2026-01-14
 */

#ifndef POI_STATE_HANDLER_H
#define POI_STATE_HANDLER_H

#include <memory>
#include <vector>
#include <string>
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "components/ui_edit_text.h"
#include "components/ui_simple_list.h"
#include "IMapStateHandler.h"
#include "MapPageContext.h"
#include "IMapPageListener.h"
#include "MapResultContentItemView.h"

namespace OHOS {

class PoiStateHandler : public IMapStateHandler, public UIView::OnClickListener, public IPoiStateListener {
public:
    explicit PoiStateHandler(MapPageContext &ctx);
    ~PoiStateHandler() override = default;

    void Enter() override;
    void Exit() override;
    bool OnDragStart(const DragEvent &event) override
    {
        return true;
    }
    bool OnDrag(const DragEvent &event) override
    {
        return true;
    }
    bool OnDragEnd(const DragEvent &event) override;

    // UIView::OnClickListener
    bool OnClick(UIView &view, const ClickEvent &event) override;

    // IPoiStateListener 回调
    void OnPoiSearchSuccess() override;
    void OnPoiSearchFailed() override;

private:
    void SetUpUI();
    void CreateContainer();
    void CreateSwitchButton();
    void CreateEditText();
    void CreateFindButton();
    void CreateResultList();
    void RefreshResultList();

    // UISimpleList 的 C 风格回调（通过静态指针转发）
    static UIView *CreateContentListViewCb(uint8_t type);
    static void UpdateContentListViewCb(UIView *view, void *data, uint8_t type);

    static constexpr const char *PLACEHOLDER_TEXT = "请输入";
    static constexpr const char *POI_EDITTEXT_ID = "poiedittext";
    static constexpr const char *POI_FIND_BUTTON_ID = "poifindbutton";
    static constexpr const char *POI_SWITCH_BUTTON_ID = "poiswitchbutton";
    static constexpr int16_t DOWNCONTENTLIST_HEIGHT = 266;

    MapPageContext &ctx_;

    // 控件统一使用 unique_ptr 管理生命周期
    std::unique_ptr<UIViewGroup> poiContainer_;
    std::unique_ptr<UIEditText> editText_;
    std::unique_ptr<UILabelButton> switchButton_;
    std::unique_ptr<UILabelButton> buttonFind_;
    std::unique_ptr<UISimpleList> resultList_;

    std::vector<MapResultContentItemView *> createdViews_;
    std::vector<MapResultItem> localResults_;
    bool searchInProgress_ = false;
    size_t currentPresetIndex_ = 0;
};

}  // namespace OHOS
#endif  // POI_STATE_HANDLER_H
