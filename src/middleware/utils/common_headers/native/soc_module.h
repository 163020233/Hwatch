/*
 * Copyright (C) HiSilicon (Shanghai) Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: Module ID definition.
 */

#ifndef __SOC_MODULE_H__
#define __SOC_MODULE_H__

#define SOC_INVALID_MODULE_ID (0xffffffff)

typedef enum {
    /* common */ /* CNcomment: 系统通用模块 */
    SOC_ID_RESERVED = 0x00,
    SOC_ID_LIBC = 0x01,
    SOC_ID_BSP = 0x02,
    SOC_ID_SYS = 0x03,
    SOC_ID_MAIN = 0x04,

    /* Peripheral */ /* CNcomment: 外设相关模块 */
    SOC_ID_VAU = 0x05,
    SOC_ID_DPU = 0x06,
    SOC_ID_AIDSP = 0x07,
    SOC_ID_CGRA = 0x08,
    SOC_ID_VDEC = 0x09,
    SOC_ID_VENC = 0x0A,
    SOC_ID_VIU = 0x0B,
    SOC_ID_VB = 0x0C,
    SOC_ID_SCM = 0x0D,
    SOC_ID_ISP = 0x0E,
    SOC_ID_VGS = 0x10,
    SOC_ID_GDC = 0x11,
    SOC_ID_PPU = 0x12,
    SOC_ID_DIS = 0x13,
    SOC_ID_MIPI_RX = 0x14,
    SOC_ID_CIS = 0x15,
    SOC_ID_FSD = 0x16,
    SOC_ID_PIPELINE = 0x17,
    SOC_ID_GYRODIS = 0x18,
    SOC_ID_MOTIONFUSION = 0x19,

    /* Component */ /* CNcomment: 组件相关模块 */
    DIAG_MOD_ID_WIFI_HOST               = 0x00,
    DIAG_MOD_ID_WIFI_DEVICE             = 0x00,
    DIAG_MOD_ID_OAM_HOST                = 0x00,

    DIAG_MOD_ID_PLC_NM                  = 0x20,

    SOC_ID_MAX = 0x3F
} soc_mod_id;

#endif /* __SOC_MODULE_H__ */

