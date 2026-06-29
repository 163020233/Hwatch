/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   HAL UTILS INTERFACE
 * Author: @CompanyNameTag
 * Create:  2018-10-15
 */

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdint.h>
#include "platform.h"
#include "errcode.h"
#include "vectors.h"
#if defined(__LITEOS__)
#include "los_list.h"
#endif

/** @defgroup connectivity_drivers_hal_hal HAL
  * @ingroup  connectivity_drivers_hal
  * @{
  */
#define WAIT_APPS_DUMP_DELAY_MS_EACH_TIME 16000U
#define WAIT_APPS_DUMP_DELAY_TIMES 2
#define WAIT_APPS_REFRESH_FLASH_MS 2000

#define EXCEP_INFO_FOR_R4_TO_R11 8
#ifndef LOSCFG_EXC_SIMPLE_INFO
#if defined(__LITEOS__)
#define EXC_INFO_SAVE_TP_INDEX 16
#endif
#if defined(__FREERTOS__)
#if CORE == WIFI
#define EXC_INFO_SAVE_TP_INDEX 9
#else
#define EXC_INFO_SAVE_TP_INDEX 8
#endif
#endif
#else
#define EXC_INFO_SAVE_X4_INDEX 15
#endif
#define FS_MAX_FILE_HANDLES 1024
enum INT_ID_FAULT_TYPE_ENUM {
    INT_ID_NMI_FAULT = 2,
    INT_ID_HARD_FAULT = 3,
    INT_ID_MEM_FAULT = 4,
    INT_ID_BUS_FAULT = 5,
    INT_ID_USAGE_FAULT = 6,
    INT_ID_CHIP_WATCHDOG_FAULT = 59,
    INT_ID_WATCHDOG_FAULT = 47,

    INT_ID_INDEX_BUTT
};

/**
 * riscv fault type
 */
enum IRQ_ID_FAULT_TYPE_ENMU {
    IRQ_ID_INSTRUCTION_ADDRESS_MISALIGNED = 0,
    IRQ_ID_INSTRUCTION_ACCESS_FAULT = 1,
    IRQ_ID_ILLEGAL_INSTRUCTION = 2,
    IRQ_ID_BREAKPOINT = 3,
    IRQ_ID_LOAD_ADDERSS_MISALIGNED = 4,
    IRQ_ID_LOAD_ACCESS_FAULT = 5,
    IRQ_ID_STORE_OR_AMO_ADDRESS_MISALIGNED = 6,
    IRQ_ID_STORE_OR_AMO_ACCESS_FALUT = 7,
    IRQ_ID_ENVIRONMENT_CALL_FROM_UMODE = 8,
    IRQ_ID_ENVIRONMENT_CALL_FROM_SMODE = 9,
    IRQ_ID_RESERVED_0 = 10,
    IRQ_ID_ENVIRONMENT_CALL_FROM_MMODE = 11,
    IRQ_ID_INSTRUCTION_PAGE_FAULT = 12,
    IRQ_ID_LOAD_PAGE_FAULT = 13,
    IRQ_ID_RESERVED_1 = 14,
    IRQ_ID_STORE_OR_AMO_PAGE_FAULT = 15,

#if (ARCH == RISCV70)
    IRQ_ID_ASYNCHRONOUS_EXCEPTION = 24,
    IRQ_ID_NMI_INTERRUPT = 0x00000FFF
#else
    IRQ_ID_HARD_FAULT = 16,
    IRQ_ID_LOCK_UP = 17,
    IRQ_ID_NMI_INTERRUPT = 0x8000000C
#endif
};

/**
 * @brief  Structure which is pushed onto the stack by the Cortex-M0 during
 * exception processing - stack frame.
 */
typedef struct stack_frame_t {
    unsigned long stacked_r0;
    unsigned long stacked_r1;
    unsigned long stacked_r2;
    unsigned long stacked_r3;
    unsigned long stacked_r12;
    unsigned long stacked_lr;
    unsigned long stacked_pc;
    unsigned long stacked_psr;
} exception_stack_frame_t;

