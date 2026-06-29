/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux dut at service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-13， Create file. \n
 */

#include <string.h>
#include <stdlib.h>
#include "at.h"
#include "app_at_uart.h"
#include "debug_print.h"
#include "ux_api_power.h"
#include "ux_errcode.h"
#include "ux_system_manager.h"
#include "ux_at_dut.h"

#define AT_DUT_START "1"
#define AT_DUT_END   "0"
#define at_uart_printf PRINT /* 仅在此文件中重定向，后续3322引入at_dut后切换 */


typedef at_ret_t (*at_dut_cmd_handle_t)(const char *para1);

typedef struct {
    const char *cmd;
    at_dut_cmd_handle_t handle;
} at_dut_cmd_t;

/* AT^FTM=1 */
static at_ret_t dut_set_start(const char *para1)
{
    unused(para1);
    set_ftm_mode(STATUS_FTM_ENABLE);
    set_working_state(STATUS_ON_DUT);
    at_uart_printf("^FTM=1\n");
    return AT_RET_OK;
}

/* AT^FTM=0 */
static at_ret_t dut_set_end(const char *para1)
{
    unused(para1);
    set_ftm_mode(STATUS_FTM_DISABLE);
    ux_api_power_reset(UX_API_POWER_OFF_OTA, 0);
    at_uart_printf("^FTM=0\n");
    return AT_RET_OK;
}

/* AT^FTM? */
static at_ret_t dut_read_state(void)
{
    uint8_t mode = get_working_state();
    if (mode == STATUS_ON_DUT) {
        at_uart_printf("^FTM=1\n");
    } else {
        at_uart_printf("^FTM=0\n");
    }
    return AT_RET_OK;
}

at_ret_t at_dut_set_process(const ux_at_para1_args_t *args)
{
    if (args == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }

    static at_dut_cmd_t dut_cmd[] = {
        { AT_DUT_START, dut_set_start },
        { AT_DUT_END,   dut_set_end   }
    };

    for (uint8_t i = 0; i < arry_num(dut_cmd); i++) {
        if (strcasecmp((char *)args->para1, dut_cmd[i].cmd) == UX_ERR_SUCCESS) {
            return dut_cmd[i].handle(args->para1);
        }
    }
    return AT_RET_SYNTAX_ERROR;
}

at_ret_t at_dut_read_process(void)
{
    return dut_read_state();
}
