/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __SYSCONFIG_H__
#define __SYSCONFIG_H__

#include "vfmw.h"
#include "soc_reg.h"
#include "chip_core_irq.h"

#define SCD_NUM      1
#define VDH_NUM      1
#define MDMA_NUM     0
#define VDH_REG_NUM  8
#define VDH_MSG_NUM  1
#define VDH_OLC_NUM  1 // number of outer loop core per outer loop group
#define VDH_BSP_NUM  VDH_OLC_NUM  // as same as VDVH_OLC_NUM

#define VDH_REGISTER_LEN      (0x10000)
#define VDH_BASE_PHY_ADDR     (0x535C0000)
#define VDM_REG_PHY_ADDR      (VDH_BASE_PHY_ADDR)
#define SCD_REG_PHY_ADDR      (VDH_BASE_PHY_ADDR + 0x9c00)

#define VDH_CRG_REGISTER_LEN  (0x100)
#define VDH_CRG_REG_PHY_ADDR  (VDH_BASE_PHY_ADDR + 0xa000)
#define VDM_CLK_EN_OFFSET     (0x4)
#define VDM_RST_REQ_OFFSET    (0xc)
#define VDM_RST_OK_OFFSET     (0x14)

#define VDM_CLKSEL_PHY_ADDR    (VDH_CRG_REG_PHY_ADDR + 0x04)
#define MEM_CLKSEL_PHY_ADDR    (VDH_CRG_REG_PHY_ADDR + 0x08)
#define VDM_RST_REG_PHY_ADDR   (VDH_CRG_REG_PHY_ADDR + 0x0c)
#define VDM_RST_OK_PHY_ADDR    (VDH_CRG_REG_PHY_ADDR + 0x14)

#define SYS_CRG_REG_PHY_ADDR   VIDEO_CTL_RB_VIDEO_CTL_ID_REG
#define SYS_CRG_REGISTER_LEN   (0x330)
#define SYS_CRG_VO_VDH0_OFFSET (VIDEO_CTL_RB_CRG_VO_VDH0_REG - VIDEO_CTL_RB_VIDEO_CTL_ID_REG)
#define SYS_CRG_VO_VDH1_OFFSET (VIDEO_CTL_RB_CRG_VO_VDH1_REG - VIDEO_CTL_RB_VIDEO_CTL_ID_REG)

#define VDM_PXP_INT_NUM       (VDH_PXP_NORM_IRQN)
#define SCD_INT_NUM           (VDH_SCD_NORM_IRQN)
#define VDH_IRQ_NAME_INDEX    (0)
#define SCD_IRQ_NAME_INDEX    (0)

#define VDH_CLK_MAX           (200)

#define vfmw_align_stride(w)  soc_sys_get_stride(w)

typedef struct {
    td_u32 vdh_all_srst_req     : 1;
    td_u32 vdh_mfde_srst_req    : 1;
    td_u32 vdh_mdma_srst_req    : 1;
    td_u32 vdh_scd_srst_req     : 1;
    td_u32 vdh_bpd_srst_req     : 1;
    td_u32 reserved             : 27;
} vcrg_vdh_srst_req;

typedef struct {
    td_u32 vdh_all_srst_ok      : 1;
    td_u32 vdh_mfde_srst_ok     : 1;
    td_u32 vdh_mdma_srst_ok     : 1;
    td_u32 vdh_scd_srst_ok      : 1;
    td_u32 vdh_bpd_srst_ok      : 1;
    td_u32 reserved             : 27;
} vcrg_vdh_srst_req_ok;

typedef struct {
    td_u32 vdh_all_clken        : 2;
    td_u32 vdh_mfde_clken       : 2;
    td_u32 vdh_pxp_clken        : 2;
    td_u32 vdh_mdma_clken       : 2;
    td_u32 vdh_scd_clken        : 2;
    td_u32 vdh_bpd_clken        : 2;
    td_u32 vdh_bsp_clken        : 2;
    td_u32 reserved             : 18;
} vcrg_vdh_clksel;

typedef struct {
    td_u32 cfg_vo_vdh_clken       : 1;
    td_u32 cfg_vo_soft_rst_vdh_n  : 1;
    td_u32 cfg_vo_vdh_apb_clken   : 1;
    td_u32 reserved               : 29;
} sys_crg_clk_ctrl;

typedef struct {
    td_u32 reserved0              : 1;
    td_u32 cfg_vo_vdh_diven       : 1;
    td_u32 cfg_vo_vdh_load_div_en : 1;
    td_u32 cfg_vo_vdh_div_num     : 5;
    td_u32 cfg_vo_vdh_div_denom   : 5;
    td_u32 cfg_vo_vdh_cksel       : 2;
    td_u32 reserved1              : 17;
} sys_crg_clksel;

typedef struct {
    UADDR sys_crg_phy;
    td_u8 *sys_crg_vir;

    UADDR vdh_crg_phy;
    td_u8 *vdh_crg_vir;

    UADDR vdh_reg_phy;
    td_u8 *vdh_reg_vir;
} vdh_glb_reg_info;

#endif
