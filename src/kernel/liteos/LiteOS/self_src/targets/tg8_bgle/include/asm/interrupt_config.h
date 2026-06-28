/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: os platform config files
 */

#ifndef _ASM_INTERRUPT_CONFIG_H
#define _ASM_INTERRUPT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define NUM_HAL_INTERRUPT_TIMER          7
#define OS_TICK_INT_NUM                  NUM_HAL_INTERRUPT_TIMER

#define CLIC_REG_BASE                    0x300000

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASM_INTERRUPT_CONFIG_H */
