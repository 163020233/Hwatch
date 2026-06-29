/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide timer set service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define sec_to_milsec(x) ((x) * 1000)
typedef enum {
    UX_TIMER_STATE_RUNNING = 0,
    UX_TIMER_STATE_STOP,
    UX_TIMER_STATE_INVALID,
    UX_TIMER_STATE_UNREGISTERED,

    UX_TIMER_STATE_BUTT
} ux_timer_state_t;

typedef enum {
    POWER_ON_DELAY = 0,
    SOUND_TO_HAID_DELAY,
    WEAR_IN_HAID_DELAY,               /* 入耳进入助听 */
    SOUND_FORCE_TO_HAID_DELAY,        /* 音频状态强制进入助听 */

    PEER_CON_TIMER,                   /* 对耳配对 */
    PEER_CON_TIMEOUT_RESET_TIMER = 5, /* 对耳连接超时reset */
    PEER_RECON_TIMER,                 /* 对耳回连配对 */
    PEER_OVERRANGE_RECON_TIMER,       /* 对耳超距回连配对 */
    PEER_DISCOVER_TRIGGER_TIMER,      /* 对耳扫描触发 */
    PEER_DISCOVER_RESET_TIMER,        /* 对耳扫描后重启定时器 */
    DEVICE_CON_TIMER = 10,            /* 设备配对 */
    DEVICE_RECON_TIMER,               /* 设备回连配对 */
    DEVICE_OVERRANGE_RECON_TIMER,     /* 设备超距回连配对 */
    PAIRING_STANDBY_TIMER,            /* 设备配对超时两次，蓝牙下电 */
    LOWBATTERY_WARNING_TIMER,         /* 低电量告警定时器 */
    RECOVER_TIMER,                    /* 恢复出厂设置 */
    DUT_RECOVER_TIMER,                /* DUT恢厂设置 */
    CLOSE_BOX_DELAY,                  /* 延时关盒定时器 */
    LOWBATTERY_HANDOVER_DELAY,        /* 低电量关机主副切定时器 */
    ACC_OFF_DELAY,                    /* 下电前通知BT核要下电 */
    OTA_RESET_DELAY,                  /* OTA升级成功复位延时定时器 */
    POWER_RESET_EXC,                  /* power 异常复位 */
    POWER_ON_EXC,                     /* power on异常保护 */
    POWER_OFF_EXC,                    /* power off ack异常保护 */
    POWER_OFF_DELAY,                  /* power off延时定时器 */
    BUTTON_MIX_MODE,                  /* 混合按键延时定时器 */
    WEAR_IN_START_DELAY_TIMER,        /* 延时入耳定时器 */
    DUT_RESET_DELAY,                  /* DUT复位延时定时器 */
    OUT_BOX_TIGGER_ACC_OFF_TIMER,     /* 出盒触发下电保护定时器 */
	CHARGE_FULL_TIMER,                /* 满电下电定时器 */
    NOTIFY_BT_OFF_DELAY_TIMER,        /* 通知BT关机延时定时器 */
    AFC_CALIBRATE_TIMER,              /* 啸叫校准定时器 */
    ACC_OFF_DELAY_TIMER,              /* 延时下电定时器 */
    RESET_DELAY_TIMER,                /* 延时重启定时器 */
    BT_ACC_OFF_DELAY_TIMER,           /* B核延时下电定时器 */
    CHARGER_BEAT_TIMER,               /* Charger心跳定时器 */
    AUTO_POWER_OFF_TIMER,             /* 自动关机检测定时器 */
    MASTER_RESET_DELAY,               /* 整机复位延时定时器 */

    SLE_PAIRING_TIMER,                /* 星闪配对超时计时器 */
    SLE_TRANSMIT_MODE_CHANGE_TIMER,   /* 星闪工作模式切换计时器 */
    SLE_RSSI_REPORT,                  /* 星闪连接信号强度上报计时器 */
    NOTIFY_REMAIN_TIME,               /* 剩余录制时间上报计时器 */

    BLE_START_DISCOVER_TIMER,         /* 启动BLE广播定时器 */
    BLE_STOP_DISCOVER_TIMER,          /* 停止BLE广播定时器 */
    SLE_START_DISCOVER_TIMER,         /* 启动SLE广播定时器 */
    OUT_EAR_DETECTION_TIMER,          /* 出耳后佩戴检测功能持续时间 */
    PROTOCOL_ASYNC_PROCESS_TIMER,     /* 模拟对外协议异步流程 */
    USB_UAC_EVENT_TIMER,              /* USB定时检测有效事件 */
    POWER_SYNC_SLAVE_BATTERY,         /* 同步副设备电量到主设备 */
    POWER_SYNC_MASTER_BATTERY,        /* 同步主设备电量到显示屏 */

    UX_TIMER_SET_MAX
} ux_timer_index_t;


typedef void (*timer_callback_t)(void);

errcode_t ux_timer_register(uint8_t index, timer_callback_t func);
errcode_t ux_timer_start(uint8_t index, uint32_t time);
errcode_t ux_timer_stop(uint8_t index);
uint8_t get_timer_state(uint8_t index);
void ux_timer_list_stop(uint8_t *list, uint8_t size);
errcode_t ux_timer_deregister(uint8_t index);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
