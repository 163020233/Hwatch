/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide api functions for at \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-21， Create file. \n
 */
#include <string.h>
#include <stdlib.h>
#include "stdint.h"
#include "at.h"
#include "app_at_uart.h"
#include "ux_device_manager.h"
#include "ux_common_log.h"
#include "ux_event_define.h"
#include "ux_hal_memory.h"
#include "ux_at_test.h"
#include "ux_api_bt.h"
#include "test/ux_at_test.h"
#include "test/ux_at_spp_test.h"

#define BYTE_HEX_LEN 2

static errcode_t parse_send_data_process(const char *param, const char *param1, const char *param2)
{
    unused(param2);
    errcode_t ret = ERRCODE_SUCC;
    uint16_t length = 0;
    uint8_t* data;

    if (param == NULL || *param == '\0' || param1 == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    length = (uint16_t)ux_at_test_get_digit(param);
    if (length == 0) {
        length = (uint16_t)(strlen(param1) / BYTE_HEX_LEN);
    }
    data = ux_hal_memory_acquire(UX_MEM_SPP, length);
    if (data == NULL) {
        return ERRCODE_MALLOC;
    }

    ret = ux_at_test_str_to_data(param1, (uint16_t)MIN(strlen(param1) / BYTE_HEX_LEN, length), data);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][UXTEST]ux_at_test_str_to_data failed!\n");
        ux_hal_memory_release(UX_MEM_SPP, data);
        data = NULL;
        return AT_RET_SYNTAX_ERROR;
    }
    if (strlen(param1) / BYTE_HEX_LEN < length) {
        (void)ux_at_test_fill_data(data + strlen(param1) / BYTE_HEX_LEN,
            length - strlen(param1) / BYTE_HEX_LEN, data, strlen(param1) / BYTE_HEX_LEN);
    }

    ret = ux_api_spp_service_send_data(0, data, length);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][UXTEST]ux_api_spp_service_send_data failed!\n");
        ux_hal_memory_release(UX_MEM_SPP, data);
        data = NULL;
        return AT_RET_SYNTAX_ERROR;
    }
    ux_hal_memory_release(UX_MEM_SPP, data);
    data = NULL;
    return ret;
}

errcode_t ux_at_test_spp_set_process(const char *func_name, const char *device, const char *param,
    const char *sub_param)
{
    static ux_at_test_opts_map_t opts_map[] = {
        { "write", parse_send_data_process },   // AT^UXTEST=spp,write,x
    };

    uint32_t opts_cnt = (uint32_t)arry_num(opts_map);
    for (uint32_t i = 0; i < opts_cnt; i++) {
        if (strcasecmp(func_name, opts_map[i].cmd) == 0) {
            return opts_map[i].operator_func(device, param, sub_param);
        }
    }

    at_uart_printf("[ux][UXTEST]ux_at_test_spp_set_process\n");
    return ERRCODE_FAIL;
}
