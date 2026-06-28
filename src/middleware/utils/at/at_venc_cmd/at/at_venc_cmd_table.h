/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: At venc cmd table
 *
 */

#if !defined(AT_VENC_CMD_TALBE_H)
#define AT_VENC_CMD_TALBE_H

#include "at.h"

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
    char                 *para6;
} venc_para_t;

const at_para_parse_syntax_t venc_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 100,
        .offset = offsetof(venc_para_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 200,
        .offset = offsetof(venc_para_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 100,
        .offset = offsetof(venc_para_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 100,
        .offset = offsetof(venc_para_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 100,
        .offset = offsetof(venc_para_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 100,
        .last = true,
        .offset = offsetof(venc_para_t, para6)
    },
};

at_ret_t venc_process(const venc_para_t *args);
const at_cmd_entry_t at_venc_cmd_parse_table[] = {
    {
        "VENC",
        1,
        0,
        venc_syntax,
        NULL,
        (at_set_func_t)venc_process,
        NULL,
        NULL,
    },
};
#endif  /* AT_VENC_CMD_AT_CMD_TALBE_H */
