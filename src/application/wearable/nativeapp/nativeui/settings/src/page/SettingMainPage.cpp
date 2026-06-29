/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingMainPage
 * Create: 2025-06-01
 */
#include "components/ui_list_nested.h"
#include "components/ui_scroll_view_nested.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "settings/common/SettingCommon.h"
#include "settings/model/SettingMainModel.h"
#include "settings/model/SettingDesktopModel.h"
#include "settings/page/SettingMainPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::MAIN_PAGE, SettingMainPage, true);

static constexpr int16_t SETTING_LIST_POSITION_X = 4;
static constexpr int16_t SETTING_LIST_WIDTH = 446;
static constexpr int16_t SETTING_SCROLLBLANKSIZE = 96;
static constexpr int16 MAX_SCR_DIS = 1000;
static constexpr int16 SNUM_2 = 2;
static SettingMainPage *g_pSettingMainPage = nullptr;

static bool TransformItemBaseOnPosition(UITransformList* list, UIView* item)
{
    if (list == nullptr || item == nullptr) {
        return false;
    }
    if (item->GetViewType() != UI_TRANSFORM_GROUP) {
        return false;
    }
    UITransformGroup* group = dynamic_cast<UITransformGroup*>(item);
    if (group == nullptr) {
        return false;
    }
    int16_t w = group->GetWidth();
    int16_t h = group->GetHeight();
    if (h <= 0) {
        return false;
    }
    const uint16_t SCALE_FACTOR = list->GetWidth();
    if (SCALE_FACTOR <= 0) {
        return false;
    }
    if (list->GetDirection() == UIList::VERTICAL) {
        int16_t midY = list->GetY() + (list->GetHeight() / 2);
        int16_t y = group->GetY();
        int16_t offset;
        int16_t top = midY - (h / 2);
        int16_t buttom = midY + (h / 2);
        /**
         *   offset = y - k*h
         *   当k=0,   offset表示item左上角到中线的y轴位移 ，此时 y < top
         *   当k=1/2, offset表示item左侧居中位置到中线的y轴位移 ，此时  y = midY
         *   当k=1,   offset表示item左下角到中线的y轴位移 ，此时 y > buttom
         */
        if (y < top) {
            offset = y - midY;
        } else if (top <= y && y <= buttom) {
            float kh = 1.0f * (y - top) * (y - top) * (3 * buttom - top - 2 * y) / (h * h);
            offset = y - midY + static_cast<int16_t>(kh);
        } else {
            offset = y - midY + h;
        }
        int temp = EasingEquation::SineEaseIn(0, SCALE_FACTOR, abs(offset), SCALE_FACTOR);
        float scale = 1.0 - (static_cast<float>(temp) / SCALE_FACTOR);
        if (scale < 0.2) { // 0.2 : min scale
            scale = 0;
        }
        group->Scale(Vector2<float>(scale, scale), Vector2<float>(w / 2, h / 2));
        if ((y <= midY) && (y + group->GetHeight() >= midY)) {
            return true;
        }
    }
    return false;
}

void SettingMainPage::OnStart(void *data)
{
    g_pSettingMainPage = this;
    simpleList_ = new UISimpleTransformList();
    if (simpleList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingMainPage::OnStart simpleList_ new fail");
        return;
    }
    simpleList_->SetPosition(SETTING_LIST_POSITION_X, 0, SETTING_LIST_WIDTH, HORIZONTAL_RESOLUTION);
    simpleList_->SetScrollBlankSize(SETTING_SCROLLBLANKSIZE, UIAbstractScroll::Direction::BOTTOM);
    simpleList_->SetSelectPosition(HORIZONTAL_RESOLUTION / SNUM_2);
    simpleList_->SetTouchable(true);
    simpleList_->SetDraggable(true);
    simpleList_->SetThrowDrag(true);
    simpleList_->SetMaxScrollDistance(MAX_SCR_DIS);
    simpleList_->SetItemTransformFunc(&TransformItemBaseOnPosition);
    scrollView_ = new UIScrollViewNested();
    if (scrollView_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingMainPage::OnStart scrollView_ new fail");
        return;
    }
    scrollView_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    scrollView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    scrollView_->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    scrollView_->SetThrowDrag(true);
    scrollView_->SetDirection(UIAbstractScroll::HORIZONTAL);
    scrollView_->SetBoundaryIntercept(DragEvent::DIRECTION_LEFT_TO_RIGHT, false);
    scrollView_->Add(simpleList_);
    AddViewToPageContainer(scrollView_);
    if (PageTransitionMgr::GetInstance().GetTopSlideBackImage() == nullptr) { // 滑动跟手返回
        scrollView_->SetOnDragListener(this);
    }
    LoadListData();
}

