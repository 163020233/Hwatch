/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Lcd driver.
 *
 * Create: 2022-03-05
 */

#include "soc_lcd_api.h"
#include <stdint.h>
#include "soc_errno.h"
#include "soc_osal.h"
#include "lcd_config.h"
#include "lcd_bus.h"
#include "lcd_qspi.h"
#include "lcd_mipi.h"
#include "lcd_common.h"
#include "lcd_private.h"
#include "lcd_dfx.h"
#include "lcd_mcu_int.h"
#include "pinctrl_porting.h"

typedef struct lcd_attr {
    bool info_register;           /* 注册指定屏幕组件标志 */
    bool is_inited;               /* LCD组件初始化标志 */
    lcd_type screen_type;         /* LCD屏类型 */
    uint32_t lcd_id;              /* LCD ID号 */
    lcd_drv_cfg *lcd_cfg;         /* LCD参数 */
    uint16_t brightness;          /* 显示亮度参数 */
    ext_lcd_status status;        /* LCD状态标志 */
    osal_mutex lcd_mutex;         /* LCD驱动多线程调用保护信号量 */
    lcd_bus_api *bus_api;         /* 总线接口 */
    lcd_drv_common *common_api;   /* LCD公共接口 */
    lcd_drv_private *private_api; /* LCD差异化接口 */
} lcd_attr;

lcd_attr g_lcd_info = {0};
volatile bool g_idle_before_sleep = false; /* false: normal display, true: idle mode */
static bool g_lcd_on_flag = false;
static bool g_ulps_entry_suspend_state = false;
static bool g_lcd_is_refresh = false;

bool lcd_get_status(void)
{
    return g_lcd_on_flag;
}

void lcd_set_status(bool lcd_on)
{
    g_lcd_on_flag = lcd_on;
}

static ext_errno lcd_lock_init(void)
{
    ext_errno ret;
    if (g_lcd_info.lcd_mutex.mutex != NULL) {
        return EXT_ERR_SUCCESS;
    }

    ret = osal_mutex_init(&(g_lcd_info.lcd_mutex));
    if (ret != EXT_ERR_SUCCESS) {
        g_lcd_info.lcd_mutex.mutex = NULL;
        return ret;
    }
    if (g_lcd_info.lcd_mutex.mutex == NULL) {
        return EXT_ERR_LCD_MUTEX_INIT_FAIL;
    }
    return EXT_ERR_SUCCESS;
}

static void lcd_lock_deinit(void)
{
    if (g_lcd_info.lcd_mutex.mutex != NULL) {
        osal_mutex_destroy(&(g_lcd_info.lcd_mutex));
    }

    g_lcd_info.lcd_mutex.mutex = NULL;
}

static void lcd_lock(void)
{
    if (g_lcd_info.lcd_mutex.mutex == NULL) {
        return;
    }
    osal_mutex_lock(&(g_lcd_info.lcd_mutex));
}

static void lcd_unlock(void)
{
    if (g_lcd_info.lcd_mutex.mutex == NULL) {
        return;
    }
    osal_mutex_unlock(&(g_lcd_info.lcd_mutex));
}

static void lcd_set_display_info(void)
{
    if (g_lcd_info.bus_api->bus_register_panel_info != NULL) {
        g_lcd_info.bus_api->bus_register_panel_info(g_lcd_info.lcd_cfg->x_start, g_lcd_info.lcd_cfg->y_start,
            g_lcd_info.lcd_cfg->fps);
    }
}

static ext_errno lcd_auto_adapt_init_prepare(void)
{
    ext_errno ret;
    combo_dev_cfg_t *escape_config = lcd_get_bus_escape_config();
    /* init with default parameter */
    ret = g_lcd_info.bus_api->bus_init(escape_config);
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("default bus_init fail, ret = %X\r\n", ret);
        return ret;
    }

    ret = g_lcd_info.common_api->get_chip_id(&g_lcd_info.lcd_id);
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("get_chip_id fail, ret = %X\r\n", ret);
    }

    if (g_lcd_info.bus_api->bus_deinit() != EXT_ERR_SUCCESS) {
        lcd_err("bus deinit fail\r\n");
    }

    return ret;
}

