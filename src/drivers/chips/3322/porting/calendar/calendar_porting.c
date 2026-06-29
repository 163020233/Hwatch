/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides calendar port template. \n
 * The Systick module needs to be used for simulation, calendar port file is stored in this path.
 *
 * History: \n
 * 2025-03-22， Create file. \n
 */
#include <stdio.h>
#include "sys/time.h"
#include "time.h"
#include "time64.h"
#include "los_tick.h"
#include "hal_calendar.h"
#include "systick.h"
#include "tcxo.h"
#include "soc_reg.h"
#include "chip_io.h"
#ifdef CONFIG_NV_FEATURE_SUPPORT
#include "nv.h"
#endif
#include "rtc_porting.h"
#include "systick_porting.h"
#include "soc_osal.h"
#include "pmu_ulp.h"
#include "pmu_alarm.h"
#include "calendar_porting.h"

#define RTC_TIME_BASE   (946684800000 * 1000) /* Base time is 2000/01/01 */

static uint64_t g_calendar_timebase = RTC_TIME_BASE;
static uint64_t g_calendar_tick_us;
static uint64_t g_calendar_tick_count;
static int32_t g_timezone = 0;
static uint8_t g_poweron_flag = 0;
static bool g_calendar_inited = false;

static uint64_t hal_calendar_get_aon_timebase(void)
{
    uint64_t base_time = 0;

    base_time = (uint64_t)readw(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_0_REG);
    base_time |= (uint64_t)readw(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_1_REG) << 16U;
    base_time |= (uint64_t)readw(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_2_REG) << 32U;
    base_time |= (uint64_t)readw(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_3_REG) << 48U;

    return base_time;
}

static void hal_calendar_set_aon_timebase(uint64_t timebase)
{
    writew(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_0_REG, timebase & 0xFFFF);
    writew(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_1_REG, (timebase >> 16U) & 0xFFFF);
    writew(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_2_REG, (timebase >> 32U) & 0xFFFF);
    writew(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_3_REG, (timebase >> 48U) & 0xFFFF);
}

static uint64_t hal_calendar_get_aon_tick_us(void)
{
    uint64_t tick_ms = 0;

    tick_ms = (uint64_t)readw(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_4_REG);
    tick_ms |= (uint64_t)readw(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_5_REG) << 16U;
    tick_ms |= (uint64_t)readw(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_6_REG) << 32U;
    tick_ms |= (uint64_t)readw(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_7_REG) << 48U;

    return tick_ms;
}

static void hal_calendar_set_aon_tick_us(uint64_t tick_ms)
{
    writew(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_4_REG, tick_ms & 0xFFFF);
    writew(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_5_REG, (tick_ms >> 16U) & 0xFFFF);
    writew(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_6_REG, (tick_ms >> 32U) & 0xFFFF);
    writew(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_7_REG, (tick_ms >> 48U) & 0xFFFF);
}

static uint64_t hal_calendar_get_aon_tick_count(void)
{
    uint64_t tick_count = 0;

    tick_count = (uint64_t)readw(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_8_REG);
    tick_count |= (uint64_t)readw(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_9_REG) << 16U;
    tick_count |= (uint64_t)readw(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_10_REG) << 32U;
    tick_count |= (uint64_t)readw(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_11_REG) << 48U;

    return tick_count;
}

static void hal_calendar_set_aon_tick_count(uint64_t tick_count)
{
    writew(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_8_REG, tick_count & 0xFFFF);
    writew(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_9_REG, (tick_count >> 16U) & 0xFFFF);
    writew(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_10_REG, (tick_count >> 32U) & 0xFFFF);
    writew(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_11_REG, (tick_count >> 48U) & 0xFFFF);
}

static uint64_t systick_aon_get_count(void)
{
    uint64_t val = 0;
    reg16_setbit(GLB_CTL_M_RB_SYS_TICK_CFG_M_REG, GLB_CTL_M_RB_SYS_TICK_FRZ_M_OFFSET);
    val = (uint64_t)readw(GLB_CTL_M_RB_SYS_TICK_VALUE_M_0_REG);
    val |= (uint64_t)(readw(GLB_CTL_M_RB_SYS_TICK_VALUE_M_1_REG)) << 16U;
    val |= (uint64_t)(readw(GLB_CTL_M_RB_SYS_TICK_VALUE_M_2_REG)) << 32U;
    val |= (uint64_t)(readw(GLB_CTL_M_RB_SYS_TICK_VALUE_M_3_REG)) << 48U;
    reg16_clrbit(GLB_CTL_M_RB_SYS_TICK_CFG_M_REG, GLB_CTL_M_RB_SYS_TICK_FRZ_M_OFFSET);
    return val;
}


// 0-3 bit 共4bit，存储值为 timestamp*10
#define SHIP_MODE_TIME_REG4_START_POS (0)
#define SHIP_MODE_TIME_REG4_START_LEN (4)
#define SHIP_MODE_TIME_MUL (100)  // 用于ms转ship_mode_time

