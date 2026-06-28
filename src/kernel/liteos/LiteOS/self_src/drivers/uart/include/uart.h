/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Uart HeadFile
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

#ifndef _DRIVERS_UART_H
#define _DRIVERS_UART_H

#include "los_typedef.h"
#ifdef LOSCFG_DRIVERS_BASE
#include "los_driverbase.h"
#endif
#include "los_spinlock.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct UartOps {
    INT32 (*startup)(struct LosDevice *dev);
    INT32 (*shutdown)(struct LosDevice *dev);
    ssize_t (*read)(struct LosDevice *dev, char *buf, size_t count);
    ssize_t (*transmit)(struct LosDevice *dev, const char *buf, size_t count);
    INT32 (*config)(struct LosDevice *dev, unsigned long ard);
    INT32 (*suspend)(struct LosDevice *dev);
    INT32 (*resume)(struct LosDevice *dev);
    INT32 (*ioctl)(struct LosDevice *dev, INT32 cmd, unsigned long arg);
};

enum UartState {
    UART_UNUSED     = 1U << 0,
    UART_OPEN       = 1U << 1,
    UART_CLOSE      = 1U << 2,
    UART_SUSPEND    = 1U << 3,
};

enum UartIoctlCmd {
    UART_IOCTL_SUSPEND,
    UART_IOCTL_RESUME,
    UART_IOCTL_SET_AFFINITY,
};

struct UartDriverData {
    UINT32 num;
    UINT32 baudrate;
    UINT32 state;
    SPIN_LOCK_S lock;
    struct UartOps *ops;
    VOID *privateData;
};

VOID UartEarlyInit(VOID);
VOID UartPuts(const CHAR *s, UINT32 len, BOOL isLock);
VOID UartPutsReg(UINTPTR regBase, const CHAR *s, UINT32 len, BOOL isLock);

struct LosDevice *UartDevGetByPort(int num);
int UartOpen(struct LosDevice *dev);
int UartClose(struct LosDevice *dev);
ssize_t UartRead(struct LosDevice *dev, char *buf, size_t count);
ssize_t UartWrite(struct LosDevice *dev, const char *buf, size_t count);
int UartIoctl(struct LosDevice *dev, int cmd, unsigned long arg);
int UartSuspend(struct LosDevice *dev);
int UartResume(struct LosDevice *dev);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DRIVERS_UART_H */
