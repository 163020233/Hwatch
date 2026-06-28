/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description : Uart-V200 Private Headfile
 * Author : Huawei LiteOS Team
 * Create : 2023-04-11
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
#ifndef _HQ_UART_V200_H
#define _HQ_UART_V200_H

#include "los_typedef.h"
#include "los_io.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************** uart register define *************/
#define UART_DR                         0x00    /* Data Register */
#define UART_RSR_ECR                    0x04    /* Receive Status and Error Clear Register */
#define UART_FR                         0x18    /* Flags Register */
#define UART_IBRD                       0x24    /* Baudrate Divide Integer Resigter */
#define UART_FBRD                       0x28    /* Baudrate Divide Fraction Resigter */
#define UART_LCR                        0x2C    /* Line Control Resigter */
#define UART_CR                         0x30    /* Control Resigter */
#define UART_IFLS                       0x34    /* Interrupt Fifo Level Select Resigter */
#define UART_IMSC                       0x38    /* Interrupt Mask Set and Clear Resigter */
#define UART_RIS                        0x3C    /* Raw Interrupt Status Register */
#define UART_MIS                        0x40    /* Masked Interrupt Status Register */
#define UART_ICR                        0x44    /* Interrupt Clear Register */

#define UART_EN                         (0x01U)
#define UART_TX_EN                      (0x01U << 8)
#define UART_RX_EN                      (0x01U << 9)
#define UART_EN_MSK                     (0x00U)

#define UART_FIFO_CFG_RTS1_8            (0x00U << 6)
#define UART_FIFO_CFG_RTS2_8            (0x01U << 6)
#define UART_FIFO_CFG_RTS4_8            (0x02U << 6)
#define UART_FIFO_CFG_RTS6_8            (0x03U << 6)
#define UART_FIFO_CFG_RTS7_8            (0x04U << 6)
#define UART_FIFO_CFG_RTS_FULL          (0x05U << 6)

#define UART_FIFO_CFG_RX1_8             (0x00U << 3)
#define UART_FIFO_CFG_RX2_8             (0x01U << 3)
#define UART_FIFO_CFG_RX4_8             (0x02U << 3)
#define UART_FIFO_CFG_RX6_8             (0x03U << 3)
#define UART_FIFO_CFG_RX7_8             (0x04U << 3)

#define UART_FIFO_CFG_TX1_8             (0x00U)
#define UART_FIFO_CFG_TX2_8             (0x01U)
#define UART_FIFO_CFG_TX4_8             (0x02U)
#define UART_FIFO_CFG_TX6_8             (0x03U)
#define UART_FIFO_CFG_TX7_8             (0x04U)

#define UART_DATA_8_BIT                 (0x03U << 5)
#define UART_DATA_7_BIT                 (0x02U << 5)
#define UART_DATA_6_BIT                 (0x01U << 5)
#define UART_DATA_5_BIT                 (0x00U << 5)

#define UART_FIFO_EN                    (0x01U << 4)

#define UART_STAT_TX_FIFO_EMPTY         (0x01U << 7)
#define UART_STAT_RX_FIFO_FULL          (0x01U << 6)
#define UART_STAT_TX_FIFO_FULL          (0x01U << 5)
#define UART_STAT_RX_FIFO_EMPTY         (0x01U << 4)

#define UART_RX_FIFO_DATA               0xFF
#define UART_DR_FRAME_ERR               (0x01U << 8)
#define UART_DR_PARITY_ERR              (0x01U << 9)
#define UART_DR_BREAK_ERR               (0x01U << 10)
#define UART_DR_OVERRUN_ERR             (0x01U << 11)
#define UART_RX_FIFO_ERR                (UART_DR_FRAME_ERR | UART_DR_PARITY_ERR | \
                                         UART_DR_BREAK_ERR | UART_DR_OVERRUN_ERR)

#define UART_CONFIG_FRAME_8_1_NO_PAR    (UART_DATA_8_BIT | UART_FIFO_EN)

#define UART_CONFIG_ENABLE              UART_EN
#define UART_CLEAR_ALL_INTER            0x7FF   /* UART_ICR */
#define UART_MASK_ALL_INTER             0x0     /* UART_IMSC: disable all interrupt */
#define UART_UNMASK_RX_INTER            0x50    /* UART_IMSC: only enable rx & rx_timeout interrupt */

#define UART_INT_MSK                    UART_IMSC
#define UART_INT_CLR                    UART_ICR
#define UART_STAT                       UART_FR
#define UART_RX_FIFO                    UART_DR
#define UART_TX_FIFO                    UART_DR
#define UART_CONFIG                     UART_CR
#define UART_FIFO_CFG                   UART_IFLS
#define UART_FRAME_CFG                  UART_LCR


STATIC INLINE VOID WriteBaudReg(UINTPTR base, UINT32 fraction, UINT32 divider)
{
    WRITE32_MB(base + UART_IBRD, divider);
    WRITE32_MB(base + UART_FBRD, fraction);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _HQ_UART_V5XX_H */
