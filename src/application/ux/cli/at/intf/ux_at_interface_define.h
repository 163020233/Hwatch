/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux manager msg info \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */

#ifndef UX_AT_INTERFACE_DEFINE_H
#define UX_AT_INTERFACE_DEFINE_H
#include "stdint.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef errcode_t (*ux_at_callback_t)(uint8_t *data, uint16_t data_len);
typedef errcode_t (*ux_at_cmd_callback_t)(uint16_t device_id, uint8_t *data,  uint16_t data_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
