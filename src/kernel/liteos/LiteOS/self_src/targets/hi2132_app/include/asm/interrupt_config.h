/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Interrupt Config HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2024-03-09
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

#ifndef _ASM_INTERRUPT_CONFIG_H
#define _ASM_INTERRUPT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define NUM_HAL_INTERRUPT_UART 39

#define NUM_HAL_INTERRUPT_TIMER 63
#define OS_TICK_INT_NUM NUM_HAL_INTERRUPT_TIMER

#define NUM_HAL_INTERRUPT_NMI 0xFFF

#define OS_RISCV_SYS_VECTOR_CNT 26

#define OS_RISCV_MIE_IRQ_VECTOR_CNT 6

#define OS_RISCV_LOCAL_IRQ_VECTOR_CNT (OS_RISCV_MIE_IRQ_VECTOR_CNT + OS_RISCV_SYS_VECTOR_CNT)

#define LOCAL_INTERRUPT0 16

#define NUM_HAL_INTERRUPT_USB_DEV (LOCAL_INTERRUPT0 + 28)

#define NUM_HAL_INTERRUPT_TIMER0 (LOCAL_INTERRUPT0 + 24)
#define NUM_HAL_INTERRUPT_TIMER1 (LOCAL_INTERRUPT0 + 25)

#define NUM_HAL_INTERRUPT_RTC0 (LOCAL_INTERRUPT0 + 26)
#define NUM_HAL_INTERRUPT_RTC1 (LOCAL_INTERRUPT0 + 27)

#define NUM_HAL_INTERRUPT_APP0 (LOCAL_INTERRUPT0 + 20)
#define NUM_HAL_INTERRUPT_APP1 (LOCAL_INTERRUPT0 + 21)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASM_INTERRUPT_CONFIG_H */
