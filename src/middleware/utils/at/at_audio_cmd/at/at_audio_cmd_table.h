/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: At audio cmd table \n
 *
 */

#if !defined(AT_AUDIO_CMD_TALBE_H)
#define AT_AUDIO_CMD_TALBE_H

#include "at.h"

typedef struct {
    uint32_t             para_map;
    char                 *para1;
} audio_at_cmd_args_t;
const at_para_parse_syntax_t audio_at_cmd_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL| AT_SYNTAX_ATTR_FIX_CASE | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 128,
        .offset = offsetof(audio_at_cmd_args_t, para1)
    }
};

at_ret_t audio_at_cmd_process(const audio_at_cmd_args_t *args);

const at_cmd_entry_t at_audio_cmd_parse_table[] = {
    {
        "AUDIO",
        0x2401,
        0,
        audio_at_cmd_syntax,
        NULL,
        (at_set_func_t)audio_at_cmd_process,
        NULL,
        NULL,
    },
};

#endif  /* AT_AUDIO_CMD_AT_CMD_TALBE_H */