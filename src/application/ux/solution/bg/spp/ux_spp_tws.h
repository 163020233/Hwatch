/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * File          ux_spp_tws.h
 * Description:  Audio ux spp tws interface
 */

#ifndef __UX_SPP_TWS_H__
#define __UX_SPP_TWS_H__

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

int32_t ux_spp_multi_dev_com_send_data(const void *param, uint16_t length, uint8_t cmd_type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
