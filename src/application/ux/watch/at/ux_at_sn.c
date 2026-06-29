/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux at sn \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-11-18， Create file. \n
 */

#include "ux_at_sn.h"
#include <string.h>
#include <stdlib.h>
#include "at.h"
#include "app_at_uart.h"

#include "ux_event_define.h"
#include "app_at_uart.h"
#include "ux_device_manager.h"
#include "ux_at_interface.h"
#include "ux_fwk_msg_def.h"
#include "ux_fwk_msg.h"
#include "ux_event_define.h"
#include "fsm_master.h"
#include "ux_fwk_process.h"
#include "ux_status_define.h"
#include "ux_system_manager.h"
#include "debug_print.h"

#include "ux_bs_property.h"
#include "bs_br_service_api.h"
#include "bs_br_service_config.h"
#include "key_id.h"

#define UX_BS_ADDR_STR_LEN 12
#define BT_ADDR_LEN 6
#define BT_ADDR_STR_LEN 20

#define at_uart_printf PRINT /* 仅在此文件中重定向，后续3322引入at_dut后切换 */

typedef enum { POS_0, POS_1, POS_2, POS_3, POS_4, POS_5 } bt_pos_code;

typedef at_ret_t (*at_dut_cmd_handle_t)(const char *para1);

typedef struct {
    const char *cmd;
    at_dut_cmd_handle_t handle;
} at_dut_cmd_t;

static at_ret_t dut_read_state(void)
{
    uint8_t btaddr[BT_ADDR_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    if (ux_bg_load_addr(NV_ID_BT_FACTORY_ADDR, btaddr, sizeof(btaddr)) != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }

    char addr[BT_ADDR_STR_LEN];
    int32_t ret = snprintf_s(addr, sizeof(addr), sizeof(addr), "%02X%02X%02X%02X%02X%02X",
        btaddr[POS_0], btaddr[POS_1], btaddr[POS_2], btaddr[POS_3], btaddr[POS_4], btaddr[POS_5]);
    if (ret < 0) {
        return AT_RET_SYNTAX_ERROR;
    }
    at_uart_printf("\r\n^SN: %s\r\n", addr);

    return AT_RET_OK;
}

static at_ret_t dut_set_bsn(const char *para)
{
    if (para == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }

    if (ux_bs_set_local_addr(UX_WORK_PROTOCOL_BT, (uint8_t *)para, UX_BS_ADDR_STR_LEN) == UX_ERR_SUCCESS) {
        return AT_RET_OK;
    }

    return AT_RET_SYNTAX_ERROR;
}

at_ret_t at_sn_set_process(const ux_at_para1_args_t *args)
{
    if (args == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }

    return dut_set_bsn(args->para1);
}

at_ret_t at_sn_read_process(void)
{
    return dut_read_state();
}
