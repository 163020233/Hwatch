/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: touch screen driver.
 * Create: 2025-11-11
 */

#include "input_feature.h"
#include "tcxo.h"
#include "i2c.h"
#include "common_def.h"
#include "soc_osal.h"
#include "touch_ctrl.h"
#include "touch_feature.h"
#include "touch_def.h"
#include "touch_ctrl.h"
#include "gpio.h"
#include "pm_veto.h"

#define TOUCH_SCREEN_DEV_NAME_LEN 16
#define TOUCH_SCREEN_DEVNAME "input/event"
#define TOUCH_SCREEN_DYNAMIC_MINOR 65
#define TOUCH_SCREEN_CMDID_MAX 16

#define TOUCH_SCREEN_TASK_PRIO 5
#define TOUCH_SCREEN_TASK_SIZE 0x800
#define TOUCH_SCREEN_QUEUE_MAX_SIZE 60
#define TOUCH_SCREEN_READY_EVENT (1 << 0)
#define TOUCH_VETO_SLEEP_MS 16

void *touch_screen_get_api(void);

struct ts_driver_data g_touch_drv = { 0 };
uint32_t g_ts_initflag = 0;
struct ts_driver_data *g_attr = NULL;
static osal_ioctl_cmd g_ts_cmd_list[TOUCH_SCREEN_CMDID_MAX] = {
    {TOUCH_SCREEN_GET_EVENT, ts_ioctl_read_touch_info},
};
input_report_event_cb g_ts_event_cb[APP_SERVICE_MAX] = {NULL};
report_event_cb g_report_event_cb = NULL;

uint32_t touch_register_report_event_cb(app_notify_service_t service_type, input_report_event_cb callback)
{
    if (callback == NULL || service_type >= APP_SERVICE_MAX) {
        return ERRCODE_FAIL;
    }
    g_ts_event_cb[service_type] = callback;
    return ERRCODE_SUCC;
}

uint32_t touch_unregister_report_event_cb(app_notify_service_t service_type)
{
    if (service_type >= APP_SERVICE_MAX) {
        return ERRCODE_FAIL;
    }
    g_ts_event_cb[service_type] = NULL;
    return ERRCODE_SUCC;
}

static inline void touch_event_cb(app_notify_service_t service, input_dev_report_data_t *data)
{
    if (g_ts_event_cb[service] == NULL) {
        return;
    }
    g_ts_event_cb[service](data);
}

static uint32_t touch_screen_get_state(struct ts_driver_data *tsdd)
{
    if (tsdd != NULL && tsdd->queue != NULL) {
        return (uint32_t)ts_queue_is_empty(tsdd->queue);
    }

    if (tsdd == NULL) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "tsdd is null!");
        return EXT_ERR_FAILURE;
    }
    if (tsdd->queue == NULL) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "tsdd queue is null!");
    }

    return EXT_ERR_FAILURE;
}

struct ts_ops g_ts_status_ops = {
    .priv_operator = touch_screen_get_state,
};

static struct ts_driver_data *touch_screen_get_driver_data(void)
{
    return &g_touch_drv;
}

static void convert_touch_info(input_dev_report_data_t *data, input_event_info_t *touch_info)
{
    data->x = (int16_t)touch_info->tp_event_info.x_axis[0];
    data->y = (int16_t)touch_info->tp_event_info.y_axis[0];
    data->type = APP_TOUCH;
    data->timestamp = (uint32_t)touch_info->tv_usec;
    if ((touch_info->tp_event == MC_TP_PRESS) || (touch_info->tp_event == MC_TP_MOVE)) {
        data->state = INDEV_STATE_PRESS;
    } else if (touch_info->tp_event == MC_TP_RELEASE) {
        data->state = INDEV_STATE_RELEASE;
    } else {
        data->state = INDEV_STATE_UNKNOWN;
    }
}

