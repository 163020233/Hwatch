/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux cmd system header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-07-22， Create file. \n
 */
#ifndef UX_CMD_RECOVERY_H
#define UX_CMD_RECOVERY_H
#include <stdint.h>
#include "errcode.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
 
errcode_t ux_cmd_recovery_process(uint16_t src_dev_id, uint8_t *data, uint16_t data_len);
void ux_cmd_recovery_register(void);
 
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif