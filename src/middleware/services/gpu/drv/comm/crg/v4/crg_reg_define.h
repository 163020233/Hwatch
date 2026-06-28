/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description : gfx crg file
 */

#ifndef COMM_CRG_REG_DEFINE_H
#define COMM_CRG_REG_DEFINE_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define VEDIO_CTRL_REG_BASEADDR     0x535D3000

#define CRG_VO_AXI                  (VEDIO_CTRL_REG_BASEADDR + 0x300)
#define CRG_VO_APB                  (VEDIO_CTRL_REG_BASEADDR + 0x304)
#define CRG_VO_PPC                  (VEDIO_CTRL_REG_BASEADDR + 0x308)
#define CRG_VO_VAU                  (VEDIO_CTRL_REG_BASEADDR + 0x30C)
#define CRG_VO_VAU_SOFT_CRG_REQ     (VEDIO_CTRL_REG_BASEADDR + 0x310)
#define CRG_VO_DPU                  (VEDIO_CTRL_REG_BASEADDR + 0x314)
#define CRG_VO_HD0                  (VEDIO_CTRL_REG_BASEADDR + 0x318)
#define CRG_VO_JPGD                 (VEDIO_CTRL_REG_BASEADDR + 0x31C)
#define CRG_VO_JPGD_SOFT_CRG_REG    (VEDIO_CTRL_REG_BASEADDR + 0x328)

#define GPU_CLK_HIGH_SPEED          196   // DPU CLK SPLL 786.432 4分频
#define GPU_CLK_AOD                 98    // DPU LOW SPLL 8分频

#define VAU_CLK_HIGH_SPEED          300   // IPLL 3分
#define VAU_CLK_LOW_SPEED           157   // SPLL 5分

#define DPU_AOD_ROT_PIXEL_CLK       22000   // 低频 压缩3倍关系，加50%余量跑到22 98/3/1.5
#define DPU_NOR_ROT_MAX_PIXEL_CLK   30000   // 30M
#define DPU_NOR_MAX_WORK_CLK        196000  // 1A22 dpu高频的工作时钟时序收敛到196M

/* Define the union crg_vo_axi */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cfg_vo_vau_axi_clken    : 1;       /* [0] */
        unsigned int cfg_vo_dpu_axi_clken    : 1;       /* [1] */
        unsigned int reserved0               : 1;       /* [2] */
        unsigned int reserved1               : 29;      /* [31:3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_crg_vo_axi;

/* Define the union crg_vo_apb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cfg_vo_vau_apb_clken    : 1;       /* [0] */
        unsigned int cfg_vo_dpu_apb_clken    : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_crg_vo_apb;

/* Define the union crg_vo_ppc */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 1;       /* [0] */
        unsigned int reserved1               : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_crg_vo_ppc;

/* Define the union crg_vo_vau */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cfg_vo_soft_rst_vau_dpu_n : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_crg_vo_vau;

