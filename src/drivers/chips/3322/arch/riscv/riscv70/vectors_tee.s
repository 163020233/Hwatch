/******************************************************************************
 * @brief    linx170 vectors module implementation
 * Copyright (c) 2022 Huawei Technologies Co. Ltd
******************************************************************************/
.equ MCAUSE_MASK_INT_NUM, 0xFFF
.equ NMI_EXCP_CODE, 0xFFF
.macro push_reg
    addi sp, sp, -32*4

    sw ra, 0 * 4(sp)
    sw sp, 1 * 4(sp)
    sw gp, 2 * 4(sp)
    sw tp, 3 * 4(sp)
    sw t0, 4 * 4(sp)
    sw t1, 5 * 4(sp)
    sw t2, 6 * 4(sp)
    sw s0, 7 * 4(sp)
    sw s1, 8 * 4(sp)
    sw a0, 9 * 4(sp)
    sw a1, 10 * 4(sp)
    sw a2, 11 * 4(sp)
    sw a3, 12 * 4(sp)
    sw a4, 13 * 4(sp)
    sw a5, 14 * 4(sp)
    sw a6, 15 * 4(sp)
    sw a7, 16 * 4(sp)
    sw s2, 17 * 4(sp)
    sw s3, 18 * 4(sp)
    sw s4, 19 * 4(sp)
    sw s5, 20 * 4(sp)
    sw s6, 21 * 4(sp)
    sw s7, 22 * 4(sp)
    sw s8, 23 * 4(sp)
    sw s9, 24 * 4(sp)
    sw s10, 25 * 4(sp)
    sw s11, 26 * 4(sp)
    sw t3, 27 * 4(sp)
    sw t4, 28 * 4(sp)
    sw t5, 29 * 4(sp)
    sw t6, 30 * 4(sp)

    csrr t0, tmepc
    sw t0, 31 * 4(sp)
.endm

.macro pop_reg
    lw    t0, 31 * 4(sp)
    csrw  tmepc, t0

    lw ra, 0 * 4(sp)

    lw t0, 4 * 4(sp)
    lw t1, 5 * 4(sp)
    lw t2, 6 * 4(sp)
    lw s0, 7 * 4(sp)
    lw s1, 8 * 4(sp)
    lw a0, 9 * 4(sp)
    lw a1, 10 * 4(sp)
    lw a2, 11 * 4(sp)
    lw a3, 12 * 4(sp)
    lw a4, 13 * 4(sp)
    lw a5, 14 * 4(sp)
    lw a6, 15 * 4(sp)
    lw a7, 16 * 4(sp)
    lw s2, 17 * 4(sp)
    lw s3, 18 * 4(sp)
    lw s4, 19 * 4(sp)
    lw s5, 20 * 4(sp)
    lw s6, 21 * 4(sp)
    lw s7, 22 * 4(sp)
    lw s8, 23 * 4(sp)
    lw s9, 24 * 4(sp)
    lw s10, 25 * 4(sp)
    lw s11, 26 * 4(sp)
    lw t3, 27 * 4(sp)
    lw t4, 28 * 4(sp)
    lw t5, 29 * 4(sp)
    lw t6, 30 * 4(sp)

    addi sp, sp, 32*4
.endm

.macro SAVE_ALL
    addi sp, sp, -36 * 4
    sw s11, 8 * 4(sp)
    sw s10, 9 * 4(sp)
    sw s9, 10 * 4(sp)
    sw s8, 11 * 4(sp)
    sw s7, 12 * 4(sp)
    sw s6, 13 * 4(sp)
    sw s5, 14 * 4(sp)
    sw s4, 15 * 4(sp)
    sw s3, 16 * 4(sp)
    sw s2, 17 * 4(sp)
    sw s1, 18 * 4(sp)
    sw s0, 19 * 4(sp)
    sw t6, 20 * 4(sp)
    sw t5, 21 * 4(sp)
    sw t4, 22 * 4(sp)
    sw t3, 23 * 4(sp)
    sw a7, 24 * 4(sp)
    sw a6, 25 * 4(sp)
    sw a5, 26 * 4(sp)
    sw a4, 27 * 4(sp)
    sw a3, 28 * 4(sp)
    sw a2, 29 * 4(sp)
    sw a1, 30 * 4(sp)
    sw a0, 31 * 4(sp)
    sw t2, 32 * 4(sp)
    sw t1, 33 * 4(sp)
    sw t0, 34 * 4(sp)
    sw ra, 35 * 4(sp)

    csrr  s4, tcxcptsc
    sw s4, 0 * 4(sp)
    csrr  s3, tmcause
    sw s3, 1 * 4(sp)
    csrr  s2, mtval
    sw s2, 2 * 4(sp)
    sw gp, 3 * 4(sp)
    csrr  s1, tmstatus
    sw s1, 4 * 4(sp)
    csrr  s0, tmepc
    sw s0, 5 * 4(sp)
    sw tp, 6 * 4(sp)
    sw sp, 7 * 4(sp)
