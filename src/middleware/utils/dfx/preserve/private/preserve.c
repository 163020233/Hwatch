/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:PRESERVE INTERFACE for security core
 *
 * Create:
 */
#include "preserve.h"
#include "securec.h"
#if defined(BUILD_APPLICATION_STANDARD)
#include "log_printf.h"
#include "systick.h"
#include "log_oam_logger.h"
#include "log_def.h"
#include "non_os_reboot.h"
#endif
#include "debug_print.h"
#include "hal_reboot.h"
#ifdef SUPPORT_CPU_TRACE_PRINT
#include "oam_trace.h"
#endif

#define RIGHT_SHIFT_32BITS  32
#define WORD_MASK_VALUE  0xFFFFFFFF
#define TIME_S_TO_MS  1000
#define RESERVED_PC_INDEX 0
#define RESERVED_LR_INDEX 1
#define RESERVED_SP_INDEX 2

PRESERVE preserve_data_t g_preserve_data_lib;
#if !defined(BUILD_APPLICATION_ROM)
STATIC preserve_data_t *g_preserve_data_lib_ptr = (preserve_data_t *)(uintptr_t)(PRESERVED_REGION_ORIGIN);
#endif

cores_t get_crash_core_type(void)
{
    return (cores_t)g_preserve_data_lib.core_id;
}

void set_crash_core_type(cores_t core)
{
    g_preserve_data_lib.core_id = (uint32_t)core;
}

void duplicate_app_preserve_mem(void)
{
#ifdef CONFIG_SUPPORT_DOUBLE_PRESERVE
    uint32_t size = (sizeof(g_preserve_data_lib) <= APP_PRESERVED_REGION_LENGTH) ?
        sizeof(g_preserve_data_lib) : APP_PRESERVED_REGION_LENGTH;
    preserve_data_t *app_preserve_data = (preserve_data_t *)(uintptr_t)(APP_PRESERVED_REGION_ORIGIN);
    if (app_preserve_data->magic != STANDARD_REBOOT_MAGIC && app_preserve_data->magic != ABNORMAL_REBOOT_MAGIC) {
        memcpy_s((void *)app_preserve_data, APP_PRESERVED_REGION_LENGTH, (void *)&g_preserve_data_lib, size);
    }
#endif
}

bool is_preserve_data_saved(uintptr_t preserve_ptr)
{
    preserve_data_t *preserve_data = (preserve_data_t *)preserve_ptr;
    return preserve_data->magic == STANDARD_REBOOT_MAGIC || preserve_data->magic == ABNORMAL_REBOOT_MAGIC;
}

static inline void set_chip_system_boot_magic(uint32_t magic)
{
    g_preserve_data_lib.magic = magic;
}

