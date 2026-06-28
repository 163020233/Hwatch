/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: cpu register lowpower process file
 */
#include "clocks_switch"

.global clocks_set_mcu_freq
.extern __nmi_stack_top__
.extern g_stack_switch_sp_save

.section .data
g_last_msplimit_val:
    .word 0

.section .text
.option rvc

.macro PUSH_CALLER_REG
    addi sp,  sp, -64
    sw   t6,  (0 * 4)(sp)
    sw   t5,  (1 * 4)(sp)
    sw   t4,  (2 * 4)(sp)
    sw   t3,  (3 * 4)(sp)
    sw   a7,  (4 * 4)(sp)
    sw   a6,  (5 * 4)(sp)
    sw   a5,  (6 * 4)(sp)
    sw   a4,  (7 * 4)(sp)
    sw   a3,  (8 * 4)(sp)
    sw   a2,  (9 * 4)(sp)
    sw   a1,  (10 * 4)(sp)
    sw   a0,  (11 * 4)(sp)
    sw   t2,  (12 * 4)(sp)
    sw   t1,  (13 * 4)(sp)
    sw   t0,  (14 * 4)(sp)
    sw   ra,  (15 * 4)(sp)
.endm

.macro POP_CALLER_REG
    lw   t6,  (0 * 4)(sp)
    lw   t5,  (1 * 4)(sp)
    lw   t4,  (2 * 4)(sp)
    lw   t3,  (3 * 4)(sp)
    lw   a7,  (4 * 4)(sp)
    lw   a6,  (5 * 4)(sp)
    lw   a5,  (6 * 4)(sp)
    lw   a4,  (7 * 4)(sp)
    lw   a3,  (8 * 4)(sp)
    lw   a2,  (9 * 4)(sp)
    lw   a1,  (10 * 4)(sp)
//    lw   a0,  (11 * 4)(sp) 不修改a0, 不影响返回值
    lw   t2,  (12 * 4)(sp)
    lw   t1,  (13 * 4)(sp)
    lw   t0,  (14 * 4)(sp)
    lw   ra,  (15 * 4)(sp)
    addi sp,  sp, 64
#endif
.endm

.align 4
clocks_set_mcu_freq:
    PUSH_CALLER_REG
    // 保存sp指针, 切换至中断RAM栈
    la      t0, g_stack_switch_sp_save
    sw      sp, 0(t0)
#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
    // 记录msplimit的值
    csrr    t1, msplimit
    la      t2, g_last_msplimit_val
    sw      t1, 0(t2)
    // 把 nmi栈顶配置为要监控的值
    la      t1, __nmi_stack_top__
    lw      t2, 0(t1)
    csrw    msplimit, t2
#endif
    la      sp, __nmi_stack_top__
    call    clocks_set_mcu_freq_ram

    // 恢复sp指针
    la      t0, g_stack_switch_sp_save
#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
    la      t1, g_last_msplimit_val
    lw      t2, 0(t1)
    csrw    msplimit, t2
#endif
    lw      sp, 0(t0)
    POP_CALLER_REG
    ret
