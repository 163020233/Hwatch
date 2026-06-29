/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "pdt_master.h"
#include "vfmw_intf.h"
#include "vfmw_sys.h"
#include "vfmw_define.h"
#include "dbg.h"
#include "vfmw_pdt.h"
#include "pdt_com.h"
#include "dec_reg.h"
#ifdef VFMW_LOW_POWER_SUPPORT
#include "pm_pmu_porting.h"
#endif

#define CLOCK_VDH_DIV_1 1
#define CLOCK_VDH_DIV_2 2
#define CLOCK_VDH_DIV_4 4

static vfmw_product_info g_pdt_info = {
    .vdh_clk_max = VDH_CLK_MAX,
};
vfmw_product_info* pdt_get_product_info(td_void)
{
    return &g_pdt_info;
}

td_bool pdt_need_attach_mem(vfmw_vid_std vid_std, mem_need_info *need)
{
    vfmw_unused(vid_std);
    vfmw_unused(need);
    return TD_FALSE;
}

static td_s32 pdt_set_clk(const td_u32 *clk_cfg)
{
    td_u32 i;
    td_u32 clk = 0xff;
    td_u32 tab_clk[] = {
        VDH_CLK_MAX / CLOCK_VDH_DIV_4,
        VDH_CLK_MAX / CLOCK_VDH_DIV_2,
        VDH_CLK_MAX / CLOCK_VDH_DIV_1 }; /* MHz */
    vfmw_product_info *pdt_info = pdt_get_product_info();

    vfmw_check_ret(clk_cfg != TD_NULL, EXT_FAILURE);

    for (i = 0; i < sizeof(tab_clk) / sizeof(tab_clk[0]); i++) {
        if (tab_clk[i] == *clk_cfg) {
            clk = i;
            break;
        }
    }
    vfmw_check_ret(clk != 0xff, EXT_FAILURE);

    pdt_info->vdh_clk_max = *clk_cfg;
    pdt_info->vdh_clk_reg = clk;

    return EXT_SUCCESS;
}

static td_void pdt_enable_apb(td_void)
{
    td_u32 vdh_clk = 0;
    vdh_glb_reg_info *glb_reg = pdt_get_glb_reg();

    vdh_clk = *((volatile td_u32*)(glb_reg->sys_crg_vir + SYS_CRG_VO_VDH0_OFFSET));
    ((sys_crg_clk_ctrl *)(&vdh_clk))->cfg_vo_vdh_apb_clken = 1;
    *((volatile td_u32*)(glb_reg->sys_crg_vir + SYS_CRG_VO_VDH0_OFFSET)) = vdh_clk;
}

static td_void pdt_disable_apb(td_void)
{
    td_u32 vdh_clk = 0;
    vdh_glb_reg_info *glb_reg = pdt_get_glb_reg();

    vdh_clk = *((volatile td_u32*)(glb_reg->sys_crg_vir + SYS_CRG_VO_VDH0_OFFSET));
    ((sys_crg_clk_ctrl *)(&vdh_clk))->cfg_vo_vdh_apb_clken = 0;
    *((volatile td_u32*)(glb_reg->sys_crg_vir + SYS_CRG_VO_VDH0_OFFSET)) = vdh_clk;
}

