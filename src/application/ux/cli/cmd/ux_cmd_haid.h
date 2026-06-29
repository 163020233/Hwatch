/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux cmd haid at service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-05-02， Create file. \n
 */
#ifndef UX_CMD_HAID_H
#define UX_CMD_HAID_H
#include <stdint.h>
#include "errcode.h"
#include "ux_api_communication.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void ux_cmd_haid_init(void);
errcode_t ux_cmd_haid_set_mode(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_haid_set_volume(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_haid_set_mix_vol_range(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_haid_set_work_type(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_haid_get_mode(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_haid_get_volume(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_haid_get_mix_vol_range(uint8_t *data, uint16_t data_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
