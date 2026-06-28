/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Register Config HeadFile
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

#ifndef _ASM_REGISTER_CONFIG_H
#define _ASM_REGISTER_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define REG_BASE_PERI_CTRL 0x20000
#define NMI_REG_BASE (REG_BASE_PERI_CTRL + 0xC18)
#define CLIC_REG_BASE 0xC0000000
#define UART0_REG_BASE 0x30002000

/* RTC register OFFSET */
#define RTC_IRQ_TIME_LSBS 0x00
#define RTC_IRQ_TIME_MSBS 0x04
#define RTC_IRQ_CLR 0x08
#define RTC_IRQ_MASK_OUT 0x0c
#define RTC_IRQ_INT_RAW_IN 0x10
#define RTC_IRQ_INT_MASK_IN 0x14
#define RTC_TIME_OUT_H 0x18
#define RTC_TIME_OUT_L 0x1c

#define RTC_TIMER_BASE_ADDR 0x30060000
#define RTC_TIMER_0_BASE_ADDR (RTC_TIMER_BASE_ADDR + 0x00)
#define RTC_TIMER_1_BASE_ADDR (RTC_TIMER_BASE_ADDR + 0x100)

/* Timer register OFFSET */
#define TIMER_LOAD_COUNT 0x00
#define TIMER_LOAD_COUNT_L 0x00
#define TIMER_LOAD_COUNT_H 0x04
#define TIMER_CURRENT_VALUE_L 0x08
#define TIMER_CURRENT_VALUE_H 0x0C
#define TIMER_CONTROL_REG 0x10
#define TIMER_EOI 0x14
#define TIMER_INT_STATUS 0x18

/* Timer reg base addr */
#define TIMER_BASE_ADDR 0x30002000
#define TIMER_0_BASE_ADDR (TIMER_BASE_ADDR + 0x100)
#define TIMER_1_BASE_ADDR (TIMER_BASE_ADDR + 0x200)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASM_REGISTER_CONFIG_H */
