/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: At media cmd table \n
 *
 */

#if !defined(AT_MEDIA_CMD_TABLE_H)
#define AT_MEDIA_CMD_TABLE_H

#include "at.h"
#include "media_cmd_manager.h"


#define MAX_MEDIA_MSG_LEN 2000

const at_para_parse_syntax_t media_cmd_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, media_cmd_str)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para6)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para7)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para8)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para9)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para10)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para11)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para12)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para13)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para14)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para15)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para16)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para17)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para18)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .offset = offsetof(media_args, para19)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE,
        .last = true,
        .offset = offsetof(media_args, para20)
    },
};

// 绑大类（strcommoncmd的第一个数字）
// 小类通过AT用例内部实现查找strcommoncmd的第二个数字
const at_cmd_entry_t at_media_cmd_parse_table[] = {
    {
        "MEDIA",
        20,
        0,
        media_cmd_syntax,
        NULL,
        (at_set_func_t)media_at_cmd_handle,
        NULL,
        NULL,
    },
};

#endif  /* AT_MEDIA_CMD_AT_CMD_TALBE_H */
