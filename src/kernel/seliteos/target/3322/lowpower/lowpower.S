/******************************************************************************
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: lowpower.S for SELiteOS.
******************************************************************************/
#include "arch/stack_ops.h"
#include "tdm_debug.h"

.section .text.lowpower

.global  tee_lowpower_resume

.align 4
.global lowpower_entry
lowpower_entry:
    TES_ENTRY_MARKER
    TDM_ENABLE

    la t2, g_cpu_suspend_regs
    sw ra, (t2)
    sw sp, 4(t2)
    addi t2, t2, 8

    csrr a7, tusp
    csrr a6, tugp
    csrr a5, tutp
    swm {a5-a7}, t2
    addi t2, t2, 12

    /* add lowpower suspend code here */
    csrr a7, mseccfg
    csrr a6, pmpte0
    csrr a5, tmescr
    csrr a4, tmesvec
    csrr a3, tmestop
    csrr a2, tmedeleg
    csrr t1, tmtvec
    csrr t0, tmepc
    swm {t0-t1, a2-a7}, t2
    addi t2, t2, 32

    csrr a7, tmstatus
    csrr a6, tmcause
    csrr a5, tmtval
    csrr a4, tmscratch
    csrr a3, tmsplimit
    csrr a2, msecctl
    csrr t1, icctl
    csrr t0, dcctl
    swm {t0-t1, a2-a7}, t2
    addi t2, t2, 32

    csrr a7, pmpaddr0
    csrr a6, pmpaddr1
    csrr a5, pmpaddr2
    csrr a4, pmpaddr3
    csrr a3, pmpaddr4
    csrr a2, pmpaddr5
    csrr t1, pmpaddr6
    csrr t0, pmpaddr7
    swm {t0-t1, a2-a7}, t2
    addi t2, t2, 32

    csrr a7, pmpaddr8
    csrr a6, pmpaddr9
    csrr a5, pmpaddr10
    csrr a4, pmpaddr11
    csrr a3, pmpaddr12
    csrr a2, pmpaddr13
    csrr t1, pmpaddr14
    csrr t0, pmpaddr15
    swm {t0-t1, a2-a7}, t2
    addi t2, t2, 32

    csrr a7, pmpaddr16
    csrr a6, pmpaddr17
    csrr a5, pmpaddr18
    csrr a4, pmpaddr19
    csrr a3, pmpaddr20
    csrr a2, pmpaddr21
    csrr t1, pmpaddr22
    csrr t0, pmpaddr23
    swm {t0-t1, a2-a7}, t2
    addi t2, t2, 32

    csrr a7, pmpaddr24
    csrr a6, pmpaddr25
    csrr a5, pmpaddr26
    csrr a4, pmpaddr27
    csrr a3, pmpaddr28
    csrr a2, pmpaddr29
    csrr t1, pmpaddr30
    csrr t0, pmpaddr31
    swm {t0-t1, a2-a7}, t2
    addi t2, t2, 32

    csrr a7, pmpcfg0
    csrr a6, pmpcfg1
    csrr a5, pmpcfg2
    csrr a4, pmpcfg3
    csrr a3, pmpcfg4
    csrr a2, pmpcfg5
    csrr t1, pmpcfg6
    csrr t0, pmpcfg7
    swm {t0-t1, a2-a7}, t2
    addi t2, t2, 32

    csrr a7, memattr0
    csrr a6, memattr1
    csrr a5, memattr2
    csrr a4, memattr3
    csrr a3, memattr4
    csrr a2, memattr5
    csrr t1, memattr6
    csrr t0, memattr7
    swm {t0-t1, a2-a7}, t2
    addi t2, t2, 32

    csrr a7, memattr8
    csrr a6, memattr9
    csrr a5, memattr10
    csrr a4, memattr11
    csrr a3, memattr12
    csrr a2, memattr13
    csrr t1, memattr14
    csrr t0, memattr15
    swm {t0-t1, a2-a7}, t2
    addi t2, t2, 32

    csrr a7, memattr16
    csrr a6, memattr17
    csrr a5, memattr18
    csrr a4, memattr19
    csrr a3, memattr20
    csrr a2, memattr21
    csrr t1, memattr22
    csrr t0, memattr23
    swm {t0-t1, a2-a7}, t2
    addi t2, t2, 32

    csrr a7, memattr24
    csrr a6, memattr25
    csrr a5, memattr26
    csrr a4, memattr27
    csrr a3, memattr28
    csrr a2, memattr29
    csrr t1, memattr30
    csrr t0, memattr31
    swm {t0-t1, a2-a7}, t2
    addi t2, t2, 32

    /* lowpower suspend c code */
    jal lowpower_suspend

    /* enter wfi */
    fence r, w
    wfi
    fence i, o
    nop
    nop

