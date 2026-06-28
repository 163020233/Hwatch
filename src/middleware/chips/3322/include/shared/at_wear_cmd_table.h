/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: At wear cmd table \n
 *
 */

#if !defined(AT_WEAR_CMD_TALBE_H)
#define AT_WEAR_CMD_TALBE_H

#include "at.h"
#include "at_wear_fs.h"
#if defined(SUPPORT_ALIPAY_SEC) || defined(SUPPORT_ALIPAY_HSC32I1)
#include "alipay_debug.h"
#endif

// uart log level at cmd args
typedef struct {
    uint32_t para_map;
    int32_t para1; /* Range: 0~7 */
} loglevel_args_t;

typedef struct {
    uint32_t para_map;
    int32_t para1; /* Range: 0~2 */
} sleepmode_args_t;

typedef struct {
    uint32_t para_map;
    int32_t para1; /* Range: 0..128 */
} pm_args_t;

typedef struct {
    uint32_t para_map;
    int32_t para1; /* Range: 0..128 */
} pm_debug_args_t;

typedef struct {
    uint32_t             para_map;
    int32_t              core;
    int32_t              type;
} wear_crash_args_t;

typedef struct {
    uint32_t para_map;
    uint32_t baud_rate; /* Range: 0..10000000 */
} log_uart_args_t;

typedef struct {
    uint32_t para_map;
    uint32_t baud_rate; /* Range: 0..10000000 */
    uint32_t burn_radio_config; /* 是否烧写RadioConfig文件, 0 不烧写 */
} burn_2131_args_t;

typedef struct {
    uint32_t             para_map;
    uint32_t             type;
} peripherals_type_t;

typedef struct {
    uint32_t             para_map;
    uint32_t             type;
    uint32_t             pin;
    uint32_t             val;
} ioconfig_type_t;

typedef struct {
    uint32_t             para_map;
    int32_t              core_type;
    int32_t              sw;
} core_power_args_t;

typedef struct {
    uint32_t             para_map;
    const uint8_t        *module;
    uint32_t             frequency;
} module_frq_args_t;

typedef struct {
    uint32_t             para_map;
    uint16_t             ldotype;
    uint16_t             voltage;
} voltage_cfg_args_t;

typedef struct {
    uint32_t para_map;
    int32_t para1;
} gatt_server_uuid_args_t;

#if defined(CONFIG_PM_VETO_TRACK_ENABLE) && (CONFIG_PM_VETO_TRACK_ENABLE == 1)
typedef struct {
    uint32_t             para_map;
    uint16_t             output_loc;
} veto_track_loc_args_t;
#endif

typedef struct {
    uint32_t             para_map;
    uint32_t             time;
    uint8_t              week_days;
    uint8_t              hour;
    uint8_t              min;
} pmu_alarm_args_t;

typedef struct {
    uint32_t             para_map;
    uint8_t              handle;
} pmu_alarm_handle_args_t;

typedef struct {
    uint32_t              para_map;
    uint32_t              start_byte;
    uint32_t              bit_offset;
} efuse_bit_args_t;

#ifdef SUPPORT_XOTRIM_ON_EFUSE
typedef struct {
    uint32_t              para_map;
    uint32_t              value;
} xotrim_args_t;
#endif

typedef struct {
    uint32_t             para_map;
    const uint8_t        *module;
} module_mem_test_args_t;

typedef struct {
    uint32_t             para_map;
    uint32_t             year;
    uint32_t             month;
    uint32_t             day;
    uint32_t             hour;
    uint32_t             min;
    uint32_t             sec;
} time_args_t;

typedef struct {
    uint32_t             para_map;
    uint32_t             task_id;
} task_args_t;

typedef struct {
    uint32_t             para_map;
    int32_t              cmd_type;
    uint32_t             task_id;
} mem_leak_t;

typedef struct {
    uint32_t             para_map;
    uint16_t             module_id;
    uint8_t              is_dest_module;
    uint8_t              switch_code;
} msg_filter_args_t;

const uint8_t * const frq_module_list[] = {
    (uint8_t *)"MCU",
    (uint8_t *)"NPU",
    (uint8_t *)"SDIO",
    (uint8_t *)"NORFLASH",
    (uint8_t *)"PSRAM",
    (uint8_t *)"EMMC",
    (uint8_t *)"HIFI",
    (uint8_t *)"DPU",
};

typedef struct {
    uint32_t             para_map;
    const uint8_t        *module;
} pm_module_args_t;

typedef struct {
    uint32_t             para_map;
    uint32_t             pin;
    uint32_t             mode;
    uint32_t             pull;
    uint32_t             ie;
    uint32_t             sleep_ie;
    uint32_t             ds;
} pin_set_args_t;

typedef struct {
    uint32_t             para_map;
    uint32_t             pin;
} pin_get_args_t;

typedef struct {
    uint32_t             para_map;
    uint32_t             block;
    uint32_t             page;
    uint32_t             pagenum;
} fmcnand_args_t;

typedef struct {
    uint32_t             para_map;
    uint8_t              core_type;
} dhry_core_args_t;

typedef struct {
    uint32_t             para_map;
    uint32_t             mode;
    uint32_t             time;
} pwrkey_args_t;

const at_para_parse_syntax_t loglevel_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 7,
        .offset = offsetof(loglevel_args_t, para1)
    },
};

#ifdef AT_IPC_V1_TEST
// <src_core> <dst_core> <channel> <times> <size>
typedef struct {
    uint32_t             para_map;
    int32_t              src_core;
    int32_t              dst_core;
    int32_t              cmd_type;
    int32_t              channel;
    int32_t              times;
    int32_t              size;
} wear_ipc_test_args_t;

const int32_t a2xipc_channel_list[] = {
    0, 1
};

const at_para_parse_syntax_t a2xipc_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 5,
        .offset = offsetof(wear_ipc_test_args_t, src_core)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 5,
        .offset = offsetof(wear_ipc_test_args_t, dst_core)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 3,
        .offset = offsetof(wear_ipc_test_args_t, cmd_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, a2xipc_channel_list},
        .offset = offsetof(wear_ipc_test_args_t, channel)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 10000,
        .offset = offsetof(wear_ipc_test_args_t, times)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 16000,
        .offset = offsetof(wear_ipc_test_args_t, size)
    },
};
#endif

const at_para_parse_syntax_t g_wear_fs_write_check_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 256,
        .offset = offsetof(fs_write_check_args_t, name)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 1024*1024*100,
        .offset = offsetof(fs_write_check_args_t, len)
    },
};

const at_para_parse_syntax_t sleepmode_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 2,
        .offset = offsetof(sleepmode_args_t, para1)
    },
};

const at_para_parse_syntax_t pm_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 128,
        .offset = offsetof(pm_args_t, para1)
    },
};

const at_para_parse_syntax_t pm_debug_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 128,
        .offset = offsetof(pm_debug_args_t, para1)
    },
};

const at_para_parse_syntax_t g_wear_crash_test_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = -1,
        .entry.int_range.max_val = 16,
        .offset = offsetof(wear_crash_args_t, core)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = -1,
        .entry.int_range.max_val = 128,
        .offset = offsetof(wear_crash_args_t, type)
    },
};

const at_para_parse_syntax_t g_peripherals_type_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = -1,
        .entry.int_range.max_val = 16,
        .offset = offsetof(peripherals_type_t, type)
    },
};

const at_para_parse_syntax_t g_ioconfig_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 10000000,
        .offset = offsetof(ioconfig_type_t, type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 10000000,
        .offset = offsetof(ioconfig_type_t, pin)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 10000000,
        .offset = offsetof(ioconfig_type_t, val)
    },
};