.endm

.macro RESTORE_ALL
    lw t0, 5 * 4(sp)
    csrw  mepc, t0

    lw s11, 8 * 4(sp)
    lw s10, 9 * 4(sp)
    lw s9, 10 * 4(sp)
    lw s8, 11 * 4(sp)
    lw s7, 12 * 4(sp)
    lw s6, 13 * 4(sp)
    lw s5, 14 * 4(sp)
    lw s4, 15 * 4(sp)
    lw s3, 16 * 4(sp)
    lw s2, 17 * 4(sp)
    lw s1, 18 * 4(sp)
    lw s0, 19 * 4(sp)
    lw t6, 20 * 4(sp)
    lw t5, 21 * 4(sp)
    lw t4, 22 * 4(sp)
    lw t3, 23 * 4(sp)
    lw a7, 24 * 4(sp)
    lw a6, 25 * 4(sp)
    lw a5, 26 * 4(sp)
    lw a4, 27 * 4(sp)
    lw a3, 28 * 4(sp)
    lw a2, 29 * 4(sp)
    lw a1, 30 * 4(sp)
    lw a0, 31 * 4(sp)
    lw t2, 32 * 4(sp)
    lw t1, 33 * 4(sp)
    lw t0, 34 * 4(sp)
    lw ra, 35 * 4(sp)

    addi sp, sp, 36 * 4
.endm

    .align 4
    .global trap_entry
trap_entry: /* exception use tmtvec, lds keep 64B align */
    SAVE_ALL

    /* Exception run with interrupts masked */
    csrc tmstatus, 0x00000008
    csrw tmscratch, sp

    la t0, MCAUSE_MASK_INT_NUM
    and  s3, t0, s3
    la t0, NMI_EXCP_CODE
    beq s3, t0, 1f
    la sp, __excp_stack_top__
    j 2f
1:
    la sp, __nmi_stack_top__
2:
    csrr a0, tmscratch
    jal do_hard_fault_handler_for_boot
    /* not be here */
    csrr sp, tmscratch
    # Remain in M-mode after mret
    li t0, 0x00001800
    csrs tmstatus, t0

    RESTORE_ALL
    mret

    .align 4
    .global trap_excp
    .section .text.trap_excp
    .option norvc
trap_excp: /* mtvec */
    j trap_entry

    .align 4
    .global trap_vector
    .section .text.trap_vector
    .option norvc
trap_vector: /* int default use mtvt, lds keep 64B align, 1A71 not use int */
    .word default_handler
    .word default_handler
    .word default_handler
    .word default_handler
    .word default_handler
    .word default_handler
    .word default_handler
    .word default_handler
    .word default_handler
    .word default_handler
    .word default_handler
    .word default_handler
    .word default_handler
    .word default_handler
    .word default_handler
    .word default_handler
    /* isr 0-9 */
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    /* isr 10-19 */
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    /* isr 20-29 */
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    /* isr 30-39 */
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    /* isr 40-49 */
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    /* isr 50-59 */
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    /* isr 60-69 */
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    /* isr 70-79 */
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    /* isr 80-89 */
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .word default_local_interrupt_handler
    .option rvc
    .align 4


default_local_interrupt_handler:
    csrci tmstatus, 0x08
    push_reg

    csrw tmscratch, sp
    la  sp, __irq_stack_top__

    jal local_interrupt_handler

    csrr  sp, tmscratch

    li  t0, 0x1880
    csrs  tmstatus, t0

    pop_reg
    mret

    .global global_interrupt_lock
global_interrupt_lock:
    csrr    a0, tmstatus
    li      t0, 0x08
    csrrc   zero, tmstatus, t0
    ret

    .global global_interrupt_unlock
global_interrupt_unlock:
    csrr    a0, tmstatus
    li      t0, 0x08
    csrrs   zero, tmstatus, t0
    ret

    .global global_interrupt_restore
global_interrupt_restore:
    csrw tmstatus, a0
    ret
