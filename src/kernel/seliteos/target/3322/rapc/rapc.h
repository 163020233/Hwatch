/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description: rapc header file for SELiteOS.
 */

#ifndef RAPC_H
#define RAPC_H
#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef union {
    struct {
        uint16_t region_apc_en       : 1;  // [0]
        uint16_t itcm_apc_enhance_en : 1;  // [1]
        uint16_t reserved            : 14; // [15:2]
    } bits;
    uint16_t u16;
} tcm_apc_en_reg_t;

typedef union {
    struct {
        uint16_t tcm_start_addr     : 5; // [4:0]
        uint16_t tcm_end_addr       : 5; // [9:5]
        uint16_t reserved1          : 2; // [11:10]
        uint16_t tcm_apc_wr_err_sts : 1; // [12]
        uint16_t tcm_apc_rd_err_sts : 1; // [13]
        uint16_t tcm_apc_err_clr    : 1; // [14]
        uint16_t reserved           : 1; // [15]
    } bits;
    uint16_t u16;
} tcm_apc_addr_reg_t;

#ifdef TZPC_PROTECT_CONFIG
typedef union {
    struct {
        uint16_t spi0_tzpc_prot_cfg   : 1;  // [0]
        uint16_t spi0_prot_check_fail : 1;  // [1]
        uint16_t spi1_tzpc_prot_cfg   : 1;  // [2]
        uint16_t spi1_prot_check_fail : 1;  // [3]
        uint16_t efuse_tzpc_prot_cfg   : 1; // [4]
        uint16_t efuse_prot_check_fail : 1; // [5]
        uint16_t reserved             : 10; // [15:6]
    } bits;
    uint16_t u16;
} tzpc_prot_cfg_reg_t;

void tzpc_set_tee_ctrl(bool flag);
#endif

void rapc_set_tee_ctrl(bool flag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RAPC_H */

