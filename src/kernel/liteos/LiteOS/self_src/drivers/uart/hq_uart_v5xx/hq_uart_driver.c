/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description : Uart-V500 Driver Implementation
 * Author : Huawei LiteOS Team
 * Create : 2022-11-8
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
#include "los_event.h"
#include "los_ringbuf.h"
#include "los_driverbase.h"
#include "uart.h"
#include "uart_pri.h"
#include "uart_driver.h"
#include "soc/uart.h"
#include "hq_uart.h"

STATIC VOID UartIrqHandler(UINT32 hwiNum, VOID *args)
{
    (VOID)hwiNum;
    UINT32 rxFifo;
    struct LosDevice *dev = args;
    struct UartDriverData *uartData = (struct UartDriverData *)dev->data;
    UartV500PriData *privData = (UartV500PriData *)uartData->privateData;
    /* mask all int */
    UINT32 mask = GET_UINT32(dev->cfg.regs->base + UART_INT_MSK);
    WRITE_UINT32(UART_MASK_ALL_INTER, dev->cfg.regs->base + UART_INT_MSK);
    /* clear all int */
    WRITE_UINT32(UART_CLEAR_ALL_INTER, dev->cfg.regs->base + UART_INT_CLR);
    while (!(GET_UINT32(dev->cfg.regs->base + UART_STAT) & UART_STAT_RX_FIFO_EMPTY)) {
        rxFifo = GET_UINT32(dev->cfg.regs->base + UART_RX_FIFO);
        if (rxFifo & UART_RX_FIFO_ERR) {
            continue;
        }
        UartRecvData(privData, (CHAR)(rxFifo & UART_RX_FIFO_DATA));
    }
    /* resume interrupt mask */
    WRITE_UINT32(mask, dev->cfg.regs->base + UART_INT_MSK);
    UartDataNotify(privData);
}

STATIC INT32 UartV500Startup(struct LosDevice *dev)
{
    UINT32 ret = LOS_HwiEnable((HWI_HANDLE_T)dev->cfg.irqNum);
    return ((ret == LOS_OK) ? 0 : -1);
}

STATIC INT32 UartV500Shutdown(struct LosDevice *dev)
{
    UINT32 ret = LOS_HwiDisable((HWI_HANDLE_T)dev->cfg.irqNum);
    return ((ret == LOS_OK) ? 0 : -1);
}

STATIC ssize_t UartV500Read(struct LosDevice *dev, char *buf, size_t count)
{
    struct UartDriverData *uartData = (struct UartDriverData *)dev->data;
    UartV500PriData *privData = (UartV500PriData *)uartData->privateData;
    if (privData == NULL) {
        return -1;
    }

    if (LOS_RingbufUsedSize(&privData->ringbuf) < count) {
        if (LOS_EventRead(&privData->event, UART_DATA_NOTIFY_EVENT,
            LOS_WAITMODE_CLR | LOS_WAITMODE_AND, LOS_WAIT_FOREVER) != UART_DATA_NOTIFY_EVENT) {
            return -1;
        }
    }
    return (ssize_t)LOS_RingbufRead(&privData->ringbuf, buf, (UINT32)count);
}

STATIC ssize_t UartV500Transmit(struct LosDevice *dev, const char *buf, size_t count)
{
    (VOID)UART_CORE_FUNC_CALL(hquart, UartPutsReg, dev->cfg.regs->base, buf, (UINT32)count, UART_WITH_LOCK);
    return (ssize_t)count;
}

STATIC INT32 UartV500Ioctl(struct LosDevice *dev, INT32 cmd, unsigned long arg)
{
#ifdef LOSCFG_KERNEL_SMP
    UINT32 ret;
    if ((cmd != UART_IOCTL_SET_AFFINITY) || (arg == 0) ||
        (((UINT32)arg & LOSCFG_KERNEL_CPU_MASK) != arg)) {
        return -1;
    }
    ret = LOS_HwiSetAffinity((HWI_HANDLE_T)dev->cfg.irqNum, (UINT32)arg);
    return ((ret == LOS_OK) ? 0 : -1);
#else
    return -1;
#endif
}

STATIC struct UartOps g_uartV500Ops = {
    .startup    = UartV500Startup,
    .shutdown   = UartV500Shutdown,
    .read       = UartV500Read,
    .transmit   = UartV500Transmit,
    .ioctl      = UartV500Ioctl,
};

STATIC BOOL CheckUartDataOk(struct UartDriverData *uartData)
{
    return ((uartData != NULL) && (uartData->num < UART_MAX_COUNTS));
}