static void system_set_reboot_status_by_crash_cause(void)
{
    reboot_cause_t cause = g_preserve_data_lib.cpu_utils_reset_cause;
    if ((REBOOT_CAUSE_BT_RESET_UNKNOWN <= cause && cause <= REBOOT_CAUSE_BT_END) ||
        (REBOOT_CAUSE_GNSS_GLOBAL <= cause && cause <= REBOOT_CAUSE_GNSS_END) ||
        (REBOOT_CAUSE_PROTOCOL_GLOBAL <= cause && cause <= REBOOT_CAUSE_PROTOCOL_END)) {
        g_preserve_data_lib.reboot_status = REBOOT_OTHER_CORE_ABNORMAL_TRIGER_STATUS;
    } else if (cause == REBOOT_CAUSE_APPLICATION_HARDFAULT) {
        g_preserve_data_lib.reboot_status = REBOOT_HARD_FAULT_TRIGER_STATUS;
    } else if (cause == REBOOT_CAUSE_APPLICATION_XIP_CTRL) {
        g_preserve_data_lib.reboot_status = REBOOT_NMI_XIP_CTRL_TRIGER_STATUS;
    } else if (cause == REBOOT_CAUSE_APPLICATION_XIP_CACHE) {
        g_preserve_data_lib.reboot_status = REBOOT_NMI_XIP_CACHE_TRIGGER_STATUS;
    } else if (cause == REBOOT_CAUSE_APPLICATION_CHIP_WDT) {
        g_preserve_data_lib.reboot_status = REBOOT_NMI_WDGTIMEOUT_TRIGGER_STATUS;
    } else if (cause == REBOOT_CAUSE_APPLICATION_PANIC) {
        g_preserve_data_lib.reboot_status = REBOOT_SOFT_PANIC_TRIGGER_STATUS;
    } else if (cause == REBOOT_CAUSE_APPLICATION_STD_BT_WDT_FRST) {
        g_preserve_data_lib.reboot_status = REBOOT_BT_WDGTIMEOUT_TRIGER_STATUS;
    } else if (cause == REBOOT_CAUSE_APPLICATION_STD_ULP_WDT_FRST) {
        g_preserve_data_lib.reboot_status = REBOOT_ULP_WDGTIMEOUT_TRIGER_STATUS;
    } else if (cause == REBOOT_CAUSE_APPLICATION_STD_ULP_PIN_FRST) {
        g_preserve_data_lib.reboot_status = REBOOT_ULP_PIN_RST_TRIGER_STATUS;
    } else if (cause == REBOOT_CAUSE_POWER_ON) {
        g_preserve_data_lib.reboot_status = REBOOT_POWER_ON_TRIGGER_STATUS;
    } else if (cause == REBOOT_CAUSE_UPG_COMPLETION) {
        g_preserve_data_lib.reboot_status = REBOOT_UPG_COMPLETION_TRIGER_STATUS;
    } else {
        g_preserve_data_lib.reboot_status = REBOOT_UNDEFINED_TRIGER_STATUS;
    }
}

#ifdef weak
weak void watchdog_porting_pmu_reboot(void)
{
}
#else
__attribute__((weak)) void watchdog_porting_pmu_reboot(void)
{
}
#endif

void system_boot_status_init(void)
{
    bool is_hardwdg_timeout = false;
    if (!hal_reboot_get_ulp_aon_no_poweroff_flag()) {
        hal_reboot_set_ulp_aon_no_poweroff_flag();
        memset_s((void *)&g_preserve_data_lib, sizeof(g_preserve_data_lib), 0, sizeof(g_preserve_data_lib));
    }
    if (g_preserve_data_lib.magic == 0) {    // Hard power-off restart
        g_preserve_data_lib.reboot_status = REBOOT_POWER_ON_TRIGGER_STATUS;
        g_preserve_data_lib.cpu_utils_reset_cause = REBOOT_CAUSE_POWER_ON;
        g_preserve_data_lib.update_reset_cause_on_boot = true;
        watchdog_porting_pmu_reboot();
    }
    is_hardwdg_timeout = hal_reboot_hard_wdg_timeout((uint16_t)g_preserve_data_lib.cpu_utils_reset_cause);
    if (is_hardwdg_timeout) {           // hard watchdog timeout
        set_update_reset_cause_on_boot(false);
        g_preserve_data_lib.reboot_status = REBOOT_HARD_WDGTIMEOUT_TRIGER_STATUS;
        g_preserve_data_lib.cpu_utils_reset_cause = REBOOT_CAUSE_APPLICATION_STD_CHIP_WDT_FRST;
    } else {                           // normal soft restart
        if (g_preserve_data_lib.magic == STANDARD_REBOOT_MAGIC) {
            g_preserve_data_lib.reboot_status = REBOOT_SOFT_RESET_TRIGER_STATUS;
        } else {
            duplicate_app_preserve_mem();
            system_set_reboot_status_by_crash_cause();
        }
    }

    if (get_update_reset_cause_on_boot()) {  // the system is restarted by writing the reset register.
        set_cpu_utils_reset_cause(REBOOT_CAUSE_UNKNOWN);
    }

    if (g_preserve_data_lib.reboot_status != REBOOT_POWER_ON_TRIGGER_STATUS) {
        g_preserve_data_lib.reboot_count++;
        if (g_preserve_data_lib.reboot_status == REBOOT_SOFT_RESET_TRIGER_STATUS) {
            g_preserve_data_lib.ep_reboot_count = 0;
            g_preserve_data_lib.enable_sha_check = 0;
        } else {
            g_preserve_data_lib.ep_reboot_count++;
        }
    } else {
        g_preserve_data_lib.reboot_status = REBOOT_POWER_ON_TRIGGER_STATUS;
        g_preserve_data_lib.cpu_utils_reset_cause = REBOOT_CAUSE_POWER_ON;
    }
    g_preserve_data_lib.magic = STANDARD_REBOOT_MAGIC;
}