const at_para_parse_syntax_t g_log_uart_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 10000000,
        .offset = offsetof(log_uart_args_t, baud_rate)
    },
};

const at_para_parse_syntax_t g_burn_2131_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 10000000,
        .offset = offsetof(burn_2131_args_t, baud_rate)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 2,
        .offset = offsetof(burn_2131_args_t, burn_radio_config)
    },
};


const at_para_parse_syntax_t g_module_frq_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.string_list= {8, frq_module_list},
        .offset = offsetof(module_frq_args_t, module)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 16,
        .entry.int_range.max_val = 800,
        .offset = offsetof(module_frq_args_t, frequency)
    },
};

const at_para_parse_syntax_t slave_core_power_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 128,
        .offset = offsetof(core_power_args_t, core_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(core_power_args_t, sw)
    },
};

const int32_t voltage_cfg_para2_values[] = {
    19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 39,
    40, 41, 42, 43, 44, 49, 54, 55, 59, 64, 65, 69, 73, 74, 75, 77, 79, 82, 84,
    85, 89, 93, 95, 99, 103, 105, 109, 113, 119, 123, 127
};

const at_para_parse_syntax_t cfg_voltage_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 45,
        .offset = offsetof(voltage_cfg_args_t, ldotype)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {50, voltage_cfg_para2_values},
        .offset = offsetof(voltage_cfg_args_t, voltage)
    },
};

#if defined(CONFIG_PM_VETO_TRACK_ENABLE) && (CONFIG_PM_VETO_TRACK_ENABLE == 1)
const at_para_parse_syntax_t veto_track_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 64,
        .offset = offsetof(veto_track_loc_args_t, output_loc)
    },
};
#endif

const at_para_parse_syntax_t pmu_alarm_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xFFFFFFF,
        .offset = offsetof(pmu_alarm_args_t, time)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0b01111111, // 0-7bit代表一周7天
        .offset = offsetof(pmu_alarm_args_t, week_days)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 23,
        .offset = offsetof(pmu_alarm_args_t, hour)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 59,
        .offset = offsetof(pmu_alarm_args_t, min)
    },
};

const at_para_parse_syntax_t pmu_alarm_handle_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 9,
        .offset = offsetof(pmu_alarm_handle_args_t, handle)
    },
};

const at_para_parse_syntax_t efuse_bit_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 283, // max 2272 bit
        .offset = offsetof(efuse_bit_args_t, start_byte)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 7,
        .offset = offsetof(efuse_bit_args_t, bit_offset)
    },
};

const at_para_parse_syntax_t g_module_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.string_list= {8, frq_module_list},
        .offset = offsetof(module_mem_test_args_t, module)
    },
};

const at_para_parse_syntax_t g_time_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1970,
        .entry.int_range.max_val = 3000,
        .offset = offsetof(time_args_t, year)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 12,
        .offset = offsetof(time_args_t, month)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 31,
        .offset = offsetof(time_args_t, day)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 23,
        .offset = offsetof(time_args_t, hour)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 59,
        .offset = offsetof(time_args_t, min)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 60,
        .offset = offsetof(time_args_t, sec)
    },
};

const at_para_parse_syntax_t g_task_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xFF,
        .offset = offsetof(task_args_t, task_id)
    },
};

const at_para_parse_syntax_t g_mem_leak_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 2,
        .offset = offsetof(mem_leak_t, cmd_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xFF,
        .offset = offsetof(mem_leak_t, task_id)
    },
};

const at_para_parse_syntax_t g_pm_mode_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(pm_module_args_t, module)
    },
};

const at_para_parse_syntax_t g_pin_set_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = PIN_NONE - 1,
        .offset = offsetof(pin_set_args_t, pin)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = PIN_MODE_MAX - 1,
        .offset = offsetof(pin_set_args_t, mode)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = PIN_PULL_MAX - 1,
        .offset = offsetof(pin_set_args_t, pull)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = PIN_IE_MAX - 1,
        .offset = offsetof(pin_set_args_t, ie)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = PIN_IE_MAX - 1,
        .offset = offsetof(pin_set_args_t, sleep_ie)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = PIN_DS_MAX - 1,
        .offset = offsetof(pin_set_args_t, ds)
    },
};

const at_para_parse_syntax_t g_pin_get_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = PIN_NONE - 1,
        .offset = offsetof(pin_get_args_t, pin)
    },
};

const at_para_parse_syntax_t g_set_msg_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xFFFF - 1,
        .offset = offsetof(msg_filter_args_t, module_id)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xFFFF - 1,
        .offset = offsetof(msg_filter_args_t, switch_code)
    },
};

const at_para_parse_syntax_t g_set_log_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 7,
        .offset = offsetof(loglevel_args_t, para1)
    },
};

const at_para_parse_syntax_t fmcnand_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xFFFF,
        .offset = offsetof(fmcnand_args_t, block)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xFFFF,
        .offset = offsetof(fmcnand_args_t, page)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xFFFF,
        .offset = offsetof(fmcnand_args_t, pagenum)
    }
};

const at_para_parse_syntax_t dhry_coretype_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 3,
        .offset = offsetof(dhry_core_args_t, core_type)
    },
};

const at_para_parse_syntax_t pwrkey_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xF,
        .offset = offsetof(pwrkey_args_t, mode)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xF,
        .offset = offsetof(pwrkey_args_t, time)
    }
};

#ifdef SUPPORT_XOTRIM_ON_EFUSE
const at_para_parse_syntax_t g_xotrim_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xFFFF,
        .offset = offsetof(xotrim_args_t, value)
    },
};
#endif

const at_para_parse_syntax_t g_gatt_cb_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xFFFF,
        .offset = offsetof(gatt_server_uuid_args_t, para1)
    },
};

#ifdef CFG_DRIVERS_MMC
typedef struct {
    uint32_t para_map;
    uint32_t oper_type; // <0 读> <1 写> <2 擦>
    uint32_t lba;       // 扇区逻辑地址
    uint32_t count;       // 操作的扇区个数
    const char *write_data;
} emmc_sector_op_args_t;

// set emmc freq 0(32M) 1(100M)
typedef struct {
    uint32_t para_map;
    int32_t type; /* Range: 0~7 */
} emmc_freq_level;

// 配置emmc器件的驱动强度
typedef struct {
    uint32_t para_map;
    int32_t ds; /* Range: 0~7 */
} emmc_driver_strength;

const at_para_parse_syntax_t g_emmc_sector_oper[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 2,
        .offset = offsetof(emmc_sector_op_args_t, oper_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 2048,
        .offset = offsetof(emmc_sector_op_args_t, lba)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 10,
        .offset = offsetof(emmc_sector_op_args_t, count)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 0x400,
        .offset = offsetof(emmc_sector_op_args_t, write_data)
    },
};
const at_para_parse_syntax_t emmc_freq_level_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(emmc_freq_level, type)
    },
};
const at_para_parse_syntax_t emmc_driver_strength_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xff,
        .offset = offsetof(emmc_driver_strength, ds)
    },
};

at_ret_t at_emmc_sector_test(const emmc_sector_op_args_t *argc);
at_ret_t at_switch_emmc_freq_level(const emmc_freq_level *argc);
at_ret_t at_set_emmc_driver_strength(const emmc_driver_strength *argc);
#endif

#ifdef SUPPORT_SPORT_HEALTH_TEST
typedef struct {
    uint32_t para_map;
    int32_t number;
} sports_health_test_file_number;

typedef struct {
    uint32_t para_map;
    int32_t type;
} sports_health_gesture_type;

