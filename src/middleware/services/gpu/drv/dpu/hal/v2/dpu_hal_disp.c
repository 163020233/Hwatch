/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: dpu hal disp
 */
#include "dpu_hal_disp.h"
#include "platform_core.h"

PLT_SEC_ITCM_T td_void dpu_reg_set_voctrl_vo_ck_gt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset,
    td_u32 vo_ck_gt_en)
{
    u_voctrl voctrl;
    voctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->voctrl.u32) + offset));
    voctrl.bits.vo_ck_gt_en = vo_ck_gt_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->voctrl.u32) + offset), voctrl.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_voctrl_intf_chk_sum_en(volatile dpu_regs_type *dpu_reg, td_u32 offset,
    td_u32 intf_chk_sum_en)
{
    u_voctrl voctrl;
    voctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->voctrl.u32) + offset));
    voctrl.bits.intf_chk_sum_en = intf_chk_sum_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->voctrl.u32) + offset), voctrl.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_voctrl_g0_ck_gt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset,
    td_u32 g0_ck_gt_en)
{
    u_voctrl voctrl;
    voctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->voctrl.u32) + offset));
    voctrl.bits.g0_ck_gt_en = g0_ck_gt_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->voctrl.u32) + offset), voctrl.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_voctrl_g1_ck_gt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset,
    td_u32 g1_ck_gt_en)
{
    u_voctrl voctrl;
    voctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->voctrl.u32) + offset));
    voctrl.bits.g1_ck_gt_en = g1_ck_gt_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->voctrl.u32) + offset), voctrl.u32);

    return;
}

td_u32 dpu_reg_get_vointsta(const volatile dpu_regs_type *dpu_reg)
{
    volatile u_vointsta vointsta;
    vointsta.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vointsta.u32)));
    return vointsta.u32;
}

td_void dpu_reg_set_vomskintsta(volatile dpu_regs_type *dpu_reg, td_u32 intmask)
{
    dpu_reg_write((uintptr_t)(&(dpu_reg->vomskintsta.u32)), intmask);
    return;
}

td_u32 dpu_reg_get_vomskintsta(const volatile dpu_regs_type *dpu_reg)
{
    volatile u_vomskintsta vomskintsta;
    vomskintsta.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vomskintsta.u32)));
    return vomskintsta.u32;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_vointmsk_enable(volatile dpu_regs_type *dpu_reg, td_u32 intmask)
{
    u_vointmsk vointmsk;
    vointmsk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vointmsk.u32)));
    vointmsk.u32 = vointmsk.u32 | intmask;
    dpu_reg_write((uintptr_t)(&(dpu_reg->vointmsk.u32)), vointmsk.u32);
    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_vointmsk_disable(volatile dpu_regs_type *dpu_reg, td_u32 intmask)
{
    u_vointmsk vointmsk;
    vointmsk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vointmsk.u32)));
    vointmsk.u32 = vointmsk.u32 & (~intmask);
    dpu_reg_write((uintptr_t)(&(dpu_reg->vointmsk.u32)), vointmsk.u32);
    return;
}

td_u32 dpu_reg_get_vocmd_intsta(const volatile dpu_regs_type *dpu_reg)
{
    volatile u_vocmd_intsta vocmd_intsta;
    vocmd_intsta.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vocmd_intsta.u32)));
    return vocmd_intsta.u32;
}

td_void dpu_reg_set_vocmd_mskintsta(volatile dpu_regs_type *dpu_reg, td_u32 intmask)
{
    dpu_reg_write((uintptr_t)(&(dpu_reg->vocmd_mskintsta.u32)), intmask);
    return;
}

td_u32 dpu_reg_get_vocmd_mskintsta(const volatile dpu_regs_type *dpu_reg)
{
    volatile u_vocmd_mskintsta vocmd_mskintsta;
    vocmd_mskintsta.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vocmd_mskintsta.u32)));
    return vocmd_mskintsta.u32;
}

td_void dpu_reg_set_vocmd_intmsk_enable(volatile dpu_regs_type *dpu_reg, td_u32 intmask)
{
    volatile u_vocmd_intmsk vocmd_intmsk;
    vocmd_intmsk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vocmd_intmsk.u32)));
    vocmd_intmsk.u32 = vocmd_intmsk.u32 | intmask;
    dpu_reg_write((uintptr_t)(&(dpu_reg->vocmd_intmsk.u32)), vocmd_intmsk.u32);
    return;
}

td_void dpu_reg_set_vocmd_intmsk_disable(volatile dpu_regs_type *dpu_reg, td_u32 intmask)
{
    volatile u_vocmd_intmsk vocmd_intmsk;
    vocmd_intmsk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vocmd_intmsk.u32)));
    vocmd_intmsk.u32 = vocmd_intmsk.u32 & (~intmask);
    dpu_reg_write((uintptr_t)(&(dpu_reg->vocmd_intmsk.u32)), vocmd_intmsk.u32);
    return;
}

td_u32 dpu_reg_get_voufsta_g1_uf_sta(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_voufsta voufsta;
    voufsta.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->voufsta.u32) + offset));

    return voufsta.bits.g1_uf_sta;
}

td_u32 dpu_reg_get_voufsta_g0_uf_sta(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_voufsta voufsta;
    voufsta.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->voufsta.u32) + offset));

    return voufsta.bits.g0_uf_sta;
}

td_void dpu_reg_set_voufclr_g1_uf_clr(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 g1_uf_clr)
{
    volatile u_voufclr voufclr;
    voufclr.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->voufclr.u32) + offset));
    voufclr.bits.g1_uf_clr = g1_uf_clr;
    dpu_reg_write((uintptr_t)(&(dpu_reg->voufclr.u32) + offset), voufclr.u32);

    return;
}

td_void dpu_reg_set_voufclr_g0_uf_clr(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 g0_uf_clr)
{
    volatile u_voufclr voufclr;
    voufclr.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->voufclr.u32) + offset));
    voufclr.bits.g0_uf_clr = g0_uf_clr;
    dpu_reg_write((uintptr_t)(&(dpu_reg->voufclr.u32) + offset), voufclr.u32);

    return;
}

td_void dpu_reg_set_cbm_bkg_cbm_bkgy(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbm_bkgy)
{
    volatile u_cbm_bkg cbm_bkg;
    cbm_bkg.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_bkg.u32) + offset));
    cbm_bkg.bits.cbm_bkgy = cbm_bkgy;
    dpu_reg_write((uintptr_t)(&(dpu_reg->cbm_bkg.u32) + offset), cbm_bkg.u32);

    return;
}

td_void dpu_reg_set_cbm_bkg_cbm_bkgcb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbm_bkgcb)
{
    volatile u_cbm_bkg cbm_bkg;
    cbm_bkg.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_bkg.u32) + offset));
    cbm_bkg.bits.cbm_bkgcb = cbm_bkgcb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->cbm_bkg.u32) + offset), cbm_bkg.u32);

    return;
}

td_void dpu_reg_set_cbm_bkg_cbm_bkgcr(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbm_bkgcr)
{
    volatile u_cbm_bkg cbm_bkg;
    cbm_bkg.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_bkg.u32) + offset));
    cbm_bkg.bits.cbm_bkgcr = cbm_bkgcr;
    dpu_reg_write((uintptr_t)(&(dpu_reg->cbm_bkg.u32) + offset), cbm_bkg.u32);

    return;
}

td_void dpu_reg_set_cbm_mix_mixer_prio2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mixer_prio2)
{
    volatile u_cbm_mix cbm_mix;
    cbm_mix.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_mix.u32) + offset));
    cbm_mix.bits.mixer_prio2 = mixer_prio2;
    dpu_reg_write((uintptr_t)(&(dpu_reg->cbm_mix.u32) + offset), cbm_mix.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_cbm_mix_mixer_prio1(volatile dpu_regs_type *dpu_reg, td_u32 offset,
    td_u32 mixer_prio1)
{
    u_cbm_mix cbm_mix;
    cbm_mix.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_mix.u32) + offset));
    cbm_mix.bits.mixer_prio1 = mixer_prio1;
    dpu_reg_write((uintptr_t)(&(dpu_reg->cbm_mix.u32) + offset), cbm_mix.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_cbm_mix_mixer_prio0(volatile dpu_regs_type *dpu_reg, td_u32 offset,
    td_u32 mixer_prio0)
{
    u_cbm_mix cbm_mix;
    cbm_mix.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_mix.u32) + offset));
    cbm_mix.bits.mixer_prio0 = mixer_prio0;
    dpu_reg_write((uintptr_t)(&(dpu_reg->cbm_mix.u32) + offset), cbm_mix.u32);

    return;
}

