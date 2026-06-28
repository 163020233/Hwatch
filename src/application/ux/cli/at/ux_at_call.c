/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description:
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-11， Create file. \n
 */
#include "at.h"
#include "ux_at_manager.h"
#include "ux_at_api.h"
#include "ux_bt_hal_hfp.h"
#include "ux_api_fwk.h"
#include "ux_bt_hal_hfp.h"
#include "ux_bt_hal_sco.h"
#include "ux_bt_call_hf.h"
#include "debug_print.h"
#include "ux_event_define.h"
#ifdef DEMO_WATCH
#include "ux_event_def.h"
#endif

#define at_uart_printf PRINT /* 仅在此文件中重定向，后续3322引入at_dut后切换 */

/*
 * AT^CALL register function
*/
at_ret_t at_call_set_process(const ux_at_para2_args_t *args)
{
    if (args == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    char phone_number[UX_BT_HAL_CALL_PHONENUM_MAX_DIGITS] = {'\0'};
    cmd_event_t call_cmd[] = {
        { "answer",  UX_BG_HF_ANSWER },
        { "hangup",  UX_BG_HF_HANGUP },
        { "reject",  UX_BG_HF_REJECT },
        { "dialing", UX_BG_HF_DIALING },
        { "volume+", UX_BG_HF_VOLUME_UP },
        { "volume-", UX_BG_HF_VOLUME_DOWN },
        { "mute",    UX_BG_HF_VOLUME_MUTE },
    };
    if (args->para1 == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    if (strcasecmp("dialing", args->para1) == 0) {
        errno_t err = strcpy_s(phone_number, arry_num(phone_number), args->para2);
        if (err != EOK) {
            return AT_RET_SYNTAX_ERROR;
        }
    }

    for (uint8_t i = 0; i < arry_num(call_cmd); i++) {
        if (strcasecmp(args->para1, call_cmd[i].cmd) == 0) {
            ux_api_fwk_push_event(UX_EVENT_TYPE_BG, call_cmd[i].event_id,
                (uint8_t*)phone_number, arry_num(phone_number));
            return AT_RET_OK;
        }
    }
    return AT_RET_SYNTAX_ERROR;
}

static void print_hfp_profile_state(ux_bt_hal_hfp_connect_state_t profile_state)
{
    if (profile_state == UX_BT_HAL_HFP_STATE_CONNECTING) {
        at_uart_printf(" hfp: connecting\n");
    } else if (profile_state == UX_BT_HAL_HFP_STATE_CONNECTED) {
        at_uart_printf(" hfp: connected\n");
    } else if (profile_state == UX_BT_HAL_HFP_STATE_DISCONNECTING) {
        at_uart_printf(" hfp: disconnecting\n");
    } else if (profile_state == UX_BT_HAL_HFP_STATE_DISCONNECTED) {
        at_uart_printf(" hfp: disconnected\n");
    } else {
        at_uart_printf(" hfp: unknow\n");
    }
}

static void print_sco_stream_state(ux_api_bt_stream_state_t sco_state)
{
    if (sco_state == UX_API_BT_STREAM_STATE_STARTING) {
        at_uart_printf(" sco: starting\n");
    } else if (sco_state == UX_API_BT_STREAM_STATE_STREAMING) {
        at_uart_printf(" sco: start\n");
    } else if (sco_state == UX_API_BT_STREAM_STATE_STOPPED) {
        at_uart_printf(" sco: stop\n");
    } else if (sco_state == UX_API_BT_STREAM_STATE_STOPPING) {
        at_uart_printf(" sco: stopping\n");
    } else {
        at_uart_printf(" sco: unknow\n");
    }
}

static void print_call_state(ux_bt_hal_call_state_t call_state)
{
    if (call_state == UX_BT_HAL_CALL_STATE_ACTIVE) {
        at_uart_printf(" call: active\n");
    } else if (call_state == UX_BT_HAL_CALL_STATE_HELD) {
        at_uart_printf(" call: held\n");
    } else if (call_state == UX_BT_HAL_CALL_STATE_DIALING) {
        at_uart_printf(" call: dialing\n");
    } else if (call_state == UX_BT_HAL_CALL_STATE_ALERTING) {
        at_uart_printf(" call: alerting\n");
    } else if (call_state == UX_BT_HAL_CALL_STATE_INCOMING) {
        at_uart_printf(" call: incoming\n");
    } else if (call_state == UX_BT_HAL_CALL_STATE_WAITING) {
        at_uart_printf(" call: waiting\n");
    } else if (call_state == UX_BT_HAL_CALL_STATE_RESPONSE_HELD) {
        at_uart_printf(" call: response held\n");
    } else if (call_state == UX_BT_HAL_CALL_STATE_FINISHED) {
        at_uart_printf(" call: finished\n");
    } else {
        at_uart_printf(" call: unknow\n");
    }
}

at_ret_t at_call_read_process(void)
{
    at_uart_printf(" ========= call ========= \r\n");
    ux_bt_call_info_t *call_info = ux_bt_call_get_info();
    uint16_t dev_id = bs_find_device_id_by_addr(BS_PROTOCOL_BREDR, call_info->addr.type, call_info->addr.addr);
    at_uart_printf(" device: %hu\n", dev_id);
    print_hfp_profile_state(call_info->hfp_state);
    print_sco_stream_state(call_info->sco_state);
    print_call_state(call_info->cat1_info.state);
    at_uart_printf(" mic volume: %hhu\n", call_info->mic_volume);
    at_uart_printf(" speaker volume: %hhu\n", call_info->speaker_volume);
    at_uart_printf(" ========== end ========= \r\n");
    return AT_RET_OK;
}