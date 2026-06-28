/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __SYSCONFIG_H__
#define __SYSCONFIG_H__

#include "vfmw.h"
#include "chip_core_irq.h"

#define SCD_NUM      1
#define VDH_NUM      1
#define MDMA_NUM     0
#define VDH_REG_NUM  1
#define VDH_MSG_NUM  1
#define VDH_OLP_NUM  1
#define VDH_OLG_NUM  1 // number of outer loop group
#define VDH_OLC_NUM  1 // number of outer loop core per outer loop group
#define VDH_BSP_NUM  VDH_OLC_NUM  // as same as VDVH_OLC_NUM
#define VDH_PXP_NUM  1
#define FPGA_PXP_NUM 1

#define VDH_REGISTER_LEN      (0x1000)
#define VDH_BASE_PHY_ADDR     (0x520B0000)
#define VDM_REG_PHY_ADDR      (VDH_BASE_PHY_ADDR)
#define SCD_REG_PHY_ADDR      (VDH_BASE_PHY_ADDR + 0xD00)

#define VDH_CRG_REGISTER_LEN  (0x100)
#define VDH_CRG_REG_PHY_ADDR  (0x520C0000)
#define VDM_RST_REQ_OFFSET    (0x18)
#define VDM_RST_OK_OFFSET     (0x1c)
#define VDM_BUSY_OFFSET       (0x20)
#define VDM_CLK_EN_OFFSET     (0x3c)
#define VDM_DIV_OFFSET        (0x40)

#define VDM_PXP_INT_NUM       (VDH_NORM_IRQN)
#define SCD_INT_NUM           (SCD_NORM_IRQN)
#define VDH_IRQ_NAME_INDEX    (0)
#define SCD_IRQ_NAME_INDEX    (0)

#define VDH_CLK_MAX           (100)

#define vfmw_align_stride(w) soc_sys_get_stride(w)

typedef struct {
    td_u32 vdh_bpd_srst_req : 1;
    td_u32 vdh_mfd_srst_req : 1;
    td_u32 vdh_scd_srst_req : 1;
    td_u32 vdh_all_srst_req : 1;
    td_u32                  : 28;
} vcrg_vdh_srst_req;

typedef struct {
    td_u32 vdh_bpd_srst_ok : 1;
    td_u32 vdh_mfd_srst_ok : 1;
    td_u32 vdh_scd_srst_ok : 1;
    td_u32 vdh_all_srst_ok : 1;
    td_u32                 : 28;
} vcrg_vdh_srst_req_ok;

typedef struct {
    td_u16 vdh_busy : 1;
    td_u16          : 15;
} vcrg_vdh_busy;

typedef struct {
    td_u16                     : 11;
    td_u16 cfg_video_vdh_clken : 1;
    td_u16                     : 4;
} vcrg_vdh_clk_en;

typedef struct {
    td_u16 cfg_video_vdh_div_denom   : 3;
    td_u16 cfg_video_vdh_load_div_en : 1;
    td_u16 cfg_video_vdh_div_en      : 1;
    td_u16                           : 11;
} vcrg_vdh_div;

typedef struct {
    UADDR sys_crg_phy;
    td_u8 *sys_crg_vir;

    UADDR vdh_crg_phy;
    td_u8 *vdh_crg_vir;

    UADDR vdh_reg_phy;
    td_u8 *vdh_reg_vir;
} vdh_glb_reg_info;

#endif
