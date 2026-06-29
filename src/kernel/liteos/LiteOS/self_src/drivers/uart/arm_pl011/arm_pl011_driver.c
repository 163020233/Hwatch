/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Arm Pl011 Driver
 * Author: Huawei LiteOS Team
 * Create: 2022-09-02
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

#include "arm_pl011.h"
#include "uart_driver.h"
#include "uart_pri.h"
#include "uart.h"
#include "soc/uart.h"
#include "los_hwi.h"
#include "los_memory.h"
#include "los_printf.h"
#include "los_base.h"
#ifdef LOSCFG_FS_VFS
#include "uart_vfs.h"
#endif

#ifdef LOSCFG_ENABLE_MAGICKEY
#include "magickey.h"
#endif
#define UART_DATA_NOTIFY_EVENT      0x1
#define RX_BUF_SIZE                 128
#define UART_RECV_RINGBUF_SIZE      (1024 * 2) /* 2K */
#ifdef LOSCFG_FS_VFS
#define MAX_DEV_NAME_SIZE           32
#define UART_DRIVER_MODE            0666
#endif

STATIC VOID Pl011IrqHandler(INT32 irq, VOID *arg)
{
    CHAR buf[RX_BUF_SIZE];
    UINT32 rxFifoEmpty;
    UINT32 status;
    UINT32 count = 0;
    struct LosDevice *dev = (struct LosDevice *)arg;
    struct UartDriverData *uartData = (struct UartDriverData *)dev->data;
    Pl011PriData *priData = NULL;

    (VOID)irq;
    LOS_ASSERT(uartData != NULL);
    priData = (Pl011PriData *)uartData->privateData;
    LOS_ASSERT(priData != NULL);

    LOS_ASSERT(dev->cfg.regs != NULL);
    status = Pl011ReadMIS(dev->cfg.regs->base);
    if ((status & (UART_MIS_RX | UART_IMSC_TIMEOUT)) != 0) {
        do {
            rxFifoEmpty = Pl011ReadFR(dev->cfg.regs->base);
            if (rxFifoEmpty & UART_FR_RXFE) {
                break;
            }

            if (count < RX_BUF_SIZE) {
                buf[count++] = Pl011ReadData(dev->cfg.regs->base);
#ifdef LOSCFG_ENABLE_MAGICKEY
                if (CheckMagicKey(buf[count - 1])) {
                    continue;
                }
#endif
            } else {
                /* All data in the FIFO must be read out, otherwise the interrupt cannot exit. */
                buf[RX_BUF_SIZE - 1] = Pl011ReadData(dev->cfg.regs->base);
            }
        } while (1);

        (VOID)LOS_RingbufWrite(&priData->recvData, buf, count);
        (VOID)LOS_EventWrite(&priData->event, UART_DATA_NOTIFY_EVENT);
    }

    Pl011ClearAllInterrupt(dev->cfg.regs->base);
}

STATIC INT32 Pl011Startup(struct LosDevice *dev)
{
    UINT32 ret;
    CHAR *buf = NULL;
    struct UartDriverData *uartData = (struct UartDriverData *)dev->data;
    Pl011PriData *priData = (Pl011PriData *)uartData->privateData;

    if (priData == NULL) {
        return -1;
    }

    ret = LOS_EventInit(&priData->event);
    if (ret != LOS_OK) {
        goto OUT_INIT_EVENT_FAIL;
    }

    buf = (CHAR *)LOS_MemAlloc(m_aucSysMem1, UART_RECV_RINGBUF_SIZE);
    if (buf == NULL) {
        goto OUT_MALLOC_FAIL;
    }

    (VOID)memset(buf, 0, UART_RECV_RINGBUF_SIZE);
    priData->recvData.status = RBUF_UNINIT;
    ret = LOS_RingbufInit(&priData->recvData, buf, UART_RECV_RINGBUF_SIZE, RBUF_NORMAL);
    if (ret != LOS_OK) {
        goto OUT_INIT_RINGBUF_FAIL;
    }

    ret = LOS_HwiEnable((HWI_HANDLE_T)dev->cfg.irqNum);
    if (ret != LOS_OK) {
        goto OUT_ENABLE_HWI_FAIL;
    }

    /* enable the clock */
    LOS_TaskLock();
    Pl011UartClkCfg(uartData->num, TRUE);
    LOS_TaskUnlock();

    UART_CORE_FUNC_CALL(pl011, UartStartupCfg, dev->cfg.regs->base);

    return 0;

OUT_ENABLE_HWI_FAIL:
    LOS_RingbufReset(&priData->recvData);
OUT_INIT_RINGBUF_FAIL:
    (VOID)LOS_MemFree(m_aucSysMem1, (VOID *)buf);
OUT_MALLOC_FAIL:
    (VOID)LOS_EventDestroy(&priData->event);
OUT_INIT_EVENT_FAIL:
    PRINTK("%s failed! ret: 0x%x\n", __FUNCTION__, ret);
    return -1;
}

