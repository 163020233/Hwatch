/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: List with reach detect function
 * Author: Hisi Team
 * Created: 2025-12
 */
#ifndef UI_SIMPLE_REACH_LIST_H
#define UI_SIMPLE_REACH_LIST_H

#include "components/ui_simple_list.h"

namespace OHOS {

class ListReachListener : public HeapBase {
public:
    ListReachListener() {}
    virtual ~ListReachListener() {}

    /**
     * @brief Called when the list has been scrolled to its start (top edge).
     *
     */
    virtual void OnReachStart() {}

    /**
     * @brief Called when the list has been scrolled to its end (bottom edge).
     *
     */
    virtual void OnReachEnd() {}
};

class UISimpleReachList : public UISimpleList {
public:
    explicit UISimpleReachList() {}
    ~UISimpleReachList() override = default;

    void SetReachListener(ListReachListener* reachListener)
    {
        reachListener_ = reachListener;
    }

protected:
    bool DragXInner(int16_t distance) override;
    bool DragYInner(int16_t distance) override;

private:
    void DetectReachBorderHor(int16_t oldHeadLeft, int16_t oldTailRight, int16_t newHeadLeft, int16_t newTailRight);
    void DetectReachBorderVer(int16_t oldHeadTop, int16_t oldTailBottom, int16_t newHeadTop, int16_t newTailBottom);
    ListReachListener* reachListener_{nullptr};
};
} // namespace OHOS
#endif // UI_SIMPLE_REACH_LIST_H

