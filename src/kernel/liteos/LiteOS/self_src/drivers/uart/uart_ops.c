/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Uart Driver
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

#include "uart.h"
#include "uart_pri.h"
#include "uart_driver.h"

struct LosDevice *g_uartDevs[UART_MAX_COUNTS] = {NULL};

STATIC INLINE INT32 UartDevCheck(struct LosDevice *dev)
{
    struct UartDriverData *uartData = NULL;
    CHAR *devErr[] = {"dev", "dev->data", "dev->cfg.regs", "dev->data->ops"};
    UINT32 errNum = 0;

    if (dev == NULL) {
        goto UART_INVALID;
    }

    errNum++;
    if (dev->data == NULL) {
        goto UART_INVALID;
    }

    errNum++;
    if (dev->cfg.regs == NULL) {
        goto UART_INVALID;
    }

    errNum++;
    uartData = (struct UartDriverData *)dev->data;
    if (uartData->ops == NULL) {
        goto UART_INVALID;
    }

    return 0;

UART_INVALID:
    PRINT_ERR("%s: %s is NULL!\n", __FUNCTION__, devErr[errNum]);
    return -1;
}

int UartOpen(struct LosDevice *dev)
{
    struct UartDriverData *uartData = NULL;
    int ret;
    UINT32 intSave;

    ret = UartDevCheck(dev);
    if (ret != 0) {
        return -1;
    }

    uartData = (struct UartDriverData *)dev->data;
    if (uartData->ops->startup == NULL) {
        return -1;
    }

    LOS_SpinLockSave(&uartData->lock, &intSave);
    if ((uartData->state & UART_OPEN) != 0) {
        goto UNLOCK_OUT;
    }

    ret = uartData->ops->startup(dev);
    if (ret != 0) {
        goto UNLOCK_OUT;
    }

    uartData->state = UART_OPEN;
    LOS_SpinUnlockRestore(&uartData->lock, intSave);
    return 0;

UNLOCK_OUT:
    LOS_SpinUnlockRestore(&uartData->lock, intSave);
    return -1;
}

int UartClose(struct LosDevice *dev)
{
    struct UartDriverData *uartData = NULL;
    int ret;
    UINT32 intSave;

    ret = UartDevCheck(dev);
    if (ret != 0) {
        return -1;
    }

    uartData = (struct UartDriverData *)dev->data;
    if (uartData->ops->shutdown == NULL) {
        return -1;
    }

    LOS_SpinLockSave(&uartData->lock, &intSave);
    if ((uartData->state & UART_OPEN) == 0) {
        goto UNLOCK_OUT;
    }

    ret = uartData->ops->shutdown(dev);
    if (ret != 0) {
        goto UNLOCK_OUT;
    }

    uartData->state = UART_CLOSE;
    LOS_SpinUnlockRestore(&uartData->lock, intSave);
    return 0;

UNLOCK_OUT:
    LOS_SpinUnlockRestore(&uartData->lock, intSave);
    return -1;
}

ssize_t UartRead(struct LosDevice *dev, char *buf, size_t count)
{
    struct UartDriverData *uartData = NULL;
    int ret;
    UINT32 intSave;

    ret = UartDevCheck(dev);
    if (ret != 0) {
        return -1;
    }

    if (buf == NULL) {
        return -1;
    }

    if (count > INT32_MAX) {
        return -1;
    }

    uartData = (struct UartDriverData *)dev->data;
    if (uartData->ops->read == NULL) {
        return -1;
    }

    LOS_SpinLockSave(&uartData->lock, &intSave);
    if (((uartData->state & UART_OPEN) == 0) || ((uartData->state & UART_SUSPEND) != 0)) {
        goto UNLOCK_OUT;
    }
    LOS_SpinUnlockRestore(&uartData->lock, intSave);

    return uartData->ops->read(dev, buf, count);

UNLOCK_OUT:
    LOS_SpinUnlockRestore(&uartData->lock, intSave);
    return -1;
}

ssize_t UartWrite(struct LosDevice *dev, const char *buf, size_t count)
{
    struct UartDriverData *uartData = NULL;
    int ret;
    UINT32 intSave;

    ret = UartDevCheck(dev);
    if (ret != 0) {
        return -1;
    }

    if (buf == NULL) {
        return -1;
    }

    if (count > INT32_MAX) {
        return -1;
    }

    uartData = (struct UartDriverData *)dev->data;
    if (uartData->ops->transmit == NULL) {
        return -1;
    }

    LOS_SpinLockSave(&uartData->lock, &intSave);
    if (((uartData->state & UART_OPEN) == 0) || ((uartData->state & UART_SUSPEND) != 0)) {
        goto UNLOCK_OUT;
    }
    LOS_SpinUnlockRestore(&uartData->lock, intSave);

    return uartData->ops->transmit(dev, buf, count);

UNLOCK_OUT:
    LOS_SpinUnlockRestore(&uartData->lock, intSave);
    return -1;
}

int UartSuspend(struct LosDevice *dev)
{
    struct UartDriverData *uartData = NULL;
    int ret;
    UINT32 intSave;

    ret = UartDevCheck(dev);
    if (ret != 0) {
        return -1;
    }

    uartData = (struct UartDriverData *)dev->data;
    if (uartData->ops->suspend == NULL) {
        return -1;
    }

    LOS_SpinLockSave(&uartData->lock, &intSave);
    if (((uartData->state & UART_OPEN) == 0) || ((uartData->state & UART_SUSPEND) != 0)) {
        goto UNLOCK_OUT;
    }

    ret = uartData->ops->suspend(dev);
    if (ret < 0) {
        goto UNLOCK_OUT;
    }

    uartData->state |= UART_SUSPEND;
    LOS_SpinUnlockRestore(&uartData->lock, intSave);
    return ret;

UNLOCK_OUT:
    LOS_SpinUnlockRestore(&uartData->lock, intSave);
    return -1;
}

int UartResume(struct LosDevice *dev)
{
    struct UartDriverData *uartData = NULL;
    int ret;
    UINT32 intSave;

    ret = UartDevCheck(dev);
    if (ret != 0) {
        return -1;
    }

    uartData = (struct UartDriverData *)dev->data;
    if (uartData->ops->resume == NULL) {
        return -1;
    }

    LOS_SpinLockSave(&uartData->lock, &intSave);
    if (((uartData->state & UART_OPEN) == 0) || ((uartData->state & UART_SUSPEND) == 0)) {
        goto UNLOCK_OUT;
    }

    ret = uartData->ops->resume(dev);
    if (ret < 0) {
        goto UNLOCK_OUT;
    }

    uartData->state &= ~UART_SUSPEND;
    LOS_SpinUnlockRestore(&uartData->lock, intSave);
    return ret;

UNLOCK_OUT:
    LOS_SpinUnlockRestore(&uartData->lock, intSave);
    return -1;
}

int UartIoctl(struct LosDevice *dev, int cmd, unsigned long arg)
{
    struct UartDriverData *uartData = NULL;
    int ret;
    UINT32 intSave;

    ret = UartDevCheck(dev);
    if (ret != 0) {
        return -1;
    }

    uartData = (struct UartDriverData *)dev->data;
    if (uartData->ops->ioctl == NULL) {
        return -1;
    }

    LOS_SpinLockSave(&uartData->lock, &intSave);
    if ((uartData->state & UART_OPEN) == 0) {
        goto UNLOCK_OUT;
    }

    ret = uartData->ops->ioctl(dev, cmd, arg);
    if (ret < 0) {
        goto UNLOCK_OUT;
    }

    LOS_SpinUnlockRestore(&uartData->lock, intSave);
    return ret;

UNLOCK_OUT:
    LOS_SpinUnlockRestore(&uartData->lock, intSave);

    return -1;
}

struct LosDevice *UartDevGetByPort(int num)
{
    if ((num < 0) || (num >= UART_MAX_COUNTS)) {
        PRINT_ERR("The uart number:%d is invalid!\n", num);
        return NULL;
    }
    return g_uartDevs[num];
}

STATIC struct LosDriver *g_uartDriverTable[] = {
#if defined(LOSCFG_DRIVERS_UART_ARM_PL011)
    &g_pl011Driver,
#endif
#if defined(LOSCFG_DRIVERS_UART_HQ_UART)
    &g_uartV500Driver,
#endif
#if defined(LOSCFG_DRIVERS_UART_VUART_RPQ)
    &g_vUartRpqDriver,
#endif
};

#define UART_DRIVER_NUM         (sizeof(g_uartDriverTable) / sizeof(g_uartDriverTable[0]))

UINT32 UartDriverInit(VOID)
{
    UINT32 ret = LOS_OK;
    UINT32 idx;
    for (idx = 0; idx < UART_DRIVER_NUM; idx++) {
        ret = LOS_DriverRegister(g_uartDriverTable[idx]);
        if (ret != LOS_OK) {
            goto FAILED;
        }
    }
    return LOS_OK;
FAILED:
    while (idx) {
        idx--;
        (VOID)LOS_DriverUnregister(g_uartDriverTable[idx]);
    }
    return ret;
}

VOID UartDriverExit(VOID)
{
    for (UINT32 idx = 0; idx < UART_DRIVER_NUM; idx++) {
        (VOID)LOS_DriverUnregister(g_uartDriverTable[idx]);
    }
}
