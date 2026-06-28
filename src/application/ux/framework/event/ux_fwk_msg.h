/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: ux task init header code.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-03-15, Create file. \n
 */
#ifndef UX_TASK_INIT_H
#define UX_TASK_INIT_H

#include "ux_api_msg.h"
#include "td_type.h"
#include "errcode.h"
#include "cmsis_os2.h"
#include "list.h"
#include "ux_event_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SYS_MODULE_APP_UX        8
#define UX_QUEUE_LEN             10
#define UX_MAX_MSG_SIZE          40
#define CONTROL_MSG_LEN_MAX      30

typedef struct {
    struct list_head list;
    struct list_head used_device;
    struct app_operation* aop;
    uint32_t mail;
    osMutexId_t app_lock;
    uint8_t tag;
    uint8_t status;
    uint16_t app_mail_delay;
} app_t;

struct app_operation {
    int (*config)(const app_t *app, td_void *para, uint16_t msg_len);
    int (*init)(app_t *app);
    int (*open)(app_t *app);
    int (*set_sample_interval)(app_t *app, uint16_t interval);
    int (*close)(app_t *app);
};

#define SHIPMODE_FLAG (0xabcdbeef)
#define UX_BOX_EVENT_BEGIN_NUM (1)
#define UX_SENSOR_EVENT_BEGIN_NUM (64)
#define UX_CHARGE_EVENT_BEGIN_NUM (128)
#define UX_SPP_EVENT_BEGIN_NUM (196)

typedef enum {
    /* box event */
    UX_BOX_EVENT_BEGIN = UX_EVENT_TYPE_BUTT + 1,
    UX_BOX_RECOVERY = UX_BOX_EVENT_BEGIN,
    UX_BOX_ENTER_PAIR_MODE,
    UX_BOX_CONNECT_INFO_INTERACTION,
    UX_BOX_RECONNECTION,
    UX_BOX_DISCONNECTED,
    UX_BOX_INBOX_INFO_INTERACTION,
    UX_BOX_BATTERY_INFO_NOTIFICATION,
    UX_BOX_LED_CONTROL,
    UX_BOX_MCU_NOTIFY_SHUTDOWN,
    UX_BOX_HEADSET_SHUTDOWN_NOTIFICATION,
    UX_BOX_TOUCHPAD_NOTIFICATION,
    UX_BOX_WEAR_STATUS_HANDLE,
    UX_BOX_WEAR_STATUS_REQ,
    UX_BOX_WAKE_UP_NOTIFICATION,
    UX_BOX_HEARTBEAT,
    UX_BOX_HEADSET_SELF_OPEN_BOX,
    UX_BOX_MOVEMENT,
    UX_BOX_USB_STATUS_REQ,
    UX_BOX_NOTIFY_USB_STATUS,
    UX_BOX_EVENT_END = UX_BOX_NOTIFY_USB_STATUS,

    UX_BOX_EVENT_MAX = UX_BOX_EVENT_END - UX_BOX_EVENT_BEGIN + 1,

    /* sensor event */
    UX_SENSOR_EVENT_BEGIN = UX_SENSOR_EVENT_BEGIN_NUM,
    UX_DC = UX_SENSOR_EVENT_BEGIN,  // double click 64
    UX_TC,                          // triple click 65
    UX_TOUCH,                       // long touch 66
    UX_SLIP_UP,                     // slip up 67
    UX_SLIP_DOWN,                   // slip down 68
    UX_WEAR_STATUS_OUT,             // out of ear 69
    UX_WEAR_STATUS_IN,              // in ear 70
    UX_OUT_BOX,                     // out of box 71
    UX_IN_BOX,                      // in box 72
    UX_PRESS,
    UX_DOUBLE_PRESS,
    UX_TRIPLE_PRESS,
    UX_LONG_PRESS,
    UX_SLIDE_UP,            // 77
    UX_SLIDE_DOWN,          // 78
    UX_SLIDE_FORWARD,       // 79
    UX_SLIDE_BACKWARD,      // 80
    UX_PRESS_2S,            // 81
    UX_COVER_ALL,           // 82
    UX_PRESS_1S,            // 83
    UX_COVER_RELEASE,       // 84
    UX_PRESS_3S,            // 85
    UX_VOLUP_PRESS,         // 86
    UX_VOLUP_2S,            // 87
    UX_VOLDOWN_PRESS,       // 88
    UX_VOLDOWN_2S,          // 89
    UX_POWER_PRESS,         // 90
    UX_POWER_DOUBLE_PRESS,  // 91
    UX_POWER_TRIPLE_PRESS,  // 92
    UX_POWER_2S_UP,         // 93
    UX_POWER_4S_UP,
    UX_POWER_2S,  // 95
    UX_POWER_4S,
    UX_POWER_10S,
    UX_POWER_TIME_ENOUGH,      // 98
    UX_POWER_TIME_ENOUGH_UP,   // 99
    UX_POWER_TIME_NOT_ENOUGH,  // 100
    UX_PWR_VOLUP_VOLDOWN_2S,   // 101
    UX_HALL_CLOSE,
    UX_HALL_OPEN,
    UX_SENSOR_EVENT_END = UX_HALL_OPEN,

    // charge event
    UX_CHARGE_EVENT_BEGIN = UX_CHARGE_EVENT_BEGIN_NUM,
    UX_HEADSET_BATTERY_POWER = UX_CHARGE_EVENT_BEGIN,  // headset battery level
    UX_HEADSET_BATTERY_LOW_POWER,
    UX_BOX_BATTERY_POWER,  // box battery level
    UX_CHARGE_EVENT_END = UX_BOX_BATTERY_POWER,

    UX_SPP_EVENT_BEGIN = UX_SPP_EVENT_BEGIN_NUM,
    UX_DEVICE_MANAGEMENT,
    UX_INTELLIGENT,
    UX_FONT,
    UX_TWS_CONFIG,
    UX_MIDWARE_SERVICE,
    UX_SPP_MGR_HEADER_STATUS,
    UX_SPP_EVENT_END = UX_SPP_MGR_HEADER_STATUS,

    UX_HAID_SER_MSG,  // 助听的业务消息

    UX_FOTA_START,
    UX_FOTA_END,

    UX_PLAY_TONE,
    UX_STOP_TONE,
    UX_STOP_ALL_TONE,
    UX_TONE_COMPLETE,

    // player相关服务
    UX_PLAYER_SET_SOURCE_PATH,
    UX_PLAYER_START_PLAY,
    UX_PLAYER_PAUSE_PLAY,
    UX_PLAYER_RESUME_PLAY,
    UX_PLAYER_STOP_PLAY,        // 销毁播放器
    UX_PLAYER_GET_STATES,       // 获取播放器状态

    UX_A2DP_STOP,
    UX_SCO_STOP,

    UX_BUTT
} ux_cmd_type;

