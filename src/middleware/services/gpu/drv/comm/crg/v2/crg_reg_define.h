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

#define VEDIO_CTRL_REG_BASEADDR     0x520C0000
#define CRG_VEDIO_JPGD_CLKEN        (VEDIO_CTRL_REG_BASEADDR + 0x14)
#define CRG_VEDIO_CRG_CKEN_CTL      (VEDIO_CTRL_REG_BASEADDR + 0x3C)
#define CRG_VEDIO_DPU_V0_HD0_DIV1   (VEDIO_CTRL_REG_BASEADDR + 0x44)
#define CRG_VEDIO_DPU_V0_HD0_DIV2   (VEDIO_CTRL_REG_BASEADDR + 0x48)
#define CRG_VEDIO_DPU_PPC_DIV       (VEDIO_CTRL_REG_BASEADDR + 0x4C)
#define CRG_VEDIO_SRST_REG          (VEDIO_CTRL_REG_BASEADDR + 0x64)

#define GPU_CLK_SRC_PLL             800
#define GPU_CLK_HIGH_SPEED          400
#define GPU_CLK_AOD                 64
#define VAU_CLK_HIGH_SPEED          200   /* 不支持 */
#define VAU_CLK_LOW_SPEED           200   /* 不支持 */

#define DPU_AOD_ROT_PIXEL_CLK       10000 // AOD 旋转压缩场景固定pixel_clk 10M
#define DPU_AOD_ROT_WORK_CLK        64000 // 64M
#define DPU_NOR_ROT_MAX_PIXEL_CLK   20000 // 20M
#define DPU_NOR_MAX_WORK_CLK        133333 // 1A21 dpu工作时钟时序只收敛到133M

/* define the union reg_jpgd_clken */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int jpgd_cken                   : 1;       /* [0] */
        unsigned int jpgd_soft_rst_req           : 1;       /* [1] */
        unsigned int reserved                    : 14;      /* [15:2] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} reg_jpgd_clken;

/* define the union reg_video_crg_clken */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int cfg_video_jpgd_ppc_clken    : 1;       /* [0] */
        unsigned int cfg_video_jpgd_apb_clken    : 1;       /* [1] */
        unsigned int cfg_video_jpgd_axi_clken    : 1;       /* [2] */
        unsigned int cfg_video_dpu_ppc_hd0_clken : 1;       /* [3] */
        unsigned int cfg_video_dpu_cfg_clken     : 1;       /* [4] */
        unsigned int cfg_video_dpu_vo_hd0_clken  : 1;       /* [5] */
        unsigned int cfg_video_dpu_apb_clken     : 1;       /* [6] */
        unsigned int cfg_video_dpu_axi_clken     : 1;       /* [7] */
        unsigned int cfg_video_vau_ppc_clken     : 1;       /* [8] */
        unsigned int cfg_video_vau_apb_clken     : 1;       /* [9] */
        unsigned int cfg_video_vau_axi_clken     : 1;       /* [10] */
        unsigned int cfg_video_vdh_clken         : 1;       /* [11] */
        unsigned int cfg_video_dsp_sub_clken     : 1;       /* [12] */
        unsigned int reserved                    : 3;       /* [15:13] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} reg_video_crg_clken;

/* define the union reg_video_crg_vo_hd0_div1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int cfg_video_vo_hd0_div_num    : 13;      /* [12:0] */
        unsigned int cfg_video_vo_hd0_load_div_en : 1;       /* [13] */
        unsigned int reserved                    : 2;       /* [15:14] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} reg_video_crg_vo_hd0_div1;

/* define the union reg_video_crg_vo_hd0_div2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int cfg_video_vo_hd0_div_denom  : 13;      /* [12:0] */
        unsigned int reserved                    : 3;       /* [15:13] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} reg_video_crg_vo_hd0_div2;

/* define the union reg_video_crg_ppc_div */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int cfg_video_dpu_ppc_div_denom : 4;       /* [3:0] */
        unsigned int cfg_video_dpu_ppc_load_div_en : 1;       /* [4] */
        unsigned int reserved                    : 11;      /* [15:5] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} reg_video_crg_ppc_div;

/* define the union reg_video_crg_soft_rst */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int cfg_video_soft_rst_off_decod_n : 1;       /* [0] */
        unsigned int cfg_video_soft_rst_sys_n    : 1;       /* [1] */
        unsigned int reserved                    : 14;      /* [15:2] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} reg_video_crg_soft_rst;

/* define the union reg_mipitx_crg_cken_srst_req */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int cfg_video_mipitx_apb_cken   : 1;       /* [0] */
        unsigned int cfg_video_mipitx_apb_srst_req : 1;       /* [1] */
        unsigned int reserved                    : 14;      /* [15:2] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} reg_mipitx_crg_cken_srst_req;

/* define the union reg_vau_dpu_srst_req */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int vau_srst_req                : 1;       /* [0] */
        unsigned int dpu_srst_req                : 1;       /* [1] */
        unsigned int reserved                    : 14;      /* [15:2] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} reg_vau_dpu_srst_req;

/* define the union reg_dpu_cr */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int ch_enable : 1;             /* [0] */
        unsigned int clk_sel : 3;               /* [3:1] */
        unsigned int div_en : 1;                /* [4] */
        unsigned int div_num : 4;               /* [8:5] */
        unsigned int reserved : 7;              /* [15:9] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} reg_dpu_cr;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* COMM_CRG_REG_DEFINE_H */
