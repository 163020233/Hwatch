/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: At media func \n
 *
 */

#include "at_media.h"
#include <string.h>
#include "at_cmd.h"
#include "at_media_cmd_register.h"
#include "at_media_cmd_table.h"
#include "debug_print.h"

#define HEX_BASE 16

#pragma pack()

proc_media_at_cmd g_media_at_cb = NULL;

#define AT_MEDIA_FUNC_NUM (sizeof(at_media_cmd_parse_table) / sizeof(at_media_cmd_parse_table[0]))
void los_at_media_cmd_register(void)
{
    print_str("los_at_media_cmd_register EXCUTE\r\n");
    uapi_at_media_register_cmd(at_media_cmd_parse_table, AT_MEDIA_FUNC_NUM);
}

void register_media_at_proc_cb(proc_media_at_cmd cb)
{
    g_media_at_cb = cb;
}