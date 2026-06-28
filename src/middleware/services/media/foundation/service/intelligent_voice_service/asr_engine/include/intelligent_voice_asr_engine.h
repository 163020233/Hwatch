/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: intelligent voice service asr engine.
 */
#ifndef INTELLIGENT_VS_ASR_H
#define INTELLIGENT_VS_ASR_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ASR_ENGINE_EVENT_STOP,
    ASR_ENGINE_EVENT_VOICE_BEGIN,
    ASR_ENGINE_EVENT_VOICE_END,
    ASR_ENGINE_EVENT_VOICE_COMMAND,
    ASR_ENGINE_EVENT_VOICE_ERROR,
    ASR_ENGINE_EVENT_MAX
} AsrEngineEvent;

typedef enum {
    ASR_ENGINE_CMD_APP_CONTROL         = 1, /* APP控制 */
    ASR_ENGINE_CMD_MUSIC_CONTROL,           /* 音乐控制 */
    ASR_ENGINE_CMD_ALARM_CONTROL,           /* 闹钟控制 */
    ASR_ENGINE_CMD_MOTION_CONTROL,          /* 运动控制 */
    ASR_ENGINE_CMD_TELEPHONE_CONTROL,       /* 电话控制 */
    ASR_ENGINE_CMD_TIMER_CONTROL,           /* 计数器控制 */
    ASR_ENGINE_CMD_DEVICE_CONTROL,          /* 设备控制 */
    ASR_ENGINE_CMD_WAKEUP_KEYWORD,          /* 唤醒词 */
    ASR_ENGINE_CMD_MAX,
} AsrEngineCmdId;

typedef enum {
    ASR_ENGINE_APP_CTRL_OPEN                = 1,
    ASR_ENGINE_APP_CTRL_CLOSE               = 2,
    ASR_ENGINE_APP_CTRL_MAX,
} AsrEngineAppControl;

typedef enum {
    ASR_ENGINE_APP_SETTING                = 1,  /* 打开设置 */
    ASR_ENGINE_APP_HEART_RATE,                  /* 打开心率 */
    ASR_ENGINE_APP_SPORT,                       /* 打开运动 */
    ASR_ENGINE_APP_SPORT_RECORD,                /* 打开运动记录 */
    ASR_ENGINE_APP_ACTIVATY_RECORD,             /* 打开活动记录 */
    ASR_ENGINE_APP_TRANING_STATUS,              /* 打开训练状态 */
    ASR_ENGINE_APP_SPO2,                        /* 打开血氧饱和度 */
    ASR_ENGINE_APP_SLEEP,                       /* 打开睡眠 */
    ASR_ENGINE_APP_PRESSURE,                    /* 打开压力 */
    ASR_ENGINE_APP_BREATH_TRAIN,                /* 打开呼吸训练 */
    ASR_ENGINE_APP_ELECTROCARDIO,               /* 打开心电 */
    ASR_ENGINE_APP_PAI,                         /* 打开PAI */
    ASR_ENGINE_APP_CALL_RECORDS,                /* 打开通话记录 */
    ASR_ENGINE_APP_FAVOURITES,                  /* 打开常用联系人 */
    ASR_ENGINE_APP_WEATHER,                     /* 打开天气 */
    ASR_ENGINE_APP_MUSIC_APP,                   /* 打开音乐 */
    ASR_ENGINE_APP_REMOTE_PHOTO,                /* 打开遥控拍照 */
    ASR_ENGINE_APP_NOTIFICATION,                /* 打开通知 */
    ASR_ENGINE_APP_ALARM_APP,                   /* 打开闹钟 */
    ASR_ENGINE_APP_CALENDAR,                    /* 打开日程 */
    ASR_ENGINE_APP_WALLET,                      /* 打开卡包 */
    ASR_ENGINE_APP_COMPASS,                     /* 打开指南针 */
    ASR_ENGINE_APP_STOPWATCH,                   /* 打开秒表 */
    ASR_ENGINE_APP_TIMER,                       /* 打开倒计时 */
    ASR_ENGINE_APP_WECHAT_PAY,                  /* 打开微信支付 */
    ASR_ENGINE_APP_ALI_PAY,                     /* 打开支付宝 */
    ASR_ENGINE_APP_ALI_PAY_SCAN,                /* 打开支付宝扫一扫 */
    ASR_ENGINE_APP_BAROMETER,                   /* 打开海拔气压计 */
    ASR_ENGINE_APP_ALTIMETER,                   /* 打开气压高度计 */
    ASR_ENGINE_APP_NETEASY_MUSIC,               /* 打开网易音乐 */
    ASR_ENGINE_APP_TELEPHONE,                   /* 打开电话 */
    ASR_ENGINE_APP_MESSAGE,                     /* 打开短信 */
    ASR_ENGINE_APP_WATCH_FACE,                  /* 打开表盘 */
    ASR_ENGINE_APP_BUS_CARD,                    /* 打开公交卡 */
    ASR_ENGINE_APP_ACCESS_CARD,                 /* 打开门禁卡 */
    ASR_ENGINE_APP_MAX,
} AsrEngineAppId;

typedef struct {
    AsrEngineAppId appId;
    AsrEngineAppControl appControlType;
} AsrEngineAppControlInfo;

/* 创建音乐控制 */
typedef enum {
    ASR_ENGINE_MUSIC_CTRL_PLAY,                 /* 播放音乐 */
    ASR_ENGINE_MUSIC_CTRL_PAUSE,                /* 暂停音乐 */
    ASR_ENGINE_MUSIC_CTRL_RESUME,               /* 恢复音乐 */
    ASR_ENGINE_MUSIC_CTRL_STOP,                 /* 停止音乐 */
    ASR_ENGINE_MUSIC_CTRL_PLAY_PRE,             /* 播放上一首 */
    ASR_ENGINE_MUSIC_CTRL_PLAY_NEXT,            /* 播放下一首 */
    ASR_ENGINE_MUSIC_CTRL_PLAY_LOOP,            /* 单曲循环 */
    ASR_ENGINE_MUSIC_CTRL_PLAY_SHUFFLE,         /* 随机播放 */
    ASR_ENGINE_MUSIC_CTRL_PLAY_REPEAT_LIST,     /* 列表循环 */
    ASR_ENGINE_MUSIC_CTRL_PLAY_MY_COLLECTION,   /* 播放我的收藏 */
    ASR_ENGINE_MUSIC_CTRL_PLAY_RUNNING_MUSIC,   /* 播放跑步音乐 */
    ASR_ENGINE_MUSIC_CTRL_PLAY_ALL_MUSIC,       /* 播放全部歌曲 */
    ASR_ENGINE_MUSIC_CTRL_PLAY_PHONE_MUSIC,     /* 播放手机音乐 */
    ASR_ENGINE_MUSIC_CTRL_PLAY_WATCH_MUSIC,     /* 播放手表音乐 */
    ASR_ENGINE_MUSIC_CTRL_MAX,
} AsrEngineMusicControl;

/* 创建音乐控制 */
typedef struct {
    AsrEngineMusicControl musicControlType;
} AsrEngineMusicControlInfo;

typedef enum {
    ASR_ENGINE_TIME_INVALID,                                /* 无效值 */
    ASR_ENGINE_TIME_TODAY,                                  /* 今天 */
    ASR_ENGINE_TIME_TOMORROW,                               /* 明天 */
    ASR_ENGINE_TIME_THE_DAY_AFTER_TOMORROW,                 /* 后天 */
    ASR_ENGINE_TIME_THE_DAY_AFTER_THE_DAY_AFTER_TOMORROW,   /* 大后天 */
    ASR_ENGINE_TIME_MAX,
} AsrEngineTimeDay;

typedef enum {
    ASR_ENGINE_TIME_PERIOD_INVALID,            /* 无效值 */
    ASR_ENGINE_TIME_PERIOD_AM,                 /* 上午 */
    ASR_ENGINE_TIME_PERIOD_PM,                 /* 下午 */
    ASR_ENGINE_TIME_PERIOD_MAX,
} AsrEngineTimePeriod;

