/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:  Application Core Main function for Standard.
 * Author:
 * Create:
 */
#include "kernel_init.h"
#include "chip_init.h"
#include "board_init.h"
#include "service_init.h"
#include "thread_pri_init.h"
#include "cmsis_os2.h"
#include "main.h"

ree_nmi_stack_t g_nmi_stack;

int main(void)
{
    kernel_init();

    chip_init();

    board_init();

    service_init();

    thread_pri_init();

    (void)osKernelStart();
    return 0;
}