static td_void pdt_enable_vdh(td_void)
{
    td_u32 div;
    td_u32 cksel;
    td_u32 vdh_clk = 0;
    td_u32 vdh_cksel = 0;
    vdh_glb_reg_info *glb_reg = pdt_get_glb_reg();
    vfmw_product_info *pdt_info = pdt_get_product_info();

    /* set frequency division */
    div = VDH_CLK_MAX / pdt_info->vdh_clk_max;
    if (div == CLOCK_VDH_DIV_1) {
        cksel = 0; /* 0 No frequency division */
    } else if (div == CLOCK_VDH_DIV_2) {
        cksel = 1; /* 1 Frequency division by 2 */
    } else if (div == CLOCK_VDH_DIV_4) {
        cksel = 2; /* 2 Frequency division by 4 */
    } else {
        cksel = 0; /* 0 No frequency division */
    }

    vdh_cksel = *((volatile td_u32*)(glb_reg->sys_crg_vir + SYS_CRG_VO_VDH1_OFFSET));
    ((sys_crg_clksel *)(&vdh_cksel))->cfg_vo_vdh_div_num = 0x1; /* default */
    ((sys_crg_clksel *)(&vdh_cksel))->cfg_vo_vdh_div_denom = 0x1; /* default */
    ((sys_crg_clksel *)(&vdh_cksel))->cfg_vo_vdh_load_div_en = 0; /* default close */
    ((sys_crg_clksel *)(&vdh_cksel))->cfg_vo_vdh_diven = 0; /* default close */
    ((sys_crg_clksel *)(&vdh_cksel))->cfg_vo_vdh_cksel = cksel;
    *((volatile td_u32*)(glb_reg->sys_crg_vir + SYS_CRG_VO_VDH1_OFFSET)) = vdh_cksel;

    vdh_clk = *((volatile td_u32*)(glb_reg->sys_crg_vir + SYS_CRG_VO_VDH0_OFFSET));
    ((sys_crg_clk_ctrl *)(&vdh_clk))->cfg_vo_vdh_clken = 1;
    *((volatile td_u32*)(glb_reg->sys_crg_vir + SYS_CRG_VO_VDH0_OFFSET)) = vdh_clk;

    vdh_clk = *((volatile td_u32*)(glb_reg->sys_crg_vir + SYS_CRG_VO_VDH0_OFFSET));
    ((sys_crg_clk_ctrl *)(&vdh_clk))->cfg_vo_soft_rst_vdh_n = 1;
    *((volatile td_u32*)(glb_reg->sys_crg_vir + SYS_CRG_VO_VDH0_OFFSET)) = vdh_clk;
}

static td_s32 pdt_disable_vdh(td_void)
{
    td_u32 vdh_clk = 0;
    vdh_glb_reg_info *glb_reg = pdt_get_glb_reg();

    vdh_clk = *((volatile td_u32*)(glb_reg->sys_crg_vir + SYS_CRG_VO_VDH0_OFFSET));
    ((sys_crg_clk_ctrl *)(&vdh_clk))->cfg_vo_soft_rst_vdh_n = 0;
    *((volatile td_u32*)(glb_reg->sys_crg_vir + SYS_CRG_VO_VDH0_OFFSET)) = vdh_clk;

    vdh_clk = *((volatile td_u32*)(glb_reg->sys_crg_vir + SYS_CRG_VO_VDH0_OFFSET));
    ((sys_crg_clk_ctrl *)(&vdh_clk))->cfg_vo_vdh_clken = 0;
    *((volatile td_u32*)(glb_reg->sys_crg_vir + SYS_CRG_VO_VDH0_OFFSET)) = vdh_clk;

    return EXT_SUCCESS;
}

static td_s32 pdt_reset_vdh(td_void)
{
    td_s32 i;
    td_s32 ret = EXT_FAILURE;
    td_u8 *vir_addr = TD_NULL;
    td_u32 reg_ofs, rst_req, rst_ok;

    vir_addr = os_kmap_reg(VDH_CRG_REG_PHY_ADDR, 0x20);
    vfmw_check_ret(vir_addr != TD_NULL, EXT_FAILURE);

    reg_ofs = VDM_RST_REG_PHY_ADDR - VDH_CRG_REG_PHY_ADDR;
    rst_req = *(volatile td_u32 *)(vir_addr + reg_ofs);
    ((vcrg_vdh_srst_req *)(&rst_req))->vdh_all_srst_req = 1;
    *(volatile td_u32 *)(vir_addr + reg_ofs) = rst_req;
    os_mb();

    for (i = 0; i < 1000; i++) { /* 1000 :a number */
        reg_ofs = VDM_RST_OK_PHY_ADDR - VDH_CRG_REG_PHY_ADDR;
        rst_ok = *(volatile td_u32 *)(vir_addr + reg_ofs);
        if (((vcrg_vdh_srst_req_ok *)(&rst_ok))->vdh_all_srst_ok == 1) {
            ret = EXT_SUCCESS;
            break;
        }
        os_udelay(1);
    }

    if (i == 1000) { /* 1000 :a number */
        dprint(PRN_FATAL, "reset VDH ERROR!!!\n");
        ret = EXT_FAILURE;
        goto out;
    }

    reg_ofs = VDM_RST_REG_PHY_ADDR - VDH_CRG_REG_PHY_ADDR;
    rst_req = *(volatile td_u32 *)(vir_addr + reg_ofs);
    ((vcrg_vdh_srst_req *)(&rst_req))->vdh_all_srst_req = 0;
    *(volatile td_u32 *)(vir_addr + reg_ofs) = rst_req;
    os_mb();

    for (i = 0; i < 1000; i++) { /* 1000 :a number */
        reg_ofs = VDM_RST_OK_PHY_ADDR - VDH_CRG_REG_PHY_ADDR;
        rst_ok = *(volatile td_u32 *)(vir_addr + reg_ofs);
        if (((vcrg_vdh_srst_req_ok *)(&rst_ok))->vdh_all_srst_ok == 0) {
            ret = EXT_SUCCESS;
            break;
        }
        os_udelay(1);
    }
    if (i == 1000) { /* 1000 :a number */
        dprint(PRN_FATAL, "cancel reset VDH ERROR!!!\n");
        ret = EXT_FAILURE;
    }

out:
    os_kunmap_reg(vir_addr);

    return ret;
}