/* 闹钟控制时间 */
typedef struct {
    AsrEngineTimeDay day;
    AsrEngineTimePeriod timePeriod;
    int32_t hour;
    int32_t minute;
} AsrEngineDateTime;

/* 闹钟控制 */
typedef enum {
    ASR_ENGINE_ALARM_CTRL_SET,                 /* 设置闹钟 */
    ASR_ENGINE_ALARM_CTRL_MAX,
} AsrEngineAlarmControl;

/* 创建闹钟控制信息 */
typedef struct {
    AsrEngineAlarmControl alarmControlType;
    AsrEngineDateTime alarmDateTime;
} AsrEngineAlarmControlInfo;

/* 运动控制类型 */
typedef enum {
    ASR_ENGINE_MOTION_CTRL_START_SPORT,                 /* 开始运动 */
    ASR_ENGINE_MOTION_CTRL_START_INDOOR_RUNNING,        /* 开始室内跑 */
    ASR_ENGINE_MOTION_CTRL_START_OUTDOOR_RUNNING,       /* 开始户外跑 */
    ASR_ENGINE_MOTION_CTRL_START_RUNNING_COURSE,        /* 开始跑步课程 */
    ASR_ENGINE_MOTION_CTRL_START_INDOOR_WALKING,        /* 开始室内健走 */
    ASR_ENGINE_MOTION_CTRL_START_WALKING,               /* 开始健走 */
    ASR_ENGINE_MOTION_CTRL_START_INDOOR_CYCLING,        /* 开始室内骑行 */
    ASR_ENGINE_MOTION_CTRL_START_OUTDOOR_CYCLING,       /* 开始户外骑行 */
    ASR_ENGINE_MOTION_CTRL_START_TREADMILL,             /* 开始跑步机 */
    ASR_ENGINE_MOTION_CTRL_START_OPENWATER_SWIMMING,    /* 开始公开水域游泳 */
    ASR_ENGINE_MOTION_CTRL_START_POOL_SWIMMING,         /* 开始泳池游泳 */
    ASR_ENGINE_MOTION_CTRL_START_ELLIPTICAL,            /* 开始椭圆机 */
    ASR_ENGINE_MOTION_CTRL_START_CLIMBING,              /* 开始登山 */
    ASR_ENGINE_MOTION_CTRL_START_TRAIL_RUNNING,         /* 开始越野跑 */
    ASR_ENGINE_MOTION_CTRL_START_SKIING,                /* 开始滑雪 */
    ASR_ENGINE_MOTION_CTRL_START_ROWING,                /* 开始划船机 */
    ASR_ENGINE_MOTION_CTRL_START_TRIATHLON,             /* 开始铁人三项 */
    ASR_ENGINE_MOTION_CTRL_START_GOLF,                  /* 开始高尔夫 */
    ASR_ENGINE_MOTION_CTRL_START_FREE_TRAINING,         /* 开始自由训练 */
    /* 无目标意图 */
    ASR_ENGINE_MOTION_CTRL_ADD_SPORT,                   /* 开始添加运动 */
    ASR_ENGINE_MOTION_CTRL_PAUSE_SPORT,                 /* 暂停运动 */
    ASR_ENGINE_MOTION_CTRL_RESUME_SPORT,                /* 继续运动 */
    ASR_ENGINE_MOTION_CTRL_STOP_SPORT,                  /* 停止运动 */
    ASR_ENGINE_MOTION_CTRL_START_LAST_SPORT,            /* 开始上次运动 */
    ASR_ENGINE_MOTION_CTRL_MAX,
} AsrEngineMotionControl;

typedef enum {
    ASR_ENGINE_MOTION_TARGET_NONE = 0x00,   /* 无目标 */
    ASR_ENGINE_MOTION_TARGET_DISTANCE_KM,   /* 距离目标：千米 */
    ASR_ENGINE_MOTION_TARGET_STEPS,         /* 步数目标 */
    ASR_ENGINE_MOTION_TARGET_TRIPS,         /* 趟数/圈数目标 */
    ASR_ENGINE_MOTION_TARGET_DISTANCE_M,    /* 距离目标：米 */
    ASR_ENGINE_MOTION_TARGET_CALORIE,       /* 卡路里目标 */
    ASR_ENGINE_MOTION_TARGET_MAX,
} AsrEngineMotionTarget;