const at_para_parse_syntax_t sports_health_test_file_number_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 100,
        .offset = offsetof(sports_health_test_file_number, number)
    },
};
const at_para_parse_syntax_t sports_health_test_gesture_type_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 4,
        .offset = offsetof(sports_health_gesture_type, type)
    },
};
#endif

typedef struct {
    uint32_t                para_map;
    uint16_t                para1;
    uint16_t                para2;
} plt_xo_cali_args_t;

static const at_para_parse_syntax_t g_plt_xo_cali_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = false,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range = { 0, 1 },
        .offset = offsetof(plt_xo_cali_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range = { 0, 255 },  /* max 255 */
        .offset = offsetof(plt_xo_cali_args_t, para2)
    },
};

typedef struct {
    uint32_t para_map;
    int32_t para1; /* Range: 0..6 */
} trans_channel_args_t;

const at_para_parse_syntax_t g_set_trans_channel_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 6,
        .offset = offsetof(trans_channel_args_t, para1)
    },
};

typedef struct {
    uint32_t para_map;
    uint32_t channel;
    uint32_t sample_num;
    uint32_t sample_discard;
} adc_manual_args_t;

const at_para_parse_syntax_t g_adc_manual_args_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 7,
        .offset = offsetof(adc_manual_args_t, channel)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 100000,
        .offset = offsetof(adc_manual_args_t, sample_num)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 10000,
        .offset = offsetof(adc_manual_args_t, sample_discard)
    },
};

typedef struct {
    uint32_t para_map;
    uint32_t channel;
    uint32_t freq;
    uint32_t print_buffer;     // 是否打印buffer中的所有数据
    uint32_t continue_sample;  // 是否持续采样，直至停止
} adc_start_auto_args_t;

const at_para_parse_syntax_t g_adc_start_auto_args_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 7,
        .offset = offsetof(adc_start_auto_args_t, channel)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 7,
        .offset = offsetof(adc_start_auto_args_t, freq)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(adc_start_auto_args_t, print_buffer)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(adc_start_auto_args_t, continue_sample)
    },
};

typedef struct {
    uint32_t para_map;
    uint32_t channel;
} adc_stop_auto_args_t;

const at_para_parse_syntax_t g_adc_stop_auto_args_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 7,
        .offset = offsetof(adc_stop_auto_args_t, channel)
    },
};

typedef struct {
    uint32_t para_map;
    uint32_t type;
    uint32_t value;
} pms_set_args_t;

static const at_para_parse_syntax_t g_pms_set_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = false,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range = { 0, 0x5 },
        .offset = offsetof(pms_set_args_t, type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range = { 0, 0xFFFFFFF },
        .offset = offsetof(pms_set_args_t, value)
    },
};

typedef struct {
    uint32_t para_map;
    uint32_t type;
    uint32_t value;
} pm_mem_set_args_t;

static const at_para_parse_syntax_t g_pm_mem_set_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = false,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range = { 0, 0xf },
        .offset = offsetof(pm_mem_set_args_t, type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range = { 0, 0xf },
        .offset = offsetof(pm_mem_set_args_t, value)
    },
};
#ifdef LOSCFG_SHELL_ADAPT_AT
typedef struct {
    uint32_t para_map;
    char *cmd_name;
    uint32_t len;
} shell_args_t;

static const at_para_parse_syntax_t g_shell_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 256,
        .offset = offsetof(shell_args_t, cmd_name),
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1024*1024,
        .offset = offsetof(shell_args_t, len)
    },
};

at_ret_t at_shell_exec(shell_args_t *args);
#endif

#ifdef SUPPORT_AUTO_OTA
typedef struct {
    uint32_t             para_map;
    uint8_t*             para;
} auto_ota_args_t;
const at_para_parse_syntax_t g_auto_ota_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 12,
        .offset = offsetof(auto_ota_args_t, para)
    },
};
at_ret_t at_mcu_auto_ota_block(void);
at_ret_t at_mcu_auto_ota_run(void);
at_ret_t at_mcu_auto_ota_del_mac(const auto_ota_args_t *argc);
at_ret_t at_mcu_auto_ota_add_mac(const auto_ota_args_t *argc);
at_ret_t at_mcu_auto_ota_show_mac(void);
#endif

typedef struct {
    uint32_t             para_map;
    uint32_t             mode;
    uint32_t             para1;
    uint32_t             para2;
} test_rc32k_args_t;

const at_para_parse_syntax_t test_rc32k_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xFFFFFFF,
        .offset = offsetof(test_rc32k_args_t, mode)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xFFFFFFF,
        .offset = offsetof(test_rc32k_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xFFFFFFF,
        .offset = offsetof(test_rc32k_args_t, para2)
    },
};

typedef struct {
    uint32_t para_map;
    uint16_t ldotype;
    uint8_t state;
} ldo_set_state_args_t;

const at_para_parse_syntax_t ldo_set_state_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 45,
        .offset = offsetof(ldo_set_state_args_t, ldotype)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xFF,
        .offset = offsetof(ldo_set_state_args_t, state)
    },
};

typedef struct {
    uint32_t para_map;
    uint32_t interval;
    uint32_t duration;
} psram_access_state_args_t;

const at_para_parse_syntax_t psram_access_state_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0x7FFFFFFF,
        .offset = offsetof(psram_access_state_args_t, interval)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0x7FFFFFFF,
        .offset = offsetof(psram_access_state_args_t, duration)
    },
};

/* AT Command */
at_ret_t at_reboot(void);

/* AT+LOGL */
at_ret_t at_get_log_level(void);
at_ret_t at_set_log_level(const loglevel_args_t *args);

/* AT+MSG */
at_ret_t at_set_level_msg(const msg_filter_args_t *argc);
at_ret_t at_set_mod_id_msg(const msg_filter_args_t *argc);

at_ret_t wear_fs_write(const fs_write_check_args_t *argc);
at_ret_t wear_fs_check(const fs_write_check_args_t *argc);
at_ret_t wear_fs_rm(const fs_write_check_args_t *argc);
at_ret_t wear_coremark_test(void);
at_ret_t mcu_test_dtcm_baudwidth(void);
at_ret_t mcu_test_l2m_baudwidth(void);
at_ret_t mcu_test_fs_baudwidth(void);
at_ret_t mcu_test_psram_baudwidth(void);
at_ret_t mcu_test_comram_baudwidth(void);
at_ret_t mcu_test_norflash_baudwidth(void);

at_ret_t cmd_set_pm(const pm_args_t *args);
at_ret_t cmd_set_pm_debug_level(const pm_debug_args_t *args);
at_ret_t at_set_sleep_mode(const sleepmode_args_t *args);
at_ret_t wear_crash_test(const wear_crash_args_t *argc);
#ifdef AT_IPC_V1_TEST
at_ret_t a2x_ipc_v1_basic(const wear_ipc_test_args_t *argc);
at_ret_t a2x_ipc_v1_pmfs(const wear_ipc_test_args_t *argc);
#endif
at_ret_t at_set_log_uart_baud(const log_uart_args_t *args);
at_ret_t at_burn_2131_fwpkg(const burn_2131_args_t *args);
at_ret_t at_set_module_frq(const module_frq_args_t *args);
at_ret_t at_mmc_tune_test(void);
at_ret_t at_sdio_set_default_tune(void);
at_ret_t at_enter_shipmode(void);

/* AT+SLVCOREPWR */
at_ret_t at_core_power_handle(const core_power_args_t *args);
/* AT+SETVOLTAGE */
at_ret_t at_set_voltage(const voltage_cfg_args_t *args);