// 读取ship_mode_time，返回单位ms，精度100ms
uint64_t hal_get_ship_mode_time(void)
{
    uint64_t ship_mode_time = (uint64_t)readw(LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG5_REG);
    ship_mode_time |= ((uint64_t)readw(LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG6_REG) << 16U);
    ship_mode_time |= ((uint64_t)(reg16_getbits(LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG4_REG,
                                                SHIP_MODE_TIME_REG4_START_POS, SHIP_MODE_TIME_REG4_START_LEN)) << 32U);
    return ship_mode_time * SHIP_MODE_TIME_MUL;
}

// 设置ship_mode_time，单位ms，误差100ms
void hal_set_ship_mode_time(uint64_t timestamp_ms)
{
    uint64_t ship_mode_time = timestamp_ms / SHIP_MODE_TIME_MUL;
    writew(LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG5_REG, (uint16_t)(ship_mode_time & 0xFFFF));
    writew(LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG6_REG, (uint16_t)((ship_mode_time >> 16U) & 0xFFFF));
    reg16_setbits(LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG4_REG, SHIP_MODE_TIME_REG4_START_POS,
                  SHIP_MODE_TIME_REG4_START_LEN, (uint16_t)((ship_mode_time >> 32U) & 0xF));
}

void hal_clear_ship_mode_time(void)
{
    writew(LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG5_REG, 0);
    writew(LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG6_REG, 0);
    reg16_setbits(LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG4_REG, SHIP_MODE_TIME_REG4_START_POS,
                  SHIP_MODE_TIME_REG4_START_LEN, 0);
}

static errcode_t hal_calendar_customized_init(void)
{
    uint16_t length;
    g_calendar_timebase = hal_calendar_get_aon_timebase();
    if (g_calendar_timebase < RTC_TIME_BASE) {
        uint64_t ship_mode_time = hal_get_ship_mode_time();
        ship_mode_time *= OS_SYS_MS_PER_SECOND;
        if (ship_mode_time == 0) {
            // 冷启动或者未同步过时间戳
            hal_calendar_set_aon_timebase(RTC_TIME_BASE);
            g_calendar_timebase = RTC_TIME_BASE;
        } else {
            // 关机启动
            g_calendar_timebase = (uint64_t)ship_mode_time + uapi_ulp_systick_get_us();
            hal_calendar_set_aon_timebase(g_calendar_timebase);

            g_calendar_tick_us = 0;
            g_calendar_tick_count = systick_aon_get_count();
            hal_calendar_set_aon_tick_us(g_calendar_tick_us);
            hal_calendar_set_aon_tick_count(g_calendar_tick_count);
            hal_clear_ship_mode_time();
        }
    } else {
        // 热复位
        g_calendar_tick_us = hal_calendar_get_aon_tick_us();
        g_calendar_tick_count = hal_calendar_get_aon_tick_count();

        // 计算距离上次的差值
        uint64_t current = systick_aon_get_count();
        g_calendar_tick_us += (current - g_calendar_tick_count) * OS_SYS_US_PER_SECOND / rtc_porting_clock_value_get();
        g_calendar_tick_count = current;
        hal_calendar_set_aon_tick_us(g_calendar_tick_us);
        hal_calendar_set_aon_tick_count(g_calendar_tick_count);
    }
#ifdef CONFIG_NV_FEATURE_SUPPORT
    errcode_t ret = uapi_nv_read(NV_ID_TIMEZONE_ID, sizeof(g_timezone), &length, (uint8_t *)&g_timezone);
    if (ret != ERRCODE_SUCC) {
        osal_printk("nv read fail, time zone is utc+8.\n");
        g_timezone = (8 * 60 * 60); // UTC + 8h * 60min * 60s
    }
#endif
    timezone = g_timezone;

    g_calendar_inited = true;

    update_os_time();

    return ERRCODE_SUCC;
}

/* 单位为ms */
static uint64_t hal_calendar_customized_get_timestamp(void)
{
    if (!g_calendar_inited) {
        osal_printk("calendar not inited, timestamp return 0.\n");
        return 0;
    }

    uint64_t timestamp = 0;
    uint32_t irq_sts = osal_irq_lock();
    uint64_t current_tick = systick_aon_get_count();
    uint64_t delta = current_tick - g_calendar_tick_count;
    g_calendar_tick_us += (delta * OS_SYS_US_PER_SECOND / rtc_porting_clock_value_get());

    g_calendar_tick_count = current_tick;
    hal_calendar_set_aon_tick_us(g_calendar_tick_us);
    hal_calendar_set_aon_tick_count(g_calendar_tick_count);
    timestamp = (g_calendar_timebase + g_calendar_tick_us) / OS_SYS_US_PER_MS;
    osal_irq_restore(irq_sts);

    return timestamp;
}

