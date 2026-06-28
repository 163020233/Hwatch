/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux communication solution \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2026-04-14， Create file. \n
 */
#ifndef UX_COMMUNICATION_H
#define UX_COMMUNICATION_H

#include "errcode.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void ux_commu_init(void);
void ux_commu_deinit(void);
errcode_t ux_commu_send_msg_to_device(uint16_t cmd, uint16_t dst_dev_id, uint8_t *data, uint16_t data_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UX_COMMUNICATION_H */