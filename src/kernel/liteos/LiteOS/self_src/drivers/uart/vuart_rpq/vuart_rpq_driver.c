/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description : The vUART-RPQ (virtual UART rpqueue) Driver Implementation
 * Author : Huawei LiteOS Team
 * Create : 2023-08-12
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

#include "los_printf.h"
#include "los_spinlock.h"
#include "los_driverbase.h"
#include "los_rpqueue.h"
#include "uart.h"
#include "uart_pri.h"
#include "uart_driver.h"
#include "soc/uart.h"
#include "vuart_rpq.h"

#define VUART_RQP_POLL_DELAY     10

STATIC SPIN_LOCK_INIT(g_vUartRpqLock);
#define VUART_RPQ_LOCK(intSave)          LOS_SpinLockSave(&g_vUartRpqLock, &(intSave))
#define VUART_RPQ_UNLOCK(intSave)        LOS_SpinUnlockRestore(&g_vUartRpqLock, (intSave))
STATIC struct LosDevice *g_currDev;

STATIC INT32 VUartRpqStartup(struct LosDevice *dev)
{
    (VOID)dev;
    return 0;
}

STATIC INT32 VUartRpqShutdown(struct LosDevice *dev)
{
    (VOID)dev;
    return 0;
}

STATIC VOID VUartRpqRpqueueRecv(UINT8 cmd, UINT8 *data, UINT16 len)
{
    struct LosDevice *dev = g_currDev;
    struct UartDriverData *uartData = dev->data;
    VUartRpqPriData *priData = (VUartRpqPriData *)uartData->privateData;

    priData->currRxBuff = data;
    priData->currBufSize = (size_t)len;
    priData->currBufIdx = 0;
}

STATIC ssize_t VUartRpqRead(struct LosDevice *dev, char *buf, size_t count)
{
    UINT32 intSave;
    UINT32 ret;
    struct UartDriverData *uartData = dev->data;
    VUartRpqPriData *priData = (VUartRpqPriData *)uartData->privateData;
    size_t copySize;
    errno_t err;

    while ((priData->currRxBuff == NULL) || (priData->currBufIdx >= priData->currBufSize)) {
        VUART_RPQ_LOCK(intSave);
        g_currDev = dev;
        ret = LOS_RpqueueRecv((VOID *)priData->rxChannel, VUartRpqRpqueueRecv);
        VUART_RPQ_UNLOCK(intSave);

        if (ret != LOS_OK) {
            (VOID)LOS_TaskDelay(LOS_MS2Tick(VUART_RQP_POLL_DELAY));
        }
    }

    copySize = priData->currBufSize - priData->currBufIdx;
    copySize = copySize <= count ? copySize : count;
    err = memcpy_s(buf, count, priData->currRxBuff + priData->currBufIdx, copySize);
    if (err != EOK) {
        return -err;
    }
    priData->currBufIdx += copySize;
    return (ssize_t)copySize;
}

STATIC ssize_t VUartRpqTransmit(struct LosDevice *dev, const char *buf, size_t count)
{
    struct UartDriverData *uartData = dev->data;
    VUartRpqPriData *priData = (VUartRpqPriData *)uartData->privateData;
    LosRpqueueSendCfg queCfg = {0};
    UINT32 ret = LOS_RpqueueSend((VOID *)priData->txChannel, (const UINT8 *)buf, (UINT16)count, &queCfg);
    if (ret != LOS_OK) {
        return -1;
    }
    return (ssize_t)count;
}

STATIC struct UartOps g_vUartRpqOps = {
    .startup    = VUartRpqStartup,
    .shutdown   = VUartRpqShutdown,
    .read       = VUartRpqRead,
    .transmit   = VUartRpqTransmit,
};

STATIC INLINE VUartRpqPriData *VUartRpqPriDataCreate(VOID)
{
    VUartRpqPriData *pri = NULL;

    pri = (VUartRpqPriData *)LOS_MemAlloc(m_aucSysMem1, sizeof(*pri));
    if (pri == NULL) {
        return NULL;
    }
    (VOID)memset(pri, 0, sizeof(*pri));
    return pri;
}