typedef struct excep_info_t {
    unsigned long sp;
    unsigned long sp_bottom;
    unsigned long exp_task_id;
    unsigned long task_array;
    unsigned long task_max_num;
} exception_info_t;

#ifdef SUPPORT_CALLSTACK
typedef struct {
    uint32_t ra;
    uint32_t fp;
} exc_stack_info_t;
#define EXC_STACK_INFO_SIZE sizeof(exc_stack_info_t)
#else
typedef struct {
    uint32_t sp_addr;
    uint32_t sp_content;
} exc_backtrace_info_t;
#define EXC_STACK_INFO_SIZE sizeof(exc_backtrace_info_t)
#endif

#if defined(__LITEOS__)
typedef struct {
    char task_name[16];
    uint32_t thrd_pid;
    uint16_t type;
    uint16_t nest_cnt;
#ifndef LOSCFG_EXC_SIMPLE_INFO
    char phase[8];
#else
    uint16_t phase;
    uint16_t ret;
#endif
    uint32_t ccause;
    uint32_t mcause;
    uint32_t mtval;
    uint32_t gp;
    uint32_t mstatus;
    uint32_t mepc;
    uint32_t ra;
    uint32_t sp;
    uint32_t tp;
    uint32_t t0;
    uint32_t t1;
    uint32_t t2;
    uint32_t s0;
    uint32_t s1;
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;
    uint32_t a7;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;
    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;
#ifdef SUPPORT_CALLSTACK
    uint32_t stack_size;
    exc_stack_info_t stack[0];
#else
    uint32_t backtrace_size;
    exc_backtrace_info_t backtrace[0];
#endif
} exc_info_save_t;
#define EXC_INFO_SAVE_SIZE sizeof(exc_info_save_t)
#endif

#if defined(__FREERTOS__)
typedef struct {
    uint16_t uw_exc_type;
    uint16_t ret;
    uint32_t mepc;
    uint32_t mstatus;
    uint32_t mtval;
    uint32_t mcause;
#if CORE == WIFI
    uint32_t cxcptsc;
#endif
    uint32_t ra;
    uint32_t sp;
    uint32_t gp;
    uint32_t tp;
    uint32_t t0;
    uint32_t t1;
    uint32_t t2;
    uint32_t s0;
    uint32_t s1;
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;
    uint32_t a7;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;
    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;
#ifdef SUPPORT_CALLSTACK
    uint32_t stack_size;
    exc_stack_info_t stack[0];
#endif
} exc_info_save_t;
#define EXC_INFO_SAVE_SIZE sizeof(exc_info_save_t)
#endif

#if defined(__LITEOS__)
typedef struct {
    uintptr_t callerRa;
    uint32_t totalSize;
    uint32_t blockCount;
} CallerRaMemStat;

typedef struct {
    union {
        LOS_DL_LIST freeNodeInfo;
        struct {
            uintptr_t magic;
            uintptr_t taskId   : 16;
        };
    };
    struct tagLosMemDynNode *preNode;
#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
    uintptr_t callerRa;
#endif
    uint32_t sizeAndFlag;
} LosMemCtlNode;

typedef struct tagLosMemDynNode {
    LosMemCtlNode selfNode;
} LosMemDynNode;

#define OS_MEM_NODE_GET_USED_FLAG(sizeAndFlag) \
((sizeAndFlag) & OS_MEM_NODE_USED_FLAG)
#define OS_MEM_NODE_GET_SIZE(sizeAndFlag) \
((sizeAndFlag) & ~OS_MEM_NODE_ALIGNED_AND_USED_FLAG)
#define OS_MEM_END_NODE(pool, size) \
((LosMemDynNode *)(void *)(((uint8_t *)(pool) + (size)) - OS_MEM_NODE_HEAD_SIZE))

#define OS_MEM_NODE_USED_FLAG               0x80000000U
#define OS_MEM_NODE_ALIGNED_FLAG            0x40000000U
#define OS_MEM_NODE_ALIGNED_AND_USED_FLAG   (OS_MEM_NODE_USED_FLAG | OS_MEM_NODE_ALIGNED_FLAG)

