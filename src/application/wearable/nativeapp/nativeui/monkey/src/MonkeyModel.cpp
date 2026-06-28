/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: MonkeyModel
 * Create: 2025-04-24
 */
#include <algorithm>
#include <cstdint>
#include "tcxo.h"
#include "security_unified/trng.h"
#include "AppViewIDs.h"
#include "UiConfig.h"
#include "NativeAbility.h"
#include "flashlight/FlashLightView.h"
#include "phonemenu/PhoneMenuView.h"
#include "stopwatch/StopwatchView.h"
#include "sport/SportView.h"
#include "timers/TimerView.h"
#include "weather/WeatherView.h"
#include "blood/BloodView.h"
#include "heartrate/HeartRateView.h"
#include "camera/CameraView.h"
#include "findphone/FindPhoneView.h"
#include "sleep/SleepView.h"
#include "settings/common/SettingCommon.h"
#include "date/DateView.h"
#include "cmsis_os.h"
#include "graphic_service.h"
#if ENABLE_DEBUG
#include "common/key_code.h"
#include "dfx/event_injector.h"
#include "dock/input_device.h"
#include "events/key_event.h"
#include "components/root_view.h"
#endif
#include "monkey/MonkeyModel.h"

namespace OHOS {
static constexpr uint16_t TIME_PERIOL = 1000;
static constexpr uint16_t BACK_TO_MAINVIEW_CNT = 10000;
static constexpr uint8_t HALF = 2;
static std::vector<SlicePageId> g_page;
static uint8_t g_curTime = 0;
static constexpr uint8_t RANDOM_ROTATE = 6;
static constexpr uint8_t RANDOM_KEY_TYPE = 3;
static constexpr uint16_t RANDOM_POINT_X = RESOLUTION_WIDTH + 1;
static constexpr uint16_t RANDOM_POINT_Y = RESOLUTION_HEIGHT + 1;
static constexpr uint8_t NUMBER_2 = 2;

#if ENABLE_DEBUG
typedef enum {
    MONKEY_ROTATE,
    MONKEY_KEY,
    MONKEY_CLICK,
    MONKEY_LONGPRESS
} MonkeyEventType;

typedef struct {
    MonkeyEventType type;
    int16_t x;
    int16_t y;
} MonkeyInfo;
std::vector<MonkeyInfo> g_monkey_random;
#endif

MonkeyModel::MonkeyModel()
{
    g_page.clear();
    // 手电筒
    g_page.push_back({VIEW_FLASHLIGHT, FLASH_LIGHT_VIEW});
    g_page.push_back({VIEW_FLASHLIGHT, FLASH_LIGHT_PLAY_VIEW});
    g_page.push_back({VIEW_FLASHLIGHT, FLASH_LIGHT_SET_VIEW});
    g_page.push_back({VIEW_FLASHLIGHT, FLASH_LIGHT_COLOR_VIEW});
    g_page.push_back({VIEW_FLASHLIGHT, FLASH_LIGHT_BLINK_VIEW});
    g_page.push_back({VIEW_FLASHLIGHT, FLASH_LIGHT_RECOVER_VIEW});
    // 通话菜单
    g_page.push_back({VIEW_PHONE_MENU, PHONE_MENU_MAIN_PAGE});
    g_page.push_back({VIEW_PHONE_MENU, PHONE_MENU_CONTACTS_PAGE});
    g_page.push_back({VIEW_PHONE_MENU, PHONE_MENU_DIAL_PAGE});
    g_page.push_back({VIEW_PHONE_MENU, PHONE_MENU_DISCONNECT_PAGE});
    g_page.push_back({VIEW_PHONE_MENU, PHONE_MENU_CALLER_LOG_PAGE});
    g_page.push_back({VIEW_PHONE_MENU, PHONE_MENU_CALLING_PAGE});
    // 秒表
    g_page.push_back({VIEW_STOPWATCH, STOPWATCH_MAIN_PAGE});
    g_page.push_back({VIEW_STOPWATCH, STOPWATCH_RECORD_PAGE});
    // 运动
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_MENU});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_PREPARE});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_COUNTDOWN});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_DATA});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_PAUSE});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_PROMPT});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_SETTINGS});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_TARGET});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_TARGET_SETTING});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_TARGET_PROMPT});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_REMIND});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_REMIND_INTERVAL});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_REMIND_HEARTRATE});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_HEARTRATE_RANGE});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_MISTOUCH});
    // 设置
    g_page.push_back({VIEW_SETTING, MAIN_PAGE});
    g_page.push_back({VIEW_SETTING, BLUETOOTH_PAGE});
    g_page.push_back({VIEW_SETTING, DISPLAY_PAGE});
    g_page.push_back({VIEW_EMPTY, 1});
    g_page.push_back({VIEW_SETTING, DESKTOP_PAGE});
    g_page.push_back({VIEW_SETTING, POWER_PAGE});
    g_page.push_back({VIEW_SETTING, CARD_PAGE});
    // 剔除系统与升级页面，防止Monkey场景执行到此页面主动重启
    g_page.push_back({VIEW_SETTING, ABOUT_PAGE});
    g_page.push_back({VIEW_SETTING, DISPLAY_BRIGHTNESS_PAGE});
    g_page.push_back({VIEW_SETTING, DISPLAY_SCREEN_OFF_PAGE});
    g_page.push_back({VIEW_SETTING, DISPLAY_STEADY_ON_PAGE});
    g_page.push_back({VIEW_SETTING, DESKTOP_DIAL_PAGE});
    g_page.push_back({VIEW_EMPTY, 1});
    g_page.push_back({VIEW_SETTING, DESKTOP_DIAL_STYLE_PAGE});
    g_page.push_back({VIEW_SETTING, CARD_EFFECT_PAGE});
    g_page.push_back({VIEW_SETTING, CARD_MGR_PAGE});
    g_page.push_back({VIEW_SETTING, CARD_ADD_PAGE});
    g_page.push_back({VIEW_SETTING, SYSTEM_OPERATION_PAGE});
    // 删除系统与升级OTA页面，防止Monkey场景执行到此页面主动重启
    g_page.push_back({VIEW_SETTING, CONNECT_NEW_PHONE_PAGE});
    g_page.push_back({VIEW_SETTING, CONNECT_OPEN_PAGE});
    g_page.push_back({VIEW_EMPTY, 1});
    g_page.push_back({VIEW_SETTING, CONNECT_BLUE_SUCCESS_PAGE});
    g_page.push_back({VIEW_SETTING, CONNECT_BLUE_FAIL_PAGE});
    g_page.push_back({VIEW_SETTING, RECONNECTION_PAGE});
    g_page.push_back({VIEW_SETTING, RECONNECT_SECOND_CONFIRM_PAGE});
    g_page.push_back({VIEW_SETTING, UNPAIR_SECOND_CONFIRM_PAGE});
    g_page.push_back({VIEW_SETTING, QRCODE_CONN_PAGE});
    g_page.push_back({VIEW_EMPTY, 1});
    // 计时器
    g_page.push_back({VIEW_TIMER, TIMER_MAIN_PAGE});
    g_page.push_back({VIEW_TIMER, TIMER_TIMING_PAGE});
    g_page.push_back({VIEW_TIMER, TIMER_SETTING_PAGE});
    // 天气
    g_page.push_back({VIEW_WEATHER, WEATHER_MAIN_PAGE});
    g_page.push_back({VIEW_WEATHER, WEATHER_HOURS_PAGE});
    g_page.push_back({VIEW_WEATHER, WEATHER_DAYS_PAGE});
    g_page.push_back({VIEW_WEATHER, WEATHER_SUNRISE_PAGE});
    g_page.push_back({VIEW_WEATHER, WEATHER_MONTHRISE_PAGE});
    g_page.push_back({VIEW_WEATHER, WEATHER_LUNARPHASE_PAGE});
    // 血氧
    g_page.push_back({VIEW_BLOOD, BLOOD_MAIN_PAGE});
    g_page.push_back({VIEW_BLOOD, BLOOD_READY_PAGE});
    g_page.push_back({VIEW_BLOOD, BLOOD_MEASUREMENT_PAGE});
    g_page.push_back({VIEW_BLOOD, BLOOD_RESULT_PAGE});
    g_page.push_back({VIEW_BLOOD, BLOOD_CHART_PAGE});
    g_page.push_back({VIEW_BLOOD, BLOOD_EXPLAIN_PAGE});
    // 心率
    g_page.push_back({VIEW_HEARTRATE, HEARTRATE_MAIN_PAGE});
    g_page.push_back({VIEW_HEARTRATE, HEARTRATE_READY_PAGE});
    g_page.push_back({VIEW_HEARTRATE, HEARTRATE_MEASUREMENT_PAGE});
    // 相机
    g_page.push_back({VIEW_CAMERA, CAMERA_MAIN_PAGE});
    // 寻找手机
    g_page.push_back({VIEW_FIND_PHONE, FIND_PHONE_MAIN_PAGE});
    // 睡眠
    g_page.push_back({VIEW_SLEEPING, SLEEPING_MAIN_PAGE});
    g_page.push_back({VIEW_SLEEPING, SLEEPING_DEATIL_PAGE});
    g_page.push_back({VIEW_SLEEPING, SLEEPING_TARGET_PAGE});
    // 日历
    g_page.push_back({VIEW_DATE, DATE_MAIN_PAGE});
    g_page.push_back({VIEW_DATE, DATE_MESSAGE_PAGE});
    g_page.push_back({VIEW_EMPTY, 1});
    timeCallback_ = new TimeAnimatorCallback(TIME_PERIOL);
    if (timeCallback_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new timeCallback_ fail");
        return;
    }
    timeAnimator_ = new Animator(timeCallback_, nullptr, 0, true);
    if (timeAnimator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new timeAnimator_ fail");
        return;
    }
}

