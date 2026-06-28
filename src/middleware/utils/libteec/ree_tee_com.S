/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
* 
* filename: ree_tee_com.S
* Author: @CompanyNameTag
* Create:2024-06-06
* Description: the assembly file of ree communicate with tee
*
*/

#ifndef TESVEC_CALL_S
#define TESVEC_CALL_S

.text
.align  4

.macro SAVE_REE_CALL_TESVEC_REGS
    addi sp, sp, -32 * 2
    swm {ra, s0-s11, t6}, sp
    addi sp, sp, -32
    swm {t0-t5}, sp
.endm

.macro RESTORE_REE_CALL_TESVEC_REGS
    lwm {t0-t5}, sp
    addi sp, sp, 32
    lwm {ra, s0-s11, t6}, sp
    addi sp, sp, 32 * 2
.endm

.macro tesvec_call name, n
.global \name
.type \name, @function
\name:
    SAVE_REE_CALL_TESVEC_REGS
	csrr  t0, tmesvec
    addi  t0, t0, \n
	jalr  ra, t0, 0
    RESTORE_REE_CALL_TESVEC_REGS
    jr    ra
.endm

/* n   : Response ID.
 * name: tesvec call function name
 */
#define TESVEC_CALL_DEF(name, n)              tesvec_call name, n
#include "tesvec_calls_def.h"

#endif
