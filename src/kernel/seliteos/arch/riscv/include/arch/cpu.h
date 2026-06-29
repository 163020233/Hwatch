/******************************************************************************
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: cpu.h for SELiteOS.
******************************************************************************/

#ifndef ARCH_CPU_H
#define ARCH_CPU_H

#ifndef __ASSEMBLER__
#include "los_typedef.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SYS_MODE_DEFAULT_KERNEL LOS_MSTATUS_MPP

/* support riscv32 only. */
#define LREG                    lw
#define SREG                    sw
#define REGBYTES                4

/*  |   INTEGER_CALLER_REG_NUM    |   t0 - t6, a0 - a7, ra
    |-----------------------------|
    |   INTEGER_CALLEE_REG_NUM    |   s0 - s11
    |-----------------------------| if in Exc context
    |    EXC_SPECIAL_REG_NUM      |   CXCPTSC,MCAUSE,MTVAL,gp
    |-----------------------------| endif in Exc context
*/
#define MCAUSE_STACK_INDEX          2
#define SP_STACK_INDEX              (MCAUSE_STACK_INDEX + 1)
#define S11_STACK_INDEX             (SP_STACK_INDEX + 1)

#define EXC_SPECIAL_REG_NUM         4   // CXCPTSC,MCAUSE,MTVAL,gp
#define EXC_SPECIAL_REG_SIZE        (4 * REGBYTES)

#define INTEGER_CALLER_REG_NUM      16
#define INTEGER_CALLEE_REG_NUM      12
#define CPU_SPECIAL_REG_NUM         4   // MSTATUS,MEPC,tp,sp
#define INTEGER_REG_NUM             (INTEGER_CALLER_REG_NUM + INTEGER_CALLEE_REG_NUM + CPU_SPECIAL_REG_NUM)

#define INTEGER_CALLER_REG_SIZE     (INTEGER_CALLER_REG_NUM * REGBYTES)
#define INTEGER_CALLEE_REG_SIZE     (INTEGER_CALLEE_REG_NUM * REGBYTES)
#define CPU_SPECIAL_REG_SIZE        (CPU_SPECIAL_REG_NUM * REGBYTES)
#define INTEGER_REG_SIZE            (INTEGER_CALLER_REG_SIZE + INTEGER_CALLEE_REG_SIZE + CPU_SPECIAL_REG_SIZE)

#define INT_SIZE_ON_STACK           INTEGER_REG_SIZE

/* risc-v mstatus Register */
#define MCAUSE_INSN_MISALIGN    0x0
#define MCAUSE_INSN_FAULT       0x1
#define MCAUSE_INSN_ILLEGAL     0x2
#define MCAUSE_BRKT             0x3
#define MCSUSE_LOAD_MISALIGN    0x4
#define MCAUSE_LOAD_FAULT       0x5
#define MCAUSE_AMO_MISALIGN     0x6
#define MCAUSE_AMO_FAULT        0x7
#define MCAUSE_ECALL_U          0x8
#define MCAUSE_ECALL_M          0xB
#define MCAUSE_INSN_PAGE_FAULT  0xC
#define MCAUSE_LOAD_PAGE_FAULT  0xD
#define MCAUSE_AMO_PAGE_FAULT   0xF

#define LOS_MSTATUS_UIE         0x00000001U
#define LOS_MSTATUS_SIE         0x00000002U
#define LOS_MSTATUS_HIE         0x00000004U
#define LOS_MSTATUS_MIE         0x00000008U
#define LOS_MSTATUS_UPIE        0x00000010U
#define LOS_MSTATUS_SPIE        0x00000020U
#define LOS_MSTATUS_HPIE        0x00000040U
#define LOS_MSTATUS_MPIE        0x00000080U
#define LOS_MSTATUS_SPP         0x00000100U
#define LOS_MSTATUS_HPP         0x00000600U
#define LOS_MSTATUS_MPP         0x00001800U
#define LOS_MSTATUS_FS          0x00006000U
#define LOS_MSTATUS_XS          0x00018000U
#define LOS_MSTATUS_MPRV        0x00020000U
#define LOS_MSTATUS_PUM         0x00040000U
#define LOS_MSTATUS_VM          0x1F000000U
#define LOS_MSTATUS32_SD        0x80000000U
#define LOS_MSTATUS64_SD        0x8000000000000000U

#define LOS_MCAUSE32_CAUSE      0x7FFFFFFFU
#define LOS_MCAUSE64_CAUSE      0x7FFFFFFFFFFFFFFFU
#define LOS_MCAUSE32_INT        0x80000000U
#define LOS_MCAUSE64_INT        0x8000000000000000U

#define LOS_MCAUSE_UOP          0x0100F000

#define LOS_MCAUSE_MPIE         0x08000000
#define LOS_MCAUSE_MPP          0x30000000
#define LOSCFG_MCAUSE_M         (LOS_MCAUSE_MPIE | LOS_MCAUSE_MPP)
#define LOSCFG_MCAUSE_U         (LOS_MCAUSE_MPIE)

#define LOSCFG_MSTATUS_M        (LOS_MSTATUS_MPP | LOS_MSTATUS_MPIE)
#define LOSCFG_MSTATUS_U        LOS_MSTATUS_MPIE

#ifndef UINT32_CUT_MASK
#define UINT32_CUT_MASK         0xFFFFFFFFU
#endif
#define UINT8_CUT_MASK          0xFFU

#define NUM_HAL_INTERRUPT_NMI   0xFFF

#define MSTATUS     tmstatus
#define MEPC        tmepc
#define MCAUSE      tmcause
#define MTVEC       tmtvec
#define MTVT        tmtvt
#define MINTTHRESH  tmintthresh
#define MTVAL       tmtval
#define MSCRATCH    tmscratch
#define CXCPTSC     tcxcptsc
#define TMSPLIMIT   tmsplimit

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ARCH_CPU_H */