STATIC INT32 Pl011Shutdown(struct LosDevice *dev)
{
    CHAR *buf = NULL;
    struct UartDriverData *uartData = (struct UartDriverData *)dev->data;
    Pl011PriData *priData = (Pl011PriData *)uartData->privateData;

    if (priData == NULL) {
        return -1;
    }

    UART_CORE_FUNC_CALL(pl011, UartSuspendCfg, dev->cfg.regs->base);

    /* disable the clock */
    LOS_TaskLock();
    Pl011UartClkCfg(uartData->num, FALSE);
    LOS_TaskUnlock();

    (VOID)LOS_HwiDisable((HWI_HANDLE_T)dev->cfg.irqNum);

    buf = priData->recvData.fifo;
    LOS_RingbufReset(&priData->recvData);
    (VOID)LOS_MemFree(m_aucSysMem1, buf);

    (VOID)LOS_EventDestroy(&priData->event);

    return 0;
}

STATIC ssize_t Pl011Read(struct LosDevice *dev, char *buf, size_t count)
{
    UINT32 ret;
    struct UartDriverData *uartData = (struct UartDriverData *)dev->data;
    Pl011PriData *priData = (Pl011PriData *)uartData->privateData;

    if (priData == NULL) {
        return -1;
    }

    while (LOS_RingbufUsedSize(&priData->recvData) < count) {
        ret = LOS_EventRead(&priData->event, UART_DATA_NOTIFY_EVENT,
                            LOS_WAITMODE_CLR | LOS_WAITMODE_AND, LOS_WAIT_FOREVER);
        if (ret != UART_DATA_NOTIFY_EVENT) {
            return -1;
        }
    }

    ret = LOS_RingbufRead(&priData->recvData, buf, (UINT32)count);
    return (ssize_t)ret;
}

STATIC ssize_t Pl011Transmit(struct LosDevice *dev, const char *buf, size_t count)
{
#ifdef LOSCFG_CONSOLE_UNIFIED_SERIAL_OUTPUT
    UART_CORE_FUNC_CALL(pl011, UartPutsReg, dev->cfg.regs->base, buf, (UINT32)count, UART_WITHOUT_LOCK);
#else
    UART_CORE_FUNC_CALL(pl011, UartPutsReg, dev->cfg.regs->base, buf, (UINT32)count, UART_WITH_LOCK);
#endif
    return (ssize_t)count;
}

STATIC INT32 Pl011Config(struct LosDevice *dev, unsigned long arg)
{
    (VOID)dev;
    (VOID)arg;
    return 0;
}

STATIC INT32 Pl011Suspend(struct LosDevice *dev)
{
    UART_CORE_FUNC_CALL(pl011, UartSuspendCfg, dev->cfg.regs->base);

    /* disable the clock */
    LOS_TaskLock();
    Pl011UartClkCfg(((struct UartDriverData *)dev->data)->num, FALSE);
    LOS_TaskUnlock();
    return 0;
}

STATIC INT32 Pl011Resume(struct LosDevice *dev)
{
    /* enable the clock */
    LOS_TaskLock();
    Pl011UartClkCfg(((struct UartDriverData *)dev->data)->num, TRUE);
    LOS_TaskUnlock();

    UART_CORE_FUNC_CALL(pl011, UartStartupCfg, dev->cfg.regs->base);
    return 0;
}

STATIC INT32 Pl011Ioctl(struct LosDevice *dev, INT32 cmd, unsigned long arg)
{
    INT32 ret;
    struct UartDriverData *uartData = (struct UartDriverData *)dev->data;

    switch (cmd) {
        case UART_IOCTL_SUSPEND:
            if ((uartData->state & UART_SUSPEND) != 0) {
                return -1;
            }
            ret = Pl011Suspend(dev);
            if (ret == 0) {
                uartData->state |= UART_SUSPEND;
            }
            break;
        case UART_IOCTL_RESUME:
            if ((uartData->state & UART_SUSPEND) == 0) {
                return -1;
            }
            ret = Pl011Resume(dev);
            if (ret == 0) {
                uartData->state &= ~UART_SUSPEND;
            }
            break;
        default:
            ret = -1;
            break;
    }

    return ret;
}

STATIC struct UartOps g_pl011Ops = {
    .startup    = Pl011Startup,
    .shutdown   = Pl011Shutdown,
    .read       = Pl011Read,
    .transmit   = Pl011Transmit,
    .config     = Pl011Config,
    .suspend    = Pl011Suspend,
    .resume     = Pl011Resume,
    .ioctl      = Pl011Ioctl,
};

