/*
 *  Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _DUMP_PARSER_HI3322_A_LITEOS_H_
#define _DUMP_PARSER_HI3322_A_LITEOS_H_

#include "common.h"

#define LOSCFG_EXC_SIMPLE_INFO
#define SUPPORT_CALLSTACK
#define LOSCFG_ARCH_FPU_ENABLE
#define LOSCFG_ARCH_FPU_DOUBLE
#define CRASH_MAX_CALL_STACK 64
#define ARCH RISCV70

typedef enum panic_id {
    // No panic
    PANIC_NONE = 0,
    PANIC_ARM_CORE = 1,
    PANIC_HAL = 2,
    PANIC_DSP = 3,
    PANIC_IRMALLOC_FAIL = 4,
    PANIC_UART = 5,
    PANIC_IFLASH = 6,
    PANIC_ADC = 7,
    PANIC_WATCHDOG = 8,
    PANIC_LOG = 9,
    PANIC_DMA = 10,
    PANIC_ASSERT = 11,
    PANIC_REBOOT = 12,
    PANIC_UPDATEPKG = 13,
    PANIC_I2C = 14,
    PANIC_SPI = 15,
    PANIC_RPC = 22,
    PANIC_MEM_MONITOR = 23,
    // Someone calling abort() / exit()
    PANIC_EXIT = 24,
    // RTOS related panics
    PANIC_TASK_CREATE_FAILED = 25,
    PANIC_MEMCPY_BUFFER_OVERLAP = 26,
    PANIC_XIP = 27,
    PANIC_BTC_MALLOC_FAIL = 28,
    PANIC_LOW_POWER = 29,
    PANIC_SYSTEM_STATUS = 30,
    PANIC_BTC_INT_ERR = 31,
    PANIC_LPC_WKUP_FAIL = 32,
    PANIC_CLOCKS_GLB_ERR = 33,
    PANIC_PMU_CMU = 34,
    PANIC_PMU_LDO = 35,
    PANIC_CHIP_WDT_FRST = 36,
    PANIC_NON_OS = 37,
    PANIC_LPC_VETO = 38,
    PANIC_TIMER = 39,
    PANIC_LPC = 40,
    PANIC_RTC = 41,
    PANIC_MEMORY = 42,
    PANIC_CPU_HIFI = 43,
    PANIC_EXCEPTION_TEST = 44,
    PANIC_KV = 45,
    PANIC_EFLASH = 46,
    PANIC_LIB = 47,
    PANIC_CODELOADER = 48,
    PANIC_CRITICLA = 49,
    PANIC_IPC = 50,
    PANIC_INT = 51,
    PANIC_OS = 52,
    PANIC_LPC_WAKEUP_TIME = 53,
    PANIC_BTC_BT_FRM_DISMATCH1 = 54,
    PANIC_BTC_BT_FRM_DISMATCH2 = 55,
    PANIC_BTC_BLE_FRM_DISMATCH = 56,
    PANIC_BTC_WAKEUP_TO = 57,
    PANIC_BTH_MEMORY = 58,
    PANIC_MPU_CONFIG_ERR = 59,
    PANIC_GPU = 60,
    PANIC_SDIO_DEVICE = 61,
    PANIC_IR = 62,
    PANIC_BTC_CMD_NUM_ZERO = 63,
    PANIC_CAP = 64,
    PANIC_EPMU = 65,
    PANIC_BTS_WRITE_PROPERTY_FAIL = 66,
    PANIC_MMC_HOST = 67,
    PANIC_BTC_OSCEN_TO = 68,
    PANIC_RGB888 = 70,
    PANIC_AP_COMMU = 71,
    PANIC_CPU_CLOCKS = 72,
    PANIC_CLOCKS_SWITCH = 73,
    PANIC_PMU = 74,
    PANIC_LOG_DUMP = 75,
    PANIC_COUL = 76,
    PANIC_DSP0_POWER = 77,
    PANIC_DSP1_POWER = 78,
    PANIC_BT_DEBUG = 79,
    PANIC_PIN_NULL = 80,
    PANIC_UNITTEST = 98,
    PANIC_TESTSUIT = 99,
    PANIC_DSS_MIPI_ERR = 100,
    PANIC_GNSS_DFR_ERR = 101,
    PANIC_BT_SEC_BOOT_ERR = 102,
    PANIC_ROM_WEAK_FUNCTION = 103,
    PANIC_GLP = 104,
    PANIC_UICC = 105,
    PANIC_CRYPTO = 106,
} panic_id_t;

typedef enum {
    REBOOT_CAUSE_UNKNOWN = 0x0000,           // !< Used as a default for the reboot_reset_reason variable
    REBOOT_CAUSE_BT_RESET_UNKNOWN = 0x8000,  // !< REBOOT_CAUSE_BT_RESET_UNKNOWN
    REBOOT_CAUSE_BT_SYSRESETREQ = 0x8001,
    REBOOT_CAUSE_BT_WATCHDOG = 0x8002,
    REBOOT_CAUSE_BT_STANDARD_CHIP_WATCHDOG = 0x8004,
    REBOOT_CAUSE_BT_UPDATER_CHIP_WATCHDOG = 0x8005,
    REBOOT_CAUSE_BT_RPC_TIMEOUT = 0x8006,
    REBOOT_CAUSE_BT_HARDFAULT = 0x8007,
    REBOOT_CAUSE_BT_NNMIFAULT = 0x8008,
    REBOOT_CAUSE_BT_UNKNOWN_ISR = 0x8009,
    REBOOT_CAUSE_BT_PANIC = 0x800a,
    REBOOT_CAUSE_BT_STD_WDT_FRST = 0x800b,      /* force reboot */
    REBOOT_CAUSE_BT_STD_CHIP_WDT_FRST = 0x800c, /* force reboot */
    REBOOT_CAUSE_BT_XIP_HARDFAULT = 0x800d,
    REBOOT_CAUSE_BT_SSB_EXCEPTION_REBOOT = 0x800e,
    REBOOT_CAUSE_BT_BUCK1_PROTECT_FRST = 0x8010, /* buck1 protect */
    REBOOT_CAUSE_BT_BUCK2_PROTECT_FRST = 0x8020, /* buck2 protect */

    REBOOT_CAUSE_BT_MONITOR_REBOOT_REQ = 0x8100,  // !< Start of custom reset reasons
    REBOOT_CAUSE_BT_REGIONS_UPDATED = 0x8101,
    REBOOT_CAUSE_BT_FOTA_UPGRADE = 0x8102,
    REBOOT_CAUSE_BT_END = 0x8103,

    REBOOT_CAUSE_GNSS_GLOBAL = 0x7000,
    REBOOT_CAUSE_GNSS_NMIFAULT = 0x7001,  // !< watchdog is nmi interrupt
    REBOOT_CAUSE_GNSS_HARDFAULT = 0x7002,
    REBOOT_CAUSE_GNSS_PANIC = 0x7003,
    REBOOT_CAUSE_GNSS_END = 0x7004,

    REBOOT_CAUSE_SENSOR_GLOBAL = 0x6000,
    REBOOT_CAUSE_SENSOR_NMIFAULT = 0x6001,
    REBOOT_CAUSE_SENSOR_HARDFAULT = 0x6002,
    REBOOT_CAUSE_SENSOR_PANIC = 0x6003,
    REBOOT_CAUSE_SENSOR_END = 0x6004,

    REBOOT_CAUSE_PROTOCOL_GLOBAL = 0x4000,
    REBOOT_CAUSE_PROTOCOL_SYSRESETREQ = 0x4001,
    REBOOT_CAUSE_PROTOCOL_WATCHDOG = 0x4002,
    REBOOT_CAUSE_PROTOCOL_AXI_LOCK = 0x4003,
    REBOOT_CAUSE_PROTOCOL_AXI_LOCK_TO_BT = 0x4004,
    REBOOT_CAUSE_PROTOCOL_WDT_FRST = 0x4005, /* force reboot */
    REBOOT_CAUSE_PROTOCOL1_WATCHDOG = 0x4006,
    REBOOT_CAUSE_PROTOCOL_ADDR_MON = 0x4007,
    REBOOT_CAUSE_PROTOCOL1_ADDR_MON = 0x4008,
    REBOOT_CAUSE_PROTOCOL_RPC_TIMEOUT = 0x4100,  // !< Start of custom reset reasons
    REBOOT_CAUSE_PROTOCOL_END = 0x4101,

    REBOOT_CAUSE_APPLICATION_GLOBAL = 0x2000,
    REBOOT_CAUSE_APPLICATION_SYSRESETREQ = 0x2001,
    REBOOT_CAUSE_APPLICATION_WATCHDOG = 0x2002,
    REBOOT_CAUSE_APPLICATION_PANIC = 0x2003,
    REBOOT_CAUSE_APPLICATION_HARDFAULT = 0x2004,
    REBOOT_CAUSE_APPLICATION_NNMIFAULT = 0x2005,
    REBOOT_CAUSE_APPLICATION_UNKNOWN_ISR = 0x2006,
    REBOOT_CAUSE_APPLICATION_WDT_FRST = 0x2007, /* force reboot */
    REBOOT_CAUSE_APPLICATION_CHIP_WDT = 0x2008,
    REBOOT_CAUSE_APPLICATION_XIP_CTRL = 0x2009,
    REBOOT_CAUSE_APPLICATION_XIP_CACHE = 0x200a,
    REBOOT_CAUSE_APPLICATION_MDMA = 0x200b,
    REBOOT_CAUSE_APPLICATION_SMDMA = 0x200c,
    REBOOT_CAUSE_APPLICATION_STD_CHIP_WDT_FRST = 0x200d,
    REBOOT_CAUSE_APPLICATION_STD_BT_WDT_FRST = 0x200e,
    REBOOT_CAUSE_APPLICATION_STD_ULP_WDT_FRST = 0x200f,
    REBOOT_CAUSE_APPLICATION_STD_ULP_PIN_FRST = 0x2010,

    REBOOT_CAUSE_UPG_COMPLETION = 0x2040,  // !< Start of custom reset reasons
    REBOOT_CAUSE_MON_MEM_ALMOST_EMPTY = 0x2041,
    REBOOT_CAUSE_DFU_UPG_FAIL = 0x2042,
    REBOOT_CAUSE_START_HID_UPG = 0x2043,
    REBOOT_CAUSE_HID_UPG_FAIL = 0x2044,
    REBOOT_CAUSE_RESET_FROM_ILLEGAL_ADDR = 0x2045,

    REBOOT_CAUSE_APPLICATION_SLDM = 0x2080,

    REBOOT_CAUSE_APPLICATION_RPC_TIMEOUT = 0x2100,  // !< Start of custom reset reasons

    REBOOT_CAUSE_POWER_ON = 0xF0F0,
} reboot_cause_t;

