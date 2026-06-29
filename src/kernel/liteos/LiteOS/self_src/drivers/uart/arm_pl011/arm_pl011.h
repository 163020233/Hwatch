/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Arm Pl011 Driver HeadFile
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

#ifndef _UART_ARM_PL011_H
#define _UART_ARM_PL011_H

#include "los_typedef.h"
#include "los_ringbuf.h"
#include "los_event.h"
#include "uart_pri.h"
#include "arm_pl011_core.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* register define */
#define UART_DR                 0x0   /* data register */
#define UART_RSR                0x04  /* receive status register */
#define UART_ECR                0x04  /* error cleat register */
#define UART_FR                 0x18  /* flag register */
#define UART_IBRD               0x24  /* integer baudrate register */
#define UART_FBRD               0x28  /* decimal baudrate register */
#define UART_LCR_H              0x2C  /* line control register */
#define UART_CR                 0x30  /* control register */
#define UART_IFLS               0x34  /* fifo register */
#define UART_IMSC               0x38  /* interrupt mask register */
#define UART_RIS                0x3C  /* base interrupt state register */
#define UART_MIS                0x40  /* mask interrupt state register */
#define UART_ICR                0x44  /* interrupt clear register */

#define UART_IFLS_RX4_8         (0x02 << 3)
#define UART_IFLS_TX7_8         (0x04 << 0)

#define UART_CR_CTS             (0x01 <<15)
#define UART_CR_RTS             (0x01 <<14)
#define UART_CR_RX_EN           (0x01 << 9)
#define UART_CR_TX_EN           (0x01 << 8)
#define UART_CR_LOOPBACK        (0x01 << 7)
#define UART_CR_EN              (0x01 << 0)

#define UART_FR_TXFE            (0x01 << 7)
#define UART_FR_RXFF            (0x01 << 6)
#define UART_FR_TXFF            (0x01 << 5)
#define UART_FR_RXFE            (0x01 << 4)
#define UART_FR_BUSY            (0x01 << 3)

#define UART_LCR_H_BREAK        (0x01 << 0)
#define UART_LCR_H_PEN          (0x01 << 1)
#define UART_LCR_H_EPS          (0x01 << 2)
#define UART_LCR_H_STP2         (0x01 << 3)
#define UART_LCR_H_FIFO_EN      (0x01 << 4)
#define UART_LCR_H_8_BIT        (0x03 << 5)
#define UART_LCR_H_7_BIT        (0x02 << 5)
#define UART_LCR_H_6_BIT        (0x01 << 5)
#define UART_LCR_H_5_BIT        (0x00 << 5)
#define UART_LCR_H_SPS          (0x01 << 7)

#define UART_MIS_TIMEOUT        (0x01 << 6)
#define UART_MIS_TX             (0x01 << 5)
#define UART_MIS_RX             (0x01 << 4)

#define UART_IMSC_OVER          (0x01 << 10)
#define UART_IMSC_BREAK         (0x01 << 9)
#define UART_IMSC_CHK           (0x01 << 8)
#define UART_IMSC_ERR           (0x01 << 7)
#define UART_IMSC_TIMEOUT       (0x01 << 6)
#define UART_IMSC_RX            (0x01 << 4)

typedef struct {
    Ringbuf recvData;
    EVENT_CB_S event;
} Pl011PriData;

STATIC INLINE UINT32 Pl011ReadMIS(UINTPTR regBase)
{
    return GET_UINT32(regBase + UART_MIS);
}

STATIC INLINE UINT32 Pl011ReadFR(UINTPTR regBase)
{
    return GET_UINT32(regBase + UART_FR);
}

STATIC INLINE UINT8 Pl011ReadData(UINTPTR regBase)
{
    return (UINT8)GET_UINT32(regBase + UART_DR);
}

STATIC INLINE VOID Pl011ClearAllInterrupt(UINTPTR regBase)
{
    WRITE_UINT32(0xFFFF, regBase + UART_ICR);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _UART_ARM_PL011_H */
