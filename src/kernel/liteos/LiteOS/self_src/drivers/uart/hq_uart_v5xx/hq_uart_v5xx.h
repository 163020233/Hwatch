/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description : Uart-V500 Private Headfile
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
#ifndef _HQ_UART_V5XX_H
#define _HQ_UART_V5XX_H

#include "los_typedef.h"
#include "los_io.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************** uart register define *************/
#define UART_BAUD_CFG                   0x00    /* baudrate register */
#define UART_FRAME_CFG                  0x04    /* data frame register */
#define UART_FIFO_CFG                   0x08    /* uart fifo register */
#if defined(LOSCFG_DRIVERS_UART_HQ_UART_V500) || defined(LOSCFG_DRIVERS_UART_HQ_UART_V501)
#define UART_TX_FIFO                    0x0C    /* uart tx(write) fifo register */
#define UART_RX_FIFO                    0x10    /* uart rx(read) fifo register */
#elif defined(LOSCFG_DRIVERS_UART_HQ_UART_V502)
#define UART_TX_FIFO                    0x400   /* uart tx(write) fifo register */
#define UART_RX_FIFO                    0x800   /* uart rx(read) fifo register */
#endif
#define UART_STAT                       0x14    /* uart state register */
#define UART_INT_CLR                    0x18    /* interrupt clear register */
#define UART_INT_MSK                    0x1C    /* interrupt mask register */
#define UART_CONFIG                     0x28    /* data config register */
#define UART_DEBUG                      0x50    /* uart debug register */
#define UART_ID                         0x100   /* uart id register */

#define UART_EN                         (0x01U)
#define UART_TX_EN                      (0x01U << 1)
#define UART_RX_EN                      (0x01U << 2)
#define UART_FIFO_EN                    (0x01U << 10) /* only uart_v501/v502 valid */
#define UART_EN_MSK                     (0x01U << 16)
#define UART_TX_EN_MSK                  (0x01U << 17)
#define UART_RX_EN_MSK                  (0x01U << 18)
#define UART_FIFO_EN_MSK                (0x01U << 26) /* only uart_v501/v502 valid */

#define UART_FIFO_CFG_RTS1_8            (0x00U << 6)
#define UART_FIFO_CFG_RTS4_8            (0x02U << 6)
#define UART_FIFO_CFG_RTS7_8            (0x04U << 6)
#define UART_FIFO_CFG_RTS_FULL          (0x05U << 6)
#define UART_FIFO_CFG_RX1_8             (0x00U << 3)
#define UART_FIFO_CFG_RX4_8             (0x02U << 3)
#define UART_FIFO_CFG_RX7_8             (0x04U << 3)
#define UART_FIFO_CFG_TX1_8             (0x00U)
#define UART_FIFO_CFG_TX4_8             (0x02U)
#define UART_FIFO_CFG_TX7_8             (0x04U)

#define UART_DATA_8_BIT                 (0x03U)
#define UART_DATA_7_BIT                 (0x02U)
#define UART_DATA_6_BIT                 (0x01U)
#define UART_DATA_5_BIT                 (0x00U)

#define UART_PARITY_ODD                 (0x04U << 2)
#define UART_PARITY_EVEN                (0x05U << 2)

#define UART_1_STOP_BIT                 (0x00U << 5)
#define UART_2_STOP_BIT                 (0x01U << 5)

#define UART_STAT_TX_FIFO_EMPTY         (0x01U << 2)
#define UART_STAT_TX_FIFO_FULL          (0x01U << 3)
#define UART_STAT_RX_FIFO_EMPTY         (0x01U << 4)
#define UART_STAT_RX_FIFO_FULL          (0x01U << 5)


#define UART_RX_FIFO_DATA               0XFF
#define UART_RX_FIFO_FRAME_ERR          (0x01U << 8)
#define UART_RX_FIFO_PARITY_ERR         (0x01U << 9)
#define UART_RX_FIFO_BREAK_ERR          (0x01U << 10)
#define UART_RX_FIFO_OVERRUN_ERR        (0x01U << 11)
#define UART_RX_FIFO_ERR                (UART_RX_FIFO_FRAME_ERR | UART_RX_FIFO_PARITY_ERR | \
                                         UART_RX_FIFO_BREAK_ERR | UART_RX_FIFO_OVERRUN_ERR)

#if defined(LOSCFG_DRIVERS_UART_HQ_UART_V501) || defined(LOSCFG_DRIVERS_UART_HQ_UART_V502)
#define UART_CONFIG_ENABLE              (UART_EN | UART_FIFO_EN | UART_EN_MSK | UART_FIFO_EN_MSK) /* UART_CONFIG */
#elif defined(LOSCFG_DRIVERS_UART_HQ_UART_V500)
#define UART_CONFIG_ENABLE              (UART_EN | UART_EN_MSK) /* UART_CONFIG */
#endif

#if defined(LOSCFG_DRIVERS_UART_HQ_UART_V501) || defined(LOSCFG_DRIVERS_UART_HQ_UART_V502)
#define UART_CLEAR_ALL_INTER            0x07FF /* UART_INT_CLR */
#define UART_MASK_ALL_INTER             0x07FF /* UART_INT_MSK: disable all interrupt */
#define UART_UNMASK_RX_INTER            0x07FA /* UART_INT_MSK: only enable rx & rx_timeout interrupt */
#elif defined(LOSCFG_DRIVERS_UART_HQ_UART_V500)
#define UART_CLEAR_ALL_INTER            0xFF /* UART_INT_CLR */
#define UART_MASK_ALL_INTER             0xFF /* UART_INT_MSK: disable all interrupt */
#define UART_UNMASK_RX_INTER            0xFA /* UART_INT_MSK: only enable rx & rx_timeout interrupt */
#endif

/* 8 bits, 1 stop bit, no parity */
#define UART_CONFIG_FRAME_8_1_NO_PAR    0x03

STATIC INLINE VOID WriteBaudReg(UINTPTR base, UINT32 fraction, UINT32 divider)
{
    UINT32 baud = (fraction << 16) | divider; /* 16: fraction num shifted by 16 bits */
    WRITE32_MB(base + UART_BAUD_CFG, baud);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _HQ_UART_V5XX_H */
