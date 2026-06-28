/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: esim \n
 */

#include "at.h"
#include "soc_errno.h"
#include "at_esim_cmd_register.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
uint32_t uapi_at_esim_register_cmd(const at_cmd_entry_t *cmd_tbl, uint16_t cmd_num)
{
    uint32_t ret = EXT_ERR_FAILURE;

    ret = uapi_at_cmd_table_register(cmd_tbl, (uint32_t)cmd_num, EXT_AT_ESIM_CMD_MAX_LEN);
    if (ret != 0) {
        uapi_at_report_to_single_channel(0, "uapi_at_esim_register_cmd fail\r\n");
    }
    return ret;
}

void at_esim_cmd_register(void)
{
    los_at_esim_cmd_register();
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif