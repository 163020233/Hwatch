/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: headset notification api
 * Author: CompanyName
 * Create: 2025-04-18
 */

#ifndef HEADSET_NOTIFICATION_H
#define HEADSET_NOTIFICATION_H

static constexpr const char *HEADSET_IMAGE_NO = "headset_yes";
static constexpr const char *HEADSET_IMAGE_YES = "headset_no";
static constexpr const char *HEADSET_CONN_FAIL_BUTTON = "headset_fail";
static constexpr const char *HEADSET_DETAILS = "headset_details";
static constexpr const char *HEADSET_CANCEL = "headset_cancel";
static constexpr const char *HEADSET_CONN_VIEW = "headset_connect";
static constexpr const char *HEADSET_POP_VIEW = "headset_pop";
static constexpr const char *HEADSET_CONN_FAIL_VIEW = "headset_connect";

namespace OHOS {

UIViewGroup *CreateHeadsetConnView(char *twsName, uint32_t twsBattery, uint32_t twsVolume);
UIViewGroup *CreateHeadsetPopView(char *twsName, uint32_t twsBattery, uint32_t twsVolume);
UIViewGroup *CreateConnectFailView();
void HeadsetPopShowUp(char *twsName, uint32_t twsBattery, uint32_t twsVolume);
void FreeHeadsetConnView(void);
void FreeHeadsetPopView(void);
void FreeHeadsetConnFailView(void);

class HeadsetViewEventListener : public OHOS::UIView::OnClickListener,
                                 public OHOS::UIView::OnLongPressListener,
                                 public OHOS::UIView::OnTouchListener,
                                 public OHOS::UIView::OnDragListener {
public:
    HeadsetViewEventListener();
    virtual ~HeadsetViewEventListener();
    static HeadsetViewEventListener *GetInstance(void);
    
    bool OnDrag(UIView &view, const DragEvent &event) override;
    bool OnDragEnd(OHOS::UIView &view, const OHOS::DragEvent &event) override;
    bool OnClick(OHOS::UIView &view, const OHOS::ClickEvent &event) override;
    bool OnPress(OHOS::UIView &view, const OHOS::PressEvent &event) override
    {
        return true;
    }
    bool OnLongPress(OHOS::UIView &view, const OHOS::LongPressEvent &event) override
    {
        return true;
    }
    bool OnRelease(OHOS::UIView &view, const OHOS::ReleaseEvent &event) override
    {
        return true;
    }

    bool OnDragStart(OHOS::UIView &view, const OHOS::DragEvent &event) override
    {
        return true;
    }
};
};  // namespace OHOS

#endif // HEADSET_NOTIFICATION_H
