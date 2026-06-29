/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WaterfallDesktopFragment.cpp
 * Author:
 * Create: 2025-07-04
 */

#include "applist/WaterfallDesktopFragment.h"
#include "applist/DesktopFactory.h"
#include "common/image_cache_manager.h"
#include "common/screen.h"

namespace OHOS {
REGIST_DESKTOP_STYLE(WATERFALL_STYLE, WaterfallDesktopFragment);

constexpr const char* DELETE_IMG = APP_RES_PATH"/DELETE.bin";

void WaterfallDesktopFragment::OnCreateView(void* data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WaterfallDesktopFragment::OnCreateView ");
    if (waterfallList_ == nullptr) {
        waterfallList_ = new UIWaterfallList();
        if (!waterfallList_) {
            return;
        }
    }
    waterfallList_->SetPosition(0, 0, fragmentView_.GetWidth(), fragmentView_.GetHeight());
    imgDistance_ = static_cast<uint16_t>(waterfallList_->GetWidth() * 0.25991); // 0.25991, empirical coefficient
    imgSize_ = static_cast<uint16_t>(waterfallList_->GetWidth() * 0.23348); // 0.23348, empirical coefficient
    waterfallList_->SetImageDistance(imgDistance_);
    waterfallList_->SetImageSizeInCenter(imgSize_);
    int16_t blankSize = waterfallList_->GetRelativeRect().GetWidth() > waterfallList_->GetRelativeRect().GetHeight() ?
        waterfallList_->GetRelativeRect().GetWidth() / 2 : waterfallList_->GetRelativeRect().GetHeight() / 2; // 2, half
    waterfallList_->SetThrowDrag(true);
    uint16_t layoutHeight = static_cast<uint16_t>(imgDistance_ * Sin(60));
    waterfallList_->SetReboundSize(layoutHeight); // 60: degree, row's layout height
    waterfallList_->SetScrollBlankSize(layoutHeight, UIAbstractScroll::Direction::TOP);
    waterfallList_->SetScrollBlankSize(Screen::GetInstance().GetHeight() / 2 - layoutHeight, // 2: half
        UIAbstractScroll::Direction::BOTTOM);
    waterfallList_->SetRotateFactor(10); // 10, empirical coefficient
    waterfallList_->SetTouchable(false);
    waterfallList_->SetDraggable(false); // ensure that it can't be dragged during animation to avoid error display
    waterfallList_->EnableEdit(true);
    waterfallList_->SetGridScale(0.75);
    waterfallList_->SetOnModeChangeListener(this);
    fragmentView_.Add(waterfallList_);
}

void WaterfallDesktopFragment::OnDestroyView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WaterfallDesktopFragment::OnDestroyView");

    if (waterfallList_ == nullptr) {
        return;
    }

    if (!waterfallList_->IsEnableEdit()) {
        ClearAppItemToList();
        fragmentView_.Remove(waterfallList_);
        delete waterfallList_;
        waterfallList_ = nullptr;
        return;
    }

    AppItem* appItemArray = const_cast<AppItem*>(ApplistModel::GetInstance().GetApplistItems());
    memset_s(appItemArray, sizeof(AppItem) * APP_MAX_NUM, 0, sizeof(AppItem) * APP_MAX_NUM);

    uint16_t childrenCnt = waterfallList_->GetChildrenNumber();
    uint16_t index = 0;
    for (uint16_t i = 0; i < childrenCnt; i++) {
        WaterfallItemView* view = dynamic_cast<WaterfallItemView*>(waterfallList_->GetViewByIndex(i));
        if (view != nullptr) {
            appItemArray[index++] = view->GetItemInfo();
        }
    }
    ApplistModel::GetInstance().SetAppCnt(index);

    ClearAppItemToList();
    fragmentView_.Remove(waterfallList_);
    delete waterfallList_;
    waterfallList_ = nullptr;
}

void WaterfallDesktopFragment::OnResumeView()
{
    if (waterfallList_ != nullptr) {
        waterfallList_->SetTouchable(true);
        waterfallList_->SetDraggable(true); // resume touchable after transition animation
        waterfallList_->SetOnRotateListener(this);
        waterfallList_->RequestFocus();
    }
}

void WaterfallDesktopFragment::OnPauseView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WaterfallDesktopFragment::OnPauseView");
    if (waterfallList_ != nullptr) {
        waterfallList_->SetTouchable(false);
        waterfallList_->SetDraggable(false);
        waterfallList_->ClearFocus();
        waterfallList_->SetOnRotateListener(nullptr);
    }
}

