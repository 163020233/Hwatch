/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: platform atcmd header file \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-05-26, Create file. \n
 */
#ifndef PLAT_ATCMD_H
#define PLAT_ATCMD_H

#include "errcode.h"
#include "at.h"
#include "dut.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup dut_platform_command
 * @ingroup
 * @{
 */

typedef struct {
    uint32_t                para_map;
    const uint32_t          para1;
} plt_u32_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           para1;
} ftm_args_t;

typedef struct {
    uint32_t                para_map;
    const char             *para1; /*    Length: 16 */
} bsn_args_t;

typedef struct {
    uint32_t                para_map;
    const char             *para1;
    const char             *para2;
    const uint32_t          para3;
} uart_cfg_args_t;

/* AT+FTM=1 */
at_ret_t plat_atcmd_set_ftm(const ftm_args_t *args);
/* AT+FTM? */
at_ret_t plat_atcmd_check_ftm(void);
/* AT+DIEID? */
at_ret_t plat_atcmd_get_dieid(void);
/* AT+BSN=123456789ABCDEF */
at_ret_t plat_atcmd_set_bsn(const bsn_args_t *args);
/* AT+BSN? */
at_ret_t plat_atcmd_get_bsn(void);
/* AT+HARDWAREID=12345678 */
at_ret_t plat_atcmd_set_hardware_id(const plt_u32_args_t *args);
/* AT+HARDWAREID? */
at_ret_t plat_atcmd_get_hardware_id(void);
/* AT+SHIPMODE */
at_ret_t plat_atcmd_shipmode(void);
/* AT+BATINFO? */
at_ret_t plat_atcmd_batinfo(void);
/* AT+BOXINFO? */
at_ret_t plat_atcmd_boxinfo(void);
/* AT+RESET */
at_ret_t plat_atcmd_reset(void);
/* AT+RESET=0 */
at_ret_t plat_atcmd_delay_reset(const plt_u32_args_t *args);
/* AT+SYSINFO */
at_ret_t plat_atcmd_sysinfo(void);
/* AT+UART=L0,LOG,921600 */
at_ret_t plat_atcmd_uart_set(const uart_cfg_args_t *args);
/* AT+UART? */
at_ret_t plat_atcmd_uart_get(void);
/* AT+VERSION */
at_ret_t version_atcmd_get_version(void);
/* AT+SDKVERSION */
at_ret_t version_atcmd_get_sdk_version(void);
/* AT+LOGCNT */
at_ret_t plat_atcmd_logcnt_info(void);
at_ret_t plat_atcmd_logcnt_en(const plt_u32_args_t *args);

/* AT^CHARGEREG? */
at_ret_t plat_charger_reg_dump(void);

/* AT^BUTTONINFO? */
at_ret_t plat_atcmd_get_button_info(void);


errcode_t platform_at_cmd_init(void);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
