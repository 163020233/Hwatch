/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: At npu register \n
 *
 */

#include "at_npu_cmd_register.h"

#include "soc_errno.h"
#include "at_npu.h"
#include "at.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

uint32_t uapi_at_npu_register_cmd(const at_cmd_entry_t *cmd_tbl, uint16_t cmd_num)
{
    uint32_t ret = EXT_ERR_FAILURE;

    ret = uapi_at_cmd_table_register(cmd_tbl, (uint32_t)cmd_num, EXT_AT_NPU_CMD_MAX_LEN);
    if (ret != 0) {
        uapi_at_report_to_single_channel(0, "uapi_at_npu_register_cmd \r\n");
    }
    return ret;
}
// npu at命令注册接口
void at_npu_cmd_register(void)
{
    los_at_npu_cmd_register();
}
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
