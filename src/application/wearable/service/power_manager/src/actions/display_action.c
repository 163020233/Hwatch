/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: action display func.
 * Author:
 * Create:
 */

#include "power_display_service.h"
#include "display_action.h"
#include "common_def.h"
#include "pm_clocks.h"
#include "pm_veto.h"
#if defined(CONFIG_VIDEO_SUPPORT_LPM) && defined(SUPPORT_DISPLAY_TECH)
#include "pm_dev_porting.h"
#endif
#include "graphic_service_wrapper.h"
#include "soc_lcd_api.h"
#include "sys/time.h"
#include "time.h"
#include "tcxo.h"
#include "soc_dpu_api.h"
#ifdef SUPPORT_OHOSFWK
#include "screennotify/ui_screennotify.h"
#endif
#if defined(CONFIG_SUPPORT_FB_MEM_LP)
#include "pmu_mem.h"
#endif

uint32_t g_last_screen_on_timestamp;
uint32_t g_last_screen_off_timestamp;

uint32_t get_display_timestamp(void)
{
    struct timeval tv = { 0 };
    if (gettimeofday(&tv, NULL)) {
        return ERRCODE_FAIL;
    }
    return (uint32_t)tv.tv_sec;
}

static void caculate_screen_off_time(void)
{
    if (g_last_screen_off_timestamp == 0) {
        return;
    }
    screen_context_t *sc = power_display_get_screen_context();
    // 灭屏时间间隔等于本次亮屏的时间点减去上次灭屏的时间点。;
    sc->screen_off_time_interval = g_last_screen_on_timestamp - g_last_screen_off_timestamp;
}

static void action_display_lcd_on(void *data)
{
    unused(data);
#ifdef GPU_SUPPORT_VIDEO_MODE
    uapi_pm_add_sleep_veto(PM_ID_SCREEN);
#endif
    // get lcd status
    ext_lcd_status status = uapi_lcd_get_status();
    POWER_DISPLAY_PRINT_INFO(0, "status%u.", status.disp_status);
    if (status.disp_status == LCD_NORMAL_DISPLAY) {
        return;
    }
    uapi_pm_add_sleep_veto(PM_ID_SCREEN);
    uapi_pm_add_sleep_veto(PM_ID_UIKIT);
    // update screen on timestamp
    g_last_screen_on_timestamp = get_display_timestamp();
    caculate_screen_off_time();

    // set cpu performance mode
    uapi_clocks_config_mode(PM_ID_VIDEO, PM_MODE_PERFORMACE);

#if defined(CONFIG_VIDEO_SUPPORT_LPM) && defined(SUPPORT_DISPLAY_TECH)
    // video sub resume
    uapi_video_resume(PM_VIDEO_SWITCH_SCREEN_STA);
    uapi_lcd_display_on();
#endif

#if defined(CONFIG_SUPPORT_FB_MEM_LP)
    uapi_pmu_mem_fb_on();
#endif

    // notify gui, instruct the uikit to start the display operation.
#if defined(SUPPORT_OHOSFWK)
    NotifyScreenOn();
#endif
#ifndef GPU_SUPPORT_VIDEO_MODE
    uapi_pm_remove_sleep_veto(PM_ID_SCREEN);
#endif
}

static void caculate_screen_on_time(void)
{
    if (g_last_screen_on_timestamp == 0) {
        return;
    }
    screen_context_t *sc = power_display_get_screen_context();
    // 亮屏时间间隔等于本次灭屏的时间点减去上次亮屏的时间点。;
    sc->screen_on_time_interval = g_last_screen_off_timestamp - g_last_screen_on_timestamp;
}

static void action_display_lcd_off(void *data)
{
    unused(data);

    // get lcd status
    ext_lcd_status status = uapi_lcd_get_status();
    POWER_DISPLAY_PRINT_INFO(0, "status:%u.", status.disp_status);
    if (status.disp_status == LCD_DISPLAY_OFF) {
        return;
    }
    uapi_pm_add_sleep_veto(PM_ID_SCREEN);
    // update screen on timestamp
    g_last_screen_off_timestamp = get_display_timestamp();
    caculate_screen_on_time();

    // notify gui, instruct the uikit to close the display operation.
#if defined(SUPPORT_OHOSFWK)
    NotifyScreenOff();
#endif

#if defined(CONFIG_VIDEO_SUPPORT_LPM) && defined(SUPPORT_DISPLAY_TECH)
    // video sub suspend
    uapi_dpu_wait_for_done();
    uapi_lcd_display_off();
    uapi_video_suspend(PM_VIDEO_SWITCH_SCREEN_STA);
#endif

#if defined(CONFIG_SUPPORT_FB_MEM_LP)
    uapi_pmu_mem_fb_off();
#endif

    // set cpu lp mode
    uapi_clocks_config_mode(PM_ID_VIDEO, PM_MODE_LP);
    uapi_pm_remove_sleep_veto(PM_ID_SCREEN);
    uapi_pm_remove_sleep_veto(PM_ID_UIKIT);
}

int32_t action_display_execute_entry(uint16_t action_bitmap, uint16_t new_state, event_type_t type, uint32_t event)
{
    unused(type);
    unused(event);
    POWER_DISPLAY_PRINT_DEBUG(0, "action_display_execute_entry in,bitmap:%u,new_state:%u,type:%u,event:%u.",
        action_bitmap, new_state, type, event);
    screen_context_t *sc = power_display_get_screen_context();

    if (new_state == sc->current_state) {
        return 0;
    }

    if ((action_bitmap & GET_ACTION_MASK(SET_DISPLAY_ON)) != 0) {
        action_display_lcd_on(NULL);
    }

    if ((action_bitmap & GET_ACTION_MASK(SET_DISPLAY_TO_IDLE_MODE)) != 0) {
#if defined(SUPPORT_OHOSFWK)
        notify_screen_aod_on_event();
#endif
    }

    if ((action_bitmap & GET_ACTION_MASK(SET_DISPLAY_TO_NORMAL_MODE)) != 0) {
#if defined(SUPPORT_OHOSFWK)
        notify_screen_aod_off_event();
#endif
    }

    if ((action_bitmap & GET_ACTION_MASK(SET_DISPLAY_OFF)) != 0) {
        action_display_lcd_off(NULL);
    }

    return 0;
}
