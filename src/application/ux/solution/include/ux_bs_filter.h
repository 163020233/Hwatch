/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_bs_filter.h
 * Description:  provide ux bs filter header file.
 */
#ifndef UX_BS_FILTER_H
#define UX_BS_FILTER_H

#include "errcode.h"
#include "stddef.h"
#include "stdbool.h"
#include "ux_common_define.h"
#include "ux_bs_sle.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    ux_nv_company_id_t company_id;
    ux_nv_device_type_t device_type;
    ux_device_role_t role;
    uint8_t vendor_info[UX_VENDOR_INFO_MAX_LEN];
    uint8_t vendor_info_len;
    uint8_t reserved[UX_ANNOUNCE_DATA_RESERVED_LEN];
} ux_bs_filter_announce_config_t;

ux_nv_bs_vendor_info_t *bs_filter_context_get_vendor_info(void);

ux_nv_company_id_t *bs_filter_context_get_company_id(void);

ux_nv_device_type_t *bs_filter_context_get_device_type(void);

uint8_t *bs_filter_context_get_channel_type(void);

uint8_t ux_bs_filter_get_whitelist_num(void);

errcode_t ux_bs_filter_check_remote_whitelist(const uint8_t *addr, uint32_t addr_len, bool *flag);

errcode_t ux_bs_filter_clear_remote_whitelist_flag(const uint8_t *addr, uint32_t addr_len);

errcode_t ux_bs_filter_refresh_remote_whitelist_flag(void);

errcode_t ux_bs_filter_build_adv_data(uint8_t adv_type, uint8_t *tlv_data, uint16_t tlv_data_len, uint8_t *actual_len);

errcode_t ux_bs_filter_build_adv_filter_data(uint8_t *filter_data, uint16_t filter_data_len, uint8_t *actual_len);

errcode_t ux_bs_filter_get_announce_config(ux_bs_filter_announce_config_t *config);

errcode_t ux_bs_filter_init(void);

errcode_t ux_bs_filter_deinit(void);

ux_api_device_audio_channel_t ux_bs_filter_get_nv_channel_type(void);

errcode_t ux_bs_filter_set_channel_type(ux_api_device_audio_channel_t channel);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // end UX_BS_FILTER_H