/* 创建运动控制 */
typedef struct {
    AsrEngineMotionControl motionControlType;
    AsrEngineMotionTarget motionTarget;
    float value;
} AsrEngineMotionControlInfo;

/* 电话控制类型 */
typedef enum {
    ASR_ENGINE_TELEPHONE_CTRL_DIAL_NUMBER,  /* 拨打电话 */
    /* 无slot */
    ASR_ENGINE_TELEPHONE_CTRL_ANSWER_CALL,  /* 接听电话 */
    ASR_ENGINE_TELEPHONE_CTRL_REFUSE_CALL,  /* 拒绝电话 */
    ASR_ENGINE_TELEPHONE_CTRL_MAX,
} AsrEngineTelephoneControl;

/* 创建电话控制 */
typedef struct {
    AsrEngineTelephoneControl telephoneControlType;
    uint32_t telnum;
} AsrEngineTelephoneControlInfo;

/* 创建计数器控制 */
typedef enum {
    ASR_ENGINE_TIMER_CTRL_SET = 0x01,       /* 设置计时器，带slot */
    ASR_ENGINE_TIMER_CTRL_START = 0x02,     /* 启动计时器 */
    ASR_ENGINE_TIMER_CTRL_PAUSE = 0x03,     /* 暂停计时器 */
    ASR_ENGINE_TIMER_CTRL_RESUME = 0x04,    /* 恢复计时器(的暂停状态) */
    ASR_ENGINE_TIMER_CTRL_RESET = 0x05,     /* 复位计时器 */
    ASR_ENGINE_TIMER_CTRL_RESTART = 0x06,   /* 重启计时器 */
    ASR_ENGINE_TIMER_CTRL_MAX,              /* 用来检查动作是否合法 */
} AsrEngineTimerControl;

typedef struct {
    AsrEngineTimerControl timerControlType;
    int32_t hour; /* 单位：小时 */
    int32_t minute; /* 单位：分钟 */
    int32_t second; /* 单位：秒 */
} AsrEngineTimerControlInfo;

/* 创建设备控制 */
typedef enum {
    ASR_ENGINE_DEVICE_CTRL_SET_BRIGHTNESS,       /* 设置亮度 */
    ASR_ENGINE_DEVICE_CTRL_INCREASE_BRIGHTNESS,  /* 增大亮度 */
    ASR_ENGINE_DEVICE_CTRL_DECREASE_BRIGHTNESS,  /* 减小亮度 */
    ASR_ENGINE_DEVICE_CTRL_OPEN_MUTE,            /* 打开静音 */
    ASR_ENGINE_DEVICE_CTRL_INCREASE_VOLUME,      /* 增大音量 */
    ASR_ENGINE_DEVICE_CTRL_DECREASE_VOLUME,      /* 减小音量 */
    ASR_ENGINE_DEVICE_CTRL_OPEN_STEADY_SCREEN,   /* 打开持续亮屏 */
    ASR_ENGINE_DEVICE_CTRL_CLOSE_STEADY_SCREEN,  /* 关闭持续亮屏 */
    ASR_ENGINE_DEVICE_CTRL_SET_RING,             /* 设置铃声 */
    ASR_ENGINE_DEVICE_CTRL_LOCK_SCREEN,          /* 锁定屏幕 */
    ASR_ENGINE_DEVICE_CTRL_RETURN_HOME,          /* 回到表盘 */
    ASR_ENGINE_DEVICE_CTRL_SET_NODISTURB,        /* 设置勿扰 */
    ASR_ENGINE_DEVICE_CTRL_OPEN_NODISTURB,       /* 打开勿扰 */
    ASR_ENGINE_DEVICE_CTRL_CLOSE_NODISTURB,      /* 关闭勿扰 */
    ASR_ENGINE_DEVICE_CTRL_TIMING_NODISTURB,     /* 定时勿扰 */
    ASR_ENGINE_DEVICE_CTRL_SMART_NODISTURB,      /* 智能勿扰 */
    ASR_ENGINE_DEVICE_CTRL_OPEN_POWER_SAVE,      /* 打开省电 */
    ASR_ENGINE_DEVICE_CTRL_CLOSE_POWER_SAVE,     /* 关闭省电 */
    ASR_ENGINE_DEVICE_CTRL_OPEN_THEATER,         /* 打开剧院 */
    ASR_ENGINE_DEVICE_CTRL_CLOSE_THEATER,        /* 关闭剧院 */
    ASR_ENGINE_DEVICE_CTRL_CONNECT_PHONE,        /* 连接手机 */
    ASR_ENGINE_DEVICE_CTRL_FIND_PHONE,           /* 查找手机 */
    ASR_ENGINE_DEVICE_CTRL_OPEN_FLASHLIGHT,      /* 打开手电筒 */
    ASR_ENGINE_DEVICE_CTRL_CLOSE_FLASHLIGHT,     /* 关闭手电筒 */
    ASR_ENGINE_DEVICE_CTRL_MAX,
} AsrEngineDeviceControl;

