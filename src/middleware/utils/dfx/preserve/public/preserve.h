/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: PRESERVE INTERFACE for security core
 *
 * Create:
 */
#ifndef PRESERVE_H
#define PRESERVE_H

#include "exception.h"
#include "chip_core_definition.h"
#include "memory_config.h"
#include "panic.h"
#include "non_os_reboot.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup connectivity_libs_preserve PRESERVE
 * @ingroup  connectivity_libs
 * @{
 */
#define PRESERVE var_segment("preserve")

#pragma pack(4)

#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
typedef struct {
    uint32_t mstatus;
    uint32_t mepc;
    uint32_t tp;    /* X4 */
    uint32_t sp;    /* X2 */

    uint32_t s11;   /* X27 */
    uint32_t s10;   /* X26 */
    uint32_t s9;    /* X25 */
    uint32_t s8;    /* X24 */
    uint32_t s7;    /* X23 */
    uint32_t s6;    /* X22 */
    uint32_t s5;    /* X21 */
    uint32_t s4;    /* X20 */
    uint32_t s3;    /* X19 */
    uint32_t s2;    /* X18 */

    uint32_t s1;    /* X9 */
    uint32_t s0;    /* X8 */

    uint32_t t6;    /* X31 */
    uint32_t t5;    /* X30 */
    uint32_t t4;    /* X29 */
    uint32_t t3;    /* X28 */

    uint32_t a7;    /* X17 */
    uint32_t a6;    /* X16 */
    uint32_t a5;    /* X15 */
    uint32_t a4;    /* X14 */
    uint32_t a3;    /* X13 */
    uint32_t a2;    /* X12 */
    uint32_t a1;    /* X11 */
    uint32_t a0;    /* X10 */

    uint32_t t2;    /* X7 */
    uint32_t t1;    /* X6 */
    uint32_t t0;    /* X5 */

    uint32_t ra;    /* X1 */
} preserve_task_context_t;

typedef struct {
    uint32_t ccause;
    uint32_t mcause;
    uint32_t mtval;
    uint32_t gp;
    preserve_task_context_t task_context;
} preserve_exc_context_t;

#endif /* ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70)) */

#if ((ARCH == CM3) || (ARCH == CM7))
typedef struct {
    uint32_t fault_reason;
    uint32_t fault_address;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t primask;
    uint32_t sp;
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t xpsr;
    uint32_t reserved[16];
} preserve_exc_context_t;
#endif /* (ARCH == CM3) || (ARCH == CM7) */

typedef struct {
    uint32_t sp;
    uint32_t sp_bottom;
    uint32_t exp_task_id;
    uint32_t task_array;
    uint32_t task_max_num;
} preserve_exception_info_t;

typedef struct {
    preserve_exc_context_t exc_context;         /* 异常上下文. 144 bytes */
    preserve_exception_info_t exception_info;   /* 异常时任务信息：sp、任务ID，最大任务数. 20 bytes */
    panic_desc_t last_panic;                    /* panic信息：ID、code、时间戳、caller等. 16 bytes */
    uint32_t entry_sleep_time[2];   /* 数组长度为2, 分别保存高32位和低32位. 8 bytes */
    uint32_t rtc_delay_count[2];    /* 保存最后一次的rtc_delay值. 数组长度为2, 分别保存高32位和低32位. 8 bytes */
    uint32_t excepted_delay_time;   /* 保存唤醒时间. 4 bytes */
    uint32_t exception_time_s;      /* 保存发生异常时时间戳. 4 bytes */
    uint32_t reserved[3];                       /* The size of the reserved array n is 3, total 12 bytes. */
    reboot_cause_t cpu_utils_reset_cause;       /* 复位原因. 4 bytes */
    bool update_reset_cause_on_boot;            /* 启动时是否更新复位原因. 4 bytes */
    uint32_t core_id;                           // 4
    uint32_t reserved1;                         // 4
    uint32_t intid;                             /* 保存异常时的中断ID. 4 bytes */
    uint32_t magic;                             // exception flag
    uint32_t reboot_status;                     // 4
    uint32_t reboot_count;          /* 冷启动以来的复位次数. 4 bytes */
    uint32_t ep_reboot_count;       /* 连续异常复位次数. 4 bytes */
    uint32_t enable_sha_check;      /* 是否打开HASH校验. 4 bytes */
} preserve_data_t;
#pragma pack()

void set_crash_core_type(cores_t core);
cores_t get_crash_core_type(void);

void duplicate_app_preserve_mem(void);
bool is_preserve_data_saved(uintptr_t preserve_ptr);

void system_boot_status_init(void);

unsigned int get_system_boot_status(void);

panic_desc_t *get_last_panic_handle(void);

void set_last_panic(panic_desc_t *last_panic);
#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
uint32_t get_cpu_utils_exc_ccause(void);
uint32_t get_cpu_utils_exc_mcause(void);
uint32_t get_cpu_utils_exc_mtval(void);

void set_exception_info_riscv(exc_context_t *exc_data);
preserve_task_context_t *get_exc_task_context(void);
#endif /* ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70)) */

#if ((ARCH == CM3) || (ARCH == CM7))

void set_fault_address(uint32_t address);
void set_fault_reason(uint32_t reason);

#endif /* ((ARCH == CM3) || (ARCH == CM7)) */

void set_exception_stack_frame(exception_stack_frame_t *exception_stack_frame_src);
void set_exception_info(exception_info_t *exception_info);

preserve_exception_info_t *get_exception_info(void);

void set_fault_intid(uint32_t intid);

uint32_t get_cpu_utils_check_sha_value(void);

void set_cpu_utils_check_sha_fault_value(void);

uint8_t get_last_panic_id(void);

uint32_t get_last_panic_code(void);

uint32_t get_last_panic_caller(void);

uint32_t get_cpu_utils_exc_pc(void);
void set_cpu_utils_exc_pc(uint32_t pc);

uint32_t get_cpu_utils_exc_lr(void);
void set_cpu_utils_exc_lr(uint32_t lr);

void set_system_boot_status(unsigned int reboot_status);

void set_cpu_utils_system_boot_magic(void);

void set_cpu_utils_reset_cause(reboot_cause_t reset_cause);
unsigned int get_cpu_utils_reset_count(void);
unsigned int get_cpu_utils_epreset_count(void);
reboot_cause_t get_cpu_utils_reset_cause(void);

void set_update_reset_cause_on_boot(bool reset_cause_on_boot);

bool get_update_reset_cause_on_boot(void);

void set_entry_sleep_time(uint64_t time);
void set_rtc_delay_count(uint64_t time);
void set_excepted_sleep_time(uint32_t time);
void set_exception_time_stamp(void);

uint8_t get_last_slave_panic_id(cores_t core);

uint32_t get_last_slave_panic_code(cores_t core);

uint32_t get_last_slave_panic_caller(cores_t core);

void system_boot_reason_print(void);

void system_boot_reason_process(void);

uint32_t get_system_magic(void);

void show_reboot_info(void);
#if defined(BUILD_APPLICATION_STANDARD)
void print_system_boot_status(void);
#endif

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* PRESERVE_H */
