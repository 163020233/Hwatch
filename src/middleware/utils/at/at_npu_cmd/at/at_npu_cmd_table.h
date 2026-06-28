/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: At npu cmd table \n
 *
 */

#if !defined(AT_NPU_CMD_TABLE_H)
#define AT_NPU_CMD_TABLE_H

#include "at.h"
#include "platform_types.h"

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1;
} npucmd_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 core_id;
    const uint8_t           *para1;
} npucmd_v2_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *path;
    const uint8_t           *addr;
} load2mem_args_t;

/* Npu CMD Through IPC */
at_ret_t npu_handle_cmd_help(void);
at_ret_t npu_handle_cmd(const npucmd_args_t *args);

/* Npu CMD V2 Through IPC/local by core_id */
at_ret_t npu_handle_cmd_help_v2(void);
at_ret_t npu_handle_cmd_v2(const npucmd_v2_args_t *args);

at_ret_t load2mem_cmd_help(void);
at_ret_t load2mem_cmd(const load2mem_args_t *args);

const at_para_parse_syntax_t npucmd_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(npucmd_args_t, para1)
    },
};

const at_para_parse_syntax_t npucmd_v2_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = CORES_MAX_NUMBER_PHYSICAL - 1,
        .offset = offsetof(npucmd_v2_args_t, core_id)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(npucmd_v2_args_t, para1)
    },
};

const at_para_parse_syntax_t load2mem_v2_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(load2mem_args_t, path)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .offset = offsetof(load2mem_args_t, addr)
    },
};

const at_cmd_entry_t at_npu_cmd_parse_table[] = {
    {
        "NPUCMD",
        1,
        0,
        npucmd_syntax,
        npu_handle_cmd_help,
        (at_set_func_t)npu_handle_cmd,
        npu_handle_cmd_help,
        NULL,
    },
    /* V2 */
    {
        "NPU",
        2,
        0,
        npucmd_v2_syntax,
        npu_handle_cmd_help_v2,
        (at_set_func_t)npu_handle_cmd_v2,
        npu_handle_cmd_help_v2,
        NULL,
    },
    /* Load file to memory. */
    {
        "LOADTOMEM",
        3,
        0,
        load2mem_v2_syntax,
        load2mem_cmd_help,
        (at_set_func_t)load2mem_cmd,
        load2mem_cmd_help,
        NULL,
    },
};

#endif  /* AT_NPU_CMD_TABLE_H */