#define RA_FLAG_MASK           ((uintptr_t)1 << (sizeof(uintptr_t) * 8 - 1))
#define RA_IS_FLAGGED(ra)      (((ra) & RA_FLAG_MASK) != 0)
#define RA_SET_FLAG(ra)        ((ra) | RA_FLAG_MASK)
#define RA_CLR_FLAG(ra)        ((ra) & (~RA_FLAG_MASK))
#define RA_BATCH_SIZE          ((IRQ_STACK_SIZE) / sizeof(CallerRaMemStat))
#define GET_IRQSTACK_RASTATS() ((CallerRaMemStat *)(IRQ_STACK_TOP - sizeof(CallerRaMemStat) * RA_BATCH_SIZE))
#define OS_MEM_NODE_HEAD_SIZE  sizeof(LosMemDynNode)
#endif
/**
 * @brief  Callback to output the exception information.
 */
typedef void (*nmi_proc_func)(exc_context_t *);
typedef void (*hal_exception_dump_callback)(uint32_t irq_id, exc_context_t *exc_buf_addr);

/**
 * @brief  Prototype for the assembly language hard-fault handler. This will setup the stack and attempt to
 * generate a stack trace to aid debug.
 * @param  exc_buf_addr Fault param1.
 */
void do_hard_fault_handler(exc_context_t *exc_buf_addr);
#if defined(__LITEOS__)
void do_fault_handler(uint32_t exc_type, exc_context_t *exc_buff_addr);
#endif

/**
 * @brief  Register the exception dump callback
 * @param  callback The dump callback func
 */
void hal_register_exception_dump_callback(hal_exception_dump_callback callback);

/**
 * @brief  Save the exception reset information for debug after the restart.
 * @return
 */
void hal_save_exception_reset_info(void);

#ifndef SUPPORT_CALLSTACK
void back_trace(uint32_t fp);
#endif
#ifndef USE_CMSIS_OS
void do_trap_unknown(exc_context_t *exc_buff_addr);
void do_trap_insn_misaligned(exc_context_t *exc_buff_addr);
void do_trap_insn_fault(exc_context_t *exc_buff_addr);
void do_trap_insn_illegal(exc_context_t *exc_buff_addr);
void do_trap_load_misaligned(exc_context_t *exc_buff_addr);
void do_trap_load_fault(exc_context_t *exc_buff_addr);
void do_trap_store_misaligned(exc_context_t *exc_buff_addr);
void do_trap_store_fault(exc_context_t *exc_buff_addr);
void do_trap_ecall_u(exc_context_t *exc_buff_addr);
void do_trap_ecall_s(exc_context_t *exc_buff_addr);
void do_trap_ecall_m(exc_context_t *exc_buff_addr);
void do_trap_break(exc_context_t *exc_buff_addr);
void do_insn_page_fault(exc_context_t *exc_buff_addr);
void do_load_page_fault(exc_context_t *exc_buff_addr);
void do_store_page_fault(exc_context_t *exc_buff_addr);
void do_hard_fault(exc_context_t *exc_buff_addr);
void do_lockup(exc_context_t *exc_buff_addr);
void default_handler(void);
void nmi_handler(void);
#endif

#ifdef SAVE_EXC_INFO
#ifdef CFG_DRIVERS_MMC
/**
 * @brief  Emmc write exc info to the file system from sector after reboot system.
 * @return Success or fail.
 */
errcode_t emmc_exc_info_write(void);
#endif
#endif

/**
 * @brief  ipc exception callbck register.
 */
void ipc_exception_test_init(void);

void reset_deal_with_fs(void);

typedef void (*fs_terminate_handler)(void);
void regist_file_system_terminate_handler(fs_terminate_handler handler);
void set_file_system_mount_ret(int32_t rets);

/**
 * @brief  reboot exception callbck function.
 */
void hal_exception_mcu_callback(int32_t src);

/**
 * @brief  hard watchdog reset save trace and comram function.
 */
void hard_watchdog_reset_save_info(void);

/**
 * @brief  hard watchdog reset copy comram to tcm function.
 */
void hard_watchdog_reset_copy_comram(void);
/**
  * @}
  */
#endif
