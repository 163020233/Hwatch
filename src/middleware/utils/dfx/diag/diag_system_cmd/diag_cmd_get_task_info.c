/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: diag get task info
 * This file should be changed only infrequently and with great care.
 */

#include "diag_cmd_get_task_info.h"
#include "dfx_task.h"
#include "diag_offline_cmd.h"
#include "dfx_adapt_layer.h"
#include "errcode.h"

#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
static errcode_t save_task_info_to_file(task_info_t *infs, uint32_t task_cnt)
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
                        "\r\nTask Information Report\n"
                        "%-25s %-7s %-6s %-8s %-5s %-12s %-12s %-12s %-12s %-12s %-12s %-10s\n",
                        "Task Name", "Valid", "ID", "Status", "Pri", "Stack Size", "Stack Top", "Stack Bottom",
                        "SP Pointer", "Curr Stack", "Peak Stack", "Overflow");

    for (unsigned i = 0; i < task_cnt; i++) {
        if (!infs[i].valid) {continue;}
        len = sprintf_s((char *)offline_buffer + offset, OFFLINE_BUFFER_SIZE - offset,
                        "%-25.25s %-7s %-6u %-8u %-5u %-12u %-12u %-12u %-12u %-12u %-12u %-10s\n",
                        infs[i].name, infs[i].valid ? "Yes" : "No",
                        infs[i].id, infs[i].status, infs[i].priority, infs[i].stack_size,
                        infs[i].top_of_stack, infs[i].bottom_of_stack, infs[i].sp,
                        infs[i].curr_used, infs[i].peak_used, infs[i].overflow_flag ? "Yes" : "No");
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

errcode_t diag_cmd_get_task_info(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    uint32_t task_cnt;
    errcode_t ret = ERRCODE_SUCC;
    task_info_t *infs = NULL;

    unused(cmd_param);
    unused(cmd_param_size);

    task_cnt = dfx_os_get_task_cnt();
    if (task_cnt == 0) {
        return ERRCODE_FAIL;
    }

    infs = dfx_malloc(0, task_cnt * sizeof(task_info_t));
    if (infs == NULL) {
        return ERRCODE_FAIL;
    }

    ret = dfx_os_get_all_task_info(infs, task_cnt);
    if (ret != ERRCODE_SUCC) {
        dfx_free(0, infs);
        return ret;
    }
    if (get_offline_mode() == OFFLINE_MODE_SERIAL) {
        for (unsigned i = 0; i < task_cnt; i++) {
            task_info_t *inf = &infs[i];
            if (inf->valid) {
                uapi_diag_report_packet(cmd_id, option, (uint8_t *)inf, (uint16_t)sizeof(task_info_t), true);
            }
        }
    } else {
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
        ret = save_task_info_to_file(infs, task_cnt);
        task_info_t task_info_null_packet;
        if (ret == ERRCODE_SUCC) {
            task_info_null_packet = (task_info_t){.name = "Success"};
        } else {
            task_info_null_packet = (task_info_t){.name = "Failure"};
        }
        uapi_diag_report_packet(cmd_id, option, (const uint8_t *)(&task_info_null_packet),
                                sizeof(task_info_null_packet), true);
#endif /* CONFIG_DFX_SUPPORT_FILE_SYSTEM */
    }
    dfx_free(0, infs);
    return ret;
}