MonkeyModel::~MonkeyModel()
{
    if (timeAnimator_ != nullptr) {
        delete timeAnimator_;
        timeAnimator_ = nullptr;
    }
    if (timeCallback_ != nullptr) {
        delete timeCallback_;
        timeCallback_ = nullptr;
    }
}

MonkeyModel &MonkeyModel::GetInstance()
{
    static MonkeyModel instance;
    return instance;
}

void MonkeyModel::SetStart(bool start)
{
    if (start) {
        timeAnimator_->Start();
    } else {
        timeAnimator_->Stop();
    }
}

#if ENABLE_DEBUG
static void MonkeyGetChildren(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    while (view != nullptr) {
        UIView* tempView = view;
        view = view->GetNextSibling(); // 获取下一个控件
        if (tempView->IsViewGroup() && (tempView->GetViewType() != UI_DIGITAL_CLOCK)) {
            MonkeyGetChildren(static_cast<UIViewGroup*>(tempView)->GetChildrenHead());
        }
        if (tempView->IsTouchable()) { // 可点击
            int16_t x_click = tempView->GetContentRect().GetX() + tempView->GetContentRect().GetWidth() / HALF;
            int16_t y_click = tempView->GetContentRect().GetY() + tempView->GetContentRect().GetHeight() / HALF;
            MonkeyInfo info;
            info.type = MONKEY_CLICK;
            info.x = x_click;
            info.y = y_click;
            g_monkey_random.push_back(info);
        }
        if (tempView->GetOnLongPressListener() != nullptr) {
            int16_t x_click = tempView->GetContentRect().GetX() + tempView->GetContentRect().GetWidth() / HALF;
            int16_t y_click = tempView->GetContentRect().GetY() + tempView->GetContentRect().GetHeight() / HALF;
            MonkeyInfo info;
            info.type = MONKEY_LONGPRESS;
            info.x = x_click;
            info.y = y_click;
            g_monkey_random.push_back(info);
        }
    }
}

