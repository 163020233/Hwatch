/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2016-2022. All rights reserved.
*/

#include "soc_osal.h"
#include "osal_errno.h"
#include "los_hwi.h"
#include "arch_barrier.h"

#define OSAL_IRQ_FLAG_PRI1    1
#define OSAL_IRQ_FLAG_PRI2    2
#define OSAL_IRQ_FLAG_PRI3    3
#define OSAL_IRQ_FLAG_PRI4    4
#define OSAL_IRQ_FLAG_PRI5    5
#define OSAL_IRQ_FLAG_PRI6    6
#define OSAL_IRQ_FLAG_PRI7    7
#define OSAL_IRQ_FLAG_PRI_MASK  0x7
#define OSAL_IRQ_FLAG_NOT_IN_FLASH 0x10

#define OSAL_IRQ_FLAG_DEFAULT    OSAL_IRQ_FLAG_NOT_IN_FLASH

#define MAX_IRQ_NAME_LEN 32

#ifndef NULL
#define NULL ((void *)0)
#endif

static OSAL_LIST_HEAD(g_irq_list);

typedef struct irq_info_ {
    int irq;
    int _irq;
    char irq_name[MAX_IRQ_NAME_LEN];
    struct osal_list_head node;
} irq_info;

int osal_irq_request(unsigned int irq, osal_irq_handler handler, osal_irq_handler thread_fn,
                     const char *name, void *dev)
{
    return OSAL_SUCCESS;
}

void osal_irq_free(unsigned int irq, void *dev)
{
    (void)irq;
    (void)dev;
}

int osal_irq_set_affinity(unsigned int irq, const char *name, int cpu_mask)
{
    return 0;
}

void osal_irq_enable(unsigned int irq)
{
    return;
}

/**
 * @par Description:
 * Disable the corresponding interrupt mask of the interrupt controller,
 * so that the interrupt source can be sent to the CPU.
 *
 * @attention
 * This function depends on the hardware implementation of the interrupt controller.
 */
void osal_irq_disable(unsigned int irq)
{
    return;
}

unsigned int osal_irq_lock(void)
{
    unsigned int ret = LOS_IntLock();
    dsb();
    return ret;
}

void osal_irq_restore(unsigned int irq_status)
{
    LOS_IntRestore(irq_status);
    dsb();
}

/* tasklet is running only on one CPU simultaneously */
int osal_tasklet_init(osal_tasklet *tasklet)
{
    return 0;
}

/*
 * tasklet update should be called after tasklet init
 **/
int osal_tasklet_update(osal_tasklet *tasklet)
{
    return 0;
}

/*
 * add tasklet to tasklet_osal_vec and start tasklet
 **/
int osal_tasklet_schedule(osal_tasklet *tasklet)
{
    return 0;
}

/* kill tasklet */
int osal_tasklet_kill(osal_tasklet *tasklet)
{
    return 0;
}
