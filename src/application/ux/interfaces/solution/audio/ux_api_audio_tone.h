/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved. \n
 * Description: Provide ux audio tone uapi \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2026-01-30， Create file. \n
 */

#ifndef UX_API_AUDIO_TONE_H
#define UX_API_AUDIO_TONE_H

#include "stdint.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MIX_VOLUME_MAX 100                                  /**< 音乐或通话的混音音量占比100 */
#define MIX_VOLUME_DEFAULT 50                               /**< 音乐或通话的混音音量占50 */
#define TONE_INFO_MAP_MAX  UX_API_AUDIO_TONE_MAX_NUM       /**< 提示音信息map最大注册限制 */

/**
 * @brief 提示音类型枚举
 */
typedef enum {
    UX_API_AUDIO_TONE_NONE = 0,                /**< 无提示音 */
    UX_API_AUDIO_TONE_SHUT_DOWN,               /**< 关机提示音 */
    UX_API_AUDIO_TONE_BOOT_UP,                 /**< 开机提示音 */
    UX_API_AUDIO_TONE_ANC_ON,                  /**< 降噪开启提示音 */
    UX_API_AUDIO_TONE_ANC_OFF,                 /**< 降噪关闭提示音 */
    UX_API_AUDIO_TONE_HT,                      /**< 通透模式提示音 */
    UX_API_AUDIO_TONE_INTELLIGENT = 6,         /**< 智能提示音 */
    UX_API_AUDIO_TONE_AHA_OFF_TOBE,            /**< AHA关闭提示音 */
    UX_API_AUDIO_TONE_MUTE_AHA_OFF,            /**< 静音AHA关闭提示音 */
    UX_API_AUDIO_TONE_MUTE_ANC_ON,             /**< 静音降噪开启提示音 */
    UX_API_AUDIO_TONE_MUTE_HEAR_THROUGH,       /**< 静音通透提示音 */
    UX_API_AUDIO_TONE_MUTE_HT_ON = 11,         /**< 静音通透开启提示音 */
    UX_API_AUDIO_TONE_MUTE_HT_OFF,             /**< 静音通透关闭提示音 */
    UX_API_AUDIO_TONE_BT_CONNECT,              /**< 蓝牙连接提示音 */
    UX_API_AUDIO_TONE_BT_CONNECT_SUCC,         /**< 蓝牙连接成功提示音 */
    UX_API_AUDIO_TONE_BT_DISCONNECT,           /**< 蓝牙断连提示音 */
    UX_API_AUDIO_TONE_DEVICE_RECONNECTED = 16, /**< 设备重连提示音 */
    UX_API_AUDIO_TONE_DEVICE_RECONNECTED_SUCC, /**< 设备重连成功提示音 */
    UX_API_AUDIO_TONE_DEVICE_DISCONNECTED_AND_PARING, /**< 设备断连配对提示音 */
    UX_API_AUDIO_TONE_PAIRING,                 /**< 配对提示音 */
    UX_API_AUDIO_TONE_PAIR_CONNECTED,          /**< 配对连接提示音 */
    UX_API_AUDIO_TONE_DEVICE_PAIR = 21,        /**< 设备配对提示音 */
    UX_API_AUDIO_TONE_BT_STANDBY,              /**< 蓝牙待机提示音 */
    UX_API_AUDIO_TONE_DEVICE_OVER_DISTANCE_RECONNECT, /**< 设备超出距离重连提示音 */
    UX_API_AUDIO_TONE_BINAURAL_PAIRING,        /**< 双耳配对提示音 */
    UX_API_AUDIO_TONE_BINAURAL_RECONNECTED,    /**< 双耳重连提示音 */
    UX_API_AUDIO_TONE_OUTBAND_RING_ON = 26,    /**< 带外铃声开启提示音 */
    UX_API_AUDIO_TONE_OUTBAND_RING_OFF,        /**< 带外铃声关闭提示音 */
    UX_API_AUDIO_TONE_INCOMING_CALL,           /**< 来电提示音 */
    UX_API_AUDIO_TONE_LOW_BATTERY,             /**< 低电量提示音 */
    UX_API_AUDIO_TONE_BATTERY_LOW_POWER_OFF,   /**< 低电量关机提示音 */
    UX_API_AUDIO_TONE_TURN_UP_VOLUME = 31,     /**< 音量调高提示音 */
    UX_API_AUDIO_TONE_TURN_DOWN_VOLUME,        /**< 音量调低提示音 */
    UX_API_AUDIO_TONE_MAX_VOLUME,              /**< 最大音量提示音 */
    UX_API_AUDIO_TONE_MIN_VOLUME,              /**< 最小音量提示音 */
    UX_API_AUDIO_TONE_SEARCH_HP_ON,            /**< 搜索耳机开启提示音 */
    UX_API_AUDIO_TONE_SEARCH_HP_OFF = 36,      /**< 搜索耳机关闭提示音 */
    UX_API_AUDIO_TONE_ADD_FAVORITES_SUCCESS,   /**< 添加收藏成功提示音 */
    UX_API_AUDIO_TONE_ADD_FAVORITES_FAIL,      /**< 添加收藏失败提示音 */
    UX_API_AUDIO_TONE_PROGRAM_ZERO,            /**< 节目0提示音 */
    UX_API_AUDIO_TONE_PROGRAM_ONE,             /**< 节目1提示音 */
    UX_API_AUDIO_TONE_PROGRAM_TWO = 41,        /**< 节目2提示音 */
    UX_API_AUDIO_TONE_PROGRAM_THREE,           /**< 节目3提示音 */
    UX_API_AUDIO_TONE_PROGRAM_FOUR,            /**< 节目4提示音 */
    UX_API_AUDIO_TONE_PROGRAM_FIVE,            /**< 节目5提示音 */
    UX_API_AUDIO_TONE_PROGRAM_SIX,             /**< 节目6提示音 */
    UX_API_AUDIO_TONE_FACTORY_RESTORE = 46,    /**< 恢复出厂设置提示音 */
    UX_API_AUDIO_TONE_DUT_MODE,                /**< DUT模式提示音 */
    UX_API_AUDIO_TONE_SINGLE_CLICK_BUTTON,     /**< 单击按钮提示音 */
    UX_API_AUDIO_TONE_WEAR_IN,                 /**< 佩戴入耳提示音 */
    UX_API_AUDIO_TONE_BINAURAL_DISCONNECTED,   /**< 双耳断连提示音 */
    UX_API_AUDIO_TONE_LOW_BATTERY_RESUME = 51, /**< 低电量恢复提示音 */
    UX_API_AUDIO_TONE_LOW_LATENCY_OFF,         /**< 低延迟关闭提示音 */
    UX_API_AUDIO_TONE_LOW_LATENCY_ON,          /**< 低延迟开启提示音 */
    UX_API_AUDIO_TONE_DEVICE_DISCONNECTED,     /**< 设备断连提示音 */
    UX_API_AUDIO_TONE_SET_ANC,                 /**< 设置降噪提示音 */
    UX_API_AUDIO_TONE_LOCAL_TOUCH = 56,        /**< 本地触摸提示音 */
    UX_API_AUDIO_TONE_SYNC_TOUCH,              /**< 同步触摸提示音 */
    UX_API_AUDIO_TONE_MAX_NUM,                 /**< 最大数量 */
} ux_api_audio_tone_t;