static void MonkeyRotateInit(void)
{
    MonkeyInfo m_info;
    m_info.type = MONKEY_ROTATE;
    uint32_t randnum;
    uapi_drv_cipher_trng_get_random(&randnum);
    m_info.x = (static_cast<int16_t>(randnum) % RANDOM_ROTATE);
    m_info.y = 0;
    g_monkey_random.push_back(m_info);
}

static void MonkeyKeyInit(void)
{
    MonkeyInfo m_info;
    uint32_t randnum;
    uapi_drv_cipher_trng_get_random(&randnum);
    uint8_t key_random = (randnum & 1); // 1: 按键类型只有0: 表示power， 1: 表示func
    ZliteKeyCode keyId = key_random == 0 ? ZliteKeyCode::ZLITE_KEY_POWER : ZliteKeyCode::ZLITE_KEY_FUNC;
    uint16_t state = 0;
    uapi_drv_cipher_trng_get_random(&randnum);
    int16_t state_random = static_cast<int16_t>(randnum % RANDOM_KEY_TYPE);
    if (state_random == 0) {
        state = InputDevice::STATE_RELEASE;
    } else if (state_random == 1) {
        state = InputDevice::STATE_PRESS;
    } else if (state_random == NUMBER_2) {
        state = InputDevice::STATE_LONG_PRESS;
    }
    m_info.type = MONKEY_KEY;
    m_info.x = static_cast<uint16_t>(keyId);
    m_info.y = state;
    g_monkey_random.push_back(m_info);
}

