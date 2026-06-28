/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: diag get memory info
 * This file should be changed only infrequently and with great care.
 */

#include "diag_cmd_get_mem_info.h"
#include "dfx_mem.h"
#include "diag_offline_cmd.h"
#include "dfx_adapt_layer.h"
#include "errcode.h"

#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
static void save_mem_info_to_file(mdm_mem_info_t *info)
{
    uint8_t *offline_buffer = NULL;
    int32_t offset = 0;
    offline_buffer = (uint8_t *)dfx_malloc(0, OFFLINE_BUFFER_SIZE);
    if (offline_buffer == NULL) {
        dfx_log_err("Malloc failed: insufficient memory.");
        return;
    }

    offset = sprintf_s((char *)offline_buffer + offset, OFFLINE_BUFFER_SIZE - offset,
                       "\r\nMemory Usage Report\nTotal: %u bytes\nUsed: %u bytes\nFree: %u bytes\n"
                       "Free Nodes: %u\nUsed Nodes: %u\nMax Free Block: %u bytes\nPeak Size: %u bytes\n",
                       info->total, info->used, info->free, info->free_node_num, info->used_node_num,
                       info->max_free_node_size, info->peek_size);

    write_to_file((char *)offline_buffer, (size_t)offset);
    dfx_free(0, offline_buffer);
}
#endif /* CONFIG_DFX_SUPPORT_FILE_SYSTEM */

errcode_t diag_cmd_get_mem_info(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    errcode_t ret;
    mdm_mem_info_t info;

    unused(cmd_param);
    unused(cmd_param_size);

    ret = dfx_mem_get_sys_pool_info(&info);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if (get_offline_mode() == OFFLINE_MODE_SERIAL) {
        uapi_diag_report_packet(cmd_id, option, (uint8_t *)&info, (uint16_t)sizeof(mdm_mem_info_t), true);
    } else {
    #if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
        save_mem_info_to_file(&info);
    #endif /* CONFIG_DFX_SUPPORT_FILE_SYSTEM */
        mdm_mem_info_t mem_info_null_packet = {0};
        uapi_diag_report_packet(cmd_id, option, (const uint8_t *)(&mem_info_null_packet),
                                sizeof(mem_info_null_packet), true);
    }
    return ERRCODE_SUCC;
}