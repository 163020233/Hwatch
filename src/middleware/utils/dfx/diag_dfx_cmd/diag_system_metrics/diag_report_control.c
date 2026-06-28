/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag report control
 * This file should be changed only infrequently and with great care.
 */

#include "exception.h"
#include "preserve.h"
#include "diag_filter.h"
#include "dfx_adapt_layer.h"
#include "diag_device_info.h"
#include "diag_adapt_layer.h"
#include "diag_filter.h"
#include "diag_report_control.h"
static bool g_report_control = false;   // 控制上报策略，设备注册后设置为true

bool diag_get_report_ctrl(void)
{
    return g_report_control;
}

void diag_set_report_ctrl(bool ctrl)
{
    dfx_log_info("[DIAG] diag_set_report_ctrl %d,origin: %d", ctrl, g_report_control);
    g_report_control = ctrl;
}

errcode_t diag_cmd_report_boot_info(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                    diag_option_t *option)
{
    unused(cmd_param);
    unused(cmd_param_size);
    unused(option);
    dfx_log_err("[DIAG] get_boot_info\n");
    errcode_t ret = ERRCODE_SUCC;

    ret = diag_device_info_report();
    if (ret == ERRCODE_SUCC) {
        ret = diag_crash_info_report();
    }

    diag_set_report_ctrl(true);
    uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ret, (uint16_t)sizeof(errcode_t), true);
    return ret;
}