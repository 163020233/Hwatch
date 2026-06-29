/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: esim \n
 */
#ifndef AT_ESIM_CMD_REGISTER_H
#define AT_ESIM_CMD_REGISTER_H
#include "at.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define EXT_AT_ESIM_CMD_MAX_LEN 256

uint32_t uapi_at_esim_register_cmd(const at_cmd_entry_t *cmd_tbl, uint16_t cmd_num);
void at_esim_esim_register(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* AT_ESIM_CMD_REGISTER_H */