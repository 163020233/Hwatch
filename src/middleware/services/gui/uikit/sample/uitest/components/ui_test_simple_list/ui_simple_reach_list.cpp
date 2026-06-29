/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: List with built-in adapter sample
 * Author: Hisi Team
 * Created: 2025-12
 */

#include "ui_simple_reach_list.h"

namespace OHOS {

bool UISimpleReachList::DragXInner(int16_t distance)
{
    int16_t oldTailLeft = childrenHead_->GetX();
    int16_t oldHeadRight = childrenTail_->GetRelativeRect().GetRight() + childrenTail_->GetStyle(STYLE_MARGIN_RIGHT);
    bool ret = UIList::DragXInner(distance);
    int16_t newTailLeft = childrenHead_->GetX();
    int16_t newHeadRight = childrenTail_->GetRelativeRect().GetRight() + childrenTail_->GetStyle(STYLE_MARGIN_RIGHT);
    if (reachListener_ != nullptr) {
        DetectReachBorderHor(oldTailLeft, oldHeadRight, newTailLeft, newHeadRight);
    }
    return ret;
}

bool UISimpleReachList::DragYInner(int16_t distance)
{
    int16_t oldHeadTop = childrenHead_->GetY();
    int16_t oldTailBottom = childrenTail_->GetRelativeRect().GetBottom() + childrenTail_->GetStyle(STYLE_MARGIN_BOTTOM);
    bool ret = UIList::DragYInner(distance);
    int16_t newHeadTop = childrenHead_->GetY();
    int16_t newTailBottom = childrenTail_->GetRelativeRect().GetBottom() + childrenTail_->GetStyle(STYLE_MARGIN_BOTTOM);
    if (reachListener_ != nullptr) {
        DetectReachBorderVer(oldHeadTop, oldTailBottom, newHeadTop, newTailBottom);
    }
    return ret;
}

void UISimpleReachList::DetectReachBorderHor(int16_t oldHeadLeft, int16_t oldTailRight,
                                             int16_t newHeadLeft, int16_t newTailRight)
{
    int16_t listWidth = GetWidth();
    int16_t listLeft = scrollBlankSizeLeft_;
    int16_t listRight = listWidth - scrollBlankSizeRight_;
    int16_t firstIndex = 0;
    int16_t lastIndex = GetItemCount() - 1;
    bool isReachLeft = false;
    bool isReachRight = false;
    if (reboundSizeLeft_ == 0) {
        if (oldHeadLeft < listLeft && newHeadLeft >= listLeft && childrenHead_->GetViewIndex() == firstIndex) {
            isReachLeft = true;
        }
    } else {
        if (oldHeadLeft > listLeft && newHeadLeft <= listLeft && childrenHead_->GetViewIndex() == firstIndex) {
            isReachLeft = true;
        }
    }
    if (reboundSizeRight_ == 0) {
        if (oldTailRight > listRight && newTailRight <= listRight && childrenTail_->GetViewIndex() == lastIndex) {
            isReachRight = true;
        }
    } else {
        if (oldTailRight < listRight && newTailRight >= (listRight - 1) && childrenTail_->GetViewIndex() == lastIndex) {
            isReachRight = true;
        }
    }

    if (reachListener_ != nullptr) {
        if (isReachLeft) {
            reachListener_->OnReachStart();
        }
        if (isReachRight) {
            reachListener_->OnReachEnd();
        }
    }
}

void UISimpleReachList::DetectReachBorderVer(int16_t oldHeadTop, int16_t oldTailBottom, int16_t newHeadTop,
                                             int16_t newTailBottom)
{
    int16_t listHeight = GetHeight();
    int16_t listTop = scrollBlankSizeTop_;
    int16_t listBottom = listHeight - scrollBlankSizeBottom_;
    int16_t firstIndex = 0;
    int16_t lastIndex = GetItemCount() - 1;
    bool isReachTop = false;
    bool isReachBottom = false;
    if (reboundSizeTop_ == 0) {
        if (oldHeadTop < listTop && newHeadTop >= listTop && childrenHead_->GetViewIndex() == firstIndex) {
            isReachTop = true;
        }
    } else {
        if (oldHeadTop > listTop && newHeadTop <= listTop && childrenHead_->GetViewIndex() == firstIndex) {
            isReachTop = true;
        }
    }
    if (reboundSizeBottom_ == 0) {
        if (oldTailBottom > listBottom && newTailBottom <= listBottom && childrenTail_->GetViewIndex() == lastIndex) {
            isReachBottom = true;
        }
    } else {
        if (oldTailBottom < listBottom && newTailBottom >= (listBottom - 1)
            && childrenTail_->GetViewIndex() == lastIndex) {
            isReachBottom = true;
        }
    }

    if (reachListener_ != nullptr) {
        if (isReachTop) {
            reachListener_->OnReachStart();
        }
        if (isReachBottom) {
            reachListener_->OnReachEnd();
        }
    }
}

} // namespace OHOS
