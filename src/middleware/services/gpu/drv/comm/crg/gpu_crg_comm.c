/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. */

#include "gpu_crg_comm.h"
#include "platform_core.h"
#include "vau_reg_define.h"
#include "gpu_dfx.h"

PLT_SEC_ITCM_T td_void gpu_crg_wait_vau_softrst_succ(td_bool reset_req)
{
    td_u32 retry = 0;
    u_vau_soft_rst_state vau_soft_rst_state = {.bits.soft_rst_state = reset_req};
    uapi_reg_read(HAL_VAU_SOFTRST_STATE, vau_soft_rst_state.u32);

    while (vau_soft_rst_state.bits.soft_rst_state != reset_req) {
        uapi_reg_read(HAL_VAU_SOFTRST_STATE, vau_soft_rst_state.u32);
        if (retry++ >= 0xFF) {
            gpu_err("vau hal reset %d fail!", reset_req);
            return;
        }
    }
}