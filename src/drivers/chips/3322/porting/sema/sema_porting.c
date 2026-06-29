/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.  \n
 *
 * Description: Provides sema port \n
 */
#include "sema_porting.h"
#include "soc_osal.h"
#include "hal_sema.h"

void sema_porting_init(void)
{
}

void sema_porting_deinit(void)
{
}

bool sema_check_index_is_valid(uint8_t sema_index)
{
    return (bool)(sema_index >= SEMA_BUTT);
}

uint32_t sema_get_sem_status_regaddr(uint8_t sema_index)
{
    return SEMA_CTL_BASE_ADDR + sema_index * SEMA_SIZE + SEMA_STS_OFFSET;
}

uint32_t sema_get_sem_forceclr_regaddr(uint8_t sema_index)
{
    return SEMA_CTL_BASE_ADDR + sema_index * SEMA_SIZE + SEMA_FRC_CLR_OFFSET;
}

errcode_t sema_porting_get(uint8_t sema_index, uint32_t wait_time_us)
{
    uint32_t irq_sts = osal_irq_lock();
    errcode_t ret = hal_sema_get_sem(sema_index, wait_time_us);
    osal_irq_restore(irq_sts);

    return ret;
}

errcode_t sema_porting_force_clear(uint8_t sema_index)
{
    uint32_t irq_sts = osal_irq_lock();
    errcode_t ret = hal_sema_force_clear(sema_index);
    osal_irq_restore(irq_sts);

    return ret;
}

errcode_t sema_porting_put(uint8_t sema_index)
{
    uint32_t irq_sts = osal_irq_lock();
    /* 1A21锁存释放和获取的标记位连续，偏移8位即可得到释放标记位 */
    errcode_t ret = hal_sema_put_sem(sema_index);
    osal_irq_restore(irq_sts);

    return ret;
}