static ext_errno lcd_assigned_id_check(void)
{
    if (g_lcd_info.common_api->chip_id_check == NULL) {
        return EXT_ERR_LCD_INVALID_NULL_POINTER;
    }

    return g_lcd_info.common_api->chip_id_check();
}

static void lcd_register_bus_api(void)
{
#ifdef SUPPORT_GPU_QSPI
    uapi_lcd_register_bus(BUS_DISPLAY_QSPI);
#else
    uapi_lcd_register_bus(BUS_DISPLAY_MIPI);
#endif
}

ext_errno uapi_lcd_bus_init(combo_dev_cfg_t *bus_cfg)
{
    if (g_lcd_info.is_inited) {
        return EXT_ERR_SUCCESS;
    }

    lcd_register_bus_api();
    g_lcd_info.common_api = lcd_drv_get_common_api();
    g_lcd_info.bus_api = lcd_get_bus_api();
    if (g_lcd_info.bus_api == NULL) {
        return EXT_ERR_LCD_GET_ATTR_FAIL;
    }

    ext_errno ret = g_lcd_info.bus_api->bus_init(bus_cfg);
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("lcd bus_init fail, ret = %X\r\n", ret);
        return ret;
    }

    ret = lcd_lock_init();
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("lcd_lock_init fail, ret = %X\r\n", ret);
        g_lcd_info.bus_api->bus_deinit();
        return ret;
    }

    g_lcd_info.is_inited = true;

    if (g_lcd_info.info_register) {
        ret = lcd_assigned_id_check();
        if (ret != EXT_ERR_SUCCESS) {
            lcd_err("lcd chip id check error, ret = 0x%x\r\n", ret);
            g_lcd_info.bus_api->bus_deinit();
            return ret;
        }

        lcd_set_display_info();
    }

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_bus_deinit(void)
{
    g_lcd_info.bus_api = lcd_get_bus_api();
    if (g_lcd_info.bus_api == NULL) {
        return EXT_ERR_LCD_GET_ATTR_FAIL;
    }

    ext_errno ret = g_lcd_info.bus_api->bus_deinit();
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("lcd bus_deinit fail, ret = %X\r\n", ret);
    }

    lcd_lock();
    g_lcd_info.status.disp_status = LCD_DISPLAY_OFF;
    g_lcd_info.is_inited = false;
    lcd_unlock();

    lcd_lock_deinit();

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_bus_enable(void)
{
    if (!g_lcd_info.is_inited) {
        return EXT_ERR_LCD_NOT_INIT;
    }

    g_lcd_info.bus_api = lcd_get_bus_api();
    if (g_lcd_info.bus_api == NULL) {
        return EXT_ERR_LCD_GET_ATTR_FAIL;
    }

    ext_errno ret = g_lcd_info.bus_api->bus_enable();
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("lcd bus_enable fail, ret = %X\r\n", ret);
        return ret;
    }

    lcd_lock();
    g_lcd_info.status.disp_status = LCD_NORMAL_DISPLAY;
    g_lcd_info.status.te_status = LCD_TE_ON;
    lcd_unlock();

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_bus_disable(void)
{
    if (!g_lcd_info.is_inited) {
        return EXT_ERR_LCD_NOT_INIT;
    }

    g_lcd_info.bus_api = lcd_get_bus_api();
    if (g_lcd_info.bus_api == NULL) {
        return EXT_ERR_LCD_GET_ATTR_FAIL;
    }

    ext_errno ret = g_lcd_info.bus_api->bus_disable();
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("lcd bus_disable fail, ret = %X\r\n", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

static ext_errno lcd_get_ops(void)
{
    ext_errno ret = 0;
    g_lcd_info.bus_api = lcd_get_bus_api();
    g_lcd_info.common_api = lcd_drv_get_common_api();
    g_lcd_info.private_api = lcd_drv_get_private_api();
    if (g_lcd_info.bus_api == NULL || g_lcd_info.common_api == NULL || g_lcd_info.private_api == NULL) {
        return EXT_ERR_LCD_GET_ATTR_FAIL;
    }

    if (g_lcd_info.private_api->power_on == NULL) {
        return EXT_ERR_LCD_GET_ATTR_FAIL;
    }

    g_lcd_info.private_api->power_on();

    if (!g_lcd_info.info_register) {
        ret = lcd_auto_adapt_init_prepare();
        if (ret != EXT_ERR_SUCCESS) {
            goto fail;
        }
    }

    g_lcd_info.lcd_cfg = g_lcd_info.common_api->get_lcd_config();
    if (g_lcd_info.lcd_cfg == NULL) {
        lcd_err("lcd_cfg empty!\r\n");
        goto fail;
    }

    ret = g_lcd_info.bus_api->bus_init(g_lcd_info.lcd_cfg->bus_cfg);
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("bus_init fail, ret = %X\r\n", ret);
        goto fail;
    }

    if (g_lcd_info.info_register) {
        ret = lcd_assigned_id_check();
        if (ret != EXT_ERR_SUCCESS) {
            lcd_err("chip id check error, ret = 0x%x\r\n", ret);
            g_lcd_info.bus_api->bus_deinit();
            goto fail;
        }
    }

    g_lcd_info.info_register = TD_TRUE;
    g_lcd_info.brightness = DEFAULT_BRIGHTNESS;

    return EXT_ERR_SUCCESS;
fail:

    if (g_lcd_info.private_api->power_off != NULL) {
        g_lcd_info.private_api->power_off();
    }
    g_lcd_info.bus_api = NULL;
    g_lcd_info.common_api = NULL;
    g_lcd_info.private_api = NULL;
    return ret;
}

ext_lcd_status uapi_lcd_get_status(void)
{
    return g_lcd_info.status;
}

bool uapi_is_lcd_connect(void)
{
    return g_lcd_info.is_inited;
}

static ext_errno lcd_display_on(void)
{
    ext_errno ret;
    uint32_t retry_count = 3;
    lcd_lock();

    for (; retry_count > 0; retry_count--) {
        uapi_lcd_force_exit_ulps_internal();
        ret = g_lcd_info.common_api->display_on(g_lcd_info.brightness);
        uapi_lcd_clear_exit_ulps_internal();
        if (ret == EXT_ERR_SUCCESS) {
            break;
        }

        g_lcd_info.private_api->power_off();
        g_lcd_info.private_api->power_on();

        g_lcd_info.bus_api->bus_deinit();
        ret = g_lcd_info.bus_api->bus_init(g_lcd_info.lcd_cfg->bus_cfg);
        if (ret != EXT_ERR_SUCCESS) {
            g_lcd_info.private_api->power_off();
            continue;
        }
    }
    if (retry_count > 0) {
        g_lcd_info.status.disp_status = LCD_NORMAL_DISPLAY;
        g_lcd_info.status.te_status = LCD_TE_ON;
    }

    lcd_unlock();

    return ret;
}

ext_errno lcd_set_mode(uint32_t mode)
{
    if (g_lcd_info.bus_api->bus_set_mode != NULL) {
        g_lcd_info.bus_api->bus_set_mode(mode);
    }
    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_display_on(void)
{
    ext_errno ret;
    if (!g_lcd_info.is_inited) {
        return EXT_ERR_LCD_NOT_INIT;
    }
    if (g_lcd_info.status.disp_status == LCD_NORMAL_DISPLAY) {
        return EXT_ERR_SUCCESS;
    }

    g_lcd_info.private_api->power_on();

#ifdef GPU_SUPPORT_VIDEO_MODE
    // mipi write must be in cmd mode
    lcd_set_mode(1); // 1 is cmd mode
#endif
    ret = lcd_display_on();
#ifdef GPU_SUPPORT_VIDEO_MODE
    lcd_set_mode(0); // 0 is video mode
#endif
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("display on fail, ret = %X\r\n", ret);
    }
    return ret;
}

ext_errno uapi_lcd_display_off(void)
{
    ext_errno ret;
    if (!g_lcd_info.is_inited) {
        return EXT_ERR_LCD_NOT_INIT;
    }
    if (g_lcd_info.status.disp_status == LCD_DISPLAY_OFF) {
        return EXT_ERR_SUCCESS;
    }
    lcd_lock();
    ret = g_lcd_info.common_api->display_off();
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("display_off fail, ret = %X\r\n", ret);
        lcd_unlock();
        return ret;
    }

    g_lcd_info.private_api->power_off();

    g_lcd_info.status.disp_status = LCD_DISPLAY_OFF;
    lcd_unlock();
    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_enter_sleep_mode(void)
{
    ext_errno ret;
    if (!g_lcd_info.is_inited) {
        return EXT_ERR_LCD_NOT_INIT;
    }
    if (g_lcd_info.status.disp_status == LCD_SLEEP_MODE) {
        return EXT_ERR_SUCCESS;
    } else if (g_lcd_info.status.disp_status == LCD_DISPLAY_OFF) {
        return EXT_ERR_LCD_NOT_SUPPORT_OFF_TO_SLEEP;
    }

    if (g_lcd_info.status.disp_status == LCD_IDLE_MODE) {
        g_idle_before_sleep = true;
    } else {
        g_idle_before_sleep = false;
    }
    lcd_lock();
    ret = g_lcd_info.common_api->enter_sleep_mode();
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("enter sleep mode fail, ret = %X\r\n", ret);
        lcd_unlock();
        return ret;
    }
    ret = g_lcd_info.bus_api->bus_disable();
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("bus_disable fail, ret = %X\r\n", ret);
        lcd_unlock();
        return ret;
    }
    g_lcd_info.status.disp_status = LCD_SLEEP_MODE;
    lcd_unlock();
    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_exit_sleep_mode(void)
{
    ext_errno ret;
    if (!g_lcd_info.is_inited) {
        return EXT_ERR_LCD_NOT_INIT;
    }
    if (g_lcd_info.status.disp_status != LCD_SLEEP_MODE) {
        return EXT_ERR_LCD_NOT_IN_SLEEP_MODE;
    }

    ret = g_lcd_info.common_api->exit_sleep_mode();
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("exit sleep mode fail, ret = %X\r\n", ret);
        return ret;
    }

    lcd_lock();
    ret = g_lcd_info.bus_api->bus_enable();
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("enable bus fail when exit sleep, ret = %X\r\n", ret);
        lcd_unlock();
        return ret;
    }

    if (g_idle_before_sleep) {
        g_lcd_info.status.disp_status = LCD_IDLE_MODE;
    } else {
        g_lcd_info.status.disp_status = LCD_NORMAL_DISPLAY;
    }

    lcd_unlock();

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_enter_idle_mode(void)
{
    ext_errno ret;
    if (!g_lcd_info.is_inited) {
        return EXT_ERR_LCD_NOT_INIT;
    }
    if (g_lcd_info.status.disp_status == LCD_IDLE_MODE) {
        return EXT_ERR_SUCCESS;
    }
    if (g_lcd_info.status.disp_status != LCD_NORMAL_DISPLAY) {
        return EXT_ERR_LCD_NOT_SUPPORT_CUR_TO_IDLE;
    }

    lcd_lock();
    ret = g_lcd_info.common_api->enter_idle_mode();
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("enter_idle_mode fail, ret = %X\r\n", ret);
        lcd_unlock();
        return ret;
    }
    g_lcd_info.status.disp_status = LCD_IDLE_MODE;
    lcd_unlock();

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_exit_idle_mode(void)
{
    ext_errno ret;
    if (!g_lcd_info.is_inited) {
        return EXT_ERR_LCD_NOT_INIT;
    }
    if (g_lcd_info.status.disp_status != LCD_IDLE_MODE) {
        return EXT_ERR_LCD_NOT_IN_IDLE_MODE;
    }

    lcd_lock();
    ret = g_lcd_info.common_api->exit_idle_mode();
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("exit_idle_mode fail, ret = %X\r\n", ret);
        lcd_unlock();
        return ret;
    }
    g_lcd_info.status.disp_status = LCD_NORMAL_DISPLAY;
    lcd_unlock();

    return EXT_ERR_SUCCESS;
}

