/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AppViewIDs
 * Create: 2025-11
 */

#ifndef APP_VIEW_IDS_H
#define APP_VIEW_IDS_H

#include <cstdint>

/*
 * 基础应用的view id，与MVP的V一一对应
 * 枚举命名要求：参考View名字，名字尽量简短
 */
typedef enum : uint16_t {
    VIEW_INVALID = 0,
    VIEW_MAIN = 1,
    VIEW_SETTING = 2,
    VIEW_APPLIST = 3,
    VIEW_SETTING_FIRST = 4,
    VIEW_MAIN_SAMPLE = 5,
    VIEW_MAIN_ALARM = 6,
    VIEW_MAIN_ACTIVITY = 7,
    VIEW_ALARM_POP_UP,
    VIEW_CALLER_SAMPLE,
    VIEW_BLOODOXY_SAMPLE,
    VIEW_BLUETOOTH_EARPHONE,
    VIEW_OFFLINEVOICE,
    VIEW_WIFI,
    VIEW_VOLUME,
    VIEW_COMPASS,
    VIEW_CONNECT_PHONE,
    VIEW_MAIN_TEMP,
    VIEW_REBOOT,
    VIEW_UISAMPLE,
    VIEW_BREATH,
    VIEW_FLASHLIGHT,  // 手电筒;
    VIEW_STOPWATCH,  // 秒表;
    VIEW_WEATHER, // 天气
    VIEW_TIMER, // 计时器
    VIEW_BLOOD, // 血氧
    VIEW_CAMERA, // 相机
    VIEW_FIND_PHONE, // 寻找手机
    VIEW_DATE, // 日期
    VIEW_SPORT,
    VIEW_PHONE_MENU,
    VIEW_SLEEPING,
    VIEW_EMPTY,
    VIEW_APP,
    VIEW_HELLOWORLD,
    VIEW_HEARTRATE,
    VIEW_MONKEY,
    VIEW_MAP,
#ifdef SUPPORT_ALIPAY_SEC
    VIEW_MAIN_ALIPAY,
#endif
#ifdef SUPPORT_ALIPAY_HSC32I1
    VIEW_ALIPAY_HSC32I1,
#endif
    VIEW_AOD,
    VIEW_RESTORE_DEFAULTS,
    VIEW_RECENT_APP,
    VIEW_DEV_ROTATE_DEMO,
    VIEW_SWITCH_SLICE_DEMOA,
    VIEW_SWITCH_SLICE_DEMOB,
    VIEW_SWITCH_SLICE_DEMOC,
    VIEW_SWITCH_SLICE_DEMOD,
    VIEW_PAUSE_TEST,
    VIEW_MAX_INTER_ARRY_APP = 0x7FFF, // 在此之前添加内部数组native应用ID.
    VIEW_MAX_INTER_APP = 0xFFFD, // 在此之前添加内部静态注册native应用ID,此ID在NativeRegisterType.h里单独管理
    VIEW_EXTERN_APP = 0xFFFE, // 三方应用
    VIEW_INVALIDE_APP = 0xFFFF,
} AppViewId;

/*
 * Slice页面优先级，越小优先级越高
 */
typedef enum : uint16_t {
    PRIO_0 = 0,
    PRIO_1,
    PRIO_2,
    PRIO_3,
    PRIO_4
} AppPriority;

#endif
