/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: At gnss func \n
 *
 */

#include "at_diting_gnss.h"
#include <string.h>
#include "at_cmd.h"
#include "at_diting_gnss_cmd_register.h"
#include "at_diting_gnss_cmd_table.h"
#include "gnss_process.h"
#include "debug_print.h"

#define HEX_PAIR_LENGTH 2
#define HEX_BASE 16

#define AT_DITING_GNSS_FUNC_NUM (sizeof(at_diting_gnss_cmd_parse_table) / sizeof(at_diting_gnss_cmd_parse_table[0]))
void los_at_diting_gnss_cmd_register(void)
{
    print_str("los_at_diting_gnss_cmd_register EXCUTE\r\n");
    uapi_at_diting_gnss_register_cmd(at_diting_gnss_cmd_parse_table, AT_DITING_GNSS_FUNC_NUM);
}

at_ret_t at_gnss_init(gnss_init_args *args)
{
    errcode_t ret = gnss_proc_msg_init(args->nmeaFlag, args->replayFlag, args->logLevel, args->rtkReplay);
    if (ret == ERRCODE_SUCC) {
        print_str("OK\n");
    } else {
        print_str("gnss init fail, error code: %#x\n", ret);
    }
    return AT_RET_OK;
}

at_ret_t at_gnss_start(void)
{
    errcode_t ret = gnss_proc_msg_start();
    if (ret == ERRCODE_SUCC) {
        print_str("OK\n");
    } else {
        print_str("gnss start fail, error code: %#x\n", ret);
    }

    return AT_RET_OK;
}

at_ret_t at_gnss_stop(void)
{
    errcode_t ret = gnss_proc_msg_stop();
    if (ret == ERRCODE_SUCC) {
        print_str("OK\n");
    } else {
        print_str("gnss stop fail, error code: %#x\n", ret);
    }

    return AT_RET_OK;
}

at_ret_t at_gnss_deinit(void)
{
    errcode_t ret = gnss_proc_msg_deinit();
    if (ret == ERRCODE_SUCC) {
        print_str("OK\n");
    } else {
        print_str("gnss deinit fail, error code: %#x\n", ret);
    }

    return AT_RET_OK;
}

at_ret_t at_gnss_reinit(void)
{
    errcode_t ret = gnss_proc_msg_stop();
    if (ret == ERRCODE_SUCC) {
        print_str("OK\n");
    } else {
        print_str("gnss reinit fail, error code: %#x\n", ret);
    }

    return AT_RET_OK;
}

at_ret_t at_gnss_sample(gnss_sample_args *args)
{
    errcode_t ret = gnss_proc_msg_sample(args->mode);
    if (ret == ERRCODE_SUCC) {
        print_str("OK\n");
    } else {
        print_str("gnss sample :%u exec fail, error code: %#x\n", args->mode, ret);
    }

    return AT_RET_OK;
}

at_ret_t at_gnss_nmea(gnss_nmea_args *args)
{
    errcode_t ret = gnss_proc_msg_nmea(args->nmeaFlag);
    if (ret == ERRCODE_SUCC) {
        print_str("OK\n");
    } else {
        print_str("gnss nmea cfg fail, error code: %#x\n", ret);
    }

    return AT_RET_OK;
}

at_ret_t at_gnss_log(gnss_log_args *args)
{
    errcode_t ret = gnss_proc_log_ctrl_info(args->num, args->size);
    if (ret == ERRCODE_SUCC) {
        print_str("OK\n");
    } else {
        print_str("gnss log ctrl fail, error code: %#x\n", ret);
    }

    return AT_RET_OK;
}

at_ret_t at_gnss_hex(gnss_hex_args *args)
{
    if (args->data == NULL) {
        return AT_RET_CMD_PARA_ERROR;
    }

    uint32_t dataLen = strlen(args->data);
    // strlen不计算结束符，所以必定为偶数
    if ((dataLen == 0) || (dataLen % HEX_PAIR_LENGTH != 0)) {
        return AT_RET_CMD_PARA_ERROR;
    }

    uint32_t len = dataLen / HEX_PAIR_LENGTH;
    uint8_t data[len];
    uint8_t idx = 0;
    for (uint32_t i = 0; i < dataLen; i += HEX_PAIR_LENGTH) {
        char hex_pair[HEX_PAIR_LENGTH + 1] = {args->data[i], args->data[i + 1], '\0'};
        char *endptr;
        data[idx++] = strtol(hex_pair, &endptr, HEX_BASE);
        if (*endptr != '\0') {
            return ERRCODE_FAIL;
        }
    }
    errcode_t ret = gnss_proc_msg_cfg(data, len);
    if (ret == ERRCODE_SUCC) {
        print_str("OK\n");
    } else {
        print_str("gnss hex exec fail, error code: %#x\n", ret);
    }

    return AT_RET_OK;
}

at_ret_t at_gnss_cfg(gnss_cfg_args *args)
{
    errcode_t ret = gnss_proc_board_cfg(args->posType, args->xgnssType);
    if (ret == ERRCODE_SUCC) {
        print_str("OK\n");
    } else {
        print_str("gnss cfg fail, error code: %#x\n", ret);
    }

    return AT_RET_OK;
}