static ext_errno lcd_te_param_check(void)
{
    if (!g_lcd_info.is_inited) {
        return EXT_ERR_LCD_NOT_INIT;
    }
    if (g_lcd_info.screen_type == VIDEO_LCD) {
        return EXT_ERR_LCD_VIDEO_SCREEN_NOT_SUPPORT_TE;
    }
    if (g_lcd_info.status.disp_status != LCD_NORMAL_DISPLAY && g_lcd_info.status.disp_status != LCD_IDLE_MODE) {
        return EXT_ERR_LCD_INVALID_TE_ON_MODE;
    }

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_te_on(void)
{
    ext_errno ret = lcd_te_param_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }
    if (g_lcd_info.status.te_status == LCD_TE_ON) {
        return EXT_ERR_SUCCESS;
    }

    lcd_lock();
    ret = g_lcd_info.common_api->te_on();
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("te_on fail, ret = %X\r\n", ret);
        lcd_unlock();
        return ret;
    }
    g_lcd_info.status.te_status = LCD_TE_ON;
    lcd_unlock();

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_te_off(void)
{
    ext_errno ret = lcd_te_param_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }
    if (g_lcd_info.status.te_status == LCD_TE_OFF) {
        return EXT_ERR_SUCCESS;
    }

    lcd_lock();
    ret = g_lcd_info.common_api->te_off();
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("te_off fail, ret = %X\r\n", ret);
        lcd_unlock();
        return ret;
    }
    g_lcd_info.status.te_status = LCD_TE_OFF;
    lcd_unlock();

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_set_brightness(uint16_t value)
{
    ext_errno ret;
    if (!g_lcd_info.is_inited) {
        return EXT_ERR_LCD_NOT_INIT;
    }
    if (g_lcd_info.status.disp_status > LCD_IDLE_MODE) {
        return EXT_ERR_LCD_IN_DISPLAY_OFF_STATE;
    }

    lcd_lock();
    ret = g_lcd_info.common_api->set_brightness(value);
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("set_brightness fail, ret = %X\r\n", ret);
        lcd_unlock();
        return ret;
    }
    g_lcd_info.brightness = value;
    g_lcd_info.status.bright_status = (value > LCD_NORMAL_BRIGHT_MAX) ? LCD_BRIGHT_HBM : LCD_BRIGHT_NORMAL;
    lcd_unlock();

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_get_brightness(uint16_t *value)
{
    if (!g_lcd_info.is_inited) {
        return EXT_ERR_LCD_NOT_INIT;
    }
    *value = g_lcd_info.brightness;
    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_write_reg(lcd_cmd_sequ *cmds, uint32_t cmd_count)
{
    ext_errno ret;
    if (!g_lcd_info.is_inited) {
        return EXT_ERR_LCD_NOT_INIT;
    }

    if (cmds == NULL || cmd_count == 0) {
        return EXT_ERR_LCD_INVALID_WRITE_CMD;
    }
    lcd_lock();
    ret = g_lcd_info.common_api->write(cmds, cmd_count);
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("lcd write reg fail, ret = %X\r\n", ret);
        lcd_unlock();
        return ret;
    }
    lcd_unlock();

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_read_reg(uint8_t cmd, uint8_t *buff, uint8_t buff_len)
{
    ext_errno ret;
    if (!g_lcd_info.is_inited) {
        return EXT_ERR_LCD_NOT_INIT;
    }
    if (g_lcd_info.status.disp_status > LCD_SLEEP_MODE) {
        return EXT_ERR_LCD_IN_DISPLAY_OFF_STATE;
    }
    if (buff == NULL || buff_len == 0) {
        return EXT_ERR_LCD_INVALID_READ_CMD;
    }
    lcd_lock();
    ret = g_lcd_info.common_api->read(cmd, buff, buff_len);
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("lcd read reg fail, ret = %X\r\n", ret);
        lcd_unlock();
        return ret;
    }
    lcd_unlock();

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_init(void)
{
    if (g_lcd_info.is_inited) {
        return EXT_ERR_SUCCESS;
    }
    lcd_register_bus_api();
    ext_errno ret = lcd_get_ops();
    if (ret != EXT_ERR_SUCCESS && ret != EXT_ERR_MIPI_ALREADY_INIT) {
        lcd_err("lcd_get_ops fail, ret = %X\r\n", ret);
        return ret;
    }

    ret = lcd_lock_init();
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("lcd_lock_init fail, ret = %X\r\n", ret);
        return ret;
    }

    lcd_set_display_info();

    ret = lcd_display_on();
    if (ret != EXT_ERR_SUCCESS) {
        g_lcd_info.bus_api->bus_deinit();
        lcd_lock_deinit();
        lcd_err("lcd_display_on fail, ret = %X\r\n", ret);
        return ret;
    }

    ret = g_lcd_info.bus_api->bus_enable();
    if (ret != EXT_ERR_SUCCESS) {
        g_lcd_info.bus_api->bus_deinit();
        lcd_lock_deinit();
        lcd_err("lcd_bus_enable fail, ret = %X\r\n", ret);
        return ret;
    }

    g_lcd_info.is_inited = true;

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_deinit(void)
{
    ext_errno ret;
    if (!g_lcd_info.is_inited) {
        return EXT_ERR_SUCCESS;
    }

    if (g_lcd_info.status.disp_status != LCD_DISPLAY_OFF) {
        ret = g_lcd_info.common_api->display_off();
        if (ret != EXT_ERR_SUCCESS) {
            lcd_err("display_off fail, ret = %X\r\n", ret);
        }

        ret = g_lcd_info.bus_api->bus_disable();
        if (ret != EXT_ERR_SUCCESS) {
            lcd_err("bus_disable fail, ret = %X\r\n", ret);
        }
    }
    lcd_lock();
    ret = g_lcd_info.bus_api->bus_deinit();
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("bus_deinit fail, ret = %X\r\n", ret);
    }
    g_lcd_info.private_api->power_off();

    g_lcd_info.status.disp_status = LCD_DISPLAY_OFF;
    g_lcd_info.is_inited = false;
    lcd_unlock();

    lcd_lock_deinit();

    return EXT_ERR_SUCCESS;
}