bool WaterfallDesktopFragment::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    WaterfallItemView *itemView = dynamic_cast<WaterfallItemView *>(&view);
    if (itemView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "itemView is nullptr");
        return false;
    }
    UIWaterfallList *waterfallView = dynamic_cast<UIWaterfallList *>(itemView->GetParent());
    if (waterfallView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "waterfallView is nullptr");
        return false;
    }

    UIWaterfallList::WaterfallListMode mode = waterfallView->GetMode();
    if (mode == UIWaterfallList::WaterfallListMode::NORMAL) {
        AppViewId viewId = itemView->GetViewId();
        char *uid = const_cast<char *>(itemView->GetAppUid());
        if (uid == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "wuid is nullptr");
            return false;
        }
        waterfallView->SetDraggable(false);
        waterfallView->Scale(1.0f, itemView->GetViewIndex());
        return SwitchView(viewId, uid, TransitionType::TRANSITION_WATERFALL, WATERFALL_STYLE);
    } else if (mode == UIWaterfallList::WaterfallListMode::GRID) {
        if (itemView->GetItemInfo().deletable) {
            waterfallView->Remove(&view);
            ListNode<WaterfallItemView*>* viewNode = waterfallItemList_.Head();
            while (viewNode != waterfallItemList_.End()) {
                if (viewNode->data_ == &view) {
                    delete(viewNode->data_);
                    waterfallItemList_.Remove(viewNode);
                    break;
                }
                viewNode = viewNode->next_;
            }
        }
    }
    return true;
}

void WaterfallDesktopFragment::AddAppItemToList(const AppItem &item)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WaterfallDesktopFragment::AddAppItemToList");
    WaterfallItemView *itemView = new WaterfallItemView();
    if (!itemView) {
        return;
    }
    itemView->SetStyle(STYLE_BACKGROUND_OPA, 0);
    itemView->Resize(imgSize_, imgSize_);
    if (!itemView->SetItemInfo(item)) {
        delete itemView;
        return;
    }

    itemView->SetTouchable(true);
    itemView->SetOnClickListener(this);
    if (waterfallList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AddAppItemToList failed: waterfallList_ is null");
        delete itemView;
        return;
    }
    waterfallList_->Add(itemView);
    waterfallItemList_.PushBack(itemView);
}

void WaterfallDesktopFragment::ClearAppItemToList()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WaterfallDesktopFragment::ClearAppItemToList");
    if (waterfallList_ != nullptr) {
        waterfallList_->RemoveAll();
    }
    ListNode<WaterfallItemView*>* viewNode = waterfallItemList_.Head();
    while (viewNode != waterfallItemList_.End()) {
        delete(viewNode->data_);
        viewNode = viewNode->next_;
    }
    waterfallItemList_.Clear();
}

void WaterfallDesktopFragment::RefreshAppList(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WaterfallDesktopFragment::RefreshAppList");
    waterfallList_->LayoutChildren(true);
}

void WaterfallDesktopFragment::OnModeChange(UIWaterfallList* view, UIWaterfallList::WaterfallListMode targetMode)
{
    bool deleteIconVisible = false;
    if (targetMode == UIWaterfallList::WaterfallListMode::GRID ||
        targetMode == UIWaterfallList::WaterfallListMode::GRID_FLOAT ||
        targetMode == UIWaterfallList::WaterfallListMode::GRID_TO_GRID_FLOAT) {
        deleteIconVisible = true;
    }
    WaterfallItemView* child = dynamic_cast<WaterfallItemView*>(view->GetChildrenHead());
    for (uint16_t i = 0; i < view->GetChildrenNumber(); i++) {
        const AppItem& info = child->GetItemInfo();
        if (info.deletable) {
            child->SetDeleteIconVisible(deleteIconVisible);
        }
        child = dynamic_cast<WaterfallItemView*>(child->GetNextSibling());
        if (child == nullptr) {
            return;
        }
    }
}

WaterfallItemView::~WaterfallItemView()
{
    if (itemInfo_.iconSmall == nullptr && itemInfo_.resPath != nullptr) {
        ImageCacheManager::GetInstance().UnloadOneInMultiRes(itemInfo_.resId, itemInfo_.resPath);
    }
    RemoveAll();
}

bool WaterfallItemView::SetItemInfo(const AppItem &itemInfo)
{
    itemInfo_ = itemInfo;
    if (itemInfo.iconSmall == nullptr && itemInfo.resPath != nullptr) {
        ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(itemInfo.resId, itemInfo.resPath);
        if (image == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Load App Icon path %s Image %x fail",
                itemInfo.resId, itemInfo.resPath);
            return false;
        }
        itemImg_.SetSrc(image);
    } else {
        itemImg_.SetSrc(itemInfo.iconSmall);
    }
    if (itemImg_.GetParent() != this) {
        Add(&itemImg_);
        itemImg_.Resize(GetWidth(), GetHeight());
    }

    if (itemInfo.deletable) {
        ImageInfo* deleteImgInfo = ImageCacheManager::GetInstance().LoadSingleRes(DELETE_IMG);
        if (deleteImgInfo != nullptr) {
            deleteImg_.SetSrc(deleteImgInfo);
            if (deleteImg_.GetParent() != this) {
                Add(&deleteImg_);
                deleteImg_.SetPosition(0, 0, 30, 30); // 30: width and height
                deleteImg_.SetVisible(false);
            }
        }
    }
    return true;
}

const AppItem& WaterfallItemView::GetItemInfo() const
{
    return itemInfo_;
}

AppViewId WaterfallItemView::GetViewId()
{
    return static_cast<AppViewId>(itemInfo_.id);
}

const char *WaterfallItemView::GetAppUid()
{
    return itemInfo_.uid;
}

void WaterfallItemView::SetDeleteIconVisible(bool visible)
{
    deleteImg_.SetVisible(visible);
}
}
