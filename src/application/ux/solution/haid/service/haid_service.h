/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ux hearing aid service
 * Author: AuthorNameMagicTag
 * Create: 2023-02-24
 */

#ifndef HAID_SERVICE_H
#define HAID_SERVICE_H

#include "stdint.h"
#include "diag_service.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void haid_service_init(void);
errcode_t haid_business_msg_handle(uint8_t event, uint8_t *data, uint16_t data_len);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
