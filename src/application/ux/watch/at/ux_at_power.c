/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-25， Create file. \n
 */
#include "at.h"
#include "errcode.h"
#include "ux_power.h"
#include "ux_api_power.h"
#include "ux_at_manager.h"
#include "debug_print.h"
#include "ux_event_define.h"
#ifndef ENDL
#define ENDL '\0'
#endif
#define at_uart_printf PRINT /* 仅在此文件中重定向，后续3322引入at_dut后切换 */
typedef at_ret_t (*power_cmd_handle_t)(const char *para1, const char *para2, const char *para3);

typedef struct {
    const char *cmd;
    power_cmd_handle_t handle;
} power_cmd_t;

static at_ret_t mcu_power_off(const char *para2, const char *para3)
{
    if (para2 == NULL || para3 == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    char *endptr = NULL;
    long reason = strtol(para2, &endptr, 0);
    if (endptr == NULL || *endptr != ENDL || reason < UX_API_POWER_OFF_NORMAL) {
        return AT_RET_SYNTAX_ERROR;
    }
    long time = strtol(para3, &endptr, 0);
    if (endptr == NULL || *endptr != ENDL || time < 0) {
        return AT_RET_SYNTAX_ERROR;
    }
    if (ux_api_power_off((ux_api_power_off_reason_t)reason, (uint32_t)time) != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    at_uart_printf("after %ums power off mcu\r\n", time);
    return AT_RET_OK;
}

static at_ret_t mcu_reset(const char *para2, const char *para3)
{
    char *endptr = NULL;
    long reason = strtol(para2, &endptr, 0);
    if (endptr == NULL || *endptr != ENDL || reason < UX_API_POWER_OFF_NORMAL) {
        return AT_RET_SYNTAX_ERROR;
    }
    long time = strtol(para3, &endptr, 0);
    if (endptr == NULL || *endptr != ENDL || time < 0) {
        return AT_RET_SYNTAX_ERROR;
    }
    if (ux_api_power_reset((ux_api_power_off_reason_t)reason, (uint32_t)time) != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    at_uart_printf("after %ums power reset mcu\r\n", time);
    return AT_RET_OK;
}

static at_ret_t power_set_mcu(const char *para1, const char *para2, const char *para3)
{
    if (para1 == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }

    if (strcasecmp(para1, "off") == 0) {
        return mcu_power_off(para2, para3);
    } else if (strcasecmp(para1, "reset") == 0) {
        return mcu_reset(para2, para3);
    } else if (strcasecmp(para1, "cancel_off_delay") == 0) {
        if (ux_api_power_cancel_delay_off() == ERRCODE_SUCC) {
            return AT_RET_OK;
        }
    } else if (strcasecmp(para1, "cancel_reset_delay") == 0) {
        if (ux_api_power_cancel_delay_reset() == ERRCODE_SUCC) {
            return AT_RET_OK;
        }
    }
    return AT_RET_SYNTAX_ERROR;
}

static at_ret_t power_set_bt(const char *para1, const char *para2, const char *para3)
{
    unused(para3);

    if (para1 == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }

    if (strcasecmp(para1, "on") == 0) {
        if (ux_power_bt_on() == ERRCODE_SUCC) {
            return AT_RET_OK;
        }
    } else if (strcasecmp(para1, "off") == 0) {
        if (para2 == NULL) {
            return AT_RET_SYNTAX_ERROR;
        }
        char *endptr = NULL;
        long time = strtol(para2, &endptr, 0);
        if (endptr == NULL || *endptr != ENDL) {
            return AT_RET_SYNTAX_ERROR;
        }
        if (time < 0) {
            return AT_RET_SYNTAX_ERROR;
        }
        if (ux_power_bt_off((uint32_t)time) != ERRCODE_SUCC) {
            return AT_RET_SYNTAX_ERROR;
        }
        at_uart_printf("after %ums power off bt\r\n", time);
        return AT_RET_OK;
    } else if (strcasecmp(para1, "cancel_off_delay") == 0) {
        if (ux_power_cancel_bt_delay_off() == ERRCODE_SUCC) {
            return AT_RET_OK;
        }
    }
    return AT_RET_SYNTAX_ERROR;
}

at_ret_t at_power_set_process(const ux_at_para4_args_t *args)
{
    if (args == NULL || args->para1 == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    static power_cmd_t power_cmd[] = {
        { "mcu", power_set_mcu },
        { "bt", power_set_bt},
    };

    for (uint8_t i = 0; i < arry_num(power_cmd); i++) {
        if (strcasecmp((char *)args->para1, power_cmd[i].cmd) == 0) {
            return power_cmd[i].handle(args->para2, args->para3, args->para4);
        }
    }

    return AT_RET_SYNTAX_ERROR;
}

static at_ret_t power_query_mcu(void)
{
    ux_power_state_t mcu_state;
    errcode_t err = ux_power_get_state(&mcu_state);
    if (err != ERRCODE_SUCC) {
        at_uart_printf(" get mcu state failed\r\n");
        return AT_RET_FORWORD_ERROR;
    }

    if (mcu_state == UX_POWER_NORMAL) {
        at_uart_printf(" mcu state: normal\r\n");
    } else if (mcu_state == UX_POWER_OFF_DELAY) {
        at_uart_printf(" mcu state: delay off\r\n");
    } else if (mcu_state == UX_POWER_RESET_DELAY) {
        at_uart_printf(" mcu state: delay reset\r\n");
    } else {
        at_uart_printf(" mcu state: unknown\r\n");
    }
    return AT_RET_OK;
}

static at_ret_t power_query_bt(void)
{
    ux_power_b_core_t b_core_state;
    errcode_t err = ux_power_get_bt_core_state(&b_core_state);
    if (err != ERRCODE_SUCC) {
        at_uart_printf(" get bt state failed\r\n");
        return AT_RET_FORWORD_ERROR;
    }
    if (b_core_state == UX_POWER_BT_NORMAL) {
        at_uart_printf(" bt state: normal\r\n");
    } else if (b_core_state == UX_POWER_BT_OFF) {
        at_uart_printf(" bt state: off\r\n");
    } else if (b_core_state == UX_POWER_BT_OFF_DELAY) {
        at_uart_printf(" bt state: delay off\r\n");
    } else if (b_core_state == UX_POWER_BT_ON_DOING) {
        at_uart_printf(" bt state: powering on\r\n");
    } else if (b_core_state == UX_POWER_BT_ABNORMAL) {
        at_uart_printf(" bt state: abnormal\r\n");
    } else {
        at_uart_printf(" bt state: unknown\r\n");
    }
    return AT_RET_OK;
}

static at_ret_t power_query_off_reason(void)
{
    ux_api_power_off_reason_t off_reason = ux_api_power_get_off_reason();
    if (off_reason == UX_API_POWER_OFF_NORMAL) {
        at_uart_printf(" mcu off reason: normal\r\n");
    } else if (off_reason == UX_API_POWER_OFF_OTA) {
        at_uart_printf(" mcu off reason: ota\r\n");
    } else if (off_reason == UX_API_POWER_OFF_RECOVER) {
        at_uart_printf(" mcu off reason: recovery\r\n");
    } else if (off_reason == UX_API_POWER_OFF_APP) {
        at_uart_printf(" mcu off reason: app\r\n");
    } else if (off_reason == UX_API_POWER_OFF_DUT) {
        at_uart_printf(" mcu off reason: dut\r\n");
    }  else if (off_reason == UX_API_POWER_OFF_BATTERY) {
        at_uart_printf(" mcu off reason: low battery\r\n");
    }  else if (off_reason == UX_API_POWER_OFF_USER) {
        at_uart_printf(" mcu off reason: custom\r\n");
    } else {
        at_uart_printf(" mcu off reason: unknown\r\n");
    }
    return AT_RET_OK;
}

at_ret_t at_power_read_process(void)
{
    at_uart_printf(" ========= ux power state ========= \r\n");
    power_query_mcu();
    power_query_bt();
    power_query_off_reason();
    at_uart_printf(" ============== end =============== \r\n");
    return AT_RET_OK;
}