/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux cmd ext mic service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-14， Create file. \n
 */
#ifndef UX_CMD_EXT_MIC_H
#define UX_CMD_EXT_MIC_H

#include <stdint.h>
#include "errcode.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

errcode_t ux_cmd_ext_mic_plug_in(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_ext_mic_plug_out(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_ext_mic_get_status(uint16_t device_id, uint8_t *data, uint16_t data_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif