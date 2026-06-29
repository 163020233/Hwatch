/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: At media register header \n
 *
 */

#ifndef AT_MEDIA_CMD_REGISTER_H
#define AT_MEDIA_CMD_REGISTER_H

#include "at.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define EXT_AT_MEDIA_CMD_MAX_LEN   128
typedef void (*proc_media_at_cmd)(uint16_t cmd, uint8_t *data, uint32_t len);

uint32_t uapi_at_media_register_cmd(const at_cmd_entry_t *cmd_tbl, uint16_t cmd_num);
void at_media_cmd_register(void);
void register_media_at_proc_cb(proc_media_at_cmd cb);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif
