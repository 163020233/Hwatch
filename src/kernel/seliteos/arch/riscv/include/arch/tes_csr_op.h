/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * filename:tes_csr_op.c
 * Author: @CompanyNameTag
 * Create:2025-05-10
 * Description: the header of operations of csr in seliteos
 *
 */

#ifndef __TES_CSR_OP_H__
#define __TES_CSR_OP_H__

#include <stdint.h>

#define TMESCR_EME_ENABLE_BIT       1 << 0
#define TMESCR_ETE_ENABLE_BIT       1 << 1
#define TMESCR_TVL_ENABLE_BIT       1 << 2
#define TMESCR_UFSRO_ENABLE_BIT     1 << 3
#define TMESCR_PAD_ENABLE_BIT       1 << 4
#define TMESCR_UVSRO_ENABLE_BIT     1 << 5
#define TMESCR_TSPSW_EXP_ENABLE_BIT 1 << 6
#define TMESCR_TSPSW_INT_ENABLE_BIT 1 << 7
#define TMESCR_TCE_ENABLE_BIT       1 << 8
#define TMESCR_UUDIS_ENABLE_BIT     1 << 9

void set_tmescr(void);

#endif