typedef struct {
    uint32_t mstatus;
    uint32_t mepc;
    uint32_t tp; /* X4 */
    uint32_t sp; /* X2 */

    uint32_t s11; /* X27 */
    uint32_t s10; /* X26 */
    uint32_t s9;  /* X25 */
    uint32_t s8;  /* X24 */
    uint32_t s7;  /* X23 */
    uint32_t s6;  /* X22 */
    uint32_t s5;  /* X21 */
    uint32_t s4;  /* X20 */
    uint32_t s3;  /* X19 */
    uint32_t s2;  /* X18 */

    uint32_t s1; /* X9 */
    uint32_t s0; /* X8 */

    uint32_t t6; /* X31 */
    uint32_t t5; /* X30 */
    uint32_t t4; /* X29 */
    uint32_t t3; /* X28 */

    uint32_t a7; /* X17 */
    uint32_t a6; /* X16 */
    uint32_t a5; /* X15 */
    uint32_t a4; /* X14 */
    uint32_t a3; /* X13 */
    uint32_t a2; /* X12 */
    uint32_t a1; /* X11 */
    uint32_t a0; /* X10 */

    uint32_t t2; /* X7 */
    uint32_t t1; /* X6 */
    uint32_t t0; /* X5 */

    uint32_t ra; /* X1 */
} preserve_task_context_t;