#if defined(CONFIG_PM_VETO_TRACK_ENABLE) && (CONFIG_PM_VETO_TRACK_ENABLE == 1)
/* AT+PMGETVETOTRACK */
at_ret_t at_print_veto_trackinfo(const veto_track_loc_args_t *args);
#endif

at_ret_t at_set_efuse_bit(const efuse_bit_args_t *args);
at_ret_t at_get_efuse_bit(const efuse_bit_args_t *args);
at_ret_t at_set_secure_boot(void);
at_ret_t at_get_secure_boot(void);
at_ret_t at_get_version(void);
at_ret_t at_get_die_id(void);
at_ret_t at_tp_debug(void);
at_ret_t at_module_mem_test(const module_mem_test_args_t *args);

/* dfx */
at_ret_t at_dfx_show_task_heap(const task_args_t *args);
at_ret_t at_dfx_show_all_task_heap(void);
at_ret_t at_dfx_mem_leak(const mem_leak_t *args);
at_ret_t at_dfx_show_mem_free(void);
at_ret_t at_dfx_show_mem_info(void);
at_ret_t at_dfx_show_stack_waterline(void);
at_ret_t at_dfx_list_timer(void);
at_ret_t at_dfx_os_heap_info(void);
at_ret_t at_dfx_os_stack_info(void);
at_ret_t at_dfx_pm_dump(void);
at_ret_t at_dfx_pm_rm_vote(void);
at_ret_t at_dfx_pm_add_vote(void);
at_ret_t at_dfx_pm_req_mode(const pm_module_args_t *args);
at_ret_t at_print_vetoinfo(void);
at_ret_t at_psram_print_access_status(const psram_access_state_args_t *args);

/* pin */
at_ret_t at_set_pin(const pin_set_args_t *args);
at_ret_t at_get_pin(const pin_get_args_t *args);
at_ret_t at_get_all_pin(void);

at_ret_t at_pmp_dump(void);

/* lcd */
at_ret_t at_lcd_on(void);
at_ret_t at_lcd_off(void);

/* usb dfu */
at_ret_t at_usb_dfu_enable(void);
at_ret_t at_usb_dfu_soft_trigger(void);

/* ssi sleep enable */
at_ret_t at_ssi_enable(void);

/* sdio cmd 53 test */
at_ret_t at_sdio_cmd53_test(void);

/* AT+FMCNAND */
at_ret_t wear_fs_fmc_nand_erase_test(const fmcnand_args_t *para);
at_ret_t wear_fs_fmc_nand_writeoob_test(const fmcnand_args_t *para);
at_ret_t wear_fs_fmc_nand_readoob_test(const fmcnand_args_t *para);

/* AT+PERIPHERALSID */
at_ret_t at_get_peripherals_id(const peripherals_type_t *args);

/* AT+IOCONFIG */
at_ret_t at_io_config(const ioconfig_type_t *args);

/* AT+DHRYSTONE */
at_ret_t __attribute__((weak)) at_start_dhrystone(const dhry_core_args_t *args);

/* AT+PWRKEYCFG */
at_ret_t at_set_pwrkey_cfg(const pwrkey_args_t *para);

#if defined (SUPPORT_SPORT_HEALTH_TEST)
at_ret_t parse_hr_all(void);
at_ret_t parse_hr(const sports_health_test_file_number *args);
at_ret_t parse_gesture_all(void);
at_ret_t parse_gesture(const sports_health_test_file_number *args);
at_ret_t parse_gesture_single_type(const sports_health_test_file_number *args);
#endif
/* AT+XOCALI */
at_ret_t plt_atcmd_xo_cali_ops(const plt_xo_cali_args_t *args);

/* time */
at_ret_t at_time_set(const time_args_t *args);
at_ret_t at_time_get(void);

/* alarm */
at_ret_t at_pmu_alarm_set(const pmu_alarm_args_t *args);
at_ret_t at_pmu_alarm_close(const pmu_alarm_handle_args_t *args);
at_ret_t at_pmu_alarm_restart(const pmu_alarm_handle_args_t *args);
at_ret_t at_pmu_alarm_delete(const pmu_alarm_handle_args_t *args);
at_ret_t at_pmu_alarm_query(void);
at_ret_t at_pmu_alarm_deinit(void);

#if defined (CFG_DRIVERS_NANDFLASH)
at_ret_t wear_fs_format_force(void);
#endif

#ifdef JS_ENABLE
at_ret_t wear_get_trans_channel(void);
at_ret_t wear_set_trans_channel(const trans_channel_args_t *args);
#endif

at_ret_t at_get_adc_manual(const adc_manual_args_t *args);
#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
at_ret_t at_adc_start_auto_sample(const adc_start_auto_args_t *args);
at_ret_t at_adc_stop_auto_sample(const adc_stop_auto_args_t *args);
#endif

#ifdef SUPPORT_XOTRIM_ON_EFUSE
at_ret_t at_set_xotrim0_val(const xotrim_args_t *args);
at_ret_t at_get_xotrim0_val(void);
#endif

#ifdef SUPPORT_AIVOICE
at_ret_t at_start_aivoice(void);
at_ret_t at_stop_aivoice(void);
#endif

#ifdef CONFIG_SUPPORT_NPU_SAMPLES
at_ret_t at_npu_sample_fcw(void);
#endif

/* AT+SUBPOWERINFO */
at_ret_t at_sub_power_info(void);

at_ret_t at_gatt_cb_init(const gatt_server_uuid_args_t *argc);

at_ret_t at_spp_cb_init(void);

at_ret_t at_pms_set_info(const pms_set_args_t *args);

at_ret_t at_pm_mem_set_info(const pm_mem_set_args_t *args);

at_ret_t at_test_rc32k(test_rc32k_args_t *args);

#ifdef SUPPORT_LWIP
typedef struct {
    uint32_t  para_map;
    const char *ip; /* ip */
    const char *netmask; /* 掩码 */
    const char *gw; /* 网关 */
} at_usb_device_rndis_args_t;

at_ret_t at_usb_device_rndis_net(const at_usb_device_rndis_args_t* args);

static const at_para_parse_syntax_t g_usb_device_rndis_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = false,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.string.max_length = 32, // len 32
        .offset = offsetof(at_usb_device_rndis_args_t, ip)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = false,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.string.max_length = 32, // len 32
        .offset = offsetof(at_usb_device_rndis_args_t, netmask)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.string.max_length = 32, // len 32
        .offset = offsetof(at_usb_device_rndis_args_t, gw)
    },
};
#endif
at_ret_t plt_atcmd_set_log_limit(const void *args);

at_ret_t plt_atcmd_get_log_limit_info(void);

at_ret_t at_ldo_set_state(const ldo_set_state_args_t *args);

#ifdef LOSCFG_DRIVERS_USB_DEVICE_CLASS_DRIVERS
typedef struct {
    uint32_t para_map;
    uint32_t func;
} usbhost_args_t;

const at_para_parse_syntax_t usbhost_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last=true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 2, // max value is 2
        .offset = offsetof(usbhost_args_t, func)
    },
};
at_ret_t at_usb_host_init(const usbhost_args_t *args);
#endif

#ifdef LOSCFG_DRIVERS_USB_MASS_STORAGE
typedef struct {
    uint32_t para_map;
    uint32_t func;
    uint32_t blk_cnt;
} usbhost_umass_args_t;

const at_para_parse_syntax_t usbhost_umass_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 4, // 4 testcases
        .offset = offsetof(usbhost_umass_args_t, func)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 2048,
        .offset = offsetof(usbhost_umass_args_t, blk_cnt)
    },
};
at_ret_t at_usb_host_umass(const usbhost_umass_args_t *args);
#endif

