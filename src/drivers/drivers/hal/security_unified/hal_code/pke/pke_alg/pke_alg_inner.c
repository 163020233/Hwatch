/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke alg inner implement.
 *
 * Create: 2025-01-07
*/
#include "pke_alg_inner.h"

#include "hal_pke_romable.h"
#include "hal_pke_alg.h"
#include "crypto_drv_common.h"
#include "pke_alg_inner.h"

#define FILE_ID FILE_ID_HAL_CODE_PKE_PKE_ALG_PKE_ALG_INNER

int inner_pke_alg_resume(void)
{
    return hal_pke_lock_romable();
}

void inner_pke_alg_suspend(void)
{
    hal_pke_unlock_romable();
}

int inner_batch_instr_process_opt(const rom_lib *batch_instr)
{
    return hal_pke_batch_compute_romable(batch_instr->instr_addr, batch_instr->instr_num);
}

int __attribute__((weak)) inner_batch_instr_process(const rom_lib *batch_instr, unsigned int work_len)
{
    crypto_unused(work_len);
    return hal_pke_batch_compute_romable(batch_instr->instr_addr, batch_instr->instr_num);
}