STATIC INT32 Pl011Probe(struct LosDevice *dev)
{
    struct UartDriverData *uartData = (struct UartDriverData *)dev->data;
    Pl011PriData *priData = NULL;
    UINT32 ret;
    HWI_IRQ_PARAM_S irqParam = {0};

    if (uartData == NULL) {
        return -1;
    }

    if (uartData->num >= UART_MAX_COUNTS) {
        PRINT_ERR("The uart number:%u is invalid!\n", uartData->num);
        return -1;
    }

    priData = (Pl011PriData *)LOS_MemAlloc(m_aucSysMem1, sizeof(Pl011PriData));
    if (priData == NULL) {
        PRINT_ERR("%s LOS_MemAlloc failed!\n", __FUNCTION__);
        return -1;
    }
    uartData->privateData = (VOID *)priData;
    uartData->ops = &g_pl011Ops;

#ifdef LOSCFG_FS_VFS
    CHAR devName[MAX_DEV_NAME_SIZE] = {0};
    if (snprintf_s(devName, MAX_DEV_NAME_SIZE, MAX_DEV_NAME_SIZE - 1, "/dev/uartdev-%d", dev->id) == -1) {
        PRINT_ERR("get dev name failed!\n");
        goto OUT_REGISTER_DRIVER_FAIL;
    }

    if (register_driver(devName, &g_uartDevFops, UART_DRIVER_MODE, (VOID *)dev)) {
        PRINT_ERR("register /dev/uartdev-%d failed!\n", dev->id);
        goto OUT_REGISTER_DRIVER_FAIL;
    }
#endif

    irqParam.swIrq  = (INT32)dev->cfg.irqNum;
    irqParam.pDevId = (VOID *)dev;
    irqParam.pName  = dev->name;
    ret = LOS_HwiCreate((HWI_HANDLE_T)dev->cfg.irqNum, 0, 0, (HWI_PROC_FUNC)Pl011IrqHandler, &irqParam);
    if (ret != LOS_OK) {
        goto OUT_CREATE_HWI_FAIL;
    }

    LOS_SpinInit(&uartData->lock);
    g_uartDevs[uartData->num] = dev;
    return 0;

OUT_CREATE_HWI_FAIL:

#ifdef LOSCFG_FS_VFS
    (VOID)unregister_driver(devName);
OUT_REGISTER_DRIVER_FAIL:
#endif

    (VOID)LOS_MemFree(m_aucSysMem1, priData);
    return -1;
}

STATIC INT32 Pl011Remove(struct LosDevice *dev)
{
    struct UartDriverData *uartData = (struct UartDriverData *)dev->data;
    Pl011PriData *priData = NULL;
    HWI_IRQ_PARAM_S irqParam = {0};
    UINT32 intSave;
    INT32 ret = 0;

    if (uartData == NULL) {
        return -1;
    }

    LOS_SpinLockSave(&uartData->lock, &intSave);
    if ((uartData->state & UART_OPEN) != 0) {
        ret = -1;
        goto UNLOCK_OUT;
    }

    priData = (Pl011PriData *)uartData->privateData;
    if (priData == NULL) {
        ret = -1;
        goto UNLOCK_OUT;
    }

#ifdef LOSCFG_FS_VFS
    CHAR devName[MAX_DEV_NAME_SIZE] = {0};
    if (snprintf_s(devName, MAX_DEV_NAME_SIZE, MAX_DEV_NAME_SIZE - 1, "/dev/uartdev-%d", dev->id) == -1) {
        PRINT_ERR("get dev name failed!\n");
    } else if (unregister_driver(devName)) {
        PRINT_ERR("unregister /dev/uartdev-%d failed!\n", dev->id);
    }
#endif

    irqParam.swIrq  = (INT32)dev->cfg.irqNum;
    irqParam.pDevId = (VOID *)dev;
    irqParam.pName  = dev->name;
    (VOID)LOS_HwiDelete((HWI_HANDLE_T)dev->cfg.irqNum, &irqParam);

    g_uartDevs[uartData->num] = NULL;

    (VOID)LOS_MemFree(m_aucSysMem1, (VOID *)priData);
    uartData->privateData = NULL;

UNLOCK_OUT:
    LOS_SpinUnlockRestore(&uartData->lock, intSave);
    return ret;
}

struct LosDriver g_pl011Driver = {
    .name = "uart-pl011",
    .ops = {
        .probe  = Pl011Probe,
        .remove = Pl011Remove,
    },
    .pmOps = {
        .suspend = Pl011Suspend,
        .resume  = Pl011Resume,
    },
};
