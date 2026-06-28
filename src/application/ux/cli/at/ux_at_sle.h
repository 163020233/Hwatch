/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux sle at service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-13， Create file. \n
 */
#ifndef UX_AT_SLE_H
#define UX_AT_SLE_H

#include "stdint.h"
#include "errcode.h"
#include "at.h"
#include "ux_at_manager.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AT_SLE_MODE "mode"
#define AT_SLE_GRANT_ROLE "grant"
#define AT_SLE_TERMINAL_ROLE "terminal"
#define AT_SLE_ADDR "addr"
#define AT_SLE_WHITELIST_ADDR "whitelist_addr"
#define AT_SLE_GRANT_ADDR "grant_addr"
#define AT_SLE_TERMINAL_ADDR "terminal_addr"
#define AT_SLE_PAIR "pair"
#define AT_SLE_WHITELIST_PAIR "whitelist_pair"
#define AT_SLE_UNPAIR "unpair"
#define AT_SLE_CONNECT "connect"
#define AT_SLE_DISCONNECT "disconnect"
#define AT_SLE_PAIRINFO "pair_info"
#define AT_SLE_PROTOCOL "protocol"
#define AT_SLE_ROLE "role"
#define AT_SLE_RATE_LEVEL "rate"
#define AT_SLE_PEER_ADDR "peeraddr"
#define AT_SLE_TRX "trx"
#define AT_SLE_NO_PAIR_DEVICE "no_paired_devices"
#define AT_SLE_COMMON_ERROR_LOG "sle_at_error"
#define AT_SLE_CHANNEL "channel"

#define SLE_NO_PAIR_STR "no pair"
#define SLE_PAIRING_STR "pairing"
#define SLE_PAIRED_STR "paired"

#define AT_PAIR_INFO_STR_LEN 72

typedef enum {
    UX_BS_ADDR_LOCAL,
    UX_BS_ADDR_REMOTE,
    UX_BS_ADDR_MAX,
} ux_at_bs_addr_type;

/*
    AT^SLECFG=mode,x
    AT^SLECFG=addr,xxxxxxxx
    AT^SLECFG=whitelist_addr,xxxxxxxx
    AT^SLECFG=grant_addr,xxxxxxxx
    AT^SLECFG=terminal_addr,n,xxxxxxxx
    AT^SLECFG=pair
*/
at_ret_t at_sle_set_process(const ux_at_para3_args_t *args);

/*
    AT^SLECFG?=mode
    AT^SLECFG?=addr
    AT^SLECFG?=whitelist_addr
    AT^SLECFG?=grant_addr
    AT^SLECFG?=terminal_addr
    AT^SLECFG?=pair_info
*/
at_ret_t at_sle_query_process(const ux_at_para3_args_t *args);

errcode_t at_slecfg_read_protocol_common(uint8_t *name, uint16_t name_str_max_len);
errcode_t at_slecfg_read_sleaddr_common(uint8_t *sleaddr, uint16_t addr_len);
errcode_t at_slecfg_read_peeraddr_common(uint8_t *peeraddr, uint16_t addr_len);
errcode_t at_slecfg_read_work_role_common(uint8_t *name, uint16_t name_str_max_len);
errcode_t ux_at_sle_set_addr(uint8_t *data, uint16_t data_len, ux_at_bs_addr_type addr_type);
at_ret_t ux_get_device_sle_cfg_string(char *result, uint16_t result_size);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
