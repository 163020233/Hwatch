/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: NV IPC interface of ACORE
 */
 
#include <errcode.h>
#include <common_def.h>
#include <platform_core.h>
#include <chip_io.h>
#include <nv.h>
#include "nv_ipc.h"
#include "efuse_porting.h"
#include "debug_print.h"

uint16_t nv_ipc_get_xotrim(void)
{
    uint16_t default_value = reg32_getbits(ULP_AON_FUSE_CTL_RB_FUSE_CTL_AUTO_RD_36_REG,
        ULP_AON_FUSE_CTL_RB_CFG_XO_CORE_CTRIM_MAN_OFFSET, ULP_AON_FUSE_CTL_RB_CFG_XO_CORE_CTRIM_MAN_LEN);
#ifdef SUPPORT_XOTRIM_ON_EFUSE
#define XOTRIM_BYTES_LEN        2
#define XOTRIM_DATA_OFFSET      8

    uint8_t data[XOTRIM_BYTES_LEN] = { 0 };
    uint16_t xo_trim0_val = 0;
    errcode_t ret = efuse_read_item(EFUSE_XOTRIM0_VAL_ID, (uint8_t *)&data, (uint16_t)sizeof(data));
    if (ret != ERRCODE_SUCC) {
        PRINT("nv_ipc_get_xotrim efuse read Fail \r\n");
    }
    xo_trim0_val = ((uint16_t)data[0]) | (((uint16_t)data[1]) << XOTRIM_DATA_OFFSET);
    if (xo_trim0_val != 0) {
        return xo_trim0_val;
    }
#else
    uint16_t data = 0;
    uint16_t tlen = 0;
    errcode_t ret = uapi_nv_read(NV_ID_BTSRV_XOTRIM0_CONFIG, sizeof(uint16_t), &tlen, (uint8_t *)&data);
    if (ret == ERRCODE_SUCC) {
        return data;
    }
    unused(tlen);

    nv_key_attr_t attr = { false, false, true, 0 };
    ret = uapi_nv_write_with_attr(NV_ID_BTSRV_XOTRIM0_CONFIG, (const uint8_t *)(&default_value), sizeof(uint16_t),
        &attr, NULL);
    if (ret != ERRCODE_SUCC) {
        return default_value;
    }
#endif

    return default_value;
}

void nv_ipc_set_xotrim(uint16_t value)
{
#define MODEM_CFG_BT_MDM_ECO_REGB 0x598080FC
    reg32_setbits(ULP_AON_FUSE_CTL_RB_FUSE_CTL_AUTO_RD_36_REG, ULP_AON_FUSE_CTL_RB_CFG_XO_CORE_CTRIM_MAN_OFFSET,
        ULP_AON_FUSE_CTL_RB_CFG_XO_CORE_CTRIM_MAN_LEN, value);
    writel(MODEM_CFG_BT_MDM_ECO_REGB, value);
}

void nv_ipc_reset_xotrim(void)
{
    uint16_t data = nv_ipc_get_xotrim();
    nv_ipc_set_xotrim(data);
}