tee_lowpower_resume:
    /* PMP enhancements enable */
    li t0, 0x7
    csrrw x0, mseccfg, t0

    /* config pmp tes */
    li t0, 0xffffffff
    csrrw x0, pmpte0, t0

    /* addr:0x40000 - 0x80000  tee itcm, it will replace pmpaddr31 */
    li t0, 0x17fff
    csrrw x0, pmpaddr13, t0

    /* addr:0x10000000 - 0x10010000 tee dtcm */
    li t0, 0x4001fff
    csrrw x0, pmpaddr14, t0

    /* tee itcm: pmpaddr13 rx 0x9d, tee dtcm: pmpaddr14 rw 0x9b */
    li t0, 0x009b9d00
    csrrw x0, pmpcfg3, t0

    .option push
    .option norelax
    # initialize global pointer
    la gp, _gp_
    .option pop

    /* restore registers */
    la t2, g_cpu_suspend_regs
    lw ra, (t2)
    lw sp, 4(t2)
    addi t2, t2, 8

    lwm {a5-a7}, t2
    csrw tusp, a7
    csrw tugp, a6
    csrw tutp, a5
    addi t2, t2, 12

    lwm {t0-t1, a2-a7}, t2
    csrw mseccfg, a7
    csrw pmpte0, a6
    csrw tmescr, a5
    csrw tmesvec, a4
    csrw tmestop, a3
    csrw tmedeleg, a2
    csrw tmtvec, t1
    csrw tmepc, t0
    addi t2, t2, 32

    lwm {t0-t1, a2-a7}, t2
    csrw tmstatus, a7
    csrw tmcause, a6
    csrw tmtval, a5
    csrw tmscratch, a4
    csrw tmsplimit, a3
    csrw msecctl, a2
    addi t2, t2, 32

    lwm {t0-t1, a2-a7}, t2
    csrw pmpaddr0, a7
    csrw pmpaddr1, a6
    csrw pmpaddr2, a5
    csrw pmpaddr3, a4
    csrw pmpaddr4, a3
    csrw pmpaddr5, a2
    csrw pmpaddr6, t1
    csrw pmpaddr7, t0
    addi t2, t2, 32

    lwm {t0-t1, a2-a7}, t2
    csrw pmpaddr8, a7
    csrw pmpaddr9, a6
    csrw pmpaddr10, a5
    csrw pmpaddr11, a4
    csrw pmpaddr12, a3
    csrw pmpaddr13, a2
    csrw pmpaddr14, t1
    csrw pmpaddr15, t0
    addi t2, t2, 32

    lwm {t0-t1, a2-a7}, t2
    csrw pmpaddr16, a7
    csrw pmpaddr17, a6
    csrw pmpaddr18, a5
    csrw pmpaddr19, a4
    csrw pmpaddr20, a3
    csrw pmpaddr21, a2
    csrw pmpaddr22, t1
    csrw pmpaddr23, t0
    addi t2, t2, 32

    lwm {t0-t1, a2-a7}, t2
    csrw pmpaddr24, a7
    csrw pmpaddr25, a6
    csrw pmpaddr26, a5
    csrw pmpaddr27, a4
    csrw pmpaddr28, a3
    csrw pmpaddr29, a2
    csrw pmpaddr30, t1
    csrw pmpaddr31, t0
    addi t2, t2, 32

    lwm {t0-t1, a2-a7}, t2
    csrw pmpcfg0, a7
    csrw pmpcfg1, a6
    csrw pmpcfg2, a5
    csrw pmpcfg3, a4
    csrw pmpcfg4, a3
    csrw pmpcfg5, a2
    csrw pmpcfg6, t1
    csrw pmpcfg7, t0
    addi t2, t2, 32

    lwm {t0-t1, a2-a7}, t2
    csrw memattr0, a7
    csrw memattr1, a6
    csrw memattr2, a5
    csrw memattr3, a4
    csrw memattr4, a3
    csrw memattr5, a2
    csrw memattr6, t1
    csrw memattr7, t0
    addi t2, t2, 32

    lwm {t0-t1, a2-a7}, t2
    csrw memattr8, a7
    csrw memattr9, a6
    csrw memattr10, a5
    csrw memattr11, a4
    csrw memattr12, a3
    csrw memattr13, a2
    csrw memattr14, t1
    csrw memattr15, t0
    addi t2, t2, 32

    lwm {t0-t1, a2-a7}, t2
    csrw memattr16, a7
    csrw memattr17, a6
    csrw memattr18, a5
    csrw memattr19, a4
    csrw memattr20, a3
    csrw memattr21, a2
    csrw memattr22, t1
    csrw memattr23, t0
    addi t2, t2, 32

    lwm {t0-t1, a2-a7}, t2
    csrw memattr24, a7
    csrw memattr25, a6
    csrw memattr26, a5
    csrw memattr27, a4
    csrw memattr28, a3
    csrw memattr29, a2
    csrw memattr30, t1
    csrw memattr31, t0
    addi t2, t2, 32

    /* icache invalid */
    fence iorw, iorw
    csrsi 0x7C2, 0x4
    fence iorw, iorw

    /* dcache invalid */
    fence iorw, iorw
    csrsi 0x7C3, 0x4
    fence iorw, iorw

    li t0, 0xffffffff
    /* delegate interrupt, when tes is 0, untrusted interrupt will be processed by ree */
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

    /* lowpower resume c code */
    jal lowpower_resume

    la t2, g_cpu_suspend_regs
    lw ra, (t2)

    /* after disable tdm, ree can't debug tee */
    TDM_FORCE_DISABLE

    /* jump to ree */
    jalr x0, ra, 0