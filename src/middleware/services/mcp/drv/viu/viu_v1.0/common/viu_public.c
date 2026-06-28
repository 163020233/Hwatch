/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu public function
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#include "viu_public.h"

td_s32 viu_check_null_ptr(const td_void *ptr)
{
    if (ptr == NULL) {
        viu_err_trace("NULL pointer \r\n");
        return SOC_ERR_VIU_INVALID_NULL_PTR;
    }

    return TD_SUCCESS;
}

td_s32 viu_check_mod_id(mcp_soc_mod_id mod_id)
{
    if (mod_id >= SOC_ID_MAX) {
        viu_err_trace("mod_id[%d] is invalid\r\n", mod_id);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    return TD_SUCCESS;
}

#if defined(CONFIG_CHIP_3322)
#else
td_bool read_user_linear_space_valid(td_u8 *addr_start, td_u32 len)   /* from mm_ext.h */
{
    td_u8 check;
    td_u8 *addr_end = TD_NULL;

    if (len == 0) {
        return TD_FALSE;
    }

    if (!mcp_osal_access_ok(MCP_OSAL_VERIFY_READ, addr_start, len)) {
        return TD_FALSE;
    }

    addr_end = addr_start + len - 1;
    if (mcp_osal_copy_from_user(&check, addr_end, 1)) {
        return TD_FALSE;
    }

    return TD_TRUE;
}

td_bool write_user_linear_space_valid(td_u8 *addr_start, td_u32 len)
{
    td_u8 check = 0;
    td_u8 *addr_end = TD_NULL;

    if (len == 0) {
        return TD_FALSE;
    }

    if (!mcp_osal_access_ok(MCP_OSAL_VERIFY_WRITE, addr_start, len)) {
        return TD_FALSE;
    }

    addr_end = addr_start + len - 1;
    if (mcp_osal_copy_to_user(addr_end, &check, 1)) {
        return TD_FALSE;
    }

    return TD_TRUE;
}
#endif