static void report_or_enqueue_event(input_event_info_t *touch_info, struct ts_driver_data *tsdd)
{
    if (g_report_event_cb != NULL) {
        input_dev_report_data_t data;
        convert_touch_info(&data, touch_info);
        if (data.state == INDEV_STATE_UNKNOWN) {
            return;
        }
        g_report_event_cb((InputDevData *)&data);
        data.state = touch_info->tp_event;
        touch_event_cb(APP_PMS, &data);
    } else {
        int32_t ret = ts_en_queue(tsdd->queue, touch_info);
        if (ret != EXT_ERR_SUCCESS) {
            INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "Failed to enqueue ts queue! ret=0x%x", ret);
        }
    }
}
ext_errno touch_info_process(void)
{
    struct ts_driver_data *tsdd = touch_screen_get_driver_data();
    input_event_info_t touch_info;
    int32_t ret = 0;

    if (tsdd->touch_api->touch_get_tpinfo != NULL) {
        ret = tsdd->touch_api->touch_get_tpinfo((uint8_t *)(&touch_info), sizeof(input_event_info_t));
        if (ret != EXT_ERR_SUCCESS) {
            INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "get touch info fail! ret=0x%x", ret);
            return ret;
        }
        report_or_enqueue_event(&touch_info, tsdd);
    }

    return EXT_ERR_SUCCESS;
}

static void ts_callback_func(void)
{
    ext_errno ret;
    if (g_attr != NULL) {
        common_notify_mail_t mail = { 0 };
        mail.comm.app = APP_TOUCH;
        ret = osal_msg_queue_write_copy(get_app_queue_id(), (void *)&mail, sizeof(common_notify_mail_t), 0);
        if (ret != OSAL_SUCCESS) {
            INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "touch app msg send failed! err=0x%x! \r\n", ret);
        }
    }
}

static ext_errno ts_register_callback(struct ts_driver_data *attr)
{
    ext_errno ret = EXT_ERR_FAILURE;
    g_attr = attr;

    if (attr->touch_api->register_callback != NULL) {
        ret = attr->touch_api->register_callback(attr->private_data, ts_callback_func);
        if (ret != EXT_ERR_SUCCESS) {
            INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "register touch screen callback fail! ret=0x%x", ret);
            return ret;
        }
    } else {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "touch screen callback register func is empty!");
    }

    return ret;
}

static ext_errno ts_unregister_callback(struct ts_driver_data *attr)
{
    ext_errno ret = EXT_ERR_FAILURE;

    if (attr->touch_api->unregister_callback != NULL) {
        ret = attr->touch_api->unregister_callback(attr->private_data);
        if (ret != EXT_ERR_SUCCESS) {
            INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "unregister touch screen callback fail! ret=0x%x", ret);
            return ret;
        }
        g_attr = NULL;
    } else {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "touch screen callback unregister func is empty!");
    }

    return ret;
}

int32_t ts_ioctl_read_touch_info(uint32_t cmd, void *arg, void *private_data)
{
    unused(cmd);
    ext_errno ret;
    struct ts_driver_data *tsdd = NULL;
    input_event_info_t *touch_info = (input_event_info_t *)arg;

    if (private_data == NULL) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "invalid dev or driver_data!");
        return EXT_ERR_FAILURE;
    }

    // todo: get tsdd from private_data
    tsdd = touch_screen_get_driver_data();
    if (tsdd->queue == NULL) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "invalid touch screen queue!");
        return EXT_ERR_FAILURE;
    }

    ret = ts_de_queue(tsdd->queue, touch_info);
    if (ret != EXT_ERR_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "get touch info fail! ret=0x%x", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

uint32_t ts_register_report_event_cb(report_event_cb callback)
{
    if (g_report_event_cb != NULL) {
        return EXT_ERR_FAILURE;
    }
    g_report_event_cb = callback;
    return EXT_ERR_SUCCESS;
}

uint32_t ts_unregister_report_event_cb(void)
{
    g_report_event_cb = NULL;
    return EXT_ERR_SUCCESS;
}

uint32_t ts_init(void)
{
    int32_t ret;
    struct ts_driver_data *tsdd = NULL;
    if (g_ts_initflag == 1) {
        return EXT_ERR_SUCCESS;
    }
    g_touch_drv.private_data = (void *)touch_screen_get_peri_attr();
    g_touch_drv.touch_api = (touch_peripheral_api *)touch_screen_get_api();
    g_touch_drv.ops = &g_ts_status_ops;
    tsdd = touch_screen_get_driver_data();
    tsdd->queue = ts_init_queue(TOUCH_SCREEN_QUEUE_MAX_SIZE);
    if (tsdd->queue == NULL) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "create touch screen queue fail!\r\n");
        return EXT_ERR_FAILURE;
    }

    ret = ts_register_callback(tsdd);
    if (ret != EXT_ERR_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "register callback fail! ret=0x%x\r\n", ret);
        return EXT_ERR_FAILURE;
    }
    g_ts_initflag = 1;
    return EXT_ERR_SUCCESS;
}