unsigned int get_system_boot_status(void)
{
    return g_preserve_data_lib.reboot_status;
}

uint32_t get_system_magic(void)
{
    return g_preserve_data_lib.magic;
}

void set_cpu_utils_system_boot_magic(void)
{
    unsigned int magic = STANDARD_REBOOT_MAGIC;

    switch (get_cpu_utils_reset_cause()) {
        case REBOOT_CAUSE_BT_SYSRESETREQ:
        case REBOOT_CAUSE_PROTOCOL_SYSRESETREQ:
        case REBOOT_CAUSE_APPLICATION_SYSRESETREQ:
        case REBOOT_CAUSE_UNKNOWN:
        case REBOOT_CAUSE_POWER_ON:
        case REBOOT_CAUSE_UPG_COMPLETION:
            magic = STANDARD_REBOOT_MAGIC;
            break;

        default:
            /* unidentified normal reboot cause */
            magic = ABNORMAL_REBOOT_MAGIC;
            break;
    }

    set_chip_system_boot_magic(magic);
}

void set_cpu_utils_reset_cause(reboot_cause_t reset_cause)
{
    set_update_reset_cause_on_boot(false);
    g_preserve_data_lib.cpu_utils_reset_cause = reset_cause;
}

unsigned int get_cpu_utils_reset_count(void)
{
    return g_preserve_data_lib.reboot_count;
}

unsigned int get_cpu_utils_epreset_count(void)
{
    return g_preserve_data_lib.ep_reboot_count;
}

reboot_cause_t get_cpu_utils_reset_cause(void)
{
    return g_preserve_data_lib.cpu_utils_reset_cause;
}

void set_update_reset_cause_on_boot(bool reset_cause_on_boot)
{
    g_preserve_data_lib.update_reset_cause_on_boot = reset_cause_on_boot;
}

bool get_update_reset_cause_on_boot(void)
{
    return g_preserve_data_lib.update_reset_cause_on_boot;
}

uint32_t get_cpu_utils_check_sha_value(void)
{
    return g_preserve_data_lib.enable_sha_check;
}

void set_cpu_utils_check_sha_fault_value(void)
{
    g_preserve_data_lib.enable_sha_check = 0;
}

uint8_t get_last_panic_id(void)
{
    return g_preserve_data_lib.last_panic.origin;
}

uint32_t get_last_panic_code(void)
{
    return g_preserve_data_lib.last_panic.code;
}

uint32_t get_last_panic_caller(void)
{
    return g_preserve_data_lib.last_panic.caller;
}

uint32_t get_cpu_utils_exc_pc(void)
{
#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
    return g_preserve_data_lib.exc_context.task_context.mepc;
#else
    return g_preserve_data_lib.exc_context.pc;
#endif
}

void set_cpu_utils_exc_pc(uint32_t pc)
{
#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
    g_preserve_data_lib.exc_context.task_context.mepc = pc;
#else
    g_preserve_data_lib.exc_context.pc = pc;
#endif
}

uint32_t get_cpu_utils_exc_lr(void)
{
#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
    return g_preserve_data_lib.exc_context.task_context.ra;
#else
    return g_preserve_data_lib.exc_context.lr;
#endif
}

