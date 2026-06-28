/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Arm Pl011 Simple Driver
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

#include "uart_pri.h"
#include "arm_pl011.h"
#include "soc/uart.h"
#include "los_spinlock.h"

#define UARTREG(addr)   (*((volatile UINT32 *)(UINTPTR)(addr)))
#define PL011_CLK_DIV   16U
#define PL011_NUM_8     8U

LITE_OS_SEC_BSS STATIC SPIN_LOCK_INIT(g_uartOutputSpin);

STATIC INLINE VOID UartPutcReg(UINTPTR regBase, CHAR c)
{
    UINT32 cycle = 10000;           /* 10000: wait cpu cycles */

    /* Spin while fifo is full */
    while (UARTREG(regBase + UART_FR) & UART_FR_TXFF) {
        if (cycle == 0) {
            break;
        }
        cycle--;
    }
    UARTREG(regBase + UART_DR) = c;
}

STATIC VOID UartPutStr(UINTPTR regBase, const CHAR *s, UINT32 len)
{
    UINT32 i;

    for (i = 0; i < len; i++) {
        /* Only system uart output needs to add extra '\r' to improve the compatibility on win. */
        if ((*(s + i) == '\n') && (regBase == UART_REG_BASE)) {
            if ((i == 0) || (*(s + i - 1) != '\r')) {
                UartPutcReg(regBase, '\r');
            }
        }
        UartPutcReg(regBase, *(s + i));
    }
}

VOID UART_CORE_FUNC(pl011, UartPutsReg)(UINTPTR regBase, const CHAR *s, UINT32 len, BOOL isLock)
{
#ifdef LOSCFG_SERIAL_OUTPUT_ENABLE
    UINT32 intSave;

    if (isLock) {
        LOS_SpinLockSave(&g_uartOutputSpin, &intSave);
        UartPutStr(regBase, s, len);
        LOS_SpinUnlockRestore(&g_uartOutputSpin, intSave);
    } else {
        UartPutStr(regBase, s, len);
    }
#else
    (VOID)regBase;
    (VOID)s;
    (VOID)len;
    (VOID)isLock;
    return 0;
#endif
}

VOID UART_CORE_FUNC(pl011, UartPuts)(const CHAR *s, UINT32 len, BOOL isLock)
{
    UART_CORE_FUNC_CALL(pl011, UartPutsReg, UART_REG_BASE, s, len, isLock);
}

STATIC INLINE VOID UartSetBaudrate(UINTPTR regBase)
{
    UINT32 baudRate;
    UINT32 divider;
    UINT32 remainder;
    UINT32 fraction;

    baudRate  = PL011_CLK_DIV * CONSOLE_UART_BAUDRATE;
    divider   = CONFIG_UART_CLK_INPUT / baudRate;
    remainder = CONFIG_UART_CLK_INPUT % baudRate;
    baudRate  = (PL011_NUM_8 * remainder) / CONSOLE_UART_BAUDRATE;
    fraction  = (baudRate >> 1) + (baudRate & 1);

    WRITE_UINT32(divider, regBase + UART_IBRD);
    WRITE_UINT32(fraction, regBase + UART_FBRD);
}

VOID UART_CORE_FUNC(pl011, UartEarlyInit)(VOID)
{
    /* First, disable everything */
    WRITE_UINT32(0x0, UART_REG_BASE + UART_CR);

    /* set Scale factor of baud rate */
    UartSetBaudrate(UART_REG_BASE);

    /* Set the UART to be 8 bits, 1 stop bit, no parity, fifo enabled. */
    WRITE_UINT32(UART_LCR_H_8_BIT | UART_LCR_H_FIFO_EN, UART_REG_BASE + UART_LCR_H);

    /* enable the UART */
    WRITE_UINT32(UART_CR_EN | UART_CR_TX_EN, UART_REG_BASE + UART_CR);
}

VOID UART_CORE_FUNC(pl011, UartStartupCfg)(UINTPTR regBase)
{
    UINT32 crValue = GET_UINT32(regBase + UART_CR);
    UINT32 lcrHValue = GET_UINT32(regBase + UART_LCR_H);

    /* First, disable everything */
    WRITE_UINT32(0x0, regBase + UART_CR);

    /* set the fifo threshold of interrupt */
    WRITE_UINT32(UART_IFLS_RX4_8 | UART_IFLS_TX7_8, regBase + UART_IFLS);

    /* set nonblock of recv interrupt and recv timeout interrupt */
    WRITE_UINT32(UART_IMSC_RX | UART_IMSC_TIMEOUT, regBase + UART_IMSC);

    /* enable fifo */
    lcrHValue |= UART_LCR_H_FIFO_EN;
    WRITE_UINT32(lcrHValue, regBase + UART_LCR_H);

    /* enable the UART */
    crValue |= UART_CR_EN | UART_CR_TX_EN | UART_CR_RX_EN;
    WRITE_UINT32(crValue, regBase + UART_CR);
}

VOID UART_CORE_FUNC(pl011, UartSuspendCfg)(UINTPTR regBase)
{
    UINT32 crValue;
    UINT32 lcrHValue = GET_UINT32(regBase + UART_LCR_H);

    /* disable the uart */
    crValue = GET_UINT32(regBase + UART_CR);
    crValue &= ~(UART_CR_EN | UART_CR_TX_EN | UART_CR_RX_EN);
    WRITE_UINT32(crValue, regBase + UART_CR);

    /* clear the mask */
    WRITE_UINT32(0, regBase + UART_IMSC);

    /* disable fifo */
    lcrHValue &= ~UART_LCR_H_FIFO_EN;
    WRITE_UINT32(lcrHValue, regBase + UART_LCR_H);
}