STATIC INLINE VOID VUartRpqPriDataDestroy(VUartRpqPriData *pri)
{
    (VOID)LOS_MemFree(m_aucSysMem1, (VOID *)pri);
}

STATIC INLINE BOOL VUartRpqRegCheck(const struct LosDeviceRegs *regs)
{
    return ((regs->base != (UINTPTR)NULL) && (regs->size != 0));
}

STATIC INLINE BOOL VUartRpqDevCheck(const struct LosDevice *dev)
{
    if ((dev == NULL) || (dev->data == NULL) || (dev->cfg.numRegs != VUART_RPQ_TX_CFG_NUM)) {
        return FALSE;
    }

    if (!VUartRpqRegCheck(&dev->cfg.regs[VUART_RPQ_RX_CFG_IDX]) ||
        !VUartRpqRegCheck(&dev->cfg.regs[VUART_RPQ_TX_CFG_IDX]) ||
        (dev->cfg.regs[VUART_RPQ_RX_CFG_IDX].base == dev->cfg.regs[VUART_RPQ_TX_CFG_IDX].base)) {
        return FALSE;
    }

    return TRUE;
}

STATIC INT32 VUartRpqProbe(struct LosDevice *dev)
{
    UINT32 ret;
    struct UartDriverData *uartData = NULL;

    if (!VUartRpqDevCheck(dev)) {
        return -1;
    }

    uartData = (struct UartDriverData *)dev->data;

    ret = LOS_RpqueueInit((VOID *)dev->cfg.regs[VUART_RPQ_RX_CFG_IDX].base,
                          (UINT32)dev->cfg.regs[VUART_RPQ_RX_CFG_IDX].size, 0);
    if ((ret != LOS_OK) && (ret != LOS_ERRNO_RPQUEUE_INITED)) {
        return -1;
    }
    ret = LOS_RpqueueInit((VOID *)dev->cfg.regs[VUART_RPQ_TX_CFG_IDX].base,
                          (UINT32)dev->cfg.regs[VUART_RPQ_TX_CFG_IDX].size, 0);
    if ((ret != LOS_OK) && (ret != LOS_ERRNO_RPQUEUE_INITED)) {
        return -1;
    }

    VUartRpqPriData *priData = VUartRpqPriDataCreate();
    if (priData == NULL) {
        PRINT_ERR("%s: priData create failed\n", __FUNCTION__);
        return -1;
    }

    priData->rxChannel = dev->cfg.regs[VUART_RPQ_RX_CFG_IDX].base;
    priData->rxSize = dev->cfg.regs[VUART_RPQ_RX_CFG_IDX].size;
    priData->txChannel = dev->cfg.regs[VUART_RPQ_TX_CFG_IDX].base;
    priData->txSize = dev->cfg.regs[VUART_RPQ_TX_CFG_IDX].size;
    uartData->privateData = priData;
    uartData->ops = &g_vUartRpqOps;

    LOS_SpinInit(&uartData->lock);
    g_uartDevs[uartData->num] = dev;

    return 0;
}

STATIC INT32 VUartRpqRemove(struct LosDevice *dev)
{
    UINT32 intSave;
    struct UartDriverData *uartData = NULL;

    uartData = (struct UartDriverData *)dev->data;

    LOS_SpinLockSave(&uartData->lock, &intSave);
    VUartRpqPriDataDestroy(uartData->privateData);
    uartData->privateData = NULL;
    g_uartDevs[uartData->num] = NULL;
    LOS_SpinUnlockRestore(&uartData->lock, intSave);

    return LOS_OK;
}

struct LosDriver g_vUartRpqDriver = {
    .name = "vUart-RPQ",
    .ops = {
        .probe  = VUartRpqProbe,
        .remove = VUartRpqRemove,
    }
};