void set_cpu_utils_exc_lr(uint32_t lr)
{
#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
    g_preserve_data_lib.exc_context.task_context.ra = lr;
#else
    g_preserve_data_lib.exc_context.lr = lr;
#endif
}

#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))

uint32_t get_cpu_utils_exc_ccause(void)
{
    return g_preserve_data_lib.exc_context.ccause;
}

uint32_t get_cpu_utils_exc_mcause(void)
{
    return g_preserve_data_lib.exc_context.mcause;
}

uint32_t get_cpu_utils_exc_mtval(void)
{
    return g_preserve_data_lib.exc_context.mtval;
}

#endif /* ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70)) */

void set_entry_sleep_time(uint64_t time)
{
    g_preserve_data_lib.entry_sleep_time[0] = (uint32_t)(time & WORD_MASK_VALUE);
    g_preserve_data_lib.entry_sleep_time[1] = (uint32_t)((time >> RIGHT_SHIFT_32BITS) & WORD_MASK_VALUE);
}

void set_rtc_delay_count(uint64_t time)
{
    if ((get_system_boot_status() == REBOOT_POWER_ON_TRIGGER_STATUS) ||
        (get_system_boot_status() == REBOOT_SOFT_RESET_TRIGER_STATUS)) {
        g_preserve_data_lib.rtc_delay_count[0] = (uint32_t)(time & WORD_MASK_VALUE);
        g_preserve_data_lib.rtc_delay_count[1] = (uint32_t)((time >> RIGHT_SHIFT_32BITS) & WORD_MASK_VALUE);
    }
}

void set_excepted_sleep_time(uint32_t time)
{
    g_preserve_data_lib.excepted_delay_time = time;
}

void set_exception_time_stamp(void)
{
#if defined(BUILD_APPLICATION_STANDARD)
    g_preserve_data_lib.exception_time_s = (uint32_t)((uapi_systick_get_ms() + get_log_basetime_ms()) / TIME_S_TO_MS);
#endif
}

void set_system_boot_status(unsigned int reboot_status)
{
    g_preserve_data_lib.reboot_status = reboot_status;
}

panic_desc_t *get_last_panic_handle(void)
{
    return &g_preserve_data_lib.last_panic;
}

void set_last_panic(panic_desc_t *last_panic)
{
    memcpy_s((void *)&g_preserve_data_lib.last_panic, sizeof(panic_desc_t), (void *)last_panic, sizeof(panic_desc_t));
}

#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
void set_exception_info_riscv(exc_context_t *exc_data)
{
    g_preserve_data_lib.exc_context.ccause = exc_data->ccause;
    g_preserve_data_lib.exc_context.mcause = exc_data->mcause;
    g_preserve_data_lib.exc_context.mtval = exc_data->mtval;
    g_preserve_data_lib.exc_context.gp = exc_data->gp;
    g_preserve_data_lib.exc_context.task_context.mstatus = exc_data->task_context.mstatus;
    g_preserve_data_lib.exc_context.task_context.mepc = exc_data->task_context.mepc;
    g_preserve_data_lib.exc_context.task_context.tp = exc_data->task_context.tp;    /* X4 */
    g_preserve_data_lib.exc_context.task_context.sp = exc_data->task_context.sp;    /* X2 */

    /* 拷贝s11 ~ ra */
    uint32_t size = (uintptr_t)(&(g_preserve_data_lib.exc_context.task_context.ra)) -
        (uintptr_t)(&(g_preserve_data_lib.exc_context.task_context.s11)) + sizeof(uint32_t);

    (void)memcpy_s((void *)(uintptr_t)(&(g_preserve_data_lib.exc_context.task_context.s11)), size,
        (void *)(uintptr_t)(&(exc_data->task_context.s11)), size);
}

preserve_task_context_t *get_exc_task_context(void)
{
    return &g_preserve_data_lib.exc_context.task_context;
}
#endif /* ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70)) */

