/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: 3322 platform header for SELiteOS.
 */

#ifndef ASM_PLATFORM_H
#define ASM_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void os_data_init(void);

void os_bss_init(void);

void ta_stack_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