/* 创建设备控制 */
typedef struct {
    AsrEngineDeviceControl deviceControlType;
} AsrEngineDeviceControlInfo;

/* 创建唤醒词信息 */
typedef enum {
    ASR_ENGINE_WAKEUP_KEYWORD_XIAO_YI = 0x01,        /* 小艺，小艺 */
    ASR_ENGINE_WAKEUP_KEYWORD_XIAO_WEI = 0x02,       /* 小薇，小薇 */
    ASR_ENGINE_WAKEUP_KEYWORD_HELLO_YOYO = 0x03,     /* 你好悠悠 */
    ASR_ENGINE_WAKEUP_KEYWORD_XIAO_AI = 0x04,        /* 小爱 */
    ASR_ENGINE_WAKEUP_KEYWORD_HI_CELIA = 0x05,        /* 嗨，塞莉亚 */
    ASR_ENGINE_WAKEUP_KEYWORD_MAX,
} AsrEngineWakeupKeyword;

/* 创建唤醒词 */
typedef struct {
    AsrEngineWakeupKeyword wakeupKeyword;
} AsrEngineWakeupKeywordInfo;


typedef struct {
    int32_t index;
    AsrEngineCmdId cmdId;
    union {
        AsrEngineAppControlInfo appControlInfo;
        AsrEngineMusicControlInfo musicControlInfo;
        AsrEngineAlarmControlInfo alarmControlInfo;
        AsrEngineMotionControlInfo motionControlInfo;
        AsrEngineTelephoneControlInfo telephoneControlInfo;
        AsrEngineTimerControlInfo timerControlInfo;
        AsrEngineDeviceControlInfo deviceControlInfo;
        AsrEngineWakeupKeywordInfo wakeupKeywordInfo;
    };
} AsrEngineCmdInfo;

typedef void (*AsrEngineListener)(AsrEngineEvent event, void *data, uint32_t length);

typedef enum {
    ASR_ENGINE_WAKEUP_ON_VAD,        /* keep voice engine in sleep state, until human voice is detected. */
    ASR_ENGINE_ALWAYS_WAKE,          /* no matter whether there is human voice, keep voice engine in wake state. */
} AsrEngineWorkMode;

typedef enum {
    ASR_ENGINE_CHINESE,     /* 中文. */
    ASR_ENGINE_ENGLISH,     /* 英文. 暂不支持 */
} AsrEngineLanguage;

typedef struct {
    bool  enableNpu; /* false: 固定命令词 true:模糊命令词. */
    AsrEngineLanguage language;
    AsrEngineWorkMode workMode;
    AsrEngineListener listener;
} AsrEngineInitParams;

int32_t IntellVoiceAsrEngineInit(AsrEngineInitParams *initParams);

int32_t IntellVoiceAsrEngineStartRecognition(void);

int32_t IntellVoiceAsrEngineStopRecognition(void);

int32_t IntellVoiceAsrEngineDeInit(void);

int32_t IntellVoiceAsrPickUpStreamStart(void);

int32_t IntellVoiceAsrPickUpStreamStop(void);

#ifdef __cplusplus
}
#endif

#endif // SMART_VS_H