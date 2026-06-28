/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag metrics cmd
 * This file should be changed only infrequently and with great care.
 */

#include <sys/stat.h>
#include "string.h"
#include "securec.h"
#include "fcntl.h"
#include "non_os_reboot.h"
#include "hal_reboot.h"
#include "exception.h"
#include "preserve.h"
#include "diag_config.h"
#include "dirent.h"
#include "diag_report_control.h"
#include "soc_diag_cmd_id.h"
#include "dfx_feature_config.h"
#include "diag_metrics_cmd.h"

#if defined(CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK) && (CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK == DFX_YES)
#include "diag_log_hook.h"
#endif

#define PATH_MAX_LEN 30
#define TOTAL_PATH_MAX_LEN 200

#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE) && (CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE == DFX_YES))
static char g_log_send_buf[TOTAL_PATH_MAX_LEN];

// 安全附加字符串
// return 0 ok; <0 fail
static int buf_append(char *buf, uint16_t buf_size, const char *src)
{
    // 参数检查
    if (buf == NULL || src == NULL) {
        return -1;
    }

    uint16_t cur = strlen(buf);
    uint16_t left = buf_size - cur;

    uint16_t srclen = strlen(src);
    if (srclen >= left) {
        return -1;
    }

    // 使用 snprintf_s 写入末尾
    // 从 buf+cur 开始；剩余缓冲大小 left
    int n = snprintf_s(buf + cur, left, left - 1, "%s", src);
    if (n <= 0 || (uint16_t)n >= left) {
        return -1;
    }
    return 0;
}

// 扫描 MASSDATA_FILE_PATH 目录，将发送所有以 MASSDATA_FILE_NAME 为前缀的CHR文件完整路径
static errcode_t diag_cmd_report_massdata_file(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                               diag_option_t *option)
{
    unused(cmd_param);
    unused(cmd_param_size);
    DIR *dir = opendir(MASSDATA_FILE_PATH);
    if (!dir) {
        return ERRCODE_FAIL;
    }

    g_log_send_buf[0] = '\0';       // 清空结果缓冲
    const uint16_t prefix_len = strlen(MASSDATA_FILE_NAME);
    struct dirent *ent;

    bool first = true;

    while ((ent = readdir(dir)) != NULL) {
        const char *name = ent->d_name;

        // 跳过 "." "..", 前缀过滤
        if (name[0] == '.' || strncmp(name, MASSDATA_FILE_NAME, prefix_len) != 0) {
            continue;
        }

        // 拼 path
        char fullpath[PATH_MAX_LEN];
        int n = snprintf_s(fullpath, sizeof(fullpath), sizeof(fullpath) - 1,
                           "%s/%s", MASSDATA_FILE_PATH, name);
        if (n <= 0) {
            continue;
        }

        // 判断是否为常规文件
        struct stat st;
        if (stat(fullpath, &st) != 0 || !S_ISREG(st.st_mode)) {
            continue;
        }

        // ',' 分隔
        if (!first) {
            if (buf_append(g_log_send_buf, TOTAL_PATH_MAX_LEN, ",") != 0) {
                closedir(dir);
                return ERRCODE_FAIL;
            }
        }
        first = false;

        // 添加 fullpath
        if (buf_append(g_log_send_buf, TOTAL_PATH_MAX_LEN, fullpath) != 0) {
            closedir(dir);
            return ERRCODE_FAIL;
        }
    }

    closedir(dir);

    // 一次性发送
    uint16_t total = strlen(g_log_send_buf);
    if (total > 0) {
        if (uapi_diag_report_packet(cmd_id, option, (uint8_t*)g_log_send_buf, (uint16_t)total, true) != 0) {
            return ERRCODE_FAIL;
        }
    } else {
        // 没找到返回空串
        uapi_diag_report_packet(cmd_id, option, (uint8_t*)"", 0, true);
    }

    return ERRCODE_SUCC;
}
#endif

static errcode_t diag_cmd_reboot_system(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                        diag_option_t *option)
{
    unused(cmd_param);
    unused(cmd_param_size);
    unused(option);
    errcode_t ret = ERRCODE_SUCC;
    ret = uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ret, (uint16_t)sizeof(errcode_t), true);
    reset_deal_with_fs();
    set_cpu_utils_reset_cause(REBOOT_CAUSE_APPLICATION_SYSRESETREQ);
    hal_reboot_chip();
    return ret;
}

diag_cmd_reg_obj_t g_diag_metrics_cmd_tbl[] = {
#if defined(CONFIG_DFX_SUPPORT_METRICS_REPORT) && (CONFIG_DFX_SUPPORT_METRICS_REPORT == DFX_YES)
    { DIAG_CMD_DIAG_REPORT_METRICS_INTERVAL,  DIAG_CMD_DIAG_REPORT_METRICS_INTERVAL, diag_cmd_set_metrics },
#endif  /* CONFIG_DFX_SUPPORT_METRICS_REPORT */
#if defined(CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK) && (CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK == DFX_YES)
    { DIAG_CMD_SET_REPORT_LOG_LEVEL,  DIAG_CMD_SET_REPORT_LOG_LEVEL, diag_cmd_set_report_log_level },
#endif  /* CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK */
    { DIAG_CMD_REBOOT_SYSTEM,  DIAG_CMD_REBOOT_SYSTEM, diag_cmd_reboot_system },
    { DIAG_CMD_REPORT_BOOT_INFO,  DIAG_CMD_REPORT_BOOT_INFO, diag_cmd_report_boot_info },
#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE) && (CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE == DFX_YES))
    { DIAG_CMD_REPORT_MASSDATA_FILE,  DIAG_CMD_REPORT_MASSDATA_FILE, diag_cmd_report_massdata_file },
#endif
};

errcode_t diag_metrics_cmd_init(void)
{
    return uapi_diag_register_cmd(g_diag_metrics_cmd_tbl,
                                  sizeof(g_diag_metrics_cmd_tbl) / sizeof(g_diag_metrics_cmd_tbl[0]));
}