STATIC UartV500PriData *UartV500PriDataCreate(VOID)
{
    UINT32 ret;
    CHAR *buf = NULL;
    UartV500PriData *privData = (UartV500PriData *)LOS_MemAlloc(m_aucSysMem1, sizeof(UartV500PriData));
    if (privData == NULL) {
        PRINT_ERR("%s: malloc private data failed\n", __FUNCTION__);
        return NULL;
    }
    (VOID)memset((VOID *)privData, 0, sizeof(UartV500PriData));
    ret = LOS_EventInit(&privData->event);
    if (ret != LOS_OK) {
        PRINT_ERR("%s: create event failed, code 0x%x\n", __FUNCTION__, ret);
        goto OUT_INIT_EVENT_FAIL;
    }
    buf = (CHAR *)LOS_MemAlloc(m_aucSysMem1, UART_RECV_RINGBUF_SIZE);
    if (buf == NULL) {
        PRINT_ERR("%s: malloc buf failed\n", __FUNCTION__);
        goto OUT_MALLOC_FAIL;
    }
    ret = LOS_RingbufInit(&privData->ringbuf, buf, UART_RECV_RINGBUF_SIZE, RBUF_NORMAL);
    if (ret != LOS_OK) {
        PRINT_ERR("%s: ringbuf init failed, code 0x%x\n", __FUNCTION__, ret);
        goto OUT_INIT_RINGBUF_FAIL;
    }

    return privData;

OUT_INIT_RINGBUF_FAIL:
    (VOID)LOS_MemFree(m_aucSysMem1, buf);
OUT_MALLOC_FAIL:
    (VOID)LOS_EventDestroy(&privData->event);
OUT_INIT_EVENT_FAIL:
    (VOID)LOS_MemFree(m_aucSysMem1, privData);

    return NULL;
}

STATIC VOID UartV500PriDataDestroy(UartV500PriData *privData)
{
    if (privData == NULL) {
        return;
    }

    LOS_RingbufReset(&privData->ringbuf);
    (VOID)LOS_MemFree(m_aucSysMem1, privData->ringbuf.fifo);
    privData->ringbuf.fifo = NULL;
    (VOID)LOS_EventDestroy(&privData->event);
    (VOID)LOS_MemFree(m_aucSysMem1, privData);
}

STATIC INT32 UartV500Probe(struct LosDevice *dev)
{
    UINT32 ret;
    struct UartDriverData *uartData = (struct UartDriverData *)dev->data;
    UartV500PriData *privData = NULL;
    HWI_IRQ_PARAM_S irqParam = {0};

    if (CheckUartDataOk(uartData) != TRUE) {
        PRINT_ERR("%s: invalid uart data\n", __FUNCTION__);
        return -1;
    }
    privData = UartV500PriDataCreate();
    if (privData == NULL) {
        PRINT_ERR("%s: create private data failed\n", __FUNCTION__);
        return -1;
    }

    irqParam.swIrq  = (INT32)dev->cfg.irqNum;
    irqParam.pDevId = (VOID *)dev;
    irqParam.pName  = dev->name;
    ret = LOS_HwiCreate((HWI_HANDLE_T)dev->cfg.irqNum, 0, 0, (HWI_PROC_FUNC)UartIrqHandler, &irqParam);
    if (ret != LOS_OK) {
        PRINT_ERR("%s: hwi create failed, code 0x%x\n", __FUNCTION__, ret);
        UartV500PriDataDestroy(privData);
        return -1;
    }

    LOS_SpinInit(&uartData->lock);
    uartData->ops = &g_uartV500Ops;
    uartData->privateData = privData;
    g_uartDevs[uartData->num] = dev;

    return 0;
}

STATIC INT32 UartV500Remove(struct LosDevice *dev)
{
    UINT32 intSave;
    HWI_IRQ_PARAM_S irqParam = {0};
    struct UartDriverData *uartData = (struct UartDriverData *)dev->data;
    if (CheckUartDataOk(uartData) != TRUE) {
        return -1;
    }

    LOS_SpinLockSave(&uartData->lock, &intSave);

    irqParam.swIrq  = (INT32)dev->cfg.irqNum;
    irqParam.pDevId = (VOID *)dev;
    irqParam.pName  = dev->name;
    (VOID)LOS_HwiDelete((HWI_HANDLE_T)dev->cfg.irqNum, &irqParam);

    UartV500PriDataDestroy(uartData->privateData);

    uartData->privateData = NULL;
    g_uartDevs[uartData->num] = NULL;

    LOS_SpinUnlockRestore(&uartData->lock, intSave);

    return 0;
}

struct LosDriver g_uartV500Driver = {
    .name = "uart-v500",
    .ops = {
        .probe  = UartV500Probe,
        .remove = UartV500Remove,
    }
};
