/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag offline cmd
 */
#ifndef DIAG_OFFLINE_CMD_H
#define DIAG_OFFLINE_CMD_H

#include <stddef.h>
#include "errcode.h"
#include "diag.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define DFX_OFFLINE_CMD_INFO_PATH    "/user/log/"
#define DFX_OFFLINE_CMD_INFO_NAME    "/user/log/cmd_info.txt"
#define OFFLINE_BUFFER_SIZE     4096

typedef enum {
    OFFLINE_MODE_SERIAL = 0,
    OFFLINE_MODE_FILE = 1
} offline_mode_t;

offline_mode_t get_offline_mode(void);
errcode_t write_to_file(const char *out_buffer, const size_t buffer_len);
errcode_t diag_offline_cmd(uint16_t cmd_id, uint8_t *cmd_param, uint16_t cmd_param_size, diag_option_t *option);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DIAG_OFFLINE_CMD_H */