/**
 * @brief 提示音打断次数枚举
 */
typedef enum {
    UX_API_AUDIO_TONE_INTERRUPT_ONE = 1,   /**< 打断一次 */
    UX_API_AUDIO_TONE_INTERRUPT_TWO,       /**< 打断两次 */
    UX_API_AUDIO_TONE_INTERRUPT_THREE,     /**< 打断三次 */
    UX_API_AUDIO_TONE_INTERRUPT_FOUR,      /**< 打断四次 */
    UX_API_AUDIO_TONE_INTERRUPT_FIVE,      /**< 打断五次 */
    UX_API_AUDIO_TONE_INTERRUPT_MAX = 0xffff, /**< 最大打断次数 */
} ux_api_audio_tone_interrupt_num_t;

/**
 * @brief 提示音播放模式枚举
 */
typedef enum {
    UX_API_AUDIO_TONE_PLAY_MODE_SYNC = 1,   /**< 同步播放模式 */
    UX_API_AUDIO_TONE_PLAY_MODE_LOCAL = 2,  /**< 本地播放模式 */
    UX_API_AUDIO_TONE_PLAY_MODE_MAX,        /**< 最大值 */
} ux_api_audio_tone_play_mode_t;

/**
 * @brief 提示音语言枚举
 */
typedef enum {
    UX_API_AUDIO_TONE_LANGUAGE_CN = 0,      /**< 中文 */
    UX_API_AUDIO_TONE_LANGUAGE_EN = 1,      /**< 英文 */
    UX_API_AUDIO_TONE_LANGUAGE_MAX = 10,    /**< 最大值 */
} ux_api_audio_tone_language_t;