#if ((ARCH == CM3) || (ARCH == CM7))

void set_fault_address(uint32_t address)
{
    g_preserve_data_lib.exc_context.fault_address = address;
}

void set_fault_reason(uint32_t reason)
{
    g_preserve_data_lib.exc_context.fault_reason = reason;
}

void set_exception_stack_frame(exception_stack_frame_t *exception_stack_frame_src)
{
#if ((ARCH == CM3) || (ARCH == CM7))
    g_preserve_data_lib.exc_context.r0 = exception_stack_frame_src->stacked_r0;
    g_preserve_data_lib.exc_context.r1 = exception_stack_frame_src->stacked_r1;
    g_preserve_data_lib.exc_context.r2 = exception_stack_frame_src->stacked_r2;
    g_preserve_data_lib.exc_context.r3 = exception_stack_frame_src->stacked_r3;
    g_preserve_data_lib.exc_context.r12 = exception_stack_frame_src->stacked_r12;
    g_preserve_data_lib.exc_context.lr = exception_stack_frame_src->stacked_lr;
    g_preserve_data_lib.exc_context.pc = exception_stack_frame_src->stacked_pc;
    g_preserve_data_lib.exc_context.xpsr = exception_stack_frame_src->stacked_psr;
#endif
}

#endif /* ((ARCH == CM3) || (ARCH == CM7)) */

preserve_exception_info_t *get_exception_info(void)
{
    return &g_preserve_data_lib.exception_info;
}

void set_exception_info(exception_info_t *exception_info)
{
    if (exception_info == NULL) {
        return;
    }

#if ((ARCH == CM3) || (ARCH == CM7))
    uint32_t i = 0;
    g_preserve_data_lib.exc_context.r4 = exception_info->regs[i++];
    g_preserve_data_lib.exc_context.r5 = exception_info->regs[i++];
    g_preserve_data_lib.exc_context.r6 = exception_info->regs[i++];
    g_preserve_data_lib.exc_context.r7 = exception_info->regs[i++];
    g_preserve_data_lib.exc_context.r8 = exception_info->regs[i++];
    g_preserve_data_lib.exc_context.r9 = exception_info->regs[i++];
    g_preserve_data_lib.exc_context.r10 = exception_info->regs[i++];
    g_preserve_data_lib.exc_context.r11 = exception_info->regs[i++];
#endif

    g_preserve_data_lib.exception_info.sp = exception_info->sp;
    g_preserve_data_lib.exception_info.sp_bottom = exception_info->sp_bottom;
    g_preserve_data_lib.exception_info.exp_task_id = exception_info->exp_task_id;
    g_preserve_data_lib.exception_info.task_array = exception_info->task_array;
    g_preserve_data_lib.exception_info.task_max_num = exception_info->task_max_num;
}

void set_fault_intid(uint32_t intid)
{
    g_preserve_data_lib.intid = intid;
}

#if !defined(BUILD_APPLICATION_ROM)
uint8_t get_last_slave_panic_id(cores_t core)
{
    UNUSED(core);
    return g_preserve_data_lib_ptr->last_panic.origin;
}

uint32_t get_last_slave_panic_code(cores_t core)
{
    UNUSED(core);
    return g_preserve_data_lib_ptr->last_panic.code;
}

uint32_t get_last_slave_panic_caller(cores_t core)
{
    UNUSED(core);
    return g_preserve_data_lib_ptr->last_panic.caller;
}
#endif

