/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: cpu register lowpower process file
 */

#include "chip_core_definition.h"

.extern g_cpu_suspend_regs

.global  lowpower_cpu_suspend
.global  lowpower_cpu_resume

.section .text
.option rvc

/* ree just save CPU-csr to RAM */
lowpower_cpu_suspend:
    la a0, g_cpu_suspend_regs
    /* ree save csr */
    csrr a7, mstatus
    csrr a6, misa
    csrr a5, mie
    csrr a4, mtvec
    csrr a3, mscratch
    csrr a2, mepc
    csrr a1, mcause
    csrr t0, mtvt
    swm {t0, a1-a7}, a0

    ret

/* ree Restore CPU-Registers from RAM */
lowpower_cpu_resume:
    la a0, g_cpu_suspend_regs

    lwm {t0, a1-a7}, a0
    csrw mstatus, a7
    csrw misa, a6
    csrw mie, a5
    csrw mtvec, a4
    csrw mscratch, a3
    csrw mepc, a2
    csrw mcause, a1
    csrw mtvt, t0

    ret
