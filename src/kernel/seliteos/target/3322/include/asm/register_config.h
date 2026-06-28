/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: 3322 register config header for SELiteOS.
 */

#ifndef _ASM_REGISTER_CONFIG_H
#define _ASM_REGISTER_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TEE_CTRL_BASE_ADDR              0x52018000
#define TCM_APC_EN                      (TEE_CTRL_BASE_ADDR + 0x0030)
#define ITCM_APC_ADDR                   (TEE_CTRL_BASE_ADDR + 0x0034)
#define DTCM_APC_ADDR                   (TEE_CTRL_BASE_ADDR + 0x0038)
#define ITCM_APC_ST_WR_L                (TEE_CTRL_BASE_ADDR + 0x003C)
#define ITCM_APC_ST_WR_H                (TEE_CTRL_BASE_ADDR + 0x0040)
#define ITCM_APC_ST_RD_L                (TEE_CTRL_BASE_ADDR + 0x0044)
#define ITCM_APC_ST_RD_H                (TEE_CTRL_BASE_ADDR + 0x0048)
#define DTCM_APC_ST_WR_L                (TEE_CTRL_BASE_ADDR + 0x004C)
#define DTCM_APC_ST_WR_H                (TEE_CTRL_BASE_ADDR + 0x0050)
#define DTCM_APC_ST_RD_L                (TEE_CTRL_BASE_ADDR + 0x0054)
#define DTCM_APC_ST_RD_H                (TEE_CTRL_BASE_ADDR + 0x0058)

#define TZPC_PROT_CFG                   (TEE_CTRL_BASE_ADDR + 0x005C)

#define TEE_START_PC_LOCK_CFG           (TEE_CTRL_BASE_ADDR + 0x0060)
#define TEE_START_PC_H                  (TEE_CTRL_BASE_ADDR + 0x0064)
#define TEE_START_PC_L                  (TEE_CTRL_BASE_ADDR + 0x0068)

#define SOFT_SOC_TEE_ENABLE_ADDR           0x52018020

#define TEE_CTL_RB_TEE_DEEPSLEEP_VOTE_REG  0x52018084
#define TEE_CTL_RB_TEE_START_PC_LOCK_REG   0x52018060
#define TEE_CTL_RB_TEE_START_PC_H_REG      0x52018064
#define TEE_CTL_RB_TEE_START_PC_L_REG      0x52018068

#define COM_CTL_RB_C_CRG_CLKEN_REG         0x55000604
#define COM_CTL_RB_GLB_CLKEN_REG           0x55000608
#define COM_CTL_RB_SEC_CR_REG              0x55000630
#define COM_CTL_RB_XIP_QSPI_CR_REG         0x55000634

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASM_REGISTER_CONFIG_H */

