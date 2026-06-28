/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * File          ux_secure.h
 * Description:  Audio ux common secure interface
 */

#ifndef __UX_SECURE_H__
#define __UX_SECURE_H__

#include <autoconf.h>
#include <los_typedef.h>
#include "errcode.h"
#include "ux_config_common.h"
#ifndef DEMO_WATCH
#include "key_struct.h"
#include "ssf_persistence.h"
#else
#include "common.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

int32_t ux_encrypt_linkkey(UINT8* link_key, UINT8* enc_link_key, int32_t index,
    const uint8_t *pdl_work_key, uint32_t work_key_len);
int32_t ux_decrypt_and_check_link_key(UINT8* enc_link_key, UINT8* dec_link_key, int32_t index,
    const uint8_t *pdl_work_key, uint32_t work_key_len);
int32_t ux_encrypt_bt_addr(UINT8* bt_addr, UINT8* enc_bt_addr, int32_t index,
    const uint8_t *pdl_work_key, uint32_t work_key_len);
int32_t ux_decrypt_and_check_addr(UINT8* bt_addr, int32_t index, const uint8_t *pdl_work_key, uint32_t work_key_len);
uint32_t ux_decrypt_link_key_and_bt_addr(bt_nv_data_pdl_s_t *pd_data, int32_t index,
    const uint8_t *pdl_work_key, uint32_t work_key_len);
uint32_t ux_decrypt_link_key_and_sle_addr(bs_sle_nv_obj_data_t *pd_data, int32_t index,
    const uint8_t *pdl_work_key, uint32_t work_key_len);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
