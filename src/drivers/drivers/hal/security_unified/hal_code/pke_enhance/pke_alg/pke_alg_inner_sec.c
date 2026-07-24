/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke alg inner implement.
 *
 * Create: 2025-01-07
*/
#include "pke_alg_inner.h"

#include "hal_pke_romable.h"
#include "hal_pke_func.h"
#include "hal_pke_alg.h"
#include "crypto_drv_common.h"
#include "pke_alg_inner.h"

#define FILE_ID FILE_ID_HAL_CODE_PKE_ENHANCE_PKE_ALG_PKE_ALG_INNER_SEC

int inner_pke_alg_resume_sec(void)
{
    volatile int ret = CRYPTO_ENHANCE_FAILURE;

    ret = hal_pke_lock_romable();
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_lock_romable failed\n");

    hal_pke_enable_noise();

    ret = hal_pke_set_dram_mask();
    drv_err_chk_return(ret != CRYPTO_ENHANCE_SUCCESS, ret, "hal_pke_set_dram_mask failed\n");

    return CRYPTO_ENHANCE_SUCCESS;
}

void inner_pke_alg_suspend_sec(void)
{
    hal_pke_clean_dram_mask();
    hal_pke_disable_noise();
    hal_pke_unlock_romable();
}

int inner_batch_instr_process_sec(const rom_lib *batch_instr, unsigned int work_len)
{
    crypto_unused(work_len);
    return hal_pke_batch_compute_sec(batch_instr->instr_addr, batch_instr->instr_num);
}