/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag get filesystem info
 * This file should be changed only infrequently and with great care.
 */

#include "diag_cmd_get_fs_info.h"
#include "dfx_adapt_layer.h"
#include "soc_diag_cmd_id.h"
#include "diag_offline_cmd.h"
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
#include <sys/stat.h>
#include "sys/statfs.h"
#endif
#include "errcode.h"

#ifdef CONFIG_DIAG_FS_PROC_SUPPORT
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
#define FS_NODE_INFO_SPACE      16
#define KB_SIZE                 (1024)

#ifndef NANDFLASH_SINGLE_PARTITION
#define FS_VOLUME_STRS "/boot/", "/system/", "/inner/", "/systembk/", "/user/",
#define FS_VOLUMES_NUM 5
#else
#define FS_VOLUME_STRS "/user/"
#define FS_VOLUMES_NUM 1
#endif // NANDFLASH_SINGLE_PARTITION
static const char * const g_volume_names[FS_VOLUMES_NUM] = { FS_VOLUME_STRS };
#endif // (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)

static void diag_fs_fail_report(uint16_t cmd_id, diag_option_t *option)
{
    diag_fs_info_t file_str = {"Get Failed", 0, 0, 0};
    uapi_diag_report_packet(cmd_id, option, (const uint8_t *)(&file_str), sizeof(file_str), true);
}

#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
static errcode_t save_fs_info_to_file(diag_fs_info_t *infs, int count)
{
    errcode_t ret = ERRCODE_SUCC;
    uint8_t *offline_buffer = NULL;
    int32_t offset = 0;
    int len;

    offline_buffer = (uint8_t *)dfx_malloc(0, OFFLINE_BUFFER_SIZE);
    if (offline_buffer == NULL) {
        dfx_log_err("Malloc failed: insufficient memory.");
        ret = ERRCODE_FAIL;
        return ret;
    }

    offset += sprintf_s((char *)offline_buffer + offset, OFFLINE_BUFFER_SIZE - offset,
                        "\r\nFile System Report\n"
                        "%-15s %-12s %-12s %-12s\n",
                        "Volume", "Total(KB)", "Used(KB)", "Free(KB)");

    for (int i = 0; i < count; i++) {
        if (strcmp(infs[i].name, "") == 0) {
            continue;
        }

        len = sprintf_s((char *)offline_buffer + offset, OFFLINE_BUFFER_SIZE - offset,
                        "%-15s %-12u %-12u %-12u\n",
                        infs[i].name, infs[i].total_size, infs[i].used_size, infs[i].free_size);
        if (len < 0 || offset + len >= OFFLINE_BUFFER_SIZE - 1) {
            dfx_log_err("Buffer overflow: data exceeds maximum buffer size.");
            ret = ERRCODE_FAIL;
            break;
        }
        offset += len;
    }

    write_to_file((char *)offline_buffer, (size_t)offset);
    dfx_free(0, offline_buffer);
    return ret;
}
#endif /* CONFIG_DFX_SUPPORT_FILE_SYSTEM */

static errcode_t report_filesystem_data(uint16_t cmd_id, diag_option_t *option, diag_fs_info_t *infs)
{
    errcode_t ret = ERRCODE_SUCC;
    if (get_offline_mode() == OFFLINE_MODE_SERIAL) {
        for (int i = 0; i < FS_VOLUMES_NUM; i++) {
            diag_fs_info_t *inf = &infs[i];
            if (strcmp(inf->name, "") == 0) {
                continue;
            }
            uapi_diag_report_packet(cmd_id, option, (uint8_t *)inf, (uint16_t)sizeof(diag_fs_info_t), true);
        }
    } else {
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
        ret = save_fs_info_to_file(infs, FS_VOLUMES_NUM);
#endif /* CONFIG_DFX_SUPPORT_FILE_SYSTEM */
        diag_fs_info_t fs_info_null_packet = {.name = "Success"};
        uapi_diag_report_packet(cmd_id, option, (const uint8_t *)(&fs_info_null_packet),
                                sizeof(fs_info_null_packet), true);
    }
    return ret;
}

errcode_t diag_cmd_get_fs_info(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_param);
    unused(cmd_param_size);
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
    errcode_t ret = ERRCODE_FAIL;
    struct statfs file_buf = {0};
    diag_fs_info_t *infs = dfx_malloc(0, FS_VOLUMES_NUM * sizeof(diag_fs_info_t));
    if (infs == NULL) {
        diag_fs_fail_report(cmd_id, option);
        return ERRCODE_FAIL;
    }

    if (memset_s(infs, FS_VOLUMES_NUM * sizeof(diag_fs_info_t), 0, FS_VOLUMES_NUM * sizeof(diag_fs_info_t)) != EOK) {
        goto fail_report;
    }

    for (int i = 0; i < FS_VOLUMES_NUM; i++) {
        if (statfs(g_volume_names[i], &file_buf) != 0) {
            continue;
        }
        if (file_buf.f_bfree >= FS_NODE_INFO_SPACE) {
            file_buf.f_bfree = file_buf.f_bfree - FS_NODE_INFO_SPACE;
        } else {
            file_buf.f_bfree = 0;
        }
        diag_fs_info_t *inf = &infs[i];
        size_t name_len = strlen(g_volume_names[i]);
        if (name_len >= MAX_FS_VOLUMES_LEN) {
            goto fail_report;
        }
        if (memcpy_s(inf->name, MAX_FS_VOLUMES_LEN, g_volume_names[i], name_len) != EOK) {
            goto fail_report;
        }
        inf->name[name_len] = '\0';
        inf->total_size = ((uint32_t)(file_buf.f_blocks * file_buf.f_bsize) / KB_SIZE);
        inf->used_size = ((uint32_t)((file_buf.f_blocks - file_buf.f_bfree) * file_buf.f_bsize) / KB_SIZE);
        inf->free_size = ((uint32_t)(file_buf.f_bfree * file_buf.f_bsize) / KB_SIZE);
    }
    ret = report_filesystem_data(cmd_id, option, infs);
    goto cleanup;

fail_report:
    diag_fs_fail_report(cmd_id, option);
cleanup:
    dfx_free(0, infs);
    return ret;
#else
    diag_fs_fail_report(cmd_id, option);
    return ERRCODE_FAIL;
#endif
}

diag_cmd_reg_obj_t g_diag_fs_proc_cmd_tbl[] = {
    { DIAG_CMD_GET_FS_INFO, DIAG_CMD_GET_FS_INFO, diag_cmd_get_fs_info },
};

errcode_t diag_fs_proc_init(void)
{
    return uapi_diag_register_cmd(g_diag_fs_proc_cmd_tbl,
        sizeof(g_diag_fs_proc_cmd_tbl) / sizeof(g_diag_fs_proc_cmd_tbl[0]));
}
#endif // CONFIG_DIAG_FS_PROC_SUPPORT