td_u32 dpu_reg_get_cbm_mix_priority(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_cbm_mix cbm_mix;
    cbm_mix.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_mix.u32) + offset));
    return cbm_mix.u32;
}

td_void dpu_reg_set_cbm_ctrl_bypass_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bypass_mode)
{
    volatile u_cbm_ctrl cbm_ctrl;
    cbm_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_ctrl.u32) + offset));
    cbm_ctrl.bits.bypass_mode = bypass_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->cbm_ctrl.u32) + offset), cbm_ctrl.u32);

    return;
}

td_void dpu_reg_set_cbm_ctrl_layer0_bypass_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bypass_en)
{
    volatile u_cbm_ctrl cbm_ctrl;
    cbm_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_ctrl.u32) + offset));
    cbm_ctrl.bits.layer0_bypass_en = bypass_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->cbm_ctrl.u32) + offset), cbm_ctrl.u32);

    return;
}

td_void dpu_reg_set_partial_cfg_vld(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_bool is_valid)
{
    volatile u_dpu_partial_ctrl dpu_partial_ctrl;
    dpu_partial_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dpu_partial_ctrl.u32) + offset));
    dpu_partial_ctrl.bits.dpu_partial_cfg_vld = is_valid;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dpu_partial_ctrl.u32) + offset), dpu_partial_ctrl.u32);

    return;
}

td_void dpu_reg_set_partial_hdr0(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 partial_hdr0)
{
    volatile u_dpu_partial_gen_hdr0 dpu_partial_gen_hdr0;
    dpu_partial_gen_hdr0.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dpu_partial_gen_hdr0.u32) + offset));
    dpu_partial_gen_hdr0.u32 = partial_hdr0;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dpu_partial_gen_hdr0.u32) + offset), dpu_partial_gen_hdr0.u32);

    return;
}

td_void dpu_reg_set_partial_pld_data00(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pld_data00)
{
    volatile u_dpu_partial_gen_pld_data00 dpu_partial_gen_pld_data00;
    dpu_partial_gen_pld_data00.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dpu_partial_gen_pld_data00.u32) + offset));
    dpu_partial_gen_pld_data00.u32 = pld_data00;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dpu_partial_gen_pld_data00.u32) + offset), dpu_partial_gen_pld_data00.u32);

    return;
}

td_void dpu_reg_set_partial_pld_data01(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pld_data01)
{
    volatile u_dpu_partial_gen_pld_data01 dpu_partial_gen_pld_data01;
    dpu_partial_gen_pld_data01.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dpu_partial_gen_pld_data01.u32) + offset));
    dpu_partial_gen_pld_data01.u32 = pld_data01;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dpu_partial_gen_pld_data01.u32) + offset), dpu_partial_gen_pld_data01.u32);

    return;
}

td_void dpu_reg_set_partial_hdr1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 partial_hdr1)
{
    volatile u_dpu_partial_gen_hdr1 dpu_partial_gen_hdr1;
    dpu_partial_gen_hdr1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dpu_partial_gen_hdr1.u32) + offset));
    dpu_partial_gen_hdr1.u32 = partial_hdr1;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dpu_partial_gen_hdr1.u32) + offset), dpu_partial_gen_hdr1.u32);

    return;
}

td_void dpu_reg_set_partial_pld_data10(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pld_data10)
{
    volatile u_dpu_partial_gen_pld_data10 dpu_partial_gen_pld_data10;
    dpu_partial_gen_pld_data10.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dpu_partial_gen_pld_data10.u32) + offset));
    dpu_partial_gen_pld_data10.u32 = pld_data10;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dpu_partial_gen_pld_data10.u32) + offset), dpu_partial_gen_pld_data10.u32);

    return;
}

td_void dpu_reg_set_partial_pld_data11(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pld_data11)
{
    volatile u_dpu_partial_gen_pld_data11 dpu_partial_gen_pld_data11;
    dpu_partial_gen_pld_data11.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dpu_partial_gen_pld_data11.u32) + offset));
    dpu_partial_gen_pld_data11.u32 = pld_data11;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dpu_partial_gen_pld_data11.u32) + offset), dpu_partial_gen_pld_data11.u32);

    return;
}

td_void dpu_reg_set_link_ctrl_g0_bypass_dhd0_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bypass_en)
{
    volatile u_link_ctrl link_ctrl;
    link_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->link_ctrl.u32) + offset));
    link_ctrl.bits.g0_bypass_dhd0_en = bypass_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->link_ctrl.u32) + offset), link_ctrl.u32);

    return;
}

td_void dpu_reg_set_link_ctrl_format_link(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 format_link)
{
    volatile u_link_ctrl link_ctrl;
    link_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->link_ctrl.u32) + offset));
    link_ctrl.bits.format_link = format_link;
    dpu_reg_write((uintptr_t)(&(dpu_reg->link_ctrl.u32) + offset), link_ctrl.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_dhd0_ctrl_intf_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_en)
{
    u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.intf_en = intf_en;
    dhd0_ctrl.bits.intf_pcnt_en = intf_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_cbar_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbar_en)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.cbar_en = cbar_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_intf_pcnt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_pcnt_en)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.intf_pcnt_en = intf_pcnt_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_cbar_sel(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbar_sel)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.cbar_sel = cbar_sel;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_cbar_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbar_mode)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.cbar_mode = cbar_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_uf_offline_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 uf_offline_en)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.uf_offline_en = uf_offline_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_dhd0_ctrl_iop(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 iop)
{
    u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.iop = iop;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_disp_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 disp_mode)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.disp_mode = disp_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_regup(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 regup)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.regup = regup;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_dhd0_vsync1_vbb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vbb)
{
    u_dhd0_vsync1 dhd0_vsync1;
    dhd0_vsync1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vsync1.u32) + offset));
    dhd0_vsync1.bits.vbb = vbb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vsync1.u32) + offset), dhd0_vsync1.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_dhd0_vsync1_vact(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vact)
{
    u_dhd0_vsync1 dhd0_vsync1;
    dhd0_vsync1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vsync1.u32) + offset));
    dhd0_vsync1.bits.vact = vact;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vsync1.u32) + offset), dhd0_vsync1.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_dhd0_vsync2_vfb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vfb)
{
    u_dhd0_vsync2 dhd0_vsync2;
    dhd0_vsync2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vsync2.u32) + offset));
    dhd0_vsync2.bits.vfb = vfb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vsync2.u32) + offset), dhd0_vsync2.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_dhd0_hsync1_hbb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 hbb)
{
    u_dhd0_hsync1 dhd0_hsync1;
    dhd0_hsync1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_hsync1.u32) + offset));
    dhd0_hsync1.bits.hbb = hbb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_hsync1.u32) + offset), dhd0_hsync1.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_dhd0_hsync1_hact(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 hact)
{
    u_dhd0_hsync1 dhd0_hsync1;
    dhd0_hsync1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_hsync1.u32) + offset));
    dhd0_hsync1.bits.hact = hact;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_hsync1.u32) + offset), dhd0_hsync1.u32);

    return;
}

td_void dpu_reg_set_dhd0_hsync2_hmid(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 hmid)
{
    volatile u_dhd0_hsync2 dhd0_hsync2;
    dhd0_hsync2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_hsync2.u32) + offset));
    dhd0_hsync2.bits.hmid = hmid;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_hsync2.u32) + offset), dhd0_hsync2.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_dhd0_hsync2_hfb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 hfb)
{
    u_dhd0_hsync2 dhd0_hsync2;
    dhd0_hsync2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_hsync2.u32) + offset));
    dhd0_hsync2.bits.hfb = hfb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_hsync2.u32) + offset), dhd0_hsync2.u32);

    return;
}

td_void dpu_reg_set_dhd0_vplus1_bvbb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bvbb)
{
    volatile u_dhd0_vplus1 dhd0_vplus1;
    dhd0_vplus1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vplus1.u32) + offset));
    dhd0_vplus1.bits.bvbb = bvbb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vplus1.u32) + offset), dhd0_vplus1.u32);

    return;
}