void SettingMainPage::LoadListData()
{
    simpleList_->ClearAll();
    SettingMainModel::GetInstance().UpdateMainUnionData();
    for (auto& it : SettingMainModel::GetInstance().GetMainUnionData()) {
        Contents cont;
        cont.createFunc = SettingMainModel::OnSimpleListCreate;
        cont.updateFunc = SettingMainPage::OnSimpleListUpdate;
        cont.type = it.GetUnionItemType();
        cont.data = &it;
        simpleList_->AddContent(cont);
    }
    simpleList_->RefreshList();
}

void SettingMainPage::OnStop()
{
    g_pSettingMainPage = this;
    if (scrollView_ != nullptr) {
        scrollView_->RemoveAll();
        delete scrollView_;
        scrollView_ = nullptr;
    }
    if (simpleList_ != nullptr) {
        delete simpleList_;
        simpleList_ = nullptr;
    }
}

bool SettingMainPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) &&
        (event.GetDeltaX() > X_DRAG_OFFSET) && (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return false;
}

bool SettingMainPage::OnClick(UIView& view, const ClickEvent& event)
{
    uintptr_t simpleListViewId = (uintptr_t)view.GetViewId();
    if (simpleListViewId == (uintptr_t)ICON_TEXT_NAVIGATION) {
        int16_t viewIndex = view.GetViewIndex();
        auto& mainUnionData = SettingMainModel::GetInstance().GetMainUnionData();
        if (viewIndex < 0 || viewIndex >= static_cast<int16_t>(mainUnionData.size())) {
            return true;
        }

        SettingUnionItemData& itemData = mainUnionData[viewIndex];
        if (itemData.GetUnionItemType() == ICON_TEXT_NAVIGATION) {
            IconTextNavigationItemData* data = static_cast<IconTextNavigationItemData*>(itemData.GetUnionItemData());
            NativeAbility::GetInstance().SwitchPageInSlice(data->pageId,
                TransitionType::TRANSITION_INVALID, true);
        }
    }
    return true;
}

void SettingMainPage::OnSimpleListUpdate(UIView* view, void* data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    if (type == SettingUnionItemType::PLAIN_TEXT) {
        PlainTextItemView *itemGroup = dynamic_cast<PlainTextItemView*>(view);
        if (itemGroup == nullptr) {
            return;
        }
        PlainTextItemData* itemData = static_cast<PlainTextItemData*>(data);
        itemGroup->UpdateViewData(itemData);
    } else if (type == SettingUnionItemType::ICON_TEXT_TOGGLE) {
        IconTextToggleItemView *itemGroup = dynamic_cast<IconTextToggleItemView*>(view);
        if (itemGroup == nullptr) {
            return;
        }
        IconTextToggleItemData* itemData = static_cast<IconTextToggleItemData*>(data);
        itemGroup->UpdateViewData(itemData);
        itemGroup->SetOnClickListener(g_pSettingMainPage);
    } else if (type == SettingUnionItemType::ICON_TEXT_NAVIGATION) {
        IconTextNavigationItemView *itemGroup = dynamic_cast<IconTextNavigationItemView*>(view);
        if (itemGroup == nullptr) {
            return;
        }
        IconTextNavigationItemData* itemData = static_cast<IconTextNavigationItemData*>(data);
        itemGroup->UpdateViewData(itemData);
        itemGroup->SetOnClickListener(g_pSettingMainPage);
    }
    return;
}
}