td_void pdt_init_product_info(td_void)
{
    vfmw_product_info *info = pdt_get_product_info();
    td_u32 vdh_clk_max = info->vdh_clk_max;

    vfmw_check_sec_func(memset_s(info, sizeof(vfmw_product_info), 0, sizeof(vfmw_product_info)));

    info->smmu_support = 0;
    info->tee_support = 0;
    info->vdh_max_frate = 60; /* 60 fps */
    info->vdh_max_width = H264_MAX_PIX_WIDTH;
    info->vdh_clk_max = vdh_clk_max;
    info->pdt_veto.veto_mask = 0;
    info->linear_no_gap_support = TD_TRUE;
    os_spin_lock_init(&info->spin_lock);
    os_spin_lock_init(&info->apb_spin_lock);
}

td_void pdt_deinit_product_info(td_void)
{
    vfmw_product_info *info = pdt_get_product_info();

    os_spin_lock_exit(info->apb_spin_lock);
    os_spin_lock_exit(info->spin_lock);
}

td_s32 pdt_suspend(const td_void *args)
{
    vfmw_unused(args);

    return pdt_disable_vdh();
}

td_s32 pdt_resume(const td_void *args)
{
    td_s32 ret;

    vfmw_unused(args);

    pdt_enable_vdh();

    ret = pdt_reset_vdh(); /* vdh needs to be reset after the clock is turned on. */
    if (ret != EXT_SUCCESS) {
        dprint(PRN_ALWS, "%s reset error!\r\n", __func__);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 pdt_add_sleep_veto(vfmw_veto_id id)
{
#ifdef VFMW_LOW_POWER_SUPPORT
    td_s32 ret;
    td_ulong lock_flag;
    vfmw_product_info *info = pdt_get_product_info();

    if (id >= VFMW_ID_MAX) {
        dprint(PRN_ALWS, "%s id %d error!\r\n", __func__, id);
        return EXT_FAILURE;
    }

    os_spin_lock(info->spin_lock, &lock_flag);

    if (info->pdt_veto.veto_mask == 0) {
        /* vote to power on */
        ret = uapi_pmu_control(PMU_VDH_SUB, PMU_POWER_ON_AND_RUN);
        vfmw_check_goto(ret == EXT_SUCCESS, err0);
        /* open clock */
        pdt_enable_vdh();
        /* vdh needs to be reset after the clock is turned on. */
        if (pdt_reset_vdh() != EXT_SUCCESS) {
            dprint(PRN_ALWS, "%s reset error!\r\n", __func__);
            goto err1;
        }
    }
    info->pdt_veto.veto_mask |= (1 << id);
    info->pdt_veto.count[id]++;

    os_spin_unlock(info->spin_lock, &lock_flag);

    return EXT_SUCCESS;

err1:
    pdt_disable_vdh();
    uapi_pmu_control(PMU_VDH_SUB, PMU_POWER_OFF);
err0:
    os_spin_unlock(info->spin_lock, &lock_flag);
    return EXT_FAILURE;
#else
    vfmw_unused(id);
    return EXT_SUCCESS;
#endif
}

td_s32 pdt_remove_sleep_veto(vfmw_veto_id id)
{
#ifdef VFMW_LOW_POWER_SUPPORT
    td_ulong lock_flag;
    vfmw_product_info *info = pdt_get_product_info();

    if (id >= VFMW_ID_MAX) {
        dprint(PRN_ALWS, "%s id %d error!\r\n", __func__, id);
        return EXT_FAILURE;
    }

    os_spin_lock(info->spin_lock, &lock_flag);

    info->pdt_veto.count[id]--;
    if (info->pdt_veto.veto_mask == 0) {
        os_spin_unlock(info->spin_lock, &lock_flag);
        return EXT_SUCCESS;
    }

    info->pdt_veto.veto_mask &= (~(1 << id));
    if (info->pdt_veto.veto_mask == 0) {
        /* close clock */
        vfmw_check_goto(pdt_disable_vdh() == EXT_SUCCESS, err0);
        /* power off */
        uapi_pmu_control(PMU_VDH_SUB, PMU_POWER_OFF);
    }
    os_spin_unlock(info->spin_lock, &lock_flag);

    return EXT_SUCCESS;
err0:
    os_spin_unlock(info->spin_lock, &lock_flag);
    return EXT_FAILURE;
#else
    vfmw_unused(id);
    return EXT_SUCCESS;
#endif
}

td_s32 pdt_check_power_is_off(td_bool *is_power_off)
{
#ifdef VFMW_LOW_POWER_SUPPORT
    td_s32 id;
    vfmw_product_info *info = pdt_get_product_info();

    if (info->pdt_veto.veto_mask == 0) {
        *is_power_off = TD_TRUE;
    } else {
        *is_power_off = TD_FALSE;
        for (id = 0; id < VFMW_ID_MAX; id++) {
            dprint(PRN_FATAL, "id:%d count:%d\r\n", id, info->pdt_veto.count[id]);
            if (info->pdt_veto.count[id] != 0) {
                pdt_remove_sleep_veto(id); /* force to close power */
            }
        }
    }

    return EXT_SUCCESS;
#else
    *is_power_off = TD_TRUE;
    return EXT_SUCCESS;
#endif
}

td_s32 pdt_add_sleep_apb_veto(vfmw_veto_id id)
{
#ifdef VFMW_LOW_POWER_SUPPORT
    td_ulong lock_flag;
    vfmw_product_info *pdt_info = pdt_get_product_info();

    if (id >= VFMW_ID_MAX) {
        dprint(PRN_ALWS, "%s id %d error!\r\n", __func__, id);
        return EXT_FAILURE;
    }

    os_spin_lock(pdt_info->apb_spin_lock, &lock_flag);

    if (pdt_info->apb_veto_mask == 0) {
        /* vote to open apb */
        pdt_enable_apb();
    }
    pdt_info->apb_veto_mask |= (1 << id);

    os_spin_unlock(pdt_info->apb_spin_lock, &lock_flag);

    return EXT_SUCCESS;
#else
    vfmw_unused(id);
    return EXT_SUCCESS;
#endif
}

td_s32 pdt_remove_sleep_apb_veto(vfmw_veto_id id)
{
#ifdef VFMW_LOW_POWER_SUPPORT
    td_ulong lock_flag;
    vfmw_product_info *pdt_info = pdt_get_product_info();

    if (id >= VFMW_ID_MAX) {
        dprint(PRN_ALWS, "%s id %d error!\r\n", __func__, id);
        return EXT_FAILURE;
    }

    os_spin_lock(pdt_info->apb_spin_lock, &lock_flag);

    if (pdt_info->apb_veto_mask == 0) {
        os_spin_unlock(pdt_info->apb_spin_lock, &lock_flag);
        return EXT_SUCCESS;
    }

    pdt_info->apb_veto_mask &= (~(1 << id));
    if (pdt_info->apb_veto_mask == 0) {
        /* close apb */
        pdt_disable_apb();
    }
    os_spin_unlock(pdt_info->apb_spin_lock, &lock_flag);

    return EXT_SUCCESS;
#else
    vfmw_unused(id);
    return EXT_SUCCESS;
#endif
}

td_s32 pdt_check_apb_is_off(td_bool *is_apb_off)
{
#ifdef VFMW_LOW_POWER_SUPPORT
    td_s32 id;
    vfmw_product_info *pdt_info = pdt_get_product_info();

    if (pdt_info->apb_veto_mask == 0) {
        *is_apb_off = TD_TRUE;
    } else {
        *is_apb_off = TD_FALSE;
        for (id = 0; id < VFMW_ID_MAX; id++) {
            if ((pdt_info->apb_veto_mask & (1 << id)) != 0) {
                pdt_remove_sleep_apb_veto(id); /* force to close apb */
            }
        }
    }

    return EXT_SUCCESS;
#else
    *is_apb_off = TD_TRUE;
    return EXT_SUCCESS;
#endif
}

td_u32 g_row_map_table_y[2][4][16] = { /* 2 :a number 4 :a number 16 :a number */
    {
        {0, 1, 2, 3, 4, 5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 15},
        {12, 13, 14, 15, 8, 9, 10, 11,  0, 1, 2, 3, 4, 5, 6, 7},
        {4, 5, 6, 7, 0, 1, 2, 3,  12, 13, 14, 15, 8, 9, 10, 11},
        {8, 9, 10, 11, 12, 13, 14, 15,  4, 5, 6, 7, 0, 1, 2, 3}
    },
    {
        {8, 9, 10, 11, 12, 13, 14, 15,  0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7,  12, 13, 14, 15, 8, 9, 10, 11},
        {12, 13, 14, 15, 8, 9, 10, 11,  4, 5, 6, 7, 0, 1, 2, 3},
        {4, 5, 6, 7, 0, 1, 2, 3,  8, 9, 10, 11, 12, 13, 14, 15}
    }
};

td_u32 g_row_map_table_uv[2][4][8] = {  /* 2 :a number 4 :a number 8 :a number */
    {
        {0, 1, 2, 3,  4, 5, 6, 7},
        {4, 5, 6, 7,  0, 1, 2, 3},
        {4, 5, 6, 7,  0, 1, 2, 3},
        {0, 1, 2, 3,  4, 5, 6, 7},
    },
    {
        {4, 5, 6, 7,  0, 1, 2, 3},
        {0, 1, 2, 3,  4, 5, 6, 7},
        {0, 1, 2, 3,  4, 5, 6, 7},
        {4, 5, 6, 7,  0, 1, 2, 3},
    }
};

td_u32 g_row_map_table_ynb[4][16] = {
    {0, 1, 2, 3, 4, 5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 15},
    {8, 9, 10, 11, 12, 13, 14, 15,  0, 1, 2, 3, 4, 5, 6, 7},
    {8, 9, 10, 11, 12, 13, 14, 15,  0, 1, 2, 3, 4, 5, 6, 7},
    {0, 1, 2, 3, 4, 5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 15},
};

td_u32 g_row_map_table_uvnb[8] = {
    0, 1, 2, 3, 4, 5, 6, 7
};

static td_u32 pdt_get_10bit_data(const pdt_yuv *yuv, td_u32 yc, td_u32 h, td_u32 w)
{
    td_u32 bit, byte, data;
    td_u8 *src = TD_NULL;
    td_u8 *dst = TD_NULL;

    vfmw_check_ret(yuv != TD_NULL, 0);
    vfmw_check_ret(w < VFMW_UINT_MAX, 0);
    src = yc == 1 ? yuv->luma_vir_addr : yuv->chrom_vir_addr;
    byte = (w * 10) / 0x8; /* 10: bit */
    bit = (w * 10) % 0x8; /* 10: bit */
    dst = src + yuv->image_stride * h + byte;
    data = *(td_u32 *)dst >> bit;

    return data;
}

td_u32 pdt_get_y8b_data(pdt_yuv *yuv, td_u32 h, td_u32 w)
{
    td_u32 data;
    td_u32 stride;
    td_u32 src_idx;
    td_u32 dst_idx;
    td_u32 tilex;
    td_u32 tiley;
    td_u8 *src = TD_NULL;
    td_u8 *dst = TD_NULL;

    vfmw_check_ret(yuv != TD_NULL, 0);

    stride = yuv->image_stride; /* 4 :a number */
    src = yuv->luma_vir_addr;

    if (yuv->image_disp == 1 && yuv->bit_depth > 0x8) {
        data = pdt_get_10bit_data(yuv, 1, h, w);
        data = (data >> 2) & 0xff; /* 2: bit */
        return data;
    }

    if (yuv->mem_format == 0) {
        dst = src + stride * h + w; /* 16 :a number */
    } else {
        tiley = (h / 16) % 2; /* 16 :a number 2 :a number */
        tilex = (w / 64) % 4; /* 64 :a number 4 :a number */
        src_idx = h % 16;     /* 16 :a number */
        dst_idx = g_row_map_table_y[tiley][tilex][src_idx];
        /* 16 :a number 64 :a number 64 :a number 64 :a number 16 :a number */
        dst = src + stride * (h / 16) + dst_idx * 64 + (w / 64) * 64 * 16;
        dst += w % 64; /* 64 :a number */
    }

    data = dst[0];

    return data;
}

td_u32 pdt_get_c8b_data(const pdt_yuv *yuv, td_u32 h, td_u32 w)
{
    td_u32 data;
    td_u32 stride;
    td_u32 src_idx;
    td_u32 dst_idx;
    td_u32 tilex;
    td_u32 tiley;
    td_u8 *src = TD_NULL;
    td_u8 *dst = TD_NULL;
    td_u32 w_tmp = w;

    vfmw_check_ret(yuv != TD_NULL, 0);
    vfmw_check_ret(w < VFMW_INT16_MAX, 0);

    w_tmp = w * 2; /* 2 :a number */
    stride = yuv->image_stride; /* 4 :a number */
    src = yuv->chrom_vir_addr;

    if (yuv->image_disp == 1 && yuv->bit_depth > 0x8) {
        data = pdt_get_10bit_data(yuv, 0, h, w_tmp);
        data = (((data >> 0x2) & 0xff) << 0x8) + ((data >> 12) & 0xff); /* 12: bit */
        return data;
    }

    if (yuv->mem_format == 0) {
        dst = src + stride * h + w_tmp; /* 16 :a number */
    } else {
        tiley = (h / 8) % 2; /* 8 :a number 2 :a number */
        tilex = (w_tmp / 64) % 4; /* 64 :a number 4 :a number */
        src_idx = h % 8; /* 8 :a number */
        dst_idx = g_row_map_table_uv[tiley][tilex][src_idx];
        /* 2 :a number 8 :a number 64 :a number 64 :a number 64 :a number 8 :a number */
        dst = src + (stride / 2) * (h / 8) + dst_idx * 64 + (w_tmp / 64) * 64 * 8;
        dst += w_tmp % 64; /* 64 :a number */
    }

    data = (dst[0] << 8) + dst[1]; /* 8 :a number */

    return data;
}

static td_u32 pdt_get_u8b_data(const pdt_yuv *yuv, td_u32 h, td_u32 w)
{
    return pdt_get_c8b_data(yuv, h, w) & 0xff;
}

static td_u32 pdt_get_v8b_data(pdt_yuv *yuv, td_u32 h, td_u32 w)
{
    return (pdt_get_c8b_data(yuv, h, w) >> 8) & 0xff; /* 8 :a number */
}

td_u32 pdt_get_ynb_data(pdt_yuv *yuv, td_u32 h, td_u32 w)
{
    td_u32 data = 0;
    td_u32 stride;
    td_u32 tile_x;
    td_u8 *src = TD_NULL;
    td_u8 *dst = TD_NULL;
    td_u32 src_idx;
    td_u32 dst_idx;

    vfmw_check_ret(yuv != TD_NULL, 0);

    stride = yuv->image_stride_2bit; /* 5 :a number */
    src = yuv->luma_2bit_vir_addr;

    if (yuv->image_disp == 1 && yuv->bit_depth > 0x8) {
        data = pdt_get_10bit_data(yuv, 1, h, w);
        data = data & 0x3;
        return data;
    }

    tile_x = (w / 64) % 4; /* 64 4 num */
    src_idx = h % 16; /* 16 num */
    dst_idx = g_row_map_table_ynb[tile_x][src_idx];
    dst = src + stride * (h / 16) + (dst_idx * 64 + (w / 64) * 64 * 16) / 4; /* 64 16 4 num */
    dst += (w % 64) >> 2; /* 64 2 num */

    vfmw_check_ret(dst != TD_NULL, 0);
    data = (dst[0] >> ((w % 4) * 2)) & 0x3; /* 4 2 0x3 num */

    return data;
}

td_u32 pdt_get_cnb_data(const pdt_yuv *yuv, td_u32 h, td_u32 w)
{
    td_u32 data;
    td_u32 stride;
    td_u8 *src = TD_NULL;
    td_u8 *dst = TD_NULL;
    td_u32 src_idx;
    td_u32 dst_idx;
    td_u32 w_tmp = w;

    vfmw_check_ret(yuv != TD_NULL, 0);
    vfmw_check_ret(w < VFMW_INT16_MAX, 0);

    w_tmp = w * 2; /* 2: num */
    stride = yuv->image_stride_2bit; /* 4: num */
    src = yuv->chrom_2bit_vir_addr;

    if (yuv->image_disp == 1 && yuv->bit_depth > 0x8) {
        data = pdt_get_10bit_data(yuv, 0, h, w_tmp);
        data = ((data & 0x3) << 0) + (((data >> 10) & 0x3) << 0x2); /* 10: bit */
        return data;
    }

    src_idx = h % 8; /* 8 num */
    dst_idx = g_row_map_table_uvnb[src_idx];
    dst = src + (stride / 2) * (h / 8) + (dst_idx * 64 + (w_tmp / 64) * 64 * 8) / 4; /* 64 8 4 2 num */
    dst += (w_tmp % 64) >> 2; /* 64 2 num */

    data = (dst[0] >> ((w_tmp % 4) * 2)) & 0xf; /* 4 2 num */

    return data;
}

static td_u32 pdt_get_unb_data(const pdt_yuv *yuv, td_u32 h, td_u32 w)
{
    return (pdt_get_cnb_data(yuv, h, w) >> 2) & 3; /* 2 :a number 3 :a number */
}

static td_u32 pdt_get_vnb_data(pdt_yuv *yuv, td_u32 h, td_u32 w)
{
    return pdt_get_cnb_data(yuv, h, w) & 3; /* 3 :a number */
}

td_u32 pdt_gety_data(pdt_yuv *yuv, td_u32 h, td_u32 w)
{
    td_u32 data;

    vfmw_check_ret(yuv != TD_NULL, 0);

    data = pdt_get_y8b_data(yuv, h, w);
    if (yuv->bit_depth > 8) { /* 8 :a number */
        data <<= 2; /* 2 :a number */
        data += pdt_get_ynb_data(yuv, h, w);
    }
    return data;
}

td_u32 pdt_getu_data(const pdt_yuv *yuv, td_u32 h, td_u32 w)
{
    td_u32 data;

    vfmw_check_ret(yuv != TD_NULL, 0);

    data = pdt_get_u8b_data(yuv, h, w);
    if (yuv->bit_depth > 8) { /* 8 :a number */
        data <<= 2; /* 2 :a number */
        data += pdt_get_unb_data(yuv, h, w);
    }

    return data;
}

td_u32 pdt_getv_data(pdt_yuv *yuv, td_u32 h, td_u32 w)
{
    td_u32 data;

    vfmw_check_ret(yuv != TD_NULL, 0);

    data = pdt_get_v8b_data(yuv, h, w);
    if (yuv->bit_depth > 8) { /* 8 :a number */
        data <<= 2; /* 2 :a number */
        data += pdt_get_vnb_data(yuv, h, w);
    }

    return data;
}

static td_void pdt_get_yuv_info(const vfmw_image *img, pdt_yuv *yuv_info)
{
    td_u64 chrom_vir_addr;
    td_u64 luma_2bit_vir_addr = 0;
    td_u64 chrom_2bit_vir_addr = 0;

    if (img->disp_info.linear_en == 1) {
        yuv_info->mem_format = 0;
    } else {
        yuv_info->mem_format = 1;
    }

    yuv_info->bit_depth = img->bit_depth;
    yuv_info->vid_std = img->vid_std;

    yuv_info->image_stride = img->disp_info.y_stride;
    yuv_info->image_stride_2bit = img->disp_info.stride_2bit;
    yuv_info->luma_vir_addr = uint64_ptr(img->disp_info.luma_vir_addr);
    yuv_info->image_disp = img->disp_info.linear_en == 1;

    chrom_vir_addr = img->disp_info.luma_vir_addr + (img->disp_info.chrom_phy_addr -
                                                     img->disp_info.luma_phy_addr);

    if (img->bit_depth > 8 && img->disp_info.luma_vir_addr != 0) { /* 8 :a number */
        luma_2bit_vir_addr = img->disp_info.luma_vir_addr + img->disp_info.luma_phy_addr_2bit -
                             img->disp_info.luma_phy_addr;
        chrom_2bit_vir_addr = luma_2bit_vir_addr + (img->disp_info.chrom_phy_addr_2bit -
                                                    img->disp_info.luma_phy_addr_2bit);
    }

    yuv_info->chrom_vir_addr = uint64_ptr(chrom_vir_addr);
    yuv_info->luma_2bit_vir_addr = uint64_ptr(luma_2bit_vir_addr);
    yuv_info->chrom_2bit_vir_addr = uint64_ptr(chrom_2bit_vir_addr);
    dprint(PRN_ALWS, "%s, mem_format %d, bit_depth %d, image_stride %d, image_stride_2bit %d\n",
        __func__, yuv_info->mem_format, yuv_info->bit_depth, yuv_info->image_stride, yuv_info->image_stride_2bit);
}

td_s32 pdt_yuv_2d(const vfmw_image *img, td_u8 *dst)
{
    td_u32 w, h;
    td_u8 *ydst = dst;
    td_u8 *udst = TD_NULL;
    td_u8 *vdst = TD_NULL;
    td_u32 width, height, ysize, pix_byte;
    pdt_yuv yuv_info;

    vfmw_check_ret(img != TD_NULL && ydst != TD_NULL, EXT_FAILURE);
    width = img->disp_info.disp_width;
    height = img->disp_info.disp_height;

    pix_byte = (img->bit_depth > 8) ? 2 : 1; /* 8 :a number 2 :a number */
    vfmw_check_ret(width * height < VFMW_UINT_MAX, EXT_FAILURE);
    vfmw_check_ret(width * height * pix_byte < VFMW_UINT_MAX, EXT_FAILURE);
    ysize = width * height * pix_byte;

    udst = ydst + ysize;
    vdst = udst + ysize / 4; /* 4 :a number */
    pdt_get_yuv_info(img, &yuv_info);

    if (img->bit_depth <= 8) { /* 8 :a number */
        for (h = 0; h < height; h++) {
            for (w = 0; w < width; w++) {
                ydst[h * width + w] = (td_u8)pdt_gety_data(&yuv_info, h, w);
            }
        }

        for (h = 0; h < height / 2; h++) { /* 2 :a number */
            for (w = 0; w < width / 2; w++) { /* 2 :a number */
                udst[h * width / 2 + w] = (td_u8)pdt_getu_data(&yuv_info, h, w); /* 2 :a number */
                vdst[h * width / 2 + w] = (td_u8)pdt_getv_data(&yuv_info, h, w); /* 2 :a number */
            }
        }
    } else {
        for (h = 0; h < height; h++) {
            for (w = 0; w < width; w++) {
                ((td_u16 *)ydst)[h * width + w] = (td_u16)pdt_gety_data(&yuv_info, h, w);
            }
        }

        for (h = 0; h < height / 2; h++) { /* 2 :a number */
            for (w = 0; w < width / 2; w++) { /* 2 :a number */
                ((td_u16 *)udst)[h * width / 2 + w] = (td_u16)pdt_getu_data(&yuv_info, h, w); /* 2 :a number */
                ((td_u16 *)vdst)[h * width / 2 + w] = (td_u16)pdt_getv_data(&yuv_info, h, w); /* 2 :a number */
            }
        }
    }

    return EXT_SUCCESS;
}

td_s32 pdt_get_attr(attr_id attr_id, td_void *args)
{
    td_s32 ret;

    switch (attr_id) {
        case ATTR_GET_CAP:
            ret = pdt_get_dec_cap((vfmw_cap *)args);
            break;
        case ATTR_GET_SEG_BUF_SIZE:
            ret = pdt_get_seg_buf_size(args);
            break;
        default:
            ret = EXT_FAILURE;
            dprint(PRN_WARN, "%s unkown attr_id %d\n", __func__, attr_id);
            break;
    }

    return ret;
}

td_s32 pdt_set_attr(attr_id attr_id, const td_void *args)
{
    td_s32 ret;

    switch (attr_id) {
        case ATTR_SET_CLK:
            vfmw_check_ret(args != TD_NULL, EXT_FAILURE);
            ret = pdt_set_clk((td_u32 *)args);
            break;

        case ATTR_ENABLE_VDH:
            pdt_enable_vdh();
            pdt_enable_apb();
            ret = EXT_SUCCESS;
            break;

        case ATTR_DISABLE_VDH:
            pdt_disable_apb();
            ret = pdt_disable_vdh();
            break;

        case ATTR_RESET_VDH:
            ret = pdt_reset_vdh();
            break;

        default:
            dprint(PRN_FATAL, "%s:%s\n", __func__, "default");
            ret = EXT_FAILURE;
            dprint(PRN_WARN, "%s unkown attr_id %d\n", __func__, attr_id);
            break;
    }

    return ret;
}
