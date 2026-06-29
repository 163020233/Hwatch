/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LiteOS USB Driver KalIrq HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2024-02-28
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#ifndef KAL_IRQ_H
#define KAL_IRQ_H

#if defined(__LITEOS__)
#include <los_hwi.h>
#elif defined(__NONOS__)
#include "stdint.h"
#include "common_def.h"
#include "osal_interrupt.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BUS_IRQ_DEFAULT_PRIORITY  0x0

typedef int (*KalIrqHandler)(uint32_t irqNum, void *arg);

#if defined(__LITEOS__)
static inline uint32_t KAL_IrqRegister(uint32_t irqNum, KalIrqHandler irqHandler, void *arg)
{
	HWI_IRQ_PARAM_S irqParam;

	irqParam.swIrq = irqNum;
	irqParam.pDevId = arg;
	irqParam.pName = NULL;

	return LOS_HwiCreate(irqNum, BUS_IRQ_DEFAULT_PRIORITY, 0, (HWI_PROC_FUNC)irqHandler, &irqParam);
}

static inline uint32_t KAL_IrqUnregister(uint32_t irqNum)
{
    HWI_IRQ_PARAM_S irqParam;

    irqParam.swIrq = irqNum;
	irqParam.pDevId = NULL;

    return LOS_HwiDelete((HWI_HANDLE_T)irqNum, &irqParam);
}

static inline uint32_t KAL_IrqEnable(uint32_t irqNum)
{
    return LOS_HwiEnable(irqNum);
}

static inline uint32_t KAL_IrqDisable(uint32_t irqNum)
{
    return LOS_HwiDisable(irqNum);
}

#elif defined(__NONOS__)

static inline uint32_t KAL_IrqRegister(uint32_t irqNum, KalIrqHandler irqHandler, void *arg)
{
    return osal_irq_request(irqNum, (osal_irq_handler)irqHandler, NULL, NULL, NULL);
}
 
static inline uint32_t KAL_IrqUnregister(uint32_t irqNum)
{
    osal_irq_free(irqNum, NULL);
    return 0;
}
 
static inline uint32_t KAL_IrqEnable(uint32_t irqNum)
{
    osal_irq_set_priority(irqNum, 1);
    osal_irq_enable(irqNum);
    return 0;
}
 
static inline uint32_t KAL_IrqDisable(uint32_t irqNum)
{
    osal_irq_disable(irqNum);
    return 0;
}
 
static inline uint32_t KAL_IntLock(uint32_t irqNum)
{
    osal_irq_disable(irqNum);
    return 0;
}

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* KAL_IRQ_H */