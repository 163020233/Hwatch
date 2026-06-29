/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: At gnss cmd table \n
 *
 */

#if !defined(AT_DITING_GNSS_CMD_TABLE_H)
#define AT_DITING_GNSS_CMD_TABLE_H

#include "at.h"

typedef struct {
    uint32_t para_map;
    uint32_t nmeaFlag;
    uint32_t replayFlag;
    uint32_t logLevel;
    uint32_t rtkReplay;
} gnss_init_args;

typedef struct {
    uint32_t para_map;
    uint32_t mode;
} gnss_sample_args;

typedef struct {
    uint32_t para_map;
    uint32_t nmeaFlag;
} gnss_nmea_args;

typedef struct {
    uint32_t para_map;
    uint32_t num;
    uint32_t size;
} gnss_log_args;

typedef struct {
    uint32_t    para_map;
    char        *data;          /* HEX数据内容 */
} gnss_hex_args;

typedef struct {
    uint32_t para_map;
    uint32_t posType;
    uint32_t xgnssType;
} gnss_cfg_args;

at_ret_t at_gnss_init(gnss_init_args *args);
at_ret_t at_gnss_start(void);
at_ret_t at_gnss_stop(void);
at_ret_t at_gnss_deinit(void);
at_ret_t at_gnss_sample(gnss_sample_args *args);
at_ret_t at_gnss_log(gnss_log_args *args);
at_ret_t at_gnss_nmea(gnss_nmea_args *args);
at_ret_t at_gnss_hex(gnss_hex_args *args);
at_ret_t at_gnss_cfg(gnss_cfg_args *args);

const at_para_parse_syntax_t gnss_init_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(gnss_init_args, nmeaFlag)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(gnss_init_args, replayFlag)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 5,
        .offset = offsetof(gnss_init_args, logLevel)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(gnss_init_args, rtkReplay)
    },
};

const at_para_parse_syntax_t gnss_sample_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 6,
        .offset = offsetof(gnss_sample_args, mode)
    },
};

const at_para_parse_syntax_t gnss_nmea_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(gnss_nmea_args, nmeaFlag)
    },
};

const at_para_parse_syntax_t gnss_log_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 10,
        .offset = offsetof(gnss_log_args, num)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 5,
        .offset = offsetof(gnss_log_args, size)
    },
};

const at_para_parse_syntax_t gnss_hex_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = AT_CMD_MAX_LENGTH,
        .offset = offsetof(gnss_hex_args, data)
    },
};

const at_para_parse_syntax_t gnss_cfg_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(gnss_cfg_args, posType)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(gnss_cfg_args, xgnssType)
    },
};

const at_cmd_entry_t at_diting_gnss_cmd_parse_table[] = {
    {
        "GNSSINIT",
        1,
        0,
        gnss_init_syntax,
        NULL,                             // 无参数时调用的函数
        (at_set_func_t)at_gnss_init,     // 有参数时调用的函数
        NULL,
        NULL,
    },
    {
        "GNSSSTART",
        2,
        0,
        NULL,
        at_gnss_start,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "GNSSSTOP",
        3,
        0,
        NULL,
        at_gnss_stop,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "GNSSDEINIT",
        4,
        0,
        NULL,
        at_gnss_deinit,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "GNSSSAMPLE",
        5,
        0,
        gnss_sample_syntax,
        NULL,
        (at_set_func_t)at_gnss_sample,
        NULL,
        NULL,
    },
    {
        "GNSSNMEA",
        6,
        0,
        gnss_nmea_syntax,
        NULL,
        (at_set_func_t)at_gnss_nmea,
        NULL,
        NULL,
    },
    {
        "GNSSLOGSIZE",
        7,
        0,
        gnss_log_syntax,
        NULL,
        (at_set_func_t)at_gnss_log,
        NULL,
        NULL,
    },
    {
        "GNSSHEX",
        8,
        0,
        gnss_hex_syntax,
        NULL,
        (at_set_func_t)at_gnss_hex,
        NULL,
        NULL,
    },
    {
        "GNSSCFG",
        9,
        0,
        gnss_cfg_syntax,
        NULL,
        (at_set_func_t)at_gnss_cfg,
        NULL,
        NULL,
    },
};

#endif  /* AT_GNSS_CMD_AT_CMD_TALBE_H */
