/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: OS Abstract Layer.
 */

#include "osal_interrupt.h"
#include "soc_osal.h"
#include "interrupt.h"
#include "common_def.h"
#include "non_os.h"
#include "oal_interface.h"
#include "irmalloc.h"

unsigned int osal_irq_unlock(void)
{
    non_os_exit_critical();
    return OSAL_SUCCESS;
}

int osal_irq_request(unsigned int irq, osal_irq_handler handler, osal_irq_handler thread_fn,
    const char *name, void *dev)
{
    unused(thread_fn);
    unused(name);
    unused(dev);
    if (handler == NULL) {
        return 1;
    }
    (void)int_set_irq_func((int32_t)irq, (isr_function)handler);

    return OSAL_SUCCESS;
}

int osal_irq_set_priority(unsigned int irq, unsigned short priority)
{
    int_set_priority((int32_t)irq, priority);
    return OSAL_SUCCESS;
}

void osal_irq_enable(unsigned int irq)
{
    int_enable_irq((int32_t)irq);
}

void osal_irq_disable(unsigned int irq)
{
    oal_int_delete(irq);
}

void osal_irq_free(unsigned int irq, void *dev)
{
    unused(dev);
    unused(irq);
}

uint32_t oal_int_create(uint32_t int_num, uint32_t int_prio, oal_int_func func, uint32_t param)
{
    /* software may want to use those unused interrupt,so here we don't check the irq id */
    unused(param);
    if (func == NULL) {
        return OSAL_FAILURE;
    }
    int_set_priority((int32_t)int_num, int_prio);
    (void)int_set_irq_func((int32_t)int_num, func);
    int_enable_irq((int32_t)int_num);
    return OSAL_SUCCESS;
}

uint32_t oal_int_delete(uint32_t int_num)
{
    int_clear_pending_irq((int32_t)int_num);
    int_disable_irq((int32_t)int_num);
    return OSAL_SUCCESS;
}

unsigned int osal_irq_clear(unsigned int vector)
{
    int_clear_pending_irq((int32_t)vector);
    return OSAL_SUCCESS;
}
