/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux bs at service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-13， Create file. \n
 */
#ifndef UX_AT_BS_H
#define UX_AT_BS_H

#include "stdint.h"
#include "errcode.h"
#include "at.h"
#include "ux_at_manager.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AT_BS_PROTOCOL "protocol"
#define AT_BS_ROLE "role"
#define AT_BS_NAME "name"
#define AT_BS_EARSIDE "earside"
#define AT_SLE_RATE_LEVEL "rate"
#define AT_SLE_LINK_QUALITY_PARAM "link_quality"
#define AT_BS_VENDOR_INFO "vendor_info"
#define AT_BS_BT_ADDR "btaddr"
#define AT_BS_BT_PEER_ADDR "btpeeraddr"
#define AT_BS_SLE_ADDR "sleaddr"
#define AT_BS_SLE_PEER_ADDR "slepeeraddr"
#define AT_BS_BLE_ADDR "bleaddr"
#define AT_BS_BLE_PEER_ADDR "blepeeraddr"
#define AT_BS_BLE_NAME "blename"
#define AT_BS_SLE_CDS_KEY "cds_key"
#define UNKNOWN_PROTOCOL "unknown protocol"


/*
    AT^BSCFG=protocol,x
    AT^BSCFG=role,x
    AT^BSCFG=name,xxx
*/
at_ret_t at_bs_set_process(const ux_at_para3_args_t *args);

/*
    AT^BSCFG?=role
    AT^BSCFG?=protocol
    AT^BSCFG?=name
*/
at_ret_t at_bs_query_process(const ux_at_para3_args_t *args);

/*
    AT^BSCFG?=
*/
at_ret_t at_bs_read_process(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
