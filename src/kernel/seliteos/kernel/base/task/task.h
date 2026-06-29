/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All
 * rights reserved. Description: task header for SELiteOS.
 */

#ifndef TASK_H
#define TASK_H

#include "ta_manager.h"

void task_init(void);

typedef void (*task_entry)(unsigned long a0, unsigned long a1, unsigned long a2, unsigned long a3, unsigned long a4);

void clear_task_ta_info(void);

void set_task_ta_info(struct ta_header *ta);

uint32_t switch_to_user_task(task_entry entry_func, unsigned long a0,
                             unsigned long a1, unsigned long a2,
                             unsigned long a3, unsigned long a4);
#endif