#if defined(BUILD_APPLICATION_STANDARD)
void system_boot_reason_print(void)
{
    switch (get_system_boot_status()) {
        case REBOOT_SOFT_RESET_TRIGER_STATUS:
            oml_pf_log_print0(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_ERROR, \
                              "System Boot Normal");
            break;
        case REBOOT_NMI_WDGTIMEOUT_TRIGGER_STATUS:
            oml_pf_log_print0(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_ERROR, \
                              "System Boot Abnoraml, watchdog timeout");
            break;
        case REBOOT_SOFT_PANIC_TRIGGER_STATUS:
            /* reboot from cpu fault & wdt int etc. */
            oml_pf_log_print0(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_ERROR, \
                              "System Boot Panic");
            break;
        case REBOOT_NMI_XIP_CTRL_TRIGER_STATUS:
        case REBOOT_NMI_XIP_CACHE_TRIGGER_STATUS:
            oml_pf_log_print0(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_ERROR, \
                              "System Boot Abnoraml, xip crash");
            break;
        case REBOOT_HARD_FAULT_TRIGER_STATUS:
            oml_pf_log_print0(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_ERROR, \
                              "System Boot Abnoraml, hard fault crash");
            break;
        case REBOOT_POWER_ON_TRIGGER_STATUS:
            /* first power on, vsys power reset or power_on reset */
            oml_pf_log_print0(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_ERROR, "System Power On");
            break;
        case REBOOT_OTHER_CORE_ABNORMAL_TRIGER_STATUS:
            oml_pf_log_print0(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_ERROR, \
                              "System Boot Abnoraml, other core crash");
            break;
        default:
            oml_pf_log_print1(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_ERROR, \
                              "Unkown Sytem Boot Type:0x%x", get_system_boot_status());
            break;
    }
}

void system_boot_reason_process(void)
{
    unsigned int reset_cause = (unsigned int)get_cpu_utils_reset_cause();
    unsigned int reset_count = get_cpu_utils_reset_count();
    unsigned int ep_reboot_count = get_cpu_utils_epreset_count();
    oml_pf_log_print3(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_ERROR,\
                      "System Reboot cause:0x%x , total reboot count:%u, exception reboot count:%u",\
                      reset_cause, reset_count, ep_reboot_count);
    if ((reset_cause == REBOOT_CAUSE_APPLICATION_HARDFAULT) || (reset_cause == REBOOT_CAUSE_BT_HARDFAULT)) {
        oml_pf_log_print2(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_ERROR,\
                          "Exception reboot pc:0x%x , lr:%x",\
                          get_cpu_utils_exc_pc(), get_cpu_utils_exc_lr());
    }
    switch (reset_cause) {
        case REBOOT_CAUSE_BT_PANIC:
            oml_pf_log_print3(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_ERROR,"panic id:0x%x, \
                              panic code: 0x%x, panic caller: 0x%x" , get_last_slave_panic_id(CORES_BT_CORE), \
                              get_last_slave_panic_code(CORES_BT_CORE), get_last_slave_panic_caller(CORES_BT_CORE));
            break;
        case REBOOT_CAUSE_APPLICATION_PANIC:
            oml_pf_log_print3(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_ERROR,"panic id:0x%x, \
                              panic code: 0x%x, panic caller: 0x%x" , get_last_panic_id(), \
                              get_last_panic_code(), get_last_panic_caller());
            break;
        case REBOOT_CAUSE_APPLICATION_STD_CHIP_WDT_FRST:
#ifdef SUPPORT_CPU_TRACE_PRINT
            riscv_cpu_trace_print();
#endif
            panic(PANIC_CHIP_WDT_FRST, 0);
            break;
        case REBOOT_CAUSE_BT_WATCHDOG:
        case REBOOT_CAUSE_BT_HARDFAULT:
        case REBOOT_CAUSE_BT_NNMIFAULT:
            oml_pf_log_print2(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_ERROR, \
                "bt exception reboot with pc:0x%x, lr: 0x%x", get_cpu_utils_exc_pc(), get_cpu_utils_exc_lr());
            break;
        case REBOOT_CAUSE_APPLICATION_HARDFAULT:
        case REBOOT_CAUSE_APPLICATION_NNMIFAULT:
        case REBOOT_CAUSE_APPLICATION_CHIP_WDT:
        case REBOOT_CAUSE_APPLICATION_XIP_CTRL:
        case REBOOT_CAUSE_APPLICATION_XIP_CACHE:
        case REBOOT_CAUSE_APPLICATION_MDMA:
        case REBOOT_CAUSE_APPLICATION_SMDMA:
            oml_pf_log_print2(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_ERROR, \
                "app exception reboot pc:0x%x, lr: 0x%x", get_cpu_utils_exc_pc(), get_cpu_utils_exc_lr());
            break;
        default:
            break;
    }
}
#endif