/* Define the union crg_vo_vau_soft_crg_req */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cfg_vo_soft_rst_dpu_ppc_req : 1;       /* [0] */
        unsigned int cfg_vo_soft_rst_vau_ppc_req : 1;       /* [1] */
        unsigned int cfg_vo_soft_rst_dpu_req : 1;       /* [2] */
        unsigned int cfg_vo_soft_rst_vau_req : 1;       /* [3] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_crg_vo_vau_soft_crg_req;

/* Define the union crg_vo_dpu */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cfg_vo_dpu_ppc_clken    : 1;       /* [0] */
        unsigned int cfg_vo_dpu_ppc_ckdiv_uniform_clken : 1;       /* [1] */
        unsigned int cfg_vo_dpu_ppc_load_div_en : 1;       /* [2] */
        unsigned int cfg_vo_dpu_ppc_div_num  : 5;       /* [7:3] */
        unsigned int cfg_vo_dpu_ppc_div_denom : 5;       /* [12:8] */
        unsigned int cfg_vo_dpu_cfg_clken    : 1;       /* [13] */
        unsigned int reserved                : 18;      /* [31:14] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_crg_vo_dpu;

/* Define the union crg_vo_hd0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cfg_vo_hd0_clken        : 1;       /* [0] */
        unsigned int cfg_vo_hd0_clkdiv_uniform_clken : 1;       /* [1] */
        unsigned int cfg_vo_vo_hd0_load_div_en : 1;       /* [2] */
        unsigned int cfg_vo_vo_hd0_div_num   : 13;      /* [15:3] */
        unsigned int cfg_vo_vo_hd0_div_denom : 13;      /* [28:16] */
        unsigned int reserved                : 3;       /* [31:29] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_crg_vo_hd0;

/* Define the union crg_vo_jpgd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cfg_vo_jpgd_clken       : 1;       /* [0] */
        unsigned int cfg_vo_soft_rst_jpgd_n  : 1;       /* [1] */
        unsigned int cfg_vo_jpgd_apb_clken   : 1;       /* [2] */
        unsigned int reserved                : 29;      /* [31:3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_crg_vo_jpgd;

/* Define the union crg_vo_vdh0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cfg_vo_vdh_clken        : 1;       /* [0] */
        unsigned int cfg_vo_soft_rst_vdh_n   : 1;       /* [1] */
        unsigned int cfg_vo_vdh_apb_clken    : 1;       /* [2] */
        unsigned int reserved                : 29;      /* [31:3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_crg_vo_vdh0;

/* Define the union crg_vo_vdh1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 1;       /* [0] */
        unsigned int cfg_vo_vdh_diven        : 1;       /* [1] */
        unsigned int cfg_vo_vhd_load_div_en  : 1;       /* [2] */
        unsigned int cfg_vo_vdh_div_num      : 5;       /* [7:3] */
        unsigned int cfg_vo_vdh_div_denom    : 5;       /* [12:8] */
        unsigned int cfg_vo_vdh_cksel        : 2;       /* [14:13] */
        unsigned int reserved1               : 17;      /* [31:15] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_crg_vo_vdh1;

/* Define the union crg_vo_jpgd_soft_crg_req */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cfg_vo_soft_rst_jpgd_req : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_crg_vo_jpgd_soft_crg_req;

/* Define the union crg_vo_hpm */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cfg_vo_soft_rst_hpm_n   : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_crg_vo_hpm;

/* define the union reg_vo_ini_cr */
typedef union {
    struct {
        unsigned int ch_enable : 1;             /* [0] */
        unsigned int clk_sel : 3;               /* [3:1] */
        unsigned int div_en : 1;                /* [4] */
        unsigned int div_num : 4;               /* [8:5] */
        unsigned int load_div_en : 1;           /* [9] */
        unsigned int reserved : 22;             /* [31:10] */
    } bits;

    unsigned int u32;
} reg_vo_ini_cr;

/* define the union reg_router_ch0 */
typedef union {
    struct {
        unsigned int ch_enable : 1;             /* [0] */
        unsigned int clk_sel : 3;               /* [3:1] */
        unsigned int div_en : 1;                /* [4] */
        unsigned int div_num : 4;               /* [8:5] */
        unsigned int load_div_en : 1;           /* [9] */
        unsigned int reserved : 22;             /* [31:10] */
    } bits;

    unsigned int u32;
} reg_router_ch0;

/* define the union reg_router_ch1 */
typedef union {
    struct {
        unsigned int ch_enable : 1;             /* [0] */
        unsigned int clk_sel : 3;               /* [3:1] */
        unsigned int div_en : 1;                /* [4] */
        unsigned int div_num : 4;               /* [8:5] */
        unsigned int load_div_en : 1;           /* [9] */
        unsigned int reserved0 : 2;             /* [11:10] */
        unsigned int ch_sel_sts : 2;            /* [13:12] */
        unsigned int reserved1 : 18;            /* [31:14] */
    } bits;

    unsigned int u32;
} reg_router_ch1;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* COMM_CRG_REG_DEFINE_H */
