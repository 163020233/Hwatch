/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: drvbox call asm for SELiteOS.
 */

#ifndef DRVBOX_CALL_S
#define DRVBOX_CALL_S

.macro PUSH_ARG_REGS
    addi sp, sp, -8 * 4
    swm {a0-a7}, sp
.endm

.macro POP_ARG_REGS
    lwm {a0-a7}, sp
    addi sp, sp, 8 * 4
.endm

.global switch_drvbox
switch_drvbox:
    addi  sp, sp, -4
    sw    ra, 0(sp)
    PUSH_ARG_REGS
    jal   crypto_switch_drvbox
    POP_ARG_REGS
    lw    ra, 0(sp)
    addi  sp, sp, 4
    ret

.global switch_usrbox
switch_usrbox:
    addi  sp, sp, -4
    sw    ra, 0(sp)
    PUSH_ARG_REGS
    jal   crypto_switch_usrbox
    POP_ARG_REGS
    lw    ra, 0(sp)
    addi  sp, sp, 4
    ret

.macro drvbox_call name, n
.global drvbox_\name
.type drvbox_\name, @function
drvbox_\name:
    addi  sp, sp, -4
    sw    ra, 0(sp)
    jal   switch_drvbox
    li    a7, \n
    la    t0, g_security_unified
    slli  a7, a7, 2
    add   t0, t0, a7
    lw    t0, 0(t0)
    jalr  t0
    jal   switch_usrbox
    lw    ra, 0(sp)
    addi  sp, sp, 4
    ret
.endm

/* n   : Response ID.
 * name: drvbox call function name, drvbox call user state interface drvbox_name,
 *       kernel state interface name.
 */
#define LITEOS_DRVBOX_CALL_DEF(name, n)                 drvbox_call name, n
#include "drvbox_calls_def.h"

#endif
