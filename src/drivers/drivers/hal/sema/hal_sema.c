/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. \n
 *
 * Description: Provides HAL sema \n
 */

#include "hal_sema.h"
#include "common_def.h"
#include "sema_porting.h"
#include "tcxo.h"

errcode_t hal_sema_get_sem(uint8_t sema_index, uint32_t wait_time_us)
{
    uint64_t time_init;
    uint64_t time_cur;

    time_init = uapi_tcxo_get_us();

    /* 各子系统先通过sema_set写入1抢占信号量，回读状态位判断是否抢占成功 */
    uapi_reg_setbit16(sema_get_sem_status_regaddr(sema_index), SEMA_CTRL_SUBSYS_MASK);
    while (uapi_reg_getbits16(sema_get_sem_status_regaddr(sema_index), SEMA_STS_SUBSYS_MASK, 1) == 0) {
        time_cur = uapi_tcxo_get_us();
        if (wait_time_us < (uint32_t)(time_cur - time_init)) {
            return ERRCODE_FAIL;
        }
        uapi_reg_setbit16(sema_get_sem_status_regaddr(sema_index), SEMA_CTRL_SUBSYS_MASK);
    }

    return ERRCODE_SUCC;
}

errcode_t hal_sema_put_sem(uint8_t sema_index)
{
    uapi_reg_setbit16(sema_get_sem_forceclr_regaddr(sema_index), SEMA_CTRL_SUBSYS_MASK);
    return ERRCODE_SUCC;
}

errcode_t hal_sema_force_clear(uint8_t sema_index)
{
    uapi_reg_setbit16(sema_get_sem_forceclr_regaddr(sema_index), SEMA_FORCE_CLR_MASK);
    return ERRCODE_SUCC;
}