/* 单位为ms */
static errcode_t hal_calendar_customized_set_timestamp(uint64_t timestamp)
{
    if (timestamp == 0) {
        return hal_calendar_customized_init();
    }

    uint64_t old_timestamp = hal_calendar_customized_get_timestamp() / OS_SYS_MS_PER_SECOND;

    uint32_t irq_sts = osal_irq_lock();
    g_calendar_timebase = timestamp * OS_SYS_US_PER_MS;
    hal_calendar_set_aon_timebase(g_calendar_timebase);

    g_calendar_tick_count = systick_aon_get_count();
    hal_calendar_set_aon_tick_count(g_calendar_tick_count);

    g_calendar_tick_us = 0;
    hal_calendar_set_aon_tick_us(g_calendar_tick_us);
    osal_irq_restore(irq_sts);

    // 同步alarm
    if (old_timestamp != timestamp / OS_SYS_MS_PER_SECOND) {
        alarm_sync_timestamp(timestamp / OS_SYS_MS_PER_SECOND);
    }

    return ERRCODE_SUCC;
}

static errcode_t hal_calendar_customized_set_timezone(int32_t time_zone)
{
    g_timezone = time_zone;
#ifdef CONFIG_NV_FEATURE_SUPPORT
    return uapi_nv_write(NV_ID_TIMEZONE_ID, (const uint8_t *)&g_timezone, sizeof(g_timezone));
#else
    return ERRCODE_SUCC;
#endif
}

static int32_t hal_calendar_customized_get_timezone(void)
{
    return (int32_t)g_timezone;
}

static hal_calendar_funcs_t g_hal_calendar_customized_funcs = {
    .set_timestamp = hal_calendar_customized_set_timestamp,
    .get_timestamp = hal_calendar_customized_get_timestamp,
    .set_timezone = hal_calendar_customized_set_timezone,
    .get_timezone = hal_calendar_customized_get_timezone,
};

static hal_calendar_funcs_t *hal_calendar_customized_funcs_get(void)
{
    return &g_hal_calendar_customized_funcs;
}

errcode_t calendar_port_register_hal_funcs(void)
{
    return hal_calendar_register_funcs(hal_calendar_customized_funcs_get());
}

errcode_t calendar_port_unregister_hal_funcs(void)
{
    return hal_calendar_unregister_funcs();
}

uint8_t *calendar_get_first_poweron_flag(void)
{
    return &g_poweron_flag;
}

errcode_t calendar_sync_before_shipmode(void)
{
    alarm_before_shipmode();    // 日历同步清空

    hal_set_ship_mode_time(hal_calendar_customized_get_timestamp());

    uint64_t left_count = uapi_alarm_get_left_count();
    uapi_ulp_systick_count_clear();

    if ((left_count > rtc_porting_clock_value_get()) || (left_count == 0) || (g_poweron_flag == 1)) {
        if (left_count != 0) {
            uapi_alarm_set_count(left_count);
        } else {
            uapi_alarm_disable();
        }
        return ERRCODE_SUCC;
    }
    osal_printk("alarm < 1s, do not enter shipmode.\n");
    return ERRCODE_FAIL;
}

#define SECSPERMIN           60     /* second per minute */
#define TIME_ZONE_MAX        840    /* UTC+14:00, the first time zone */
#define TIME_ZONE_MIN        (-720) /* UTC-12:00, the last time zone */

int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    if ((tv == NULL) && (tz == NULL)) {
        return -1;
    }

    if ((tv != NULL) && ((tv->tv_usec < 0) || (tv->tv_usec >= OS_SYS_US_PER_SECOND) || (tv->tv_sec < 0))) {
        return -1;
    }

    if (tz != NULL) {
        if ((tz->tz_minuteswest >= TIME_ZONE_MIN) && (tz->tz_minuteswest <= TIME_ZONE_MAX)) {
            timezone = tz->tz_minuteswest * SECSPERMIN;
        } else {
            return -1;
        }
        (void)hal_calendar_customized_set_timezone(timezone);
    }

    if (tv != NULL) {
        uint64_t ms = (uint64_t)(tv->tv_sec) * OS_SYS_MS_PER_SECOND + (uint64_t)tv->tv_usec / OS_SYS_MS_PER_SECOND;
        if (hal_calendar_customized_set_timestamp(ms) < 0) {
            return -1;
        }

        update_os_time();
    }

    return 0;
}

int gettimeofday(struct timeval *tv, void *tz)
{
    if (tv != NULL) {
        int64_t currentTime = (int64_t)hal_calendar_customized_get_timestamp();
        tv->tv_sec = currentTime / OS_SYS_MS_PER_SECOND;
        tv->tv_usec = (currentTime % OS_SYS_MS_PER_SECOND) * OS_SYS_MS_PER_SECOND;
    }

    if (tz != NULL) {
        int32_t timeZone = hal_calendar_customized_get_timezone();
        struct timezone *ptz = (struct timezone *)tz;
        ptz->tz_minuteswest = timeZone / 60; /* 60:Seconds in a minute */
        ptz->tz_dsttime = 0;
    }
    return 0;
}

void calendar_cail_update(void)
{
    hal_calendar_customized_get_timestamp();
}

void update_os_time(void)
{
    struct timeval cur_time = { 0 };
    gettimeofday(&cur_time, NULL);
    struct timeval64 setTime = {0};
    setTime.tv_sec = cur_time.tv_sec;
    setTime.tv_usec = cur_time.tv_usec;
    settimeofday64(&setTime, NULL); // 更新os的时间
}