/**
 * @brief 提示音播放完成后回调函数类型定义
 */
typedef void (*ux_api_audio_play_after_callback_t)(void);

/**
 * @brief 提示音信息结构体
 */
typedef struct {
    uint16_t id;                        /**< tone id */
    char* path;                         /**< 音源路径 */
    uint8_t play_mode;                  /**< 播放方式 */
    bool is_slave_play;                 /**< 副耳是否播放 */
    int8_t priority;                    /**< 播放优先级 */
    ux_api_audio_play_after_callback_t play_after;   /**< play_after 播放相应提示音后对应的处理，默认空 */
    int32_t mix_volume;                 /**< 当前提示音播放时音乐或通话的混音音量占比 */
    int32_t max_interrupt_count;        /**< 提示音最大打断次数 */
    bool is_immediate_callback ;        /**< 是否播放完成后立刻回调LITE_PLAY_EVENT_COMPLETE事件 */
} ux_api_audio_tone_info_t;

/**
 * @defgroup 支持产品 [earphone] [soundbox]
 * @brief 提示音信息表初始化
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_tone_map_init(void);

/**
 * @defgroup 支持产品 [earphone] [soundbox]
 * @brief 提示音信息表去初始化
 */
void ux_api_audio_tone_map_deinit(void);

/**
 * @defgroup 支持产品 [earphone] [soundbox]
 * @brief 注册提示音语言，目前支持中英文
 * @param [in] id 提示音语言编号
 * @param [in] language_str 提示音语言缩写的字符串
 */
void ux_api_audio_tone_language_register(uint8_t id, const char *language_str);

/**
 * @defgroup 支持产品 [earphone] [soundbox]
 * @brief 注册提示音信息
 * @param [in] tone_info 提示音信息，参考 @ref ux_api_audio_tone_info_t
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_tone_info_register(const ux_api_audio_tone_info_t* tone_info);

/**
 * @brief 提示音播放状态枚举
 */
typedef enum {
    UX_API_AUDIO_TONE_STATE_STOP,     /**< 停止状态 */
    UX_API_AUDIO_TONE_STATE_PLAYING,  /**< 播放中状态 */
    UX_API_AUDIO_TONE_STATE_MAX       /**< 状态最大值 */
} ux_api_audio_tone_state;

/**
 * @brief 提示音铃声状态枚举
 */
typedef enum {
    UX_API_AUDIO_TONE_RING_STATE_PLAY,  /**< 铃声播放状态 */
    UX_API_AUDIO_TONE_RING_STATE_STOP,  /**< 铃声停止状态 */
    UX_API_AUDIO_TONE_RING_STATE_MAX    /**< 最大值 */
} ux_api_audio_tone_ring_state;

/**
 * @defgroup 支持产品 [earphone] [soundbox]
 * @brief 获取指定提示音的播放状态
 * @return ux_api_audio_tone_state 返回提示音的播放状态
 */
ux_api_audio_tone_state ux_api_audio_tone_get_play_state(void);

/**
 * @defgroup 支持产品 [earphone] [soundbox]
 * @brief 播放指定的提示音
 * @param [in] tone_id 要播放的提示音的ID
 * @return None
 */
void ux_api_audio_tone_play(uint16_t tone_id);

/**
 * @defgroup 支持产品 [earphone] [soundbox]
 * @brief 停止所有正在播放的提示音
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_tone_stop_all(void);

/**
 * @defgroup 支持产品 [earphone] [soundbox]
 * @brief 设置提示音音量
 * @param [in] tone_vol 提示音音量值
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_tone_set_vol(int32_t tone_vol);

/**
 * @defgroup 支持产品 [earphone] [soundbox]
 * @brief 获取当前提示音音量
 * @return int32_t 返回当前提示音音量值
 */
int32_t ux_api_audio_tone_get_vol(void);

/**
 * @defgroup 支持产品 [earphone] [soundbox]
 * @brief 设置带外铃声状态
 * @param [in] state 铃声状态 {@ref ux_api_audio_tone_ring_state}
 * @return ERRCODE_SUCC 成功；Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_tone_set_outband_ring_state(ux_api_audio_tone_ring_state state);

/**
 * @defgroup 支持产品 [earphone] [soundbox]
 * @brief 获取正在播放的提示音ID
 * @return uint16_t 当前播放的提示音ID
 */
uint16_t ux_api_audio_tone_get_playing_tone_id(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
