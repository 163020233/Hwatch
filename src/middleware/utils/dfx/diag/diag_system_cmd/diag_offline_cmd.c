/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: diag offline cmd
 * This file should be changed only infrequently and with great care.
 */

#include "diag_offline_cmd.h"
#include "dfx_adapt_layer.h"
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
#include "fcntl.h"
#include "unistd.h"
#include "sys/stat.h"
#endif /* CONFIG_DFX_SUPPORT_FILE_SYSTEM */
#define DIAG_OFFLINE_CMD_FAIL     "Off-cmd fail\r\n"
#define DIAG_OFFLINE_CMD_SERIAL   "Off-cmd serial\r\n"
#define DIAG_OFFLINE_CMD_FILE     "Off-cmd file\r\n"

static offline_mode_t g_offline_mode = OFFLINE_MODE_SERIAL;

#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
static int g_fd = -1;
errcode_t write_to_file(const char *out_buffer, const size_t buffer_len)
{
    ssize_t written = write(g_fd, out_buffer, buffer_len);
    if ((size_t)written != buffer_len) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
#endif /* CONFIG_DFX_SUPPORT_FILE_SYSTEM */

offline_mode_t get_offline_mode(void)
{
    return g_offline_mode;
}

#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
STATIC errcode_t diag_open_offline_file(void)
{
    int ret;

    if (g_fd >= 0) {
        dfx_log_err("Already in Off-cmd file\r\n");
        return ERRCODE_FAIL;
    }

    if (access(DFX_OFFLINE_CMD_INFO_PATH, 0) != 0) {
        ret = mkdir(DFX_OFFLINE_CMD_INFO_PATH, S_IRWXU | S_IRWXG | S_IRWXO);
        if (ret != 0) {
            dfx_log_err("Mkdir %s failed\r\n", DFX_OFFLINE_CMD_INFO_PATH);
            return ERRCODE_FAIL;
        }
    }

    g_fd = open(DFX_OFFLINE_CMD_INFO_NAME,
        O_RDWR | O_CREAT | O_APPEND, 0666); /* 0666: open file with rw-rw-rw- mode */
    if (g_fd < 0) {
        dfx_log_err("Open %s failed.\r\n", DFX_OFFLINE_CMD_INFO_NAME);
        g_offline_mode = OFFLINE_MODE_SERIAL;
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}
#endif

errcode_t diag_offline_cmd(uint16_t cmd_id, uint8_t *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_param_size);
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
    uint32_t enable;
    if (cmd_param == NULL) {
        dfx_log_err("Please enter mode 0 or 1.\r\n");
        goto err;
    }
    enable = *(uint32_t *)cmd_param;
    if (enable == OFFLINE_MODE_SERIAL || enable == OFFLINE_MODE_FILE) {
        g_offline_mode = enable;
    } else {
        goto err;
    }
    if (g_offline_mode == OFFLINE_MODE_FILE) {
        if (diag_open_offline_file() != ERRCODE_SUCC) {
            goto err;
        }

        uapi_diag_report_packet(cmd_id, option, (uint8_t *)DIAG_OFFLINE_CMD_FILE,
                                sizeof(DIAG_OFFLINE_CMD_FILE), true);
    } else {
        if (g_fd >= 0) {
            close(g_fd);
            g_fd = -1;
        }
        uapi_diag_report_packet(cmd_id, option, (uint8_t *)DIAG_OFFLINE_CMD_SERIAL,
                                sizeof(DIAG_OFFLINE_CMD_SERIAL), true);
    }
    return ERRCODE_SUCC;
err:
    uapi_diag_report_packet(cmd_id, option, (uint8_t *)DIAG_OFFLINE_CMD_FAIL,
                            sizeof(DIAG_OFFLINE_CMD_FAIL), true);
    return ERRCODE_FAIL;
#else
    unused(cmd_param);
    uapi_diag_report_packet(cmd_id, option, (uint8_t *)DIAG_OFFLINE_CMD_FAIL,
                            sizeof(DIAG_OFFLINE_CMD_FAIL), true);
    dfx_log_err("Unable to enable offline commands.");
    return ERRCODE_FAIL;
#endif /* CONFIG_DFX_SUPPORT_FILE_SYSTEM */
}