void print_system_boot_status(void)
{
    unsigned int reset_cause = (unsigned int)get_cpu_utils_reset_cause();

    switch (get_system_boot_status()) {
        case REBOOT_SOFT_RESET_TRIGER_STATUS:
            PRINT("System Boot Noraml" NEWLINE);
            break;
        case REBOOT_NMI_WDGTIMEOUT_TRIGGER_STATUS:
            PRINT("System Boot watchdog timeout" NEWLINE);
            break;
        case REBOOT_NMI_XIP_CTRL_TRIGER_STATUS:
        case REBOOT_NMI_XIP_CACHE_TRIGGER_STATUS:
            PRINT("System Boot Abnoraml, XIP crash" NEWLINE);
            break;
        case REBOOT_HARD_FAULT_TRIGER_STATUS:
            PRINT("System Boot Abnoraml, hard fault crash" NEWLINE);
            break;
        case REBOOT_POWER_ON_TRIGGER_STATUS:
            /* normal system reboot, except reboot */
            PRINT("System Power On" NEWLINE);
            break;
        case REBOOT_SOFT_PANIC_TRIGGER_STATUS:
            PRINT("System Boot Abnoraml, panic crash" NEWLINE);
            break;
        case REBOOT_OTHER_CORE_ABNORMAL_TRIGER_STATUS:
            PRINT("System Boot Abnoraml, other core crash" NEWLINE);
            break;
        case REBOOT_HARD_WDGTIMEOUT_TRIGER_STATUS:
            PRINT("System Boot Abnoraml, chip watchdog timeout" NEWLINE);
            break;
        default:
            PRINT("Unkown Sytem Boot Type 0x%x" NEWLINE, get_system_boot_status());
            break;
    }
    PRINT("System Reboot cause:0x%x, total reboot count:%u, exception reboot count:%u\r\n",\
          reset_cause, get_cpu_utils_reset_count(), get_cpu_utils_epreset_count());
    if (reset_cause == REBOOT_CAUSE_BT_PANIC) {
        PRINT("panic id:0x%x, panic code: 0x%x, panic caller: 0x%x\r\n", get_last_slave_panic_id(CORES_BT_CORE), \
              get_last_slave_panic_code(CORES_BT_CORE), get_last_slave_panic_caller(CORES_BT_CORE));
    } else if  (reset_cause == REBOOT_CAUSE_APPLICATION_PANIC) {
        PRINT("panic id:0x%x, panic code: 0x%x, panic caller: 0x%x\r\n", get_last_panic_id(), \
              get_last_panic_code(), get_last_panic_caller());
    } else if ((reset_cause == REBOOT_CAUSE_APPLICATION_HARDFAULT) || (reset_cause == REBOOT_CAUSE_BT_HARDFAULT) ||
               (reset_cause == REBOOT_CAUSE_GNSS_HARDFAULT) || (reset_cause == REBOOT_CAUSE_SENSOR_HARDFAULT) ||
               (reset_cause == REBOOT_CAUSE_CCORE_HARDFAULT)) {
        PRINT("Exception reboot pc:0x%x , lr:%x.\r\n", get_cpu_utils_exc_pc(), get_cpu_utils_exc_lr());
#if defined(ARCH) && ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
        PRINT("reboot info list, ccause: 0x%x, mcause: 0x%x, mtval: 0x%x.\r\n",
              get_cpu_utils_exc_ccause(), get_cpu_utils_exc_mcause(), get_cpu_utils_exc_mtval());
#endif
    }
}

void show_reboot_info(void)
{
    system_boot_status_init();
    print_system_boot_status();
}
