/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: RISCV70 idle task config interface for LiteOS
 *
 * Create:  2021-10-20
 */
#ifndef IDLE_CONFIG_H
#define IDLE_CONFIG_H

#include "stdbool.h"

/**
 * @brief  los idle task config
 */
void idle_task_config(void);

void idle_set_open_pm(bool en);

#endif