typedef struct {
    uint32_t ccause;
    uint32_t mcause;
    uint32_t mtval;
    uint32_t gp;
    preserve_task_context_t task_context;
} preserve_exc_context_t;

typedef struct excep_info_t {
    unsigned int sp;
    unsigned int sp_bottom;
    unsigned int exp_task_id;
    unsigned int task_array;
    unsigned int task_max_num;
} exception_info_t;

typedef struct {
    uint32_t sp;
    uint32_t sp_bottom;
    uint32_t exp_task_id;
    uint32_t task_array;
    uint32_t task_max_num;
} preserve_exception_info_t;

typedef struct {
    panic_id_t origin;
    uint32_t code;
    uint32_t timestamp_ms;
    uint32_t caller;
} panic_desc_t;

typedef struct {
    preserve_exc_context_t exc_context;       /* 异常上下文. 144 bytes */
    preserve_exception_info_t exception_info; /* 异常时任务信息：sp、任务ID，最大任务数. 20 bytes */
    panic_desc_t last_panic;                  /* panic信息：ID、code、时间戳、caller等. 16 bytes */
    uint32_t entry_sleep_time[2];             /* 数组长度为2, 分别保存高32位和低32位. 8 bytes */
    uint32_t rtc_delay_count[2]; /* 保存最后一次的rtc_delay值. 数组长度为2, 分别保存高32位和低32位. 8 bytes */
    uint32_t excepted_delay_time;         /* 保存唤醒时间. 4 bytes */
    uint32_t exception_time_s;            /* 保存发生异常时时间戳. 4 bytes */
    uint32_t reserved[3];                 /* The size of the reserved array n is 3, total 12 bytes. */
    reboot_cause_t cpu_utils_reset_cause; /* 复位原因. 4 bytes */
    bool update_reset_cause_on_boot;      /* 启动时是否更新复位原因. 4 bytes */
    uint32_t core_id;                     // 4
    uint32_t reserved1;                   // 4
    uint32_t intid;                       /* 保存异常时的中断ID. 4 bytes */
    uint32_t magic;                       // exception flag
    uint32_t reboot_status;               // 4
    uint32_t reboot_count;                /* 冷启动以来的复位次数. 4 bytes */
    uint32_t ep_reboot_count;             /* 连续异常复位次数. 4 bytes */
    uint32_t enable_sha_check;            /* 是否打开HASH校验. 4 bytes */
} preserve_data_t;

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
#ifndef LOSCFG_EXC_SIMPLE_INFO
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
#else
    uint32_t x4;
    uint32_t x5;
    uint32_t x6;
    uint32_t x7;
    uint32_t x8;
    uint32_t x9;
    uint32_t x10;
    uint32_t x11;
    uint32_t x12;
    uint32_t x13;
    uint32_t x14;
    uint32_t x15;
    uint32_t x16;
    uint32_t x17;
    uint32_t x18;
    uint32_t x19;
    uint32_t x20;
    uint32_t x21;
    uint32_t x22;
    uint32_t x23;
    uint32_t x24;
    uint32_t x25;
    uint32_t x26;
    uint32_t x27;
    uint32_t x28;
    uint32_t x29;
    uint32_t x30;
    uint32_t x31;
#endif
#ifdef SUPPORT_CALLSTACK
    uint32_t stack_size;
    exc_stack_info_t stack[0];
#else
    uint32_t backtrace_size;
    exc_backtrace_info_t backtrace[0];
#endif
} exc_info_save_t;

typedef struct {
    uint8_t core;
    uint8_t save_after_reboot;
    uint8_t padding[2];
    uint32_t time_s;
    exception_info_t exception_info;
    panic_desc_t panic;
    exc_info_save_t exc_info_save;
    uint8_t hi3322_stack1[512];
} crash_data_t;

typedef struct {
    preserve_data_t common_preserve_data;
    preserve_data_t app_preserve_data;
    crash_data_t crash_data_core;
    uint8_t hi3322_unused2[292];
} crash_data_save_t;

#endif