uint32_t ts_read(const input_dev_report_data_t *data)
{
    unused(data);
    return EXT_ERR_SUCCESS;
}

uint32_t ts_close(void)
{
    return EXT_ERR_SUCCESS;
}

void touch_screen_suspend(void)
{
    touch_peripheral_api *touch_ptr = (touch_peripheral_api *)touch_screen_get_api();
    ext_errno ret = touch_ptr->touch_sleep();
    if (ret != EXT_ERR_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "touch sleep ret=0x%x", ret);
    }
    uapi_gpio_disable_interrupt(TOUCH_INT_GPIO); // close tp_int gpio interrupt
}

void touch_screen_resume(void)
{
    uapi_gpio_disable_interrupt(TOUCH_INT_GPIO); // close tp_int gpio interrupt
    touch_peripheral_api *touch_ptr = (touch_peripheral_api *)touch_screen_get_api();
    ext_errno ret = touch_ptr->touch_resume();
    if (ret != EXT_ERR_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "touch resume ret=0x%x", ret);
    }
    uapi_gpio_enable_interrupt(TOUCH_INT_GPIO); // open tp_int gpio interrupt
}

void touch_screen_restart(void)
{
    uapi_i2c_deinit(TOUCH_I2C_BUS);
    uapi_i2c_master_init(TOUCH_I2C_BUS, 400000, 0); /* 400000: 400KHz */
    touch_peripheral_api *touch_ptr = (touch_peripheral_api *)touch_screen_get_api();
    ext_errno ret = touch_ptr->touch_resume();
    uapi_gpio_enable_interrupt(TOUCH_INT_GPIO); // open tp_int gpio interrupt
}

static uint32_t touch_set_work_mode(input_dev_workmode_t mode)
{
    if (mode == INPUT_DEV_SLEEP_MODE) {
        touch_screen_suspend();
    } else if (mode == INPUT_DEV_NORMAL_MODE) {
        touch_screen_resume();
    }
    return EXT_ERR_SUCCESS;
}

static void touch_feature_event_handler(common_notify_mail_t *mptr)
{
    if (mptr->comm.set_mode) {
        uapi_pm_add_sleep_veto(PM_ID_TOUCH);
        touch_set_work_mode(mptr->comm.event);
        uapi_pm_remove_sleep_veto(PM_ID_TOUCH);
    } else {
        uapi_pm_add_sleep_veto(PM_ID_TOUCH);
        touch_info_process();
        uapi_pm_remove_sleep_veto(PM_ID_TOUCH);
        uapi_pm_add_sleep_veto_with_timeout(PM_ID_TOUCH, TOUCH_VETO_SLEEP_MS);
    }
    return;
}

uint32_t touch_init(void)
{
    input_register_service_cb(APP_TOUCH, touch_feature_event_handler);
    (void)ts_init();
    touch_peripheral_api *touch_ptr = (touch_peripheral_api *)touch_screen_get_api();
    struct ts_driver_data *tsdd = NULL;

    tsdd = touch_screen_get_driver_data();

    return (uint32_t)touch_ptr->touch_init(tsdd->private_data);
}

uint32_t touch_deinit(void)
{
    return EXT_ERR_SUCCESS;
}


static input_dev_info_t g_button_feature_info = {
    .dev_name = "Touch",
    .dev_type = APP_TOUCH,
    .dev_ops = {
        .input_dev_init = touch_init,
        .input_dev_deinit = touch_deinit,
        .input_dev_register_service_cb = touch_register_report_event_cb,
        .input_dev_unregister_service_cb = touch_unregister_report_event_cb,
        .input_dev_set_workmode = touch_set_work_mode,
            },
};
