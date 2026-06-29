/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: notification_manager
 * Created: 2025-12-15
 */

#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H
#include <functional>
#include <vector>

#include "common/task.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "alarm_notification.h"
#include "calendar_notification.h"
#include "phone_msg_notification.h"
#include "headset_notification.h"
#include "makephone_notification.h"

constexpr uint16_t HALF_SCREEN = 2;
constexpr int16_t NOTIFY_PRIORITY_DEFAULT = 4;

namespace OHOS {
using NewNotify = std::function<UIViewGroup* ()>;
using RefreshNotifyContent = std::function<void()>;
typedef void (*FreeNotifyfunc)();
class NotificationManager : public Task {
public:
    static NotificationManager* GetInstance()
    {
        static NotificationManager instance;
        return &instance;
    }

    /**
     * @brief add a new notify.
     * @param notify is a function of inform layout.
     * @param id is current notify id.
     * @param fullScreen is or not full screen inform notify.
     * @param needPauseSlice If the value of needPauseSlice is true, the current slice is paused and the key listener is changed.
     * When the notify is cleared, the slice is resumed and the key listener is cleared. Otherwise, the slice and key
     * listener remain unchanged.
     * @return true if the notification is shown successfully, false otherwise.
     */
    bool ShowNotify(const NewNotify& notify, uint16_t id, bool fullScreen, bool needPauseSlice = true);

    /**
    * @brief refresh showing notify content.
    * @param refreshNotifyContent is a function of refresh notify content.
    */
    void RefreshNotify(const RefreshNotifyContent& refreshNotifyContent);

    /**
    * @brief stop and destroy showing notify.
    */
    void StopNotify(void);

    /**
    * @brief notify is or not showing.
    */
    bool HasNotifyShowing();
    bool IsCurNotifyNeedPause();
    void StartShowNotify(void);
    uint16_t GetNotifyPriority(void);
    uint16_t GetShowingNotifyId(void);
    void SetNotifyPriority(uint16_t prio);
    void SetNativeUIRunning(bool flag);
    bool GetVolumeScreenStatus();
    void SetVolumeScreenStatus(bool status);
    bool GetWatchCallStatus();
    void SetWatchCallStatus(bool status);
    void RegisterNotifyCleanupFunction(FreeNotifyfunc);
    void Callback() override;
    UIViewGroup* screenContainer_ = nullptr;
    void FreeNotifyViews(void);

private:
    NotificationManager() {}
    ~NotificationManager() override {}

    void StartRefreshNotifyContent(void);
    void StopShowNotify(void);

    NewNotify notify_;
    RefreshNotifyContent refreshNotifyContent_;

    bool isShowing_ = false;
    bool isFullScreen_ = false;
    bool isNativeUI = false;
    bool volumeScreenStatus = false;
    bool watchCallStatus = false;

    bool stopIsLast_ = false;
    bool showIsLast_ = false;
    bool hasNewNotifyNeedShow_ = false;
    bool hasNotifyNeedRefresh_ = false;
    bool hasNewNotifyNeedStop_ = false;
    std::vector<FreeNotifyfunc> notifyCleanupFunctions_;
    uint16_t notifyPri = NOTIFY_PRIORITY_DEFAULT;
    uint16_t notifyId = -1;
    bool needPause_ = true;
    bool isCurNotifyNeedPause_ = true;

    NotificationManager(const NotificationManager&) = delete;
    NotificationManager& operator=(const NotificationManager&) = delete;
    NotificationManager(NotificationManager&&) = delete;
    NotificationManager& operator=(NotificationManager&&) = delete;
};
}; // namespace OHOS
#endif // NOTIFICATION_MANAGER_H