PLT_SEC_ITCM_T static INLINE ext_errno lcd_bus_api_check(void)
{
    if (!g_lcd_info.is_inited) {
        return EXT_ERR_LCD_NOT_INIT;
    }

    if (g_lcd_info.bus_api == NULL) {
        return EXT_ERR_LCD_GET_ATTR_FAIL;
    }

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_suspend(void)
{
    ext_errno ret = lcd_bus_api_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    return g_lcd_info.bus_api->bus_suspend();
}

PLT_SEC_ITCM_T ext_errno uapi_lcd_resume(void)
{
    ext_errno ret = lcd_bus_api_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    return g_lcd_info.bus_api->bus_resume();
}

static ext_errno lcd_lp_ctrl_check(void)
{
    ext_errno ret = lcd_bus_api_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    if (g_lcd_info.bus_api->bus_lp_ctrl == NULL) {
        return EXT_ERR_LCD_NOT_SUPPORT_FUNC;
    }

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_enter_sdlp(void)
{
    ext_errno ret;

    ret = lcd_lp_ctrl_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    return g_lcd_info.bus_api->bus_lp_ctrl(LP_CTRL_ENTER_SDLP, TD_NULL);
}

ext_errno uapi_lcd_exit_sdlp(void)
{
    ext_errno ret;

    ret = lcd_lp_ctrl_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    return g_lcd_info.bus_api->bus_lp_ctrl(LP_CTRL_EXIT_SDLP, TD_NULL);
}

ext_errno uapi_lcd_exit_ulps_internal(void)
{
    ext_errno ret = lcd_lp_ctrl_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    lcd_bus_ctrl_opt opt = LCD_CTRL_INTERNAL;
    return g_lcd_info.bus_api->bus_lp_ctrl(LP_CTRL_EXIT_ULPS, &opt);
}

ext_errno uapi_lcd_exit_ulps(void)
{
    ext_errno ret;

    ret = lcd_lp_ctrl_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    lcd_bus_ctrl_opt opt = LCD_CTRL_EXTERNAL;
    return g_lcd_info.bus_api->bus_lp_ctrl(LP_CTRL_EXIT_ULPS, &opt);
}

ext_errno uapi_lcd_clear_exit_ulps_internal(void)
{
    ext_errno ret;

    ret = lcd_lp_ctrl_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    lcd_bus_ctrl_opt opt = LCD_CTRL_INTERNAL;
    return g_lcd_info.bus_api->bus_lp_ctrl(LP_CTRL_CLEAR_EXIT_ULPS, &opt);
}

ext_errno uapi_lcd_clear_exit_ulps(void)
{
    ext_errno ret;

    ret = lcd_lp_ctrl_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    lcd_bus_ctrl_opt opt = LCD_CTRL_EXTERNAL;
    return g_lcd_info.bus_api->bus_lp_ctrl(LP_CTRL_CLEAR_EXIT_ULPS, &opt);
}

ext_errno uapi_lcd_set_auto_exit_ulps_en(bool enable)
{
    ext_errno ret = lcd_lp_ctrl_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    lcd_bus_lp_ctrl_t mode = (enable == TD_TRUE) ? LP_CTRL_AUTO_EXIT_ULPS_ENABLE : LP_CTRL_AUTO_EXIT_ULPS_DISABLE;
    return g_lcd_info.bus_api->bus_lp_ctrl(mode, TD_NULL);
}

ext_errno uapi_lcd_force_exit_ulps_internal(void)
{
    ext_errno ret = lcd_lp_ctrl_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    lcd_bus_ctrl_opt opt = LCD_CTRL_INTERNAL;
    return g_lcd_info.bus_api->bus_lp_ctrl(LP_CTRL_FORCE_EXIT_ULPS, &opt);
}

ext_errno uapi_lcd_force_exit_ulps(void)
{
    ext_errno ret = lcd_lp_ctrl_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    lcd_bus_ctrl_opt opt = LCD_CTRL_EXTERNAL;
    return g_lcd_info.bus_api->bus_lp_ctrl(LP_CTRL_FORCE_EXIT_ULPS, &opt);
}

ext_errno uapi_lcd_ulps_proc(void)
{
    ext_errno ret = lcd_bus_api_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    if (g_lcd_info.bus_api->bus_proc == NULL) {
        return EXT_ERR_LCD_NOT_SUPPORT_FUNC;
    }

    return g_lcd_info.bus_api->bus_proc(LCD_PROC_ULPS, TD_NULL);
}

ext_errno uapi_lcd_dfx_proc_read(void)
{
    ext_errno ret = lcd_bus_api_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    if (g_lcd_info.bus_api->bus_proc == NULL) {
        return EXT_ERR_LCD_NOT_SUPPORT_FUNC;
    }

    return g_lcd_info.bus_api->bus_proc(LCD_PROC_READ, TD_NULL);
}

STATIC void lcd_get_mcu_int_sub_cycle_num(uint32_t cycle_num, uint32_t *hard_cycle_num, uint32_t *soft_cycle_num)
{
    uint32_t max_hard_cycle_num = MIN(cycle_num, MCU_INT_HARD_MAX_CYCLE_NUM);
    for (uint32_t i = max_hard_cycle_num; i >= 1; i--) {
        if (cycle_num % i == 0) {
            *hard_cycle_num = i;
            *soft_cycle_num = cycle_num / i;
            return;
        }
    }
}

ext_errno uapi_lcd_set_mcu_int_cycle_num(uint32_t cycle_num)
{
    if (cycle_num == 0) {
        lcd_mcu_int_isr_deinit();
        return EXT_ERR_SUCCESS;
    }

    uint32_t hard_cycle_num = cycle_num;
    uint32_t soft_cycle_num = 1;
    lcd_get_mcu_int_sub_cycle_num(cycle_num, &hard_cycle_num, &soft_cycle_num);
    lcd_set_mcu_int_soft_cycle_num(soft_cycle_num);
    lcd_dfx_set_mcu_int_hard_cycle_num(hard_cycle_num);
    lcd_hal_set_mcu_te_int_cnt_val(hard_cycle_num - 1);

    ext_errno ret = lcd_mcu_int_isr_init();
    if (ret != EXT_ERR_SUCCESS) {
        lcd_err("lcd_mcu_int_isr_init fail, ret=%#x!", ret);
    }

    return ret;
}

ext_errno uapi_lcd_register_mcu_int_callback(mcu_int_callback callback)
{
    lcd_register_mcu_int_callback(callback);

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_detect_pld_errs(bool *is_err)
{
    ext_errno ret = lcd_bus_api_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    if (g_lcd_info.bus_api->bus_proc == NULL) {
        return EXT_ERR_LCD_NOT_SUPPORT_FUNC;
    }

    if (is_err == NULL) {
        return EXT_ERR_LCD_INVALID_NULL_POINTER;
    }

    return g_lcd_info.bus_api->bus_proc(LCD_PROC_PLD_ERR, is_err);
}

void uapi_lcd_register_bus(lcd_bus_type_t bus_type)
{
    if (bus_type == BUS_DISPLAY_MIPI) {
        lcd_register_mipi_bus();
    } else {
        lcd_register_qspi_bus();
    }
}

static ext_errno lcd_info_check(const lcd_drv_cfg *info)
{
    if (info == NULL) {
        return EXT_ERR_LCD_INVALID_NULL_POINTER;
    }

    if ((info->bus_type != BUS_DISPLAY_MIPI) && (info->bus_type != BUS_DISPLAY_QSPI) &&
        (info->bus_type != BUS_DISPLAY_SPI)) {
        return EXT_ERR_LCD_INVALID_SCREEN_OPS;
    }

    if (info->lcd_id == 0) {
        return EXT_ERR_LCD_INVALID_CHIP_ID;
    }

    if (info->lcd_id_info.read_len == 0) {
        return EXT_ERR_LCD_INVALID_READ_CMD;
    }

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_lcd_info_register(lcd_drv_cfg *info)
{
    ext_errno ret;

    ret = lcd_info_check(info);
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    if (g_lcd_info.is_inited) {
        return EXT_ERR_LCD_INIT_ALREADY;
    }

    if (g_lcd_info.common_api == NULL) {
        g_lcd_info.common_api = lcd_drv_get_common_api();
    }

    if (g_lcd_info.common_api->set_lcd_config == NULL) {
        return EXT_ERR_LCD_NOT_SUPPORT_FUNC;
    }

    ret = g_lcd_info.common_api->set_lcd_config(info);
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    g_lcd_info.lcd_cfg = info;

    if (info->bus_type == BUS_DISPLAY_MIPI) {
        lcd_register_mipi_bus();
    } else {
        lcd_register_qspi_bus();
    }

    g_lcd_info.info_register = true;

    return EXT_ERR_SUCCESS;
}
ext_errno uapi_lcd_frame_done_process(void)
{
    ext_errno ret = lcd_bus_api_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }
    if (g_lcd_info.bus_api->bus_frame_done == NULL) {
        return EXT_ERR_LCD_NOT_SUPPORT_FUNC;
    }
    return g_lcd_info.bus_api->bus_frame_done();
}

ext_errno uapi_lcd_ulps_entry_suspend_internal(bool enable)
{
    ext_errno ret = lcd_lp_ctrl_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    lcd_bus_lp_ctrl_t mode = (enable == TD_TRUE) ? LP_CTRL_ULPS_ENTRY_SUSPEND_ENABLE :
        LP_CTRL_ULPS_ENTRY_SUSPEND_DISABLE;
    return g_lcd_info.bus_api->bus_lp_ctrl(mode, TD_NULL);
}

ext_errno uapi_lcd_ulps_entry_suspend(bool enable)
{
    ext_errno ret = lcd_lp_ctrl_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }
    g_ulps_entry_suspend_state = enable;
    lcd_bus_lp_ctrl_t mode = (enable == TD_TRUE) ? LP_CTRL_ULPS_ENTRY_SUSPEND_ENABLE :
        LP_CTRL_ULPS_ENTRY_SUSPEND_DISABLE;
    return g_lcd_info.bus_api->bus_lp_ctrl(mode, TD_NULL);
}

bool uapi_lcd_get_ulps_entry_suspend_state(void)
{
    return g_ulps_entry_suspend_state;
}

bool uapi_lcd_get_refresh_flag(void)
{
    return g_lcd_is_refresh;
}

void uapi_lcd_set_refresh_flag(bool refresh)
{
    uint32_t irq_sts = osal_irq_lock();
    g_lcd_is_refresh = refresh;
    osal_irq_restore(irq_sts);
}