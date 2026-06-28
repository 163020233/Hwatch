/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: at cat1 cmd table
 */

#if !defined(AT_CAT1_CMD_TABLE_H)
#define AT_CAT1_CMD_TABLE_H

#include "at.h"

#ifdef SUPPORT_VOLTE_AUTOTEST
typedef struct {
    uint32_t  para_map;
    char*  parm;
} cat1_autotest_args;
#endif

#ifdef SUPPORT_IMS_CAT1_CHANNEL_TEST
typedef struct {
    uint32_t  para_map;
    uint32_t  parm;
} cat1_chan_spi_args;

typedef struct {
    uint32_t  para_map;
    uint32_t  parm;
} cat1_chan_loglevel_args;

typedef struct {
    uint32_t  para_map;
    uint32_t  sleep_switch;
    uint32_t  sleep_time;
} ril_sleep_time_args;

typedef struct {
    uint32_t  para_map;
    uint32_t  parm1;
    uint32_t  parm2;
    char*     parm3;
} cat1_debug_args;

typedef struct {
    uint32_t  para_map;
    uint32_t  parm;
    uint32_t  count;
} amr_to_spi_time_args;
#endif

#ifdef SUPPORT_IMS_FUZZ_TEST
typedef struct {
    uint32_t  para_map;
    uint32_t  parm;
} cat1_chan_fuzz_args;
#endif

#ifdef SUPPORT_IMS_CAT1_CHANNEL_TEST
at_ret_t at_cat1_delay_reset(void);
at_ret_t at_cat1_chan_spi(const void *args);
at_ret_t at_cat1_chan_loglevel(const void *args);
at_ret_t at_cat1_ril_utc(void);
at_ret_t at_cat1_ril_sleep_time(const void *args);
at_ret_t at_cat1_ims_task_info(void);
at_ret_t at_cat1_debug(const void *args);
at_ret_t at_cat1_amr_to_spi_time(const void *args);
#endif
#if (CHIP_3322_V100 == 1)
at_ret_t get_sim_card_status(void);
at_ret_t get_network_reg_status(void);
at_ret_t get_operator_info_at(void);
at_ret_t get_signal_info_at(void);
at_ret_t get_flight_mode_at(void);
at_ret_t get_utc_time_at(void);
at_ret_t get_ims_reg_status_at(void);
at_ret_t get_volte_status_at(void);
at_ret_t get_cat1_network_status_at(void);
at_ret_t get_rate_report_start(void);
at_ret_t get_rate_report_stop(void);
at_ret_t get_wifi_info(void);
#endif
at_ret_t at_cat1_ims_spi_test_handle(void);
#ifdef SUPPORT_VOLTE_AUTOTEST
at_ret_t at_cat1_autotest(const void *args);
#endif
#ifdef SUPPORT_IMS_FUZZ_TEST
at_ret_t at_cat1_chan_fuzz(const void *args);
#endif
at_ret_t at_cat1_ota_test(void);

#ifdef SUPPORT_VOLTE_AUTOTEST
const at_para_parse_syntax_t cat1_autotest_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 512,
        .offset = offsetof(cat1_autotest_args, parm)
    }
};
#endif

#ifdef SUPPORT_IMS_CAT1_CHANNEL_TEST
const at_para_parse_syntax_t cat1_chan_spi_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 65535,
        .offset = offsetof(cat1_chan_spi_args, parm)
    }
};

const at_para_parse_syntax_t cat1_chan_loglevel_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 65535,
        .offset = offsetof(cat1_chan_loglevel_args, parm)
    }
};

const at_para_parse_syntax_t ril_sleep_time_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(ril_sleep_time_args, sleep_switch)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 0x7FFFFFFF,
        .offset = offsetof(ril_sleep_time_args, sleep_time)
    }
};

const at_para_parse_syntax_t cat1_debug_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 65535,
        .offset = offsetof(cat1_debug_args, parm1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 65535,
        .offset = offsetof(cat1_debug_args, parm2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 256,
        .offset = offsetof(cat1_debug_args, parm3)
    },
};

const at_para_parse_syntax_t amr_to_spi_time_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(amr_to_spi_time_args, parm)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 0x7FFFFFFF,
        .offset = offsetof(amr_to_spi_time_args, count)
    }
};
#endif

#ifdef SUPPORT_IMS_FUZZ_TEST
const at_para_parse_syntax_t cat1_chan_fuzz_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 65535,
        .offset = offsetof(cat1_chan_fuzz_args, parm)
    }
};
#endif

const at_cmd_entry_t at_cat1_cmd_parse_table[] = {
#ifdef SUPPORT_IMS_CAT1_CHANNEL_TEST
    {
        "CAT1RESET",
        1,
        0,
        NULL,
        at_cat1_delay_reset,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "CAT1CHANSPI",
        2,
        0,
        cat1_chan_spi_syntax,
        NULL,
        at_cat1_chan_spi,
        NULL,
        NULL,
    },
    {
        "CAT1CHANLOGLEVEL",
        3,
        0,
        cat1_chan_loglevel_syntax,
        NULL,
        at_cat1_chan_loglevel,
        NULL,
        NULL,
    },
    {
        "RILTESTUTC",
        4,
        0,
        NULL,
        at_cat1_ril_utc,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "RILTESTSLEEPTIME",
        5,
        0,
        ril_sleep_time_syntax,
        NULL,
        at_cat1_ril_sleep_time,
        NULL,
        NULL,
    },
    {
        "AMRTOSPITIME",
        6,
        0,
        amr_to_spi_time_syntax,
        NULL,
        at_cat1_amr_to_spi_time,
        NULL,
        NULL,
    },
    {
        "CAT1TASKINFO",
        10,
        0,
        NULL,
        at_cat1_ims_task_info,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "CAT1DEBUG",
        11,
        0,
        cat1_debug_syntax,
        NULL,
        at_cat1_debug,
        NULL,
        NULL,
    },
#endif
    {
        "IMSSPITEST",
        12,
        0,
        NULL,
        at_cat1_ims_spi_test_handle,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
#ifdef SUPPORT_VOLTE_AUTOTEST
    {
        "IMSAUTOTEST",
        30,
        0,
        cat1_autotest_syntax,
        NULL,
        at_cat1_autotest,
        NULL,
        NULL,
    },
#endif
#ifdef SUPPORT_IMS_FUZZ_TEST
    {
        "CAT1CHANFUZZ",
        40,
        0,
        cat1_chan_fuzz_syntax,
        NULL,
        at_cat1_chan_fuzz,
        NULL,
        NULL,
    },
#endif
    {
        "CAT1OTATEST",
        50,
        0,
        NULL,
        at_cat1_ota_test,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
#if (CHIP_3322_V100 == 1)
    {
        "SIMSTATUS",
        60,
        0,
        NULL,
        get_sim_card_status,
        NULL,
        NULL,
        NULL,
    },

    {
        "REGSTATUS",
        61,
        0,
        NULL,
        get_network_reg_status,
        NULL,
        NULL,
        NULL,
    },

    {
        "OPERINFO",
        62,
        0,
        NULL,
        get_operator_info_at,
        NULL,
        NULL,
        NULL,
    },

    {
        "SIGNALINFO",
        63,
        0,
        NULL,
        get_signal_info_at,
        NULL,
        NULL,
        NULL,
    },

    {
        "FLIGHTMODE",
        64,
        0,
        NULL,
        get_flight_mode_at,
        NULL,
        NULL,
        NULL,
    },

    {
        "UTC",
        65,
        0,
        NULL,
        get_utc_time_at,
        NULL,
        NULL,
        NULL,
    },

    {
        "IMSREG",
        66,
        0,
        NULL,
        get_ims_reg_status_at,
        NULL,
        NULL,
        NULL,
    },

    {
        "VOLTESTATUS",
        67,
        0,
        NULL,
        get_volte_status_at,
        NULL,
        NULL,
        NULL,
    },

    {
        "CAT1STATUS",
        68,
        0,
        NULL,
        get_cat1_network_status_at,
        NULL,
        NULL,
        NULL,
    },

    {
        "RATEREPORTSTART",
        69,
        0,
        NULL,
        get_rate_report_start,
        NULL,
        NULL,
        NULL,
    },

    {
        "RATEREPORTSTOP",
        70,
        0,
        NULL,
        get_rate_report_stop,
        NULL,
        NULL,
        NULL,
    },

    {
        "WIFISCAN",
        71,
        0,
        NULL,
        get_wifi_info,
        NULL,
        NULL,
        NULL,
    },
#endif
};

#endif  /* AT_CAT1_CMD_TABLE_H */