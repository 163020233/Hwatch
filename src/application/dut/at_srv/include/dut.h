/**
  Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
*
* Description: Provides DUT services \n
*/

#ifndef DUT_INC_H
#define DUT_INC_H

#include <stdint.h>
#include <errcode.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define DUT_CMD_DATA_BUFFER_LEN   1024
#define DUT_CMD_SET_MAX   32

#define RESPONSE_SUFFIX        "\r\n"
#define LEN_OF_RESPONSE_SUFFIX 2
#define RESPONSE_MODULE        "AT^"
#define LEN_OF_RESPONSE_MODULE 3
#define RESPONSE_OK "\r\nOK\r\n"
#define RESPONSE_ERROR "\r\nERROR\r\n"

#define dut_print_err(log_num, fmt, args...)
#define dut_print_info(log_num, fmt, args...)
#define dut_print_debug(log_num, fmt, args...)

typedef int32_t (*dut_cmd_handle)(uint8_t *para);
typedef void (*dut_data_channel_handle)(char *buffer, uint16_t buffer_len, const char *ack, uint16_t ack_len);

typedef enum {
    DUT_EVT_DATA_UART = 0xaa,
    DUT_EVT_DATA_SPP,
    DUT_EVT_AT_SLAVE,
    DUT_EVT_END,
} dut_evt_t;

typedef enum {
    DUT_CMD_LEVEL_WHITELIST = 0x100,
    DUT_CMD_LEVEL_DUT = 0x200,
    DUT_CMD_LEVEL_DEVELOP = 0x300,
    DUT_CMD_LEVEL_BUTT = 0x400
} dut_cmd_level_t;

typedef struct {
    char *command;
    dut_cmd_level_t level;
    dut_cmd_handle cmd_handle;
} dut_cmd_proc_t;

typedef struct {
    const dut_cmd_proc_t *cmd;
    uint32_t count;
    char *name;
} dut_cmdset_t;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
