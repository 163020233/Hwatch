/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: At register \n
 *
 */

#include "at_bt_cmd_register.h"
#include "at_bt_productline.h"
#include "at_bt.h"
#include "at.h"

void bth_sle_common_at_cmd_register(void);
void bth_ble_common_at_cmd_register(void);

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_u32 uapi_at_bt_register_cmd(TD_CONST at_cmd_entry_t *cmd_tbl, td_u16 cmd_num)
{
    td_u32 ret = EXT_ERR_FAILURE;

    ret = uapi_at_cmd_table_register(cmd_tbl, (uint32_t)cmd_num, EXT_AT_BT_CMD_MAX_LEN);
    if (ret != 0) {
        uapi_at_report_to_single_channel(0, "uapi_at_bt_register_cmd \r\n");
    }
    return ret;
}

td_void at_bt_cmd_register(td_void)
{
    bth_sle_common_at_cmd_register(); // SLE AT命令
    bth_ble_common_at_cmd_register(); // BLE AT命令
    los_at_bt_productline_cmd_register(); // 产线BLE AT命令
    los_at_sle_productline_cmd_register(); // 产线SLE AT命令
}
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