td_void dpu_reg_set_dhd0_vplus1_bvact(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bvact)
{
    volatile u_dhd0_vplus1 dhd0_vplus1;
    dhd0_vplus1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vplus1.u32) + offset));
    dhd0_vplus1.bits.bvact = bvact;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vplus1.u32) + offset), dhd0_vplus1.u32);

    return;
}

td_void dpu_reg_set_dhd0_vplus2_bvfb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bvfb)
{
    volatile u_dhd0_vplus2 dhd0_vplus2;
    dhd0_vplus2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vplus2.u32) + offset));
    dhd0_vplus2.bits.bvfb = bvfb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vplus2.u32) + offset), dhd0_vplus2.u32);

    return;
}

td_void dpu_reg_set_dhd0_pwr_multichn_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 multichn_en)
{
    volatile u_dhd0_pwr dhd0_pwr;
    dhd0_pwr.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_pwr.u32) + offset));
    dhd0_pwr.bits.multichn_en = multichn_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_pwr.u32) + offset), dhd0_pwr.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_dhd0_pwr_vpw(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vpw)
{
    u_dhd0_pwr dhd0_pwr;
    dhd0_pwr.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_pwr.u32) + offset));
    dhd0_pwr.bits.vpw = vpw;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_pwr.u32) + offset), dhd0_pwr.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_dhd0_pwr_hpw(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 hpw)
{
    u_dhd0_pwr dhd0_pwr;
    dhd0_pwr.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_pwr.u32) + offset));
    dhd0_pwr.bits.hpw = hpw;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_pwr.u32) + offset), dhd0_pwr.u32);

    return;
}

td_void dpu_reg_set_dhd0_vtthd3_thd4_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 thd4_mode)
{
    volatile u_dhd0_vtthd3 dhd0_vtthd3;
    dhd0_vtthd3.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vtthd3.u32) + offset));
    dhd0_vtthd3.bits.thd4_mode = thd4_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vtthd3.u32) + offset), dhd0_vtthd3.u32);

    return;
}

td_void dpu_reg_set_dhd0_vtthd3_vtmgthd4(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vtmgthd4)
{
    volatile u_dhd0_vtthd3 dhd0_vtthd3;
    dhd0_vtthd3.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vtthd3.u32) + offset));
    dhd0_vtthd3.bits.vtmgthd4 = vtmgthd4;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vtthd3.u32) + offset), dhd0_vtthd3.u32);

    return;
}

td_void dpu_reg_set_dhd0_vtthd3_thd3_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 thd3_mode)
{
    volatile u_dhd0_vtthd3 dhd0_vtthd3;
    dhd0_vtthd3.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vtthd3.u32) + offset));
    dhd0_vtthd3.bits.thd3_mode = thd3_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vtthd3.u32) + offset), dhd0_vtthd3.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_dhd0_vtthd3_vtmgthd3(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vtmgthd3)
{
    u_dhd0_vtthd3 dhd0_vtthd3;
    dhd0_vtthd3.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vtthd3.u32) + offset));
    dhd0_vtthd3.bits.vtmgthd3 = vtmgthd3;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vtthd3.u32) + offset), dhd0_vtthd3.u32);

    return;
}

td_void dpu_reg_set_dhd0_vtthd_thd2_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 thd2_mode)
{
    volatile u_dhd0_vtthd dhd0_vtthd;
    dhd0_vtthd.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vtthd.u32) + offset));
    dhd0_vtthd.bits.thd2_mode = thd2_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vtthd.u32) + offset), dhd0_vtthd.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_dhd0_vtthd_vtmgthd2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vtmgthd2)
{
    u_dhd0_vtthd dhd0_vtthd;
    dhd0_vtthd.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vtthd.u32) + offset));
    dhd0_vtthd.bits.vtmgthd2 = vtmgthd2;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vtthd.u32) + offset), dhd0_vtthd.u32);

    return;
}

td_void dpu_reg_set_dhd0_vtthd_thd1_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 thd1_mode)
{
    volatile u_dhd0_vtthd dhd0_vtthd;
    dhd0_vtthd.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vtthd.u32) + offset));
    dhd0_vtthd.bits.thd1_mode = thd1_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vtthd.u32) + offset), dhd0_vtthd.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_dhd0_vtthd_vtmgthd1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vtmgthd1)
{
    u_dhd0_vtthd dhd0_vtthd;
    dhd0_vtthd.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vtthd.u32) + offset));
    dhd0_vtthd.bits.vtmgthd1 = vtmgthd1;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vtthd.u32) + offset), dhd0_vtthd.u32);

    return;
}

td_void dpu_reg_set_dhd0_parathd_para_thd(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 para_thd)
{
    volatile u_dhd0_parathd dhd0_parathd;
    dhd0_parathd.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_parathd.u32) + offset));
    dhd0_parathd.bits.para_thd = para_thd;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_parathd.u32) + offset), dhd0_parathd.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_dhd0_precharge_thd_vsync_te_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset,
    td_u32 mode)
{
    u_dhd0_te_ctrl dhd0_te_ctrl;
    dhd0_te_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_te_ctrl.u32) + offset));
    dhd0_te_ctrl.bits.vsync_te_mode = mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_te_ctrl.u32) + offset), dhd0_te_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_start_pos_timing_start_pos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pos)
{
    volatile u_dhd0_start_pos dhd0_start_pos;
    dhd0_start_pos.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_start_pos.u32) + offset));
    dhd0_start_pos.bits.timing_start_pos = pos;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_start_pos.u32) + offset), dhd0_start_pos.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_dhd0_start_pos_start_pos(volatile dpu_regs_type *dpu_reg, td_u32 offset,
    td_u32 start_pos)
{
    u_dhd0_start_pos dhd0_start_pos;
    dhd0_start_pos.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_start_pos.u32) + offset));
    dhd0_start_pos.bits.start_pos = start_pos;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_start_pos.u32) + offset), dhd0_start_pos.u32);

    return;
}

td_void dpu_reg_set_dhd0_clk_dv_ctrl_no_active_area_pos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pos)
{
    volatile u_dhd0_clk_dv_ctrl dhd0_clk_dv_ctrl;
    dhd0_clk_dv_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_clk_dv_ctrl.u32) + offset));
    dhd0_clk_dv_ctrl.bits.no_active_area_pos = pos;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_clk_dv_ctrl.u32) + offset), dhd0_clk_dv_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_clk_dv_ctrl_intf_dv_mux(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_dv_mux)
{
    volatile u_dhd0_clk_dv_ctrl dhd0_clk_dv_ctrl;
    dhd0_clk_dv_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_clk_dv_ctrl.u32) + offset));
    dhd0_clk_dv_ctrl.bits.intf_dv_mux = intf_dv_mux;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_clk_dv_ctrl.u32) + offset), dhd0_clk_dv_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_clk_dv_ctrl_intf_clk_mux(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_clk_mux)
{
    volatile u_dhd0_clk_dv_ctrl dhd0_clk_dv_ctrl;
    dhd0_clk_dv_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_clk_dv_ctrl.u32) + offset));
    dhd0_clk_dv_ctrl.bits.intf_clk_mux = intf_clk_mux;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_clk_dv_ctrl.u32) + offset), dhd0_clk_dv_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_rgb_fix_ctrl_rgb_fix_mux(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 rgb_fix_mux)
{
    volatile u_dhd0_rgb_fix_ctrl dhd0_rgb_fix_ctrl;
    dhd0_rgb_fix_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_rgb_fix_ctrl.u32) + offset));
    dhd0_rgb_fix_ctrl.bits.rgb_fix_mux = rgb_fix_mux;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_rgb_fix_ctrl.u32) + offset), dhd0_rgb_fix_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_rgb_fix_ctrl_fix_r(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 fix_r)
{
    volatile u_dhd0_rgb_fix_ctrl dhd0_rgb_fix_ctrl;
    dhd0_rgb_fix_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_rgb_fix_ctrl.u32) + offset));
    dhd0_rgb_fix_ctrl.bits.fix_r = fix_r;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_rgb_fix_ctrl.u32) + offset), dhd0_rgb_fix_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_rgb_fix_ctrl_fix_g(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 fix_g)
{
    volatile u_dhd0_rgb_fix_ctrl dhd0_rgb_fix_ctrl;
    dhd0_rgb_fix_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_rgb_fix_ctrl.u32) + offset));
    dhd0_rgb_fix_ctrl.bits.fix_g = fix_g;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_rgb_fix_ctrl.u32) + offset), dhd0_rgb_fix_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_rgb_fix_ctrl_fix_b(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 fix_b)
{
    volatile u_dhd0_rgb_fix_ctrl dhd0_rgb_fix_ctrl;
    dhd0_rgb_fix_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_rgb_fix_ctrl.u32) + offset));
    dhd0_rgb_fix_ctrl.bits.fix_b = fix_b;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_rgb_fix_ctrl.u32) + offset), dhd0_rgb_fix_ctrl.u32);

    return;
}

td_u32 dpu_reg_get_dhd0_intf_chksum_y_check_sum(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf_chksum_y dhd0_intf_chksum_y;
    dhd0_intf_chksum_y.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_chksum_y.u32) + offset));
    return dhd0_intf_chksum_y.bits.check_sum0;
}

td_u32 dpu_reg_get_dhd0_intf_chksum_u_check_sum(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf_chksum_u dhd0_intf_chksum_u;
    dhd0_intf_chksum_u.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_chksum_u.u32) + offset));
    return dhd0_intf_chksum_u.bits.check_sum1;
}

td_u32 dpu_reg_get_dhd0_intf_chksum_v_check_sum(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf_chksum_v dhd0_intf_chksum_v;
    dhd0_intf_chksum_v.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_chksum_v.u32) + offset));
    return dhd0_intf_chksum_v.bits.check_sum2;
}

td_u32 dpu_reg_get_dhd0_intf_pcnt_cnt(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
#ifdef SUPPORT_GPU_QSPI
    volatile u_dhd0_intf_pcnt_real dhd0_intf_pcnt_real;
    dhd0_intf_pcnt_real.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_pcnt_real.u32) + offset));
    return dhd0_intf_pcnt_real.bits.intf_pcnt_real;
#else
    volatile u_dhd0_intf_pcnt_cnt dhd0_intf_pcnt_cnt;
    dhd0_intf_pcnt_cnt.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_pcnt_cnt.u32) + offset));
    return dhd0_intf_pcnt_cnt.bits.intf_pcnt_cnt;
#endif
}

td_u32 dpu_reg_get_dhd0_intf_chksum_high1_b0_sum_high(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf_chksum_high1 dhd0_intf_chksum_high1;
    dhd0_intf_chksum_high1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_chksum_high1.u32) + offset));
    return dhd0_intf_chksum_high1.bits.b0_sum_high;
}

td_u32 dpu_reg_get_dhd0_intf_chksum_high1_g0_sum_high(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf_chksum_high1 dhd0_intf_chksum_high1;
    dhd0_intf_chksum_high1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_chksum_high1.u32) + offset));
    return dhd0_intf_chksum_high1.bits.g0_sum_high;
}

td_u32 dpu_reg_get_dhd0_intf_chksum_high1_r0_sum_high(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf_chksum_high1 dhd0_intf_chksum_high1;
    dhd0_intf_chksum_high1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_chksum_high1.u32) + offset));
    return dhd0_intf_chksum_high1.bits.r0_sum_high;
}

td_void dpu_reg_set_dhd0_frm_rdy_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 frm_rdy_mode)
{
    volatile u_dhd0_intf_cmdcfg dhd0_intf_cmdcfg;
    dhd0_intf_cmdcfg.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_cmdcfg.u32) + offset));
    dhd0_intf_cmdcfg.bits.tde_frm_rdy_mode = frm_rdy_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_intf_cmdcfg.u32) + offset), dhd0_intf_cmdcfg.u32);

    return;
}

td_void dpu_reg_set_dhd0_te_dly_cfg(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 dhd0_td_dly_cfg)
{
    volatile u_dhd0_intf_cmdcfg dhd0_intf_cmdcfg;
    dhd0_intf_cmdcfg.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_cmdcfg.u32) + offset));
    dhd0_intf_cmdcfg.bits.te_dly_cfg = dhd0_td_dly_cfg;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_intf_cmdcfg.u32) + offset), dhd0_intf_cmdcfg.u32);

    return;
}

td_void dpu_reg_set_dhd0_tde_frm_rdy_dhd0_tde_frm_rdy(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 frm_rdy)
{
    volatile u_dhd0_tde_frm_rdy dhd0_tde_frm_rdy;
    dhd0_tde_frm_rdy.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_tde_frm_rdy.u32) + offset));
    dhd0_tde_frm_rdy.bits.tde_frm_rdy = frm_rdy;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_tde_frm_rdy.u32) + offset), dhd0_tde_frm_rdy.u32);

    return;
}

td_void dpu_reg_set_dhd0_te_timeout_dhd_te_timeout_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 timeout)
{
    volatile u_dhd0_te_timeout dhd0_te_timeout;
    dhd0_te_timeout.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_te_timeout.u32) + offset));
    dhd0_te_timeout.bits.te_timeout_en = timeout;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_te_timeout.u32) + offset), dhd0_te_timeout.u32);

    return;
}

td_void dpu_reg_set_dhd0_te_timeout_dhd_te_timeout_thd(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 thd)
{
    volatile u_dhd0_te_timeout dhd0_te_timeout;
    dhd0_te_timeout.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_te_timeout.u32) + offset));
    dhd0_te_timeout.bits.te_timeout_thd = thd;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_te_timeout.u32) + offset), dhd0_te_timeout.u32);

    return;
}

td_u32 dpu_reg_get_dhd0_state_dhd_even(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_state dhd0_state;
    dhd0_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_state.u32) + offset));
    return dhd0_state.bits.dhd_even;
}

td_u32 dpu_reg_get_dhd0_state_count_int(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_state dhd0_state;
    dhd0_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_state.u32) + offset));
    return dhd0_state.bits.count_int;
}

td_u32 dpu_reg_get_dhd0_state_vcnt(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_state dhd0_state;
    dhd0_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_state.u32) + offset));
    return dhd0_state.bits.vcnt;
}

td_u32 dpu_reg_get_dhd0_state_bottom_field(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_state dhd0_state;
    dhd0_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_state.u32) + offset));
    return dhd0_state.bits.bottom_field;
}

td_u32 dpu_reg_get_dhd0_state_vblank(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_state dhd0_state;
    dhd0_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_state.u32) + offset));
    return dhd0_state.bits.vblank;
}

td_u32 dpu_reg_get_dhd0_state_vback_blank(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_state dhd0_state;
    dhd0_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_state.u32) + offset));
    return dhd0_state.bits.vback_blank;
}

td_u32 dpu_reg_get_dhd0_uf_state_start_pos(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_uf_state dhd0_uf_state;
    dhd0_uf_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_uf_state.u32) + offset));
    return dhd0_uf_state.bits.start_pos_rd;
}

td_u32 dpu_reg_get_dhd0_uf_state_ud_first_cnt(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_uf_state dhd0_uf_state;
    dhd0_uf_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_uf_state.u32) + offset));
    return dhd0_uf_state.bits.ud_first_cnt;
}

td_u32 dpu_reg_get_dhd0_vsync_te_state_vsync_te_end_sta(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_vsync_te_state dhd0_vsync_te_state;
    dhd0_vsync_te_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vsync_te_state.u32) + offset));
    return dhd0_vsync_te_state.bits.vsync_te_end_sta;
}

td_u32 dpu_reg_get_dhd0_vsync_te_state_vsync_te_start_sta1(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_vsync_te_state dhd0_vsync_te_state;
    dhd0_vsync_te_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vsync_te_state.u32) + offset));
    return dhd0_vsync_te_state.bits.vsync_te_start_sta1;
}

td_u32 dpu_reg_get_dhd0_vsync_te_state_vsync_te_start_sta(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_vsync_te_state dhd0_vsync_te_state;
    dhd0_vsync_te_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vsync_te_state.u32) + offset));
    return dhd0_vsync_te_state.bits.vsync_te_start_sta;
}

td_u32 dpu_reg_get_dhd0_vsync_te_state1_vsync_te_width(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_vsync_te_state1 dhd0_vsync_te_state1;
    dhd0_vsync_te_state1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vsync_te_state1.u32) + offset));
    return dhd0_vsync_te_state1.bits.vsync_te_width;
}

td_u32 dpu_reg_get_dhd0_vsync_te_state1_vsync_te_vfb(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_vsync_te_state1 dhd0_vsync_te_state1;
    dhd0_vsync_te_state1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vsync_te_state1.u32) + offset));
    return dhd0_vsync_te_state1.bits.vsync_te_vfb;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_intf0_ulps_ctrl_ulps_lp_mode_en(volatile dpu_regs_type *dpu_reg, td_u32 offset,
    td_u32 mode_en)
{
    u_intf0_sdlp_ctrl intf0_sdlp_ctrl;
    intf0_sdlp_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->intf0_sdlp_ctrl.u32) + offset));
    intf0_sdlp_ctrl.bits.sdlp_lp_mode_en = mode_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->intf0_sdlp_ctrl.u32) + offset), intf0_sdlp_ctrl.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_intf0_ulps_ctrl_ulps_lp_mode_thd(volatile dpu_regs_type *dpu_reg, td_u32 offset,
    td_u32 mode_thd)
{
    u_intf0_sdlp_ctrl intf0_sdlp_ctrl;
    intf0_sdlp_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->intf0_sdlp_ctrl.u32) + offset));
    intf0_sdlp_ctrl.bits.sdlp_lp_mode_thd = mode_thd;
    dpu_reg_write((uintptr_t)(&(dpu_reg->intf0_sdlp_ctrl.u32) + offset), intf0_sdlp_ctrl.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_intf0_ulps_delay_ulps_lp_end_delay(volatile dpu_regs_type *dpu_reg, td_u32 offset,
    td_u32 delay)
{
    u_intf0_sdlp_delay intf0_sdlp_delay;
    intf0_sdlp_delay.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->intf0_sdlp_delay.u32) + offset));
    intf0_sdlp_delay.bits.sdlp_lp_end_delay = delay;
    dpu_reg_write((uintptr_t)(&(dpu_reg->intf0_sdlp_delay.u32) + offset), intf0_sdlp_delay.u32);

    return;
}

PLT_SEC_ITCM_T td_void dpu_reg_set_intf0_ulps_delay_ulps_lp_start_delay(volatile dpu_regs_type *dpu_reg, td_u32 offset,
    td_u32 delay)
{
    u_intf0_sdlp_delay intf0_sdlp_delay;
    intf0_sdlp_delay.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->intf0_sdlp_delay.u32) + offset));
    intf0_sdlp_delay.bits.sdlp_lp_start_delay = delay;
    dpu_reg_write((uintptr_t)(&(dpu_reg->intf0_sdlp_delay.u32) + offset), intf0_sdlp_delay.u32);

    return;
}

td_void dpu_reg_set_intf0_ulps_stop_soft_stop_lp_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 lp_mode)
{
    volatile u_intf0_sdlp_stop intf0_sdlp_stop;
    intf0_sdlp_stop.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->intf0_sdlp_stop.u32) + offset));
    intf0_sdlp_stop.bits.soft_stop_lp_mode = lp_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->intf0_sdlp_stop.u32) + offset), intf0_sdlp_stop.u32);

    return;
}

td_u32 dpu_reg_get_intf0_ulps_sta_mipi_lp_mode_en(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_intf0_sdlp_sta intf0_sdlp_sta;
    intf0_sdlp_sta.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->intf0_sdlp_sta.u32) + offset));
    return intf0_sdlp_sta.u32;
}


// update

td_void dpu_reg_set_mapping_bind(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mapping_bind)
{
    volatile u_link_ctrl link_ctrl;
    link_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->link_ctrl.u32) + offset));
    link_ctrl.bits.mapping_bind = mapping_bind;
    dpu_reg_write((uintptr_t)(&(dpu_reg->link_ctrl.u32) + offset), link_ctrl.u32);
    return;
}

td_void dpu_reg_set_dpu_mapping_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_bool mapping_en)
{
    volatile u_dpu_mapping_ctrl mapping_ctrl;
    mapping_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->mapping_ctrl.u32) + offset));
    mapping_ctrl.bits.mapping_en = mapping_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->mapping_ctrl.u32) + offset), mapping_ctrl.u32);

    return;
}

td_void dpu_reg_set_dpu_mapping_ctrl(volatile dpu_regs_type *dpu_reg, td_u32 offset, disp_src2_mapping_ctrl *mapping)
{
    volatile u_dpu_mapping_ctrl mapping_ctrl;
    mapping_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->mapping_ctrl.u32) + offset));
    mapping_ctrl.bits.mapping_en = mapping->mapping_en;
    mapping_ctrl.bits.mapping_mode = mapping->mapping_mode;
    mapping_ctrl.bits.alpha_mapping_mode = mapping->alpha_mapping_mode;
    mapping_ctrl.bits.req_tile_sel = mapping->req_tile_sel;
    mapping_ctrl.bits.tile_linear_sel = mapping->tile_linear_sel;
    mapping_ctrl.bits.cfg_en = mapping->cfg_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->mapping_ctrl.u32) + offset), mapping_ctrl.u32);

    return;
}

td_void dpu_reg_set_req_tile_sel(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 req_tile_sel)
{
    volatile u_dpu_mapping_ctrl mapping_ctrl;
    mapping_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->mapping_ctrl.u32) + offset));
    mapping_ctrl.bits.req_tile_sel = req_tile_sel;
    dpu_reg_write((uintptr_t)(&(dpu_reg->mapping_ctrl.u32) + offset), mapping_ctrl.u32);

    return;
}

td_void dpu_reg_set_t2r0_t2r_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_bool t2r0_t2r_en)
{
    volatile u_g0_t2r0_ctrl g0_t2r0_ctrl;
    g0_t2r0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_t2r0_ctrl.u32) + offset));
    g0_t2r0_ctrl.bits.t2r0_t2r_en = t2r0_t2r_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_t2r0_ctrl.u32) + offset), g0_t2r0_ctrl.u32);

    return;
}

td_void dpu_reg_set_t2r0_tile_wth(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 t2r0_tile_wth)
{
    volatile u_g0_t2r0_ctrl g0_t2r0_ctrl;
    g0_t2r0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_t2r0_ctrl.u32) + offset));
    g0_t2r0_ctrl.bits.t2r0_tile_wth = t2r0_tile_wth;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_t2r0_ctrl.u32) + offset), g0_t2r0_ctrl.u32);

    return;
}

td_void dpu_reg_set_mapping_coef00(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_float mapping_coef00)
{
    volatile u_dpu_mapping_coef00 mapping_coef000;
    mapping_coef000.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->mapping_coef00.u32) + offset));
    mapping_coef000.bits.mapping_coef00 = mapping_coef00;
    dpu_reg_write((uintptr_t)(&(dpu_reg->mapping_coef00.u32) + offset), mapping_coef000.u32);

    return;
}

td_void dpu_reg_set_mapping_coef01(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_float mapping_coef01)
{
    volatile u_dpu_mapping_coef01 mapping_coef001;
    mapping_coef001.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->mapping_coef01.u32) + offset));
    mapping_coef001.bits.mapping_coef01 = mapping_coef01;
    dpu_reg_write((uintptr_t)(&(dpu_reg->mapping_coef01.u32) + offset), mapping_coef001.u32);

    return;
}

td_void dpu_reg_set_mapping_coef02(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_float mapping_coef02)
{
    volatile u_dpu_mapping_coef02 mapping_coef002;
    mapping_coef002.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->mapping_coef02.u32) + offset));
    mapping_coef002.bits.mapping_coef02 = mapping_coef02;
    dpu_reg_write((uintptr_t)(&(dpu_reg->mapping_coef02.u32) + offset), mapping_coef002.u32);

    return;
}

td_void dpu_reg_set_mapping_coef10(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_float mapping_coef10)
{
    volatile u_dpu_mapping_coef10 mapping_coef010;
    mapping_coef010.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->mapping_coef10.u32) + offset));
    mapping_coef010.bits.mapping_coef10 = mapping_coef10;
    dpu_reg_write((uintptr_t)(&(dpu_reg->mapping_coef10.u32) + offset), mapping_coef010.u32);

    return;
}

td_void dpu_reg_set_mapping_coef11(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_float mapping_coef11)
{
    volatile u_dpu_mapping_coef11 mapping_coef011;
    mapping_coef011.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->mapping_coef11.u32) + offset));
    mapping_coef011.bits.mapping_coef11 = mapping_coef11;
    dpu_reg_write((uintptr_t)(&(dpu_reg->mapping_coef11.u32) + offset), mapping_coef011.u32);

    return;
}

td_void dpu_reg_set_mapping_coef12(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_float mapping_coef12)
{
    volatile u_dpu_mapping_coef12 mapping_coef012;
    mapping_coef012.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->mapping_coef12.u32) + offset));
    mapping_coef012.bits.mapping_coef12 = mapping_coef12;
    dpu_reg_write((uintptr_t)(&(dpu_reg->mapping_coef12.u32) + offset), mapping_coef012.u32);

    return;
}

td_void dpu_reg_set_mapping_pattern_fill_color(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pattern_argb)
{
    volatile u_dpu_mapping_pattern_fill_color mapping_pattern_fill_color;
    mapping_pattern_fill_color.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->mapping_pattern_fill_color.u32) + offset));
    mapping_pattern_fill_color.u32 = pattern_argb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->mapping_pattern_fill_color.u32) + offset), mapping_pattern_fill_color.u32);

    return;
}

td_void dpu_reg_set_mapping_debug0_pix_cnt_dfx(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pix_cnt_dfx)
{
    volatile u_dpu_mapping_debug0 mapping_debug0;
    mapping_debug0.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->mapping_debug0.u32) + offset));
    mapping_debug0.bits.pix_cnt_dfx = pix_cnt_dfx;
    dpu_reg_write((uintptr_t)(&(dpu_reg->mapping_debug0.u32) + offset), mapping_debug0.u32);

    return;
}

td_void dpu_reg_set_mapping_crop_xy(volatile dpu_regs_type *dpu_reg,
    td_u32 offset, td_u32 mapping_crop_x, td_u32 mapping_crop_y)
{
    volatile u_dpu_mapping_crop_xy mapping_crop_xy;
    mapping_crop_xy.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->mapping_crop_xy.u32) + offset));
    mapping_crop_xy.bits.mapping_crop_x = mapping_crop_x;
    mapping_crop_xy.bits.mapping_crop_y = mapping_crop_y;
    dpu_reg_write((uintptr_t)(&(dpu_reg->mapping_crop_xy.u32) + offset), mapping_crop_xy.u32);

    return;
}

td_void dpu_reg_set_mapping_crop_wh(volatile dpu_regs_type *dpu_reg,
    td_u32 offset, td_u32 mapping_crop_w, td_u32 mapping_crop_h)
{
    volatile u_dpu_mapping_crop_wh mapping_crop_wh;
    mapping_crop_wh.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->mapping_crop_wh.u32) + offset));
    mapping_crop_wh.bits.mapping_crop_w = mapping_crop_w;
    mapping_crop_wh.bits.mapping_crop_h = mapping_crop_h;
    dpu_reg_write((uintptr_t)(&(dpu_reg->mapping_crop_wh.u32) + offset), mapping_crop_wh.u32);

    return;
}

td_void dpu_reg_set_add_pix_ctrl(volatile dpu_regs_type *dpu_reg, td_u32 offset, disp_add_pix_ctrl *d_add_pix_ctrl)
{
    volatile u_dpu_add_pix_ctrl add_pix_ctrl;
    add_pix_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->add_pix_ctrl.u32) + offset));
    add_pix_ctrl.bits.add_pix_cfg_en = d_add_pix_ctrl->add_pix_cfg_en;
    add_pix_ctrl.bits.add_pix_en = d_add_pix_ctrl->add_pix_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->add_pix_ctrl.u32) + offset), add_pix_ctrl.u32);

    return;
}

td_void dpu_reg_set_add_pix_small(volatile dpu_regs_type *dpu_reg, td_u32 offset, disp_add_pix_small *d_add_pix_small)
{
    volatile u_dpu_add_pix_small add_pix_small;
    add_pix_small.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->add_pix_small.u32) + offset));
    add_pix_small.bits.small_width = d_add_pix_small->small_wdith;
    add_pix_small.bits.small_height = d_add_pix_small->small_big;
    dpu_reg_write((uintptr_t)(&(dpu_reg->add_pix_small.u32) + offset), add_pix_small.u32);

    return;
}

td_void dpu_reg_set_add_pix_big(volatile dpu_regs_type *dpu_reg, td_u32 offset, disp_add_pix_big *d_add_pix_big)
{
    volatile u_dpu_add_pix_big add_pix_big;
    add_pix_big.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->add_pix_big.u32) + offset));
    add_pix_big.bits.big_width = d_add_pix_big->big_width;
    add_pix_big.bits.big_height = d_add_pix_big->big_height;
    dpu_reg_write((uintptr_t)(&(dpu_reg->add_pix_big.u32) + offset), add_pix_big.u32);

    return;
}

td_void dpu_reg_set_add_pix_rect(volatile dpu_regs_type *dpu_reg, td_u32 offset, disp_add_pix_rect *d_add_pix_rect)
{
    volatile u_dpu_add_pix_rect add_pix_rect;
    add_pix_rect.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->add_pix_rect.u32) + offset));
    add_pix_rect.bits.rect_x = d_add_pix_rect->rect_x;
    add_pix_rect.bits.rect_y = d_add_pix_rect->rect_y;
    dpu_reg_write((uintptr_t)(&(dpu_reg->add_pix_rect.u32) + offset), add_pix_rect.u32);

    return;
}

td_void dpu_reg_set_add_pix_tile(volatile dpu_regs_type *dpu_reg, td_u32 offset, disp_add_pix_tile *d_add_pix_tile)
{
    volatile u_dpu_add_pix_tile add_pix_tile;
    add_pix_tile.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->add_pix_tile.u32) + offset));
    add_pix_tile.bits.tile_width = d_add_pix_tile->tile_width;
    add_pix_tile.bits.tile_height = d_add_pix_tile->tile_height;
    dpu_reg_write((uintptr_t)(&(dpu_reg->add_pix_tile.u32) + offset), add_pix_tile.u32);

    return;
}


td_void dpu_reg_set_t2r0_ctrl(volatile dpu_regs_type *dpu_reg, td_u32 offset, disp_t2r0_ctrl *d_t2r0_ctrl)
{
    volatile u_g0_t2r0_ctrl t2r0_ctrl;
    t2r0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_t2r0_ctrl.u32) + offset));
    t2r0_ctrl.bits.t2r0_ck_gt_en = d_t2r0_ctrl->t2r0_ck_gt_en;
    t2r0_ctrl.bits.t2r0_tile_wth = d_t2r0_ctrl->t2r0_tile_wth;
    t2r0_ctrl.bits.t2r0_mode_sel = d_t2r0_ctrl->t2r0_mode_sel;
    t2r0_ctrl.bits.t2r0_cfg_en   = d_t2r0_ctrl->t2r0_cfg_en;
    t2r0_ctrl.bits.t2r0_delay_en = d_t2r0_ctrl->t2r0_delay_en;
    t2r0_ctrl.bits.t2r0_copy_en  = d_t2r0_ctrl->t2r0_copy_en;
    t2r0_ctrl.bits.t2r0_flip_en  = d_t2r0_ctrl->t2r0_flip_en;
    t2r0_ctrl.bits.t2r0_t2r_en   = d_t2r0_ctrl->t2r0_t2r_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_t2r0_ctrl.u32) + offset), t2r0_ctrl.u32);

    return;
}

td_void dpu_reg_set_t2r0_err_clr_t2r0_of_err_clr(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 t2r0_of_err_clr)
{
    volatile u_g0_t2r0_err_clr t2r0_err_clr;
    t2r0_err_clr.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_t2r0_err_clr.u32) + offset));
    t2r0_err_clr.bits.t2r0_of_err_clr = t2r0_of_err_clr;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_t2r0_err_clr.u32) + offset), t2r0_err_clr.u32);

    return;
}

td_void dpu_reg_set_t2r0_err_sta_t2r0_error(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 t2r0_error)
{
    volatile u_g0_t2r0_err_sta t2r0_err_sta;
    t2r0_err_sta.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_t2r0_err_sta.u32) + offset));
    t2r0_err_sta.bits.t2r0_error = t2r0_error;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_t2r0_err_sta.u32) + offset), t2r0_err_sta.u32);

    return;
}

td_void dpu_reg_set_t2r0_debug1_t2r0_debug1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 t2r0_debug1)
{
    volatile u_g0_t2r0_debug1 t2r0_debug01;
    t2r0_debug01.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_t2r0_debug1.u32) + offset));
    t2r0_debug01.bits.t2r0_debug1 = t2r0_debug1;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_t2r0_debug1.u32) + offset), t2r0_debug01.u32);

    return;
}

td_void dpu_reg_set_t2r0_debug2_t2r0_debug2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 t2r0_debug2)
{
    volatile u_g0_t2r0_debug2 t2r0_debug02;
    t2r0_debug02.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_t2r0_debug2.u32) + offset));
    t2r0_debug02.bits.t2r0_debug2 = t2r0_debug2;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_t2r0_debug2.u32) + offset), t2r0_debug02.u32);

    return;
}

td_void dpu_reg_set_t2r0_debug3_t2r0_debug3(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 t2r0_debug3)
{
    volatile u_g0_t2r0_debug3 t2r0_debug03;
    t2r0_debug03.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_t2r0_debug3.u32) + offset));
    t2r0_debug03.bits.t2r0_debug3 = t2r0_debug3;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_t2r0_debug3.u32) + offset), t2r0_debug03.u32);

    return;
}

td_void dpu_reg_set_t2r0_debug4_t2r0_debug4(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 t2r0_debug4)
{
    volatile u_g0_t2r0_debug4 t2r0_debug04;
    t2r0_debug04.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_t2r0_debug4.u32) + offset));
    t2r0_debug04.bits.t2r0_debug4 = t2r0_debug4;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_t2r0_debug4.u32) + offset), t2r0_debug04.u32);

    return;
}

td_void dpu_reg_set_crop0_xy(volatile dpu_regs_type *dpu_reg, td_u32 offset, disp_crop0_xy *d_crop0_xy)
{
    volatile u_g0_crop0_xy crop0_xy;
    crop0_xy.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_crop0_xy.u32) + offset));
    crop0_xy.bits.crop0_en = d_crop0_xy->crop_en;
    crop0_xy.bits.crop0_cfg_en = d_crop0_xy->crop_cfg_en;
    crop0_xy.bits.crop0_x = d_crop0_xy->crop_x;
    crop0_xy.bits.crop0_y = d_crop0_xy->crop_y;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_crop0_xy.u32) + offset), crop0_xy.u32);

    return;
}

td_void dpu_reg_set_vo_mux(volatile dpu_regs_type *dpu_reg, td_u32 offset, disp_vo_mux *d_vo_mux)
{
    volatile u_vo_mux vo_mux;
    vo_mux.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vo_mux.u32) + offset));
    vo_mux.bits.digital_sel = d_vo_mux->digital_sel;
    vo_mux.bits.bt_sel = d_vo_mux->bt_sel;
    vo_mux.bits.lcd_sel = d_vo_mux->lcd_sel;
    vo_mux.bits.mipi_sel = d_vo_mux->mipi_sel;
    dpu_reg_write((uintptr_t)(&(dpu_reg->vo_mux.u32) + offset), vo_mux.u32);

    return;
}

td_void dpu_reg_set_vo_mux_sync(volatile dpu_regs_type *dpu_reg, td_u32 offset, disp_vo_mux_sync *d_vo_mux_sync)
{
    volatile u_vo_mux_sync vo_mux_sync;
    vo_mux_sync.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vo_mux_sync.u32) + offset));
    vo_mux_sync.bits.sync_test_mode = d_vo_mux_sync->sync_test_mode;
    vo_mux_sync.bits.sync_field = d_vo_mux_sync->sync_field;
    vo_mux_sync.bits.sync_vsync = d_vo_mux_sync->sync_vsync;
    vo_mux_sync.bits.sync_hsync = d_vo_mux_sync->sync_hsync;
    vo_mux_sync.bits.sync_dv = d_vo_mux_sync->sync_dv;

    dpu_reg_write((uintptr_t)(&(dpu_reg->vo_mux_sync.u32) + offset), vo_mux_sync.u32);

    return;
}

td_void dpu_reg_set_vo_mux_data_vomux_data(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vomux_data)
{
    volatile u_vo_mux_data vo_mux_data;
    vo_mux_data.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vo_mux_data.u32) + offset));
    vo_mux_data.bits.vomux_data = vomux_data;
    dpu_reg_write((uintptr_t)(&(dpu_reg->vo_mux_data.u32) + offset), vo_mux_data.u32);

    return;
}

td_void dpu_reg_set_intf_lcd_ctrl(volatile dpu_regs_type *dpu_reg, td_u32 offset, disp_intf0_lcd_ctrl *d_intf0_lcd_ctrl)
{
    volatile u_intf_lcd_ctrl intf_lcd_ctrl;
    intf_lcd_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->intf_lcd_ctrl.u32) + offset));
    intf_lcd_ctrl.bits.hdmi_mode = d_intf0_lcd_ctrl->hdmi_mode;
    intf_lcd_ctrl.bits.dfir_en = d_intf0_lcd_ctrl->dfir_en;
    intf_lcd_ctrl.bits.dummy_position_sel = d_intf0_lcd_ctrl->dummy_position_sel;
    intf_lcd_ctrl.bits.lcd_serial_perd = d_intf0_lcd_ctrl->lcd_serial_perd;
    intf_lcd_ctrl.bits.lcd_comp_order = d_intf0_lcd_ctrl->lcd_comp_order;
    intf_lcd_ctrl.bits.lcd_bit_inv = d_intf0_lcd_ctrl->lcd_bit_inv;
    intf_lcd_ctrl.bits.lcd_format = d_intf0_lcd_ctrl->lcd_format;
    dpu_reg_write((uintptr_t)(&(dpu_reg->intf_lcd_ctrl.u32) + offset), intf_lcd_ctrl.u32);

    return;
}

td_void dpu_reg_set_intf_lcd_upd_regup_lcd(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 regup_lcd)
{
    volatile u_intf_lcd_upd intf_lcd_upd;
    intf_lcd_upd.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->intf_lcd_upd.u32) + offset));
    intf_lcd_upd.bits.regup_lcd = regup_lcd;
    dpu_reg_write((uintptr_t)(&(dpu_reg->intf_lcd_upd.u32) + offset), intf_lcd_upd.u32);

    return;
}

td_void dpu_reg_set_intf_lcd_sync_inv(volatile dpu_regs_type *dpu_reg,
    td_u32 offset, disp_intf0_lcd_sync_inv *d_intf0_lcd_sync_inv)
{
    volatile u_intf_lcd_sync_inv intf_lcd_sync_inv;
    intf_lcd_sync_inv.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->intf_lcd_sync_inv.u32) + offset));
    intf_lcd_sync_inv.bits.f_inv = d_intf0_lcd_sync_inv->f_inv;
    intf_lcd_sync_inv.bits.lcd_vs_inv = d_intf0_lcd_sync_inv->vs_inv;
    intf_lcd_sync_inv.bits.lcd_hs_inv = d_intf0_lcd_sync_inv->hs_inv;
    intf_lcd_sync_inv.bits.lcd_dv_inv = d_intf0_lcd_sync_inv->dv_inv;
    dpu_reg_write((uintptr_t)(&(dpu_reg->intf_lcd_sync_inv.u32) + offset), intf_lcd_sync_inv.u32);

    return;
}

td_void dpu_reg_set_lcd_intf_chksum_y_check_sum_lcd0(volatile dpu_regs_type *dpu_reg,
    td_u32 offset, td_u32 check_sum_lcd0)
{
    volatile u_lcd_intf_chksum_y lcd_intf_chksum_y;
    lcd_intf_chksum_y.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->lcd_intf_chksum_y.u32) + offset));
    lcd_intf_chksum_y.bits.check_sum_lcd0 = check_sum_lcd0;
    dpu_reg_write((uintptr_t)(&(dpu_reg->lcd_intf_chksum_y.u32) + offset), lcd_intf_chksum_y.u32);

    return;
}

td_void dpu_reg_set_lcd_intf_chksum_u_check_sum_lcd1(volatile dpu_regs_type *dpu_reg,
    td_u32 offset, td_u32 check_sum_lcd1)
{
    volatile u_lcd_intf_chksum_u lcd_intf_chksum_u;
    lcd_intf_chksum_u.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->lcd_intf_chksum_u.u32) + offset));
    lcd_intf_chksum_u.bits.check_sum_lcd1 = check_sum_lcd1;
    dpu_reg_write((uintptr_t)(&(dpu_reg->lcd_intf_chksum_u.u32) + offset), lcd_intf_chksum_u.u32);

    return;
}

td_void dpu_reg_set_lcd_intf_chksum_v_check_sum_lcd2(volatile dpu_regs_type *dpu_reg,
    td_u32 offset, td_u32 check_sum_lcd2)
{
    volatile u_lcd_intf_chksum_v lcd_intf_chksum_v;
    lcd_intf_chksum_v.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->lcd_intf_chksum_v.u32) + offset));
    lcd_intf_chksum_v.bits.check_sum_lcd2 = check_sum_lcd2;
    dpu_reg_write((uintptr_t)(&(dpu_reg->lcd_intf_chksum_v.u32) + offset), lcd_intf_chksum_v.u32);

    return;
}

td_void dpu_reg_set_mac_wchn_prio_wbc_prio(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 wbc_prio)
{
    volatile u_mac_wchn_prio mac_wchn_prio;
    mac_wchn_prio.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->mac_wchn_prio.u32) + offset));
    mac_wchn_prio.bits.wbc_prio = wbc_prio;
    dpu_reg_write((uintptr_t)(&(dpu_reg->mac_wchn_prio.u32) + offset), mac_wchn_prio.u32);

    return;
}

td_void dpu_reg_set_wbc_dhd_ctrl(volatile dpu_regs_type *dpu_reg, td_u32 offset, disp_wbc_dhd_ctrl *d_wbc_dhd_ctrl)
{
    volatile u_wbc_dhd_ctrl wbc_dhd_ctrl;
    wbc_dhd_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->wbc_dhd_ctrl.u32) + offset));
    wbc_dhd_ctrl.bits.des_en = d_wbc_dhd_ctrl->des_en;
    wbc_dhd_ctrl.bits.des_clk_en = d_wbc_dhd_ctrl->des_clk_en;
    wbc_dhd_ctrl.bits.des_fast_req = d_wbc_dhd_ctrl->des_fast_req;
    wbc_dhd_ctrl.bits.des_h_scan_ord = d_wbc_dhd_ctrl->des_h_scan_ord;
    wbc_dhd_ctrl.bits.des_v_scan_ord = d_wbc_dhd_ctrl->des_v_scan_ord;
    wbc_dhd_ctrl.bits.des_alpha_range = d_wbc_dhd_ctrl->des_alpha_range;
    wbc_dhd_ctrl.bits.des_rgb_round = d_wbc_dhd_ctrl->des_rgb_round;
    wbc_dhd_ctrl.bits.des_premult_en = d_wbc_dhd_ctrl->des_premult_en;
    wbc_dhd_ctrl.bits.des_argb_order = d_wbc_dhd_ctrl->des_argb_order;
    wbc_dhd_ctrl.bits.des_fmt = d_wbc_dhd_ctrl->des_fmt;
#ifdef SUPPORT_GPU_QSPI
    wbc_dhd_ctrl.bits.des_hold_mode = d_wbc_dhd_ctrl->des_hold_mode;
#endif
    dpu_reg_write((uintptr_t)(&(dpu_reg->wbc_dhd_ctrl.u32) + offset), wbc_dhd_ctrl.u32);

    return;
}

td_void dpu_reg_set_wbc_dhd_addr_des_addr(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 des_addr)
{
    volatile u_wbc_dhd_addr wbc_dhd_addr;
    wbc_dhd_addr.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->wbc_dhd_addr.u32) + offset));
    wbc_dhd_addr.bits.des_addr = des_addr;
    dpu_reg_write((uintptr_t)(&(dpu_reg->wbc_dhd_addr.u32) + offset), wbc_dhd_addr.u32);

    return;
}

td_void dpu_reg_set_wbc_dhd_stride_des_stride(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 des_stride)
{
    volatile u_wbc_dhd_stride wbc_dhd_stride;
    wbc_dhd_stride.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->wbc_dhd_stride.u32) + offset));
    wbc_dhd_stride.bits.des_stride = des_stride;
    dpu_reg_write((uintptr_t)(&(dpu_reg->wbc_dhd_stride.u32) + offset), wbc_dhd_stride.u32);

    return;
}

td_void dpu_reg_set_wbc_dhd_imgsize_des_wh(volatile dpu_regs_type *dpu_reg,
    td_u32 offset, td_u32 des_width, td_u32 des_height)
{
    volatile u_wbc_dhd_imgsize wbc_dhd_imgsize;
    wbc_dhd_imgsize.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->wbc_dhd_imgsize.u32) + offset));
    wbc_dhd_imgsize.bits.des_width = des_width - 1;
    wbc_dhd_imgsize.bits.des_height = des_height - 1;
    dpu_reg_write((uintptr_t)(&(dpu_reg->wbc_dhd_imgsize.u32) + offset), wbc_dhd_imgsize.u32);

    return;
}

td_void dpu_reg_set_wbc_dhd_cmd_ctrl(volatile dpu_regs_type *dpu_reg,
    td_u32 offset, disp_wbc_dhd_cmd_ctrl *d_wbc_dhd_cmd_ctrl)
{
    volatile u_wbc_dhd_cmd_ctrl wbc_dhd_cmd_ctrl;
    wbc_dhd_cmd_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->wbc_dhd_cmd_ctrl.u32) + offset));
    wbc_dhd_cmd_ctrl.bits.des_cmd_ctrl_en = d_wbc_dhd_cmd_ctrl->des_cmd_ctrl_en;
    wbc_dhd_cmd_ctrl.bits.des_cmd_interval = d_wbc_dhd_cmd_ctrl->des_cmd_interval;
    dpu_reg_write((uintptr_t)(&(dpu_reg->wbc_dhd_cmd_ctrl.u32) + offset), wbc_dhd_cmd_ctrl.u32);

    return;
}


td_void dpu_reg_set_dhd_glob_ctrl_wbc_bind_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 wbc_bind_mode)
{
    volatile u_dhd_glob_ctrl dhd_glob_ctrl;
    dhd_glob_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd_glob_ctrl.u32) + offset));
    dhd_glob_ctrl.bits.wbc_bind_mode = wbc_bind_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd_glob_ctrl.u32) + offset), dhd_glob_ctrl.u32);

    return;
}

td_void dpu_reg_set_mac_burst_type(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 burst_type)
{
    volatile u_mac_ctrl mac_ctrl;
    mac_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->mac_ctrl.u32) + offset));
    mac_ctrl.bits.burst_type = burst_type;
    dpu_reg_write((uintptr_t)(&(dpu_reg->mac_ctrl.u32) + offset), mac_ctrl.u32);
    return;
}

td_void dpu_reg_set_des_dummy_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 dummy_en)
{
    volatile u_wbc_dhd_dummy_ctrl wbc_dhd_dummy_ctrl;
    wbc_dhd_dummy_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->wbc_dhd_dummy_ctrl.u32) + offset));
    wbc_dhd_dummy_ctrl.bits.des_dummy_en = dummy_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->wbc_dhd_dummy_ctrl.u32) + offset), wbc_dhd_dummy_ctrl.u32);

    return;
}

td_void dpu_reg_set_des_chksum_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 des_chksum_en)
{
    volatile u_wbc_dhd_chksum_ctrl wbc_dhd_chksum_ctrl;
    wbc_dhd_chksum_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->wbc_dhd_chksum_ctrl.u32) + offset));
    wbc_dhd_chksum_ctrl.bits.des_chksum_en = des_chksum_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->wbc_dhd_chksum_ctrl.u32) + offset), wbc_dhd_chksum_ctrl.u32);

    return;
}
td_void dpu_reg_set_pack_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pack_en)
{
    volatile u_dhd_glob_ctrl dhd_glob_ctrl;
    dhd_glob_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd_glob_ctrl.u32) + offset));
    dhd_glob_ctrl.bits.pack_en = pack_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd_glob_ctrl.u32) + offset), dhd_glob_ctrl.u32);
    return;
}