static void MonkeyRotateHandler(int16_t rotate)
{
    if (!OHOS::EventInjector::GetInstance()->IsEventInjectorRegistered(OHOS::EventDataType::ROTATE_TYPE)) {
        OHOS::EventInjector::GetInstance()->RegisterEventInjector(OHOS::EventDataType::ROTATE_TYPE);
    }
    OHOS::EventInjector::GetInstance()->SetRotateEvent(rotate);
}

static void MonkeyKeyHandler(int16_t keyId, int16_t state)
{
    if (!OHOS::EventInjector::GetInstance()->IsEventInjectorRegistered(OHOS::EventDataType::KEY_TYPE)) {
        OHOS::EventInjector::GetInstance()->RegisterEventInjector(OHOS::EventDataType::KEY_TYPE);
    }
    OHOS::EventInjector::GetInstance()->SetKeyEvent(static_cast<uint16_t>(keyId), (uint16_t)state);
}

static void MonkeyClickHandler(int16_t x, int16_t y)
{
    Point point_click = {x, y};
    if (!OHOS::EventInjector::GetInstance()->IsEventInjectorRegistered(OHOS::EventDataType::POINT_TYPE)) {
        OHOS::EventInjector::GetInstance()->RegisterEventInjector(OHOS::EventDataType::POINT_TYPE);
    }
    GraphicService::GetInstance()->PostGraphicEvent([point_click]() {
        OHOS::EventInjector::GetInstance()->SetClickEvent(point_click);
    });
}

static void MonkeyLongPressHandler(int16_t x, int16_t y)
{
    Point point_lp = {x, y};
    if (!OHOS::EventInjector::GetInstance()->IsEventInjectorRegistered(OHOS::EventDataType::POINT_TYPE)) {
        OHOS::EventInjector::GetInstance()->RegisterEventInjector(OHOS::EventDataType::POINT_TYPE);
    }
    GraphicService::GetInstance()->PostGraphicEvent([point_lp]() {
        OHOS::EventInjector::GetInstance()->SetLongPressEvent(point_lp);
    });
}

static void MonkeyRunHandler(MonkeyEventType type, int16_t x, int16_t y)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "monkey type = %d, x = %d, y = %d\r\n", type, x, y);
    switch (type) {
        case MONKEY_ROTATE:
            MonkeyRotateHandler(x);
            break;
        case MONKEY_KEY:
            MonkeyKeyHandler(x, y);
            break;
        case MONKEY_CLICK:
            MonkeyClickHandler(x, y);
            break;
        case MONKEY_LONGPRESS:
            MonkeyLongPressHandler(x, y);
            break;
        default:
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "wrong type = %d\r\n", type);
            break;
    }
}
#endif

void MonkeyModel::TimeAnimatorCallback::Callback(UIView *view)
{
    int pageSize = static_cast<int>(g_page.size());
    if (pageSize == 0) {
        return;
    }
    static int monkeyId = 0;
    static uint64_t lastExecuteTime = 0;
    static uint16_t g_backto_main_view = 0;
    static bool first_run = true;
    uint64_t currentTime = uapi_tcxo_get_ms();
    g_backto_main_view++;
    if (g_backto_main_view == BACK_TO_MAINVIEW_CNT) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "already reach %d counts, go to main view\r\n", g_backto_main_view);
        g_backto_main_view = 0;
        NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE, TransitionType::TRANSITION_ZOOM_OUT);
    }

    if (currentTime - lastExecuteTime >= periol_ || first_run) {
        first_run = false;
        lastExecuteTime = currentTime;
        monkeyId = (monkeyId + 1) % pageSize;
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "sliceId = %d, pageId = %d\r\n", g_page[monkeyId].sliceId,
                      g_page[monkeyId].pageId);
        NativeAbility::GetInstance().SwitchSlice(g_page[monkeyId].sliceId, g_page[monkeyId].pageId);
#if ENABLE_DEBUG
        g_monkey_random.clear();
        MonkeyRotateInit();
        MonkeyKeyInit();
        MonkeyGetChildren(RootView::GetInstance());

        if (g_monkey_random.empty()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "g_monkey_random is empty\r\n");
            return;
        }

        uint32_t randnum;
        uapi_drv_cipher_trng_get_random(&randnum);
        uint8_t idx = (randnum % g_monkey_random.size());
        MonkeyRunHandler(g_monkey_random[idx].type, g_monkey_random[idx].x, g_monkey_random[idx].y);
#endif
    }
}
}