#ifdef LOSCFG_DRIVERS_USB_HOST_NET_CLASS
typedef struct {
    uint32_t para_map;
    int32_t func; // tesecase no
} usbhostnet_args_t;

const at_para_parse_syntax_t usbhosttest_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 3, // just 3 testcases
        .offset = offsetof(usbhostnet_args_t, func)
    },
};
at_ret_t at_usb_host_net(const usbhostnet_args_t *args);
#endif

typedef struct {
    uint32_t                para_map;
    const char             *para0; /* string */
    const char             *para1; /* string */
    bool                    para2; /* bool */
} plt_set_log_limit_args_t;

static const at_para_parse_syntax_t g_plt_log_limit_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = false,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.string.max_length = 32,
        .offset = offsetof(plt_set_log_limit_args_t, para0)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = false,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.string.max_length = 32,
        .offset = offsetof(plt_set_log_limit_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range = { 0, 1 },
        .offset = offsetof(plt_set_log_limit_args_t, para2)
    },
};

const at_cmd_entry_t at_wear_cmd_parse_table[] = {

/* support AT to test cmd_id [0x2101,0x2200] */
#ifdef CONFIG_SUPPORT_AT_TEST
#if defined (CFG_DRIVERS_MMC) || defined (CFG_DRIVERS_NANDFLASH) || defined (CFG_DRIVERS_SPINOR)
    // AT+FSWRITE=/user/test, 1048576
    {
        "FSWRITE",
        0x2101,
        0,
        g_wear_fs_write_check_syntax,
        NULL,
        (at_set_func_t)wear_fs_write,
        NULL,
        NULL,
    },
    // AT+FSCHECK=/user/test, 1048576
    {
        "FSCHECK",
        0x2102,
        0,
        g_wear_fs_write_check_syntax,
        NULL,
        (at_set_func_t)wear_fs_check,
        NULL,
        NULL,
    },
    // AT+FSRWTEST
    {
        "FSRWTEST",
        0x2103,
        0,
        NULL,
        wear_fs_rw_test,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
#endif
#ifdef SUPPROT_EXECUTE_CORMARK
    // AT+COREMARK
    {
        "COREMARK",
        0x2104,
        0,
        NULL,
        wear_coremark_test,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
#endif
    // AT+DTCMBW
    {
        "DTCMBW",
        0x2105,
        0,
        NULL,
        mcu_test_dtcm_baudwidth,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    // AT+L2MBW
    {
        "L2MBW",
        0x2106,
        0,
        NULL,
        mcu_test_l2m_baudwidth,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
#if defined (CFG_DRIVERS_MMC) || defined (CFG_DRIVERS_NANDFLASH) || defined (CFG_DRIVERS_SPINOR)
    // AT+FSBW
    {
        "FSBW",
        0x2107,
        0,
        NULL,
        mcu_test_fs_baudwidth,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
#endif
    // AT+PSRAMBW
    {
        "PSRAMBW",
        0x2108,
        0,
        NULL,
        mcu_test_psram_baudwidth,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    // AT+NORFLASHBW
    {
        "NORFLASHBW",
        0x2109,
        0,
        NULL,
        mcu_test_norflash_baudwidth,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    // AT+CRASHTEST
    // core: 0--bt; 1--dsp; 2--apps; 3--gnss; 4--sensor;
    // type: 0--watchdog timeout; 1--hard fault(pmp access fault); 2--panic; 3--access tee in REE state
    {
        "CRASHTEST",
        0x210A,
        0,
        g_wear_crash_test_syntax,
        NULL,
        (at_set_func_t)wear_crash_test,
        NULL,
        NULL,
    },
#ifdef AT_IPC_V1_TEST
    {
        "A2XIPCV1BASIC",
        0x210B,
        0,
        a2xipc_syntax,
        NULL,
        (at_set_func_t)a2x_ipc_v1_basic,
        NULL,
        NULL,
    },
#endif
    // AT+MMCTUNE
    {
        "MMCTUNE",
        0x210C,
        0,
        NULL,
        at_mmc_tune_test,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    // AT+SDIOSETDEFAULTTUNE
    {
        "SDIOSETDEFAULTTUNE",
        0x210D,
        0,
        NULL,
        at_sdio_set_default_tune,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    #if defined (CFG_DRIVERS_NANDFLASH)
    // AT+BOOTNANDW
    {
        "BOOTNANDW",
        0x210E,
        0,
        NULL,
        wear_fs_boot_nand_write,
        NULL,
        NULL,
        NULL,
    },
#endif
    // AT+MEMTEST
    {
        "MEMTEST",
        0x210F,
        0,
        g_module_syntax,
        NULL,
        (at_set_func_t)at_module_mem_test,
        NULL,
        NULL,
    },
    // 0x2110 reserved
#if defined (CFG_DRIVERS_SDIO)
    // AT+SDIOCMD53
    {
        "SDIOCMD53",
        0x2111,
        0,
        NULL,
        at_sdio_cmd53_test,
        NULL,
        NULL,
        NULL,
    },
#endif
#if defined (CFG_DRIVERS_MMC)
    // AT+EMMCSECTOR
    {
        "EMMCSECTOR",
        0x2112,
        0,
        g_emmc_sector_oper,
        NULL,
        (at_set_func_t)at_emmc_sector_test,
        NULL,
        NULL,
    },
    // AT+SWITCHEMMCFREQ
    {
        "SWITCHEMMCFREQ",
        0x2113,
        0,
        emmc_freq_level_syntax,
        NULL,
        (at_set_func_t)at_switch_emmc_freq_level,
        NULL,
        NULL,
    },
    // AT+EMMCDS
    {
        "EMMCDS",
        0x2114,
        0,
        emmc_driver_strength_syntax,
        NULL,
        (at_set_func_t)at_set_emmc_driver_strength,
        NULL,
        NULL,
    },
#endif
#if defined (CFG_DRIVERS_NANDFLASH)
    {
        "FMCNANDE",
        0x2115,
        0,
        fmcnand_syntax,
        NULL,
        (at_set_func_t)wear_fs_fmc_nand_erase_test,
        NULL,
        NULL,
    },
    {
        "FMCNANDWOOB",
        0x2116,
        0,
        fmcnand_syntax,
        NULL,
        (at_set_func_t)wear_fs_fmc_nand_writeoob_test,
        NULL,
        NULL,
    },
    {
        "FMCNANDROOB",
        0x2117,
        0,
        fmcnand_syntax,
        NULL,
        (at_set_func_t)wear_fs_fmc_nand_readoob_test,
        NULL,
        NULL,
    },
#endif
    {
        "DHRYSTONE",
        0x2118,
        0,
        dhry_coretype_syntax,
        NULL,
        (at_set_func_t)at_start_dhrystone,
        NULL,
        NULL,
    },
    // AT+COMRAMBW
    {
        "COMRAMBW",
        0x2119,
        0,
        NULL,
        mcu_test_comram_baudwidth,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    // AT+TESTRC32K
    {
        "TESTRC32K",
        0x211A,
        0,
        test_rc32k_syntax,
        NULL,
        (at_set_func_t)at_test_rc32k,
        NULL,
        NULL,
    },
    // AT+PSRAMACCSTAT
    {
        "PSRAMACCSTAT",
        0x211B,
        0,
        psram_access_state_syntax,
        NULL,
        (at_set_func_t)at_psram_print_access_status,
        NULL,
        NULL,
    },
#endif /* CONFIG_SUPPORT_AT_TEST */

/* support platform functionlity AT cmd_id [0x2201,0x2300] */
#if defined (CFG_DRIVERS_MMC) || defined (CFG_DRIVERS_NANDFLASH) || defined (CFG_DRIVERS_SPINOR)
    // AT+FSFORMAT
    {
        "FSFORMAT",
        0x2201,
        0,
        NULL,
        wear_fs_format,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    // AT+FSINFO
    {
        "FSINFO",
        0x2202,
        0,
        NULL,
        wear_fs_info,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },

    // AT+FSRM=/user/test, 1048576
    {
        "FSRM",
        0x2203,
        0,
        g_wear_fs_write_check_syntax,
        NULL,
        (at_set_func_t)wear_fs_rm,
        NULL,
        NULL,
    },
#endif
    // AT+HEAPINFO
    {
        "HEAPINFO",
        0x2204,
        0,
        NULL,
        at_dfx_os_heap_info,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    // AT+STACKINFO
    {
        "STACKINFO",
        0x2205,
        0,
        NULL,
        at_dfx_os_stack_info,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    // AT+PM
    {
        "PM",
        0x2206,
        0,
        pm_syntax,
        NULL,
        (at_set_func_t)cmd_set_pm,
        NULL,
        NULL,
    },
    // AT+SLP
    {
        "SLP",
        0x2207,
        0,
        sleepmode_syntax,
        NULL,
        (at_set_func_t)at_set_sleep_mode,
        NULL,
        NULL,
    },
    // AT+LOGUARTBAUD=VAL VAL: uart baud rate
    {
        "LOGUARTBAUD",
        0x2208,
        0,
        g_log_uart_syntax,
        NULL,
        (at_set_func_t)at_set_log_uart_baud,
        NULL,
        NULL,
    },
    // AT+SETFRQ
    {
        "SETFRQ",
        0x2209,
        0,
        g_module_frq_syntax,
        NULL,
        (at_set_func_t)at_set_module_frq,
        NULL,
        NULL,
    },

    // AT+SLVCOREPWR
    {
        "SLVCOREPWR",
        0x220A,
        0,
        slave_core_power_syntax,
        NULL,
        (at_set_func_t)at_core_power_handle,
        NULL,
        NULL,
    },
    // AT+SETVOLTAGE
    {
        "SETVOLTAGE",
        0x220B,
        0,
        cfg_voltage_syntax,
        NULL,
        (at_set_func_t)at_set_voltage,
        NULL,
        NULL,
    },
    // AT+PMGETVETOINFO
    {
        "PMGETVETOINFO",
        0x220C,
        0,
        NULL,
        at_print_vetoinfo,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
#if defined(CONFIG_PM_VETO_TRACK_ENABLE) && (CONFIG_PM_VETO_TRACK_ENABLE == 1)
    // AT+PMGETVETOTRACK
    {
        "PMGETVETOTRACK",
        0x220D,
        0,
        veto_track_syntax,
        NULL,
        (at_set_func_t)at_print_veto_trackinfo,
        NULL,
        NULL,
    },
#endif
    // AT+SHIPMODE
    {
        "SHIPMODE",
        0x220E,
        0,
        NULL,
        at_enter_shipmode,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    // AT+EFUSEBITWRITE
    {
        "EFUSEBITWRITE",
        0x220F,
        0,
        efuse_bit_syntax,
        NULL,
        (at_set_func_t)at_set_efuse_bit,
        NULL,
        NULL,
    },
    // AT+EFUSEBITREAD
    {
        "EFUSEBITREAD",
        0x2210,
        0,
        efuse_bit_syntax,
        NULL,
        (at_set_func_t)at_get_efuse_bit,
        NULL,
        NULL,
    },
    // AT+SECUREBOOT
    {
        "SECUREBOOT",
        0x2211,
        0,
        NULL,
        at_set_secure_boot,
        NULL,
        NULL,
        NULL,
    },
    // AT+GETWSTPVER
    {
        "GETWSTPVER",
        0x2212,
        0,
        NULL,
        at_get_version,
        NULL,
        NULL,
        NULL,
    },
#ifdef LOSCFG_SHELL_ADAPT_AT
    // AT+SHELL=xxx
    {
        "SHELL",
        0x216D,
        0,
        g_shell_syntax,
        NULL,
        (at_set_func_t)at_shell_exec,
        NULL,
        NULL,
    },
#endif
    #ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
    // AT+SHOWTSKHEAP
    {
        "SHOWTSKHEAP",
        0x2213,
        0,
        g_task_syntax,
        NULL,
        (at_set_func_t)at_dfx_show_task_heap,
        NULL,
        NULL,
    },
    // AT+SHOWALLTSKHEAP
    {
        "SHOWALLTSKHEAP",
        0x2214,
        0,
        NULL,
        at_dfx_show_all_task_heap,
        NULL,
        NULL,
        NULL,
    },
    // AT+MEMLEAK
    {
        "MEMLEAK",
        0x2215,
        0,
        g_mem_leak_syntax,
        NULL,
        (at_set_func_t)at_dfx_mem_leak,
        NULL,
        NULL,
    },
    // AT+SHOWALLMEMINFO
    {
        "SHOWALLMEMINFO",
        0x2216,
        0,
        NULL,
        at_dfx_show_mem_info,
        NULL,
        NULL,
        NULL,
    },
    // AT+SHOWTSKWTL
    {
        "SHOWTSKWTL",
        0x2217,
        0,
        NULL,
        at_dfx_show_stack_waterline,
        NULL,
        NULL,
        NULL,
    },
#endif
    // AT+LISTTIMER
    {
        "LISTTIMER",
        0x2218,
        0,
        NULL,
        at_dfx_list_timer,
        NULL,
        NULL,
        NULL,
    },
    // AT+PMDUMP
    {
        "PMDUMP",
        0x2219,
        0,
        NULL,
        at_dfx_pm_dump,
        NULL,
        NULL,
        NULL,
    },
    // AT+PMMODE
    {
        "PMMODE",
        0x221A,
        0,
        g_pm_mode_syntax,
        NULL,
        (at_set_func_t)at_dfx_pm_req_mode,
        NULL,
        NULL,
    },
    // AT+SETPIN
    {
        "SETPIN",
        0x221B,
        0,
        g_pin_set_syntax,
        NULL,
        (at_set_func_t)at_set_pin,
        NULL,
        NULL,
    },
    // AT+GETPIN
    {
        "GETPIN",
        0x221C,
        0,
        g_pin_get_syntax,
        NULL,
        (at_set_func_t)at_get_pin,
        NULL,
        NULL,
    },
    // AT+GETALLPIN
    {
        "GETALLPIN",
        0x221D,
        0,
        NULL,
        at_get_all_pin,
        NULL,
        NULL,
        NULL,
    },
    // AT+REBOOT
    {
        "REBOOT",
        0x221E,
        0,
        NULL,
        at_reboot,
        NULL,
        NULL,
        NULL,
    },
    // AT+SETLVLMSG
    // level: 0--ALERT; 1--FATAL; 2--ERROR; 3--WARN; 4--NOTICE; 5--INFO; 6--DBG; 7--TRACE;
    // switch_code: 1--turn on msg level; 0--turn off msg level;
    {
        "SETLVLMSG",
        0x221F,
        0,
        g_set_msg_syntax,
        NULL,
        (at_set_func_t)at_set_level_msg,
        NULL,
        NULL,
    },
    // AT+SETSYSMSG
    // module_id:
                // 0--WIFI;
                // 1--BT;
                // 8195--DSP;
                // 4--BT_PF;
                // 16388--APP_PF;
                // 24580--GNSS_PF;
                // 24578--GNSS;
                // 16389--MEDIA;
                // 16391--APP;
                // 16--BT_STATUS;
                // 17--BT_OTA;
                // 11--BTH;
                // 16397--BGH;
                // 16385--BTC_ACORE
    // switch_code: 1--turn on the mod_id msg; 0--turn off the mod_id msg;
    {
        "SETSYSMSG",
        0x2220,
        0,
        g_set_msg_syntax,
        NULL,
        (at_set_func_t)at_set_mod_id_msg,
        NULL,
        NULL,
    },
    // AT+GETLOG
    {
        "GETLOG",
        0x2221,
        0,
        NULL,
        at_get_log_level,
        NULL,
        NULL,
        NULL,
    },
    // AT+SETLOG
    // para1: 0--ALERT; 1--FATAL; 2--ERROR; 3--WARN; 4--NOTICE; 5--INFO; 6--DBG; 7--TRACE;
    {
        "SETLOG",
        0x2222,
        0,
        g_set_log_syntax,
        NULL,
        (at_set_func_t)at_set_log_level,
        NULL,
        NULL,
    },
    // AT+PMPDUMP
    {
        "PMPDUMP",
        0x2223,
        0,
        NULL,
        at_pmp_dump,
        NULL,
        NULL,
        NULL,
    },
    // AT+GETDIEID
    {
        "GETDIEID",
        0x2224,
        0,
        NULL,
        at_get_die_id,
        NULL,
        NULL,
        NULL,
    },
    // AT+TPDEBUG
    {
        "TPDEBUG",
        0x2225,
        0,
        NULL,
        at_tp_debug,
        NULL,
        NULL,
        NULL,
    },
    // AT+GETSECURESTATE
    {
        "GETSECURESTATE",
        0x2226,
        0,
        NULL,
        at_get_secure_boot,
        NULL,
        NULL,
        NULL,
    },
    // AT+LCDON
    {
        "LCDON",
        0x2227,
        0,
        NULL,
        at_lcd_on,
        NULL,
        NULL,
        NULL,
    },
    // AT+LCDOFF
    {
        "LCDOFF",
        0x2228,
        0,
        NULL,
        at_lcd_off,
        NULL,
        NULL,
        NULL,
    },
#if defined (CFG_DRIVERS_NANDFLASH)
    // AT+FSFORMATFORCE
    {
        "FSFORMATFORCE",
        0x2229,
        0,
        NULL,
        wear_fs_format_force,
        NULL,
        NULL,
        NULL,
    },
#endif
    // AT+USBDFU
    {
        "USBDFU",
        0x222A,
        0,
        NULL,
        at_usb_dfu_enable,
        NULL,
        NULL,
        NULL,
    },
    // AT+SSIEN
    {
        "SSIEN",
        0x222B,
        0,
        NULL,
        at_ssi_enable,
        NULL,
        NULL,
        NULL,
    },
    // AT+PWRKEYCFG
    {
        "PWRKEYCFG",
        0x222C,
        0,
        pwrkey_syntax,
        NULL,
        (at_set_func_t)at_set_pwrkey_cfg,
        NULL,
        NULL,
    },
#if defined (SUPPORT_SPORT_HEALTH_TEST)
    // AT+HRALL
    {
        "HRALL",
        0x222D,
        0,
        NULL,
        parse_hr_all,
        NULL,
        NULL,
        NULL,
    },
    // AT+HR
    {
        "HR",
        0x222E,
        0,
        sports_health_test_file_number_syntax,
        NULL,
        (at_set_func_t)parse_hr,
        NULL,
        NULL,
    },
    // AT+GESTUREALL
    {
        "GESTUREALL",
        0x222F,
        0,
        NULL,
        parse_gesture_all,
        NULL,
        NULL,
        NULL,
    },
    // AT+GESTURE
    {
        "GESTURE",
        0x2230,
        0,
        sports_health_test_file_number_syntax,
        NULL,
        (at_set_func_t)parse_gesture,
        NULL,
        NULL,
    },
    // AT+GESTURESINGLETYPE
    {
        "GESTURESINGLETYPE",
        0x2231,
        0,
        sports_health_test_gesture_type_syntax,
        NULL,
        (at_set_func_t)parse_gesture_single_type,
        NULL,
        NULL,
    },
#endif
    // AT+PERIPHERALSID
    {
        "PERIPHERALSID",
        0x2232,
        0,
        g_peripherals_type_syntax,
        NULL,
        (at_set_func_t)at_get_peripherals_id,
        NULL,
        NULL,
    },
    // AT+XOCALI
    {
        "XOCALI",
        0x2233,
        0,
        g_plt_xo_cali_syntax,
        NULL,
        (at_set_func_t)plt_atcmd_xo_cali_ops,
        NULL,
        NULL,
    },
    // AT+PMDEBUGLEVEL
    {
        "PMDEBUGLEVEL",
        0x2234,
        0,
        pm_syntax,
        NULL,
        (at_set_func_t)cmd_set_pm_debug_level,
        NULL,
        NULL,
    },
    // AT+TIMEGET
    {
        "TIMEGET",
        0x2235,
        0,
        NULL,
        (at_cmd_func_t)at_time_get,
        NULL,
        NULL,
        NULL,
    },
    // AT+PMUALARM
    {
        "PMUALARM",
        0x2236,
        0,
        pmu_alarm_syntax,
        NULL,
        (at_set_func_t)at_pmu_alarm_set,
        NULL,
        NULL,
    },
    // AT+PMUALARMDEL
    {
        "PMUALARMDEL",
        0x2237,
        0,
        pmu_alarm_handle_syntax,
        NULL,
        (at_set_func_t)at_pmu_alarm_delete,
        NULL,
        NULL,
    },
    // AT+PMUALARMDEINIT
    {
        "PMUALARMDEINIT",
        0x2238,
        0,
        NULL,
        at_pmu_alarm_deinit,
        NULL,
        NULL,
        NULL,
    },
    // AT+IOCONFIG
    {
        "IOCONFIG",
        0x2239,
        0,
        g_ioconfig_syntax,
        NULL,
        (at_set_func_t)at_io_config,
        NULL,
        NULL,
    },
    // AT+PMRMVOTE
    {
        "PMRMVOTE",
        0x223A,
        0,
        NULL,
        at_dfx_pm_rm_vote,
        NULL,
        NULL,
        NULL,
    },
    // AT+PMADDVOTE
    {
        "PMADDVOTE",
        0x223B,
        0,
        NULL,
        at_dfx_pm_add_vote,
        NULL,
        NULL,
        NULL,
    },
#ifdef JS_ENABLE
    // AT+GETTRANSCHAN
    {
        "GETTRANSCHAN",
        0x223C,
        0,
        NULL,
        wear_get_trans_channel,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    // AT+SETTRANSCHAN
    {
        "SETTRANSCHAN",
        0x223D,
        0,
        g_set_trans_channel_syntax,
        NULL,
        (at_set_func_t)wear_set_trans_channel,
        NULL,
        NULL,
    },
#endif
    // AT+GETADCMANUAL
    {
        "GETADCMANUAL",
        0x223E,
        0,
        g_adc_manual_args_syntax,
        NULL,
        (at_set_func_t)at_get_adc_manual,
        NULL,
        NULL,
    },
#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
    // AT+STARTADCAUTO
    {
        "STARTADCAUTO",
        0x223F,
        0,
        g_adc_start_auto_args_syntax,
        NULL,
        (at_set_func_t)at_adc_start_auto_sample,
        NULL,
        NULL,
    },
    // AT+STOPADCAUTO
    {
        "STOPADCAUTO",
        0x2240,
        0,
        g_adc_stop_auto_args_syntax,
        NULL,
        (at_set_func_t)at_adc_stop_auto_sample,
        NULL,
        NULL,
    },
#endif
#ifdef SUPPORT_XOTRIM_ON_EFUSE
    // AT+XOTRIMSET
    {
        "XOTRIMSET",
        0x2241,
        0,
        g_xotrim_syntax,
        NULL,
        (at_set_func_t)at_set_xotrim0_val,
        NULL,
        NULL,
    },
    // AT+XOTRIMGET
    {
        "XOTRIMGET",
        0x2242,
        0,
        NULL,
        at_get_xotrim0_val,
        NULL,
        NULL,
        NULL,
    },
#endif
    // AT+SUBPOWERINFO
    {
        "SUBPOWERINFO",
        0x2243,
        0,
        NULL,
        at_sub_power_info,
        NULL,
        NULL,
        NULL,
    },
#ifdef SUPPORT_AIVOICE
    // AT+AIVOICESTART
    {
        "AIVOICESTART",
        0x2244,
        0,
        NULL,
        at_start_aivoice,
        NULL,
        NULL,
        NULL,
    },
     // AT+AIVOICESTOP
    {
        "AIVOICESTOP",
        0x2245,
        0,
        NULL,
        at_stop_aivoice,
        NULL,
        NULL,
        NULL,
    },
#endif
    // AT+GATTCBINIT
    {
        "GATTCBINIT",
        0x2246,
        0,
        g_gatt_cb_syntax,
        NULL,
        (at_set_func_t)at_gatt_cb_init,
        NULL,
        NULL,
    },
    // AT+PMUALARMQUERY
    {
        "PMUALARMQUERY",
        0x2247,
        0,
        NULL,
        at_pmu_alarm_query,
        NULL,
        NULL,
        NULL,
    },
    // AT+GATTCBINIT
    {
        "SPPCBINIT",
        0x2248,
        0,
        NULL,
        at_spp_cb_init,
        NULL,
        NULL,
        NULL,
    },

    // AT+PMUALARMCLOSE
    {
        "PMUALARMCLOSE",
        0x2170,
        0,
        pmu_alarm_handle_syntax,
        NULL,
        (at_set_func_t)at_pmu_alarm_close,
        NULL,
        NULL,
    },
    // AT+PMUALARMRESTART
    {
        "PMUALARMRESTART",
        0x2171,
        0,
        pmu_alarm_handle_syntax,
        NULL,
        (at_set_func_t)at_pmu_alarm_restart,
        NULL,
        NULL,
    },
#ifdef SUPPORT_POWER_MANAGER
    // AT+PMSSET
    {
        "PMSSET",
        0x2248,
        0,
        g_pms_set_syntax,
        NULL,
        (at_set_func_t)at_pms_set_info,
        NULL,
        NULL,
    },
#endif
    // AT+PMMEMSET
    {
        "PMMEMSET",
        0x2249,
        0,
        g_pm_mem_set_syntax,
        NULL,
        (at_set_func_t)at_pm_mem_set_info,
        NULL,
        NULL,
    },
#ifdef SUPPORT_AUTO_OTA
    // AT+AUTOOTABLOCK
    {
        "AUTOOTABLOCK",
        0x2250,
        0,
        NULL,
        at_mcu_auto_ota_block,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    // AT+AUTOOTARUN
    {
        "AUTOOTARUN",
        0x2251,
        0,
        NULL,
        at_mcu_auto_ota_run,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    // AT+AUTOOTADELMAC
    {
        "AUTOOTADELMAC",
        0x2252,
        0,
        g_auto_ota_syntax,
        NULL,
        (at_set_func_t)at_mcu_auto_ota_del_mac,
        NULL,
        NULL,
    },
    // AT+AUTOOTAADDMAC
    {
        "AUTOOTAADDMAC",
        0x2253,
        0,
        g_auto_ota_syntax,
        NULL,
        (at_set_func_t)at_mcu_auto_ota_add_mac,
        NULL,
        NULL,
    },
    // AT+AUTOOTASHOWMAC
    {
        "AUTOOTASHOWMAC",
        0x2254,
        0,
        NULL,
        at_mcu_auto_ota_show_mac,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
#endif
#ifdef CONFIG_SUPPORT_NPU_SAMPLES
    // AT+FUZZYCOMMANDWORD
    {
        "FUZZYCOMMANDWORD",
        0x2255,
        0,
        NULL,
        at_npu_sample_fcw,
        NULL,
        NULL,
        NULL,
    },
#endif
#ifdef SUPPORT_LWIP
    {
        "USBRNDIS",
        0x2172,
        0,
        g_usb_device_rndis_syntax,
        NULL,
        (at_set_func_t)at_usb_device_rndis_net,
        NULL,
        NULL,
    },
#endif
#ifdef LOSCFG_DRIVERS_USB_DEVICE_CLASS_DRIVERS
    // AT+USBHOST
    {
        "USBHOST",
        0x2256,
        0,
        usbhost_syntax,
        NULL,
        at_usb_host_init,
        NULL,
        NULL,
    },
#endif
#ifdef LOSCFG_DRIVERS_USB_MASS_STORAGE
    // AT+USBMASS
    {
        "USBMASS",
        0x2257,
        0,
        usbhost_umass_syntax,
        NULL,
        at_usb_host_umass,
        NULL,
        NULL,
    },
#endif
#ifdef LOSCFG_DRIVERS_USB_HOST_NET_CLASS
    // AT+USBNET
    {
        "USBNET",
        0x2258,
        0,
        usbhosttest_syntax,
        NULL,
        at_usb_host_net,
        NULL,
        NULL,
    },
#endif
#if defined(SUPPORT_ALIPAY_SEC) || defined(SUPPORT_ALIPAY_HSC32I1)
    // AT+ALIPAYDEBUG
    {
        "ALIPAYDEBUG",
        0x2259,
        0,
        NULL,
        alipay_debug,
        NULL,
        NULL,
        NULL,
    },
    // AT+ALIPAYRESET
    {
        "ALIPAYRESET",
        0x225A,
        0,
        NULL,
        alipay_reset,
        NULL,
        NULL,
        NULL,
    },
#endif
    // AT+USBDFUTRIGGER
    {
        "USBDFUTRIGGER",
        0x225B,
        0,
        NULL,
        at_usb_dfu_soft_trigger,
        NULL,
        NULL,
        NULL,
    },
    {
        "LOGLIMIT",
        0x225C,
        0,
        g_plt_log_limit_syntax,
        NULL,
        plt_atcmd_set_log_limit,
        plt_atcmd_get_log_limit_info,
        NULL,
    },
    // AT+LDOSETSTATE
    {
        "LDOSETSTATE",
        0x225D,
        0,
        ldo_set_state_syntax,
        NULL,
        (at_set_func_t)at_ldo_set_state,
        NULL,
        NULL,
    },
#ifdef CONFIG_PRODUCT_EVB_DITING
    // AT+BURN2131=2000000, 0 2M 不烧写
    {
        "BURN2131",
        0x225E,
        0,
        g_burn_2131_syntax,
        NULL,
        (at_set_func_t)at_burn_2131_fwpkg,
        NULL,
        NULL,
    },
#endif
    // AT+TIMESET=2025,6,7,15,4,1
    {
        "TIMESET",
        0x225F,
        0,
        g_time_syntax,
        NULL,
        (at_set_func_t)at_time_set,
        NULL,
        NULL,
    },
};
#endif  /* AT_WEAR_CMD_TALBE_H */
