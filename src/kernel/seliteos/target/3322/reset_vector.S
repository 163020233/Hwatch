/******************************************************************************
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description: start.s for SELiteOS.
******************************************************************************/
#include "memory_config_common.h"
#include "arch/stack_ops.h"

        .section .text.entry
        .global Reset_Handler
        .extern os_data_init
        .extern os_bss_init
        .extern ta_stack_init
        .extern main
        .option norvc
Reset_Handler:
        j _start

_start:
        .option push
        .option norelax
        # initialize global pointer
        la gp, _gp_
        .option pop

        la t0, TrapVector
        addi t0, t0, 3
        csrw tmtvec, t0

        /* config pmp tes */
        li t0, 0x0000ffff
        csrrw x0, pmpte0, t0

        # initialize stack pointer
        la sp, __startup_stack_top

        /* enable tes stack pointer limiting */
        SET_SP_LIMIT __startup_stack, t0, MACRO_SP_MODE_SYMBOL

        /* init kernel stack */
        la      t0, __kernel_stack_start
        la      t1, __kernel_stack_end
        beq     t0, t1, end_set_kernel_stack_loop
        li      t2, 0xefbeadde

set_kernel_stack_loop:
        sw      t2, (t0)
        addi    t0, t0, 4
        blt     t0, t1, set_kernel_stack_loop
end_set_kernel_stack_loop:

        /* set all data section */
set_data_section:
        call os_data_init

        /* clear all bss section */
clear_bss_section:
        call os_bss_init

        /* set ta stack section */
set_ta_stack:
        call ta_stack_init

        li t0, 0xffffffff
        /* delegate exception, ree exception will be processed by ree API */
        csrrw x0, tmedeleg, t0

        /* delegate interrupt, when tes is 0, untrusted interrupt will be processed by ree API */
        csrrs t2, mclicbase, x0
        li t1, 0x00000a00
        add t1, t2, t1
        /* set readcsr(0xFBF)+0xa00 */
        sw t0, (t1)
        /* set readcsr(0xFBF)+0xa04 */
        sw t0, 4(t1)
        /* set readcsr(0xFBF)+0xa08 */
        sw t0, 8(t1)
        /* set readcsr(0xFBF)+0xa0c */
        sw t0, 12(t1)

        /* tmstatus.mie = 1 */
        csrsi tmstatus, 0x8

        /* tesvec start addr:tesvec_table and end addr:tmestop */
        la t0, tesvec_table
        csrw tmesvec, t0
        la t0, testop
        csrw tmestop, t0

        /* icache invalid */
        fence iorw, iorw
        csrsi 0x7C2, 0x4
        fence iorw, iorw

        /* dcache invalid */
        fence iorw, iorw
        csrsi 0x7C3, 0x4
        fence iorw, iorw

        jal main

        /* after disable tdm, ree can't debug tee */
        TDM_FORCE_DISABLE

        /* jump to ree addr */
        li t0, REE_ITCM_ORIGIN
        jalr x0, t0, 0
