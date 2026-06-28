/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: traceui buffer manager implementation
 * Author: Hisi Graphic Team
 * Created: 2025-9
 */

#include "osal_spinlock.h"
#include "los_typedef.h"
#include "los_memory.h"
#include "los_spinlock.h"
#include "traceui_buf.h"

static uint8_t *g_traceBuf = NULL;
static uint32_t g_traceBufPos = 0;
static uint32_t g_traceBufSize = 0;
static osal_spinlock g_traceuiSpin;

void TraceuiBufInit(uint8_t *buf, uint32_t size)
{
    unsigned long irqState;

    osal_spin_lock_irqsave(&g_traceuiSpin, &irqState);
    g_traceBuf = buf;
    g_traceBufPos = 0;
    g_traceBufSize = size;
    osal_spin_unlock_irqrestore(&g_traceuiSpin, &irqState);
}

uint8_t *TraceuiGetBuf(uint32_t len)
{
    unsigned long irqState;
    uint8_t *retBuf = NULL;

    osal_spin_lock_irqsave(&g_traceuiSpin, &irqState);
    if (g_traceBufPos + len <= g_traceBufSize) {
        retBuf = &g_traceBuf[g_traceBufPos];
        g_traceBufPos += len;
    }
    osal_spin_unlock_irqrestore(&g_traceuiSpin, &irqState);
    return retBuf;
}

uint32_t TraceuiGetBufUsedSize(void)
{
    return g_traceBufPos;
}

void TraceuiBufDeinit(void)
{
    unsigned long irqState;

    osal_spin_lock_irqsave(&g_traceuiSpin, &irqState);
    g_traceBuf = NULL;
    g_traceBufPos = 0;
    g_traceBufSize = 0;
    osal_spin_unlock_irqrestore(&g_traceuiSpin, &irqState);
}
