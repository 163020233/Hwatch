/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: cst820 touch driver.
 * Create: 2025-11-11
 */

#include "touch_cst820_drv.h"
#include "touch_ctrl.h"
#include "debug_print.h"
#include "touch_cst820.h"
#include "pinctrl.h"
#include "pinctrl_porting.h"

static ext_errno cst820_drv_init(void *attr)
{
    unused(attr);
    ext_errno ret;
    touch_chip_info_t touch_chip_info;
    touch_ctrl_ops_t *ops = touch_screen_get_peri_attr();

    if (ops == NULL) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "peripheral ops is empty!");
        return EXT_ERR_FAILURE;
    }

    if (ops->bus_init == NULL) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "peripheral bus_init func is empty!");
        return EXT_ERR_FAILURE;
    }

    ret = ops->bus_init(&(ops->attr));
    if (ret != EXT_ERR_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "cst820 bus_init fail! ret = 0x%x", ret);
        return ret;
    }
    uapi_pin_set_ie(TOUCH_INT_GPIO, PIN_IE_ENABLE);
    ret = cst820_detect();
    if (ret != EXT_ERR_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "cst820 detect fail! ret = 0x%x", ret);
        return ret;
    }
    ret = cst820_init(&touch_chip_info);
    if (ret != EXT_ERR_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "cst820 init fail! ret = 0x%x", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

static ext_errno cst820_drv_deinit(void)
{
    return EXT_ERR_SUCCESS;
}

static ext_errno cst820_drv_get_info(uint8_t *data, uint8_t data_len)
{
    return cst820_irq_callback((input_event_info_t*)data, data_len);
}

static ext_errno cst820_drv_suspend(void)
{
    return cst820_set_power_mode(INPUT_DEV_STANDBY_MODE);
}

static ext_errno cst820_drv_resume(void)
{
    return cst820_set_power_mode(INPUT_DEV_NORMAL_MODE);
}

static ext_errno cst820_drv_sleep(void)
{
    return cst820_set_power_mode(INPUT_DEV_SLEEP_MODE);
}

touch_peripheral_api g_cst820_api = {
    .touch_init = cst820_drv_init,
    .touch_deinit = cst820_drv_deinit,
    .touch_get_tpinfo = cst820_drv_get_info,
    .touch_suspend = cst820_drv_suspend,
    .touch_resume = cst820_drv_resume,
    .touch_sleep = cst820_drv_sleep,
    .touch_bus_init = (ext_errno(*)(void *))touch_host_peripheral_init,
    .register_callback = (ext_errno(*)(void *, touch_callback))touch_register_handle,
    .unregister_callback = (ext_errno(*)(void *))touch_unregister_handle,
};

void *touch_screen_get_api(void)
{
    return ((void *)&g_cst820_api);
}