typedef struct {
    ux_api_common_notify_mail_t mail;
}ux_cmd_msg_t;

typedef enum {
    CMD_SET_START = 0x0,             /* 设置指令起始 */
    CMD_SET_GAIN,                    /* 设置增益 */
    CMD_SET_NOISE_REDUCTION,         /* 设置降噪开关 */
    CMD_SET_RECORDING,               /* 设置录音开关 */
    CMD_SET_LOW_CUT,                 /* 设置低切开关 */
    CMD_SET_MUTE,                    /* 设置静音开关 */
    CMD_SET_REC_BIT,                 /* 设置录音格式 */
    CMD_SET_HOST_HID_CMD,            /* 设置HID控制 */

    CMD_NOTIFY_BAT_LEVEL,            /* 通知电量 */
    CMD_NOTIFY_DENOISE_MODE,         /* 通知降噪模式 */
    CMD_NOTIFY_EQ_MODE,              /* 通知EQ模式 */
    CMD_NOTIFY_REC_STATE,            /* 通知当前录音状态 */
    CMD_NOTIFY_MUTE_STATE,           /* 通知当前静音状态 */
    CMD_NOTIFY_REMAIN_TIME,          /* 通知剩余录制时间 */
    CMD_NOTIFY_VOLUME_LEVEL,         /* 通知音量值 */

    CMD_GET_START = 0x30,            /* 获取指令起始 */
    CMD_GET_MUTE_STATUS,             /* 获取静音状态 */
    CMD_GET_RECORD_REMAIN_TIME,      /* 获取剩余录音时长 */
    CMD_CONTROL_MAX = 0xFF,
} bt_control_cmd_t;

typedef struct {
    bt_control_cmd_t cmd;                 /* 命令类型 */
    uint8_t param[CONTROL_MSG_LEN_MAX];   /* 命令参数 */
    uint16_t param_len;                   /* 命令参数长度 */
} bt_control_msg_t;

typedef void (*bt_ssap_msg_handler)(uint16_t dev_id, const bt_control_msg_t *msg);

void ux_msg_queue_init(void);
errcode_t sys_send_msg(uint8_t msg_queue, const void *msg_ptr);
errcode_t sys_get_msg(uint8_t msg_queue, void *msg_ptr, uint32_t timeout);
errcode_t ux_msg_handle(ux_cmd_msg_t *msg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
