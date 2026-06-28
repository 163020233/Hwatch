/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: at cat1 cmd register \n
 */

#ifndef AT_CAT1_CMD_REGISTER_H
#define AT_CAT1_CMD_REGISTER_H

#include "at.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define EXT_AT_CAT1_CMD_MAX_LEN   128

uint32_t uapi_at_cat1_register_cmd(const at_cmd_entry_t *cmd_tbl, uint16_t cmd_num);
void at_cat1_cmd_register(void);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* AT_CAT1_CMD_REGISTER_H */