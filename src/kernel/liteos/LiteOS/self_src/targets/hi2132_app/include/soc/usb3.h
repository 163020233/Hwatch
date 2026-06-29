/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Usb Header File
 * Author: Huawei LiteOS Team
 * Create: 2023-09-15
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#ifndef _SOC_USB_H
#define _SOC_USB_H

#include "los_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CONFIG_USBUDC_REG_BASE_ADDRESS 0x38000000

#define USB_CACHE_ALIGN_SIZE 32
#define SKB_DATA_ALIGN(X) LOS_Align((X), USB_CACHE_ALIGN_SIZE)

#define DWC_USB_PORT1_BASE_ADDR 0x38000000
#define DWC_USB_PORT1_ADDR_OFFSET 0x40000

// *********************************************************************
// U2DEV Mode
// *********************************************************************
#define DIP_C2X_6D5T
#define INI_DELTA_PN_GEN2 66
#define INI_DELTA_PN_GEN1 66
#define INI_DELTA_CN_NORM 60
#define INI_DELTA_INT_NORM 0
#define INI_DELTA_INT_SUSPEND 4
#define INI_DELTA_PN_SUSPEND 40000
#define INI_DELTA_CN_SUSPEND 24615
#define INI_HOST_TIME_125_THLD 3249
#define INI_CFG_APP_CLK_DIVNUM 2
#define INI_CFG_U2PL_GEN2_CLK_DIVNUM 4
#define INI_CFG_U2PL_GEN1_CLK_DIVNUM 2
#define INI_CFG_U3PL_GEN2_CLK_DIVNUM 1
#define INI_CFG_U3PL_GEN1_CLK_DIVNUM 1
#define INI_TRPU_HOST_SSPRD_TRBNUM 320
#define INI_TRPU_HOST_SSNPRD_TRBNUM 64
#define INI_TRPU_HOST_HSPRD_TRBNUM 192
#define INI_TRPU_HOST_HSNPRD_TRBNUM 64
#define INI_TRPU_HOST_FSPRD_TRBNUM 88
#define INI_TRPU_HOST_FSNPRD_TRBNUM 32
#define INI_MAU_WAIT_IDLE_CNT_PERIOD_L 66
#define INI_MAU_WAIT_IDLE_CNT_PERIOD_H 66
#define INI_PM_1US_UNIT_SUSPEND 13
#define INI_PM_1US_UNIT_GEN1 66
#define INI_PM_1US_UNIT_GEN2 66
#define INI_CSR_UX_EXIT_DET_LFPS_AUX 8
#define INI_INT_CNT_PARA_GEN1 17
#define INI_INT_CNT_PARA_GEN2 17
#define INI_CSR_SUS_HIGH_US_TH 26
#define INI_CSR_SUS_LOW_US_TH 26
#define INI_PHY_SEL_MODE 0
#define INI_USBC_U3_EN 0
#define INI_U2_VBUS_DET_WINDOW 129999
#define INI_U3_VBUS_DET_WINDOW 129999
#define INI_IN_SSP_MF_DP_PBYTE_NUM 125576
#define INI_OUT_SSP_MF_DP_PBYTE_NUM 124730
#define INI_IN_SS_MF_DP_PBYTE_NUM 52809
#define INI_OUT_SS_MF_DP_PBYTE_NUM 51456
#define INI_CRPU_TOPCSR_BST_MIN_PSZ_SSP_INT_OUT 1024
#define INI_CRPU_TOPCSR_BST_MIN_PSZ_SSP_ISO_OUT 371
#define INI_CRPU_TOPCSR_BST_MIN_PSZ_SSP_INT_IN 1024
#define INI_CRPU_TOPCSR_BST_MIN_PSZ_SSP_ISO_IN 1024
#define INI_CRPU_TOPCSR_BST_MIN_PSZ_SS_INT_OUT 1024
#define INI_CRPU_TOPCSR_BST_MIN_PSZ_SS_ISO_OUT 348
#define INI_CRPU_TOPCSR_BST_MIN_PSZ_SS_INT_IN 0
#define INI_CRPU_TOPCSR_BST_MIN_PSZ_SS_ISO_IN 1024
#define INI_CRPU_TOPCSR_BST_MIN_PSZ_SS_SBI_INT_OUT 1024
#define INI_CRPU_TOPCSR_BST_MIN_PSZ_SS_SBI_ISO_OUT 539
#define INI_CRPU_TOPCSR_BST_MIN_PSZ_SS_SBI_INT_IN 0
#define INI_CRPU_TOPCSR_BST_MIN_PSZ_SS_SBI_ISO_IN 0
#define INI_CSR_SE0RST_DET_AUX 650
#define INI_CSR_RMW_WK_UP_DCT_U3_TH 51
#define INI_CSR_RMW_WK_UP_DCT_U2_TH 51

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SOC_USB_H */
