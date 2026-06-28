/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  Application Core Main function for Standard.
 * Author:
 * Create: 2021-03-09
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
typedef struct {
    uint32_t s_regs[12];    // store s0~s11
    uint32_t t_regs[7];     // store t0~t6
} ree_nmi_stack_t;

int main(void) __attribute__((weak));
#endif
