/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: header of touch screen driver.
 * Create: 2025-11-11
 */

#ifndef TOUCH_FEATURE_H
#define TOUCH_FEATURE_H

#include "soc_osal.h"
#include "touch_queue.h"

struct ts_driver_data {
    osal_task *task_handle;
    osal_event event;
    ts_queue_t *queue;
    osal_wait wait;
    struct ts_ops *ops;
    touch_peripheral_api *touch_api;
    void *private_data;
};

struct ts_ops {
    /* private operation */
    uint32_t (*priv_operator)(struct ts_driver_data *udd);
};

uint32_t touch_init(void);
ext_errno touch_screen_module_init(void);
ext_errno touch_screen_module_deinit(void);
ext_errno touch_info_process(void);
int32_t ts_ioctl_read_touch_info(uint32_t cmd, void *arg, void *private_data);
uint32_t touch_register_report_event_cb(app_notify_service_t service_type, input_report_event_cb callback);

#endif