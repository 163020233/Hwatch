/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-07-17
 */

#include "pdt_com.h"
#include "vfmw_define.h"
#include "vfmw_sys.h"
#include "vctrl.h"

#define VFMW_WIDTH_FHD 1920
#define VFMW_HEIGHT_FHD 1080
#define VFMW_WIDTH_4K 3840
#define VFMW_HEIGHT_4K 2160
#define VFMW_WIDTH_8K 7680
#define VFMW_HEIGHT_8K 4320
#define VFMW_BIT_DEPTH_8 8
#define VFMW_BIT_DEPTH_10 10
#define VFMW_FRM_RATE_30 30
#define VFMW_FRM_RATE_60 60
#define VFMW_FRM_RATE_100 100
#define VFMW_FRM_RATE_120 120
#define VFMW_SEG_BUF_MAX (1024 * 1024)
#define VFMW_SEG_BUF_MIN (5 * 1024)

static vdh_glb_reg_info g_vdh_glb_reg = {0};
static td_u32 g_pdt_init = 0;

static td_void pdt_get_slot_wh(mem_req_info *req)
{
    const td_u32 alignw = 64;
    td_u32 alignh = 32;

    if (req->vid_std == VFMW_H264 ||
        req->vid_std == VFMW_HEVC ||
        req->vid_std == VFMW_AVS2 ||
        req->vid_std == VFMW_VP9 ||
        req->vid_std == VFMW_REAL8 ||
        req->vid_std == VFMW_REAL9) {
        alignh = 64; /* 64 align */
    }

    req->slot_width = vfmw_align_up(req->dec_width, alignw);
    req->slot_height = vfmw_align_up(req->dec_height, alignh);

    return;
}

static td_s32 pdt_get_disp_bit_depth(const mem_req_info *req, td_bool compress)
{
    td_s8 disp_bit_depth = 8;

    vfmw_unused(req);
    vfmw_unused(compress);
    return disp_bit_depth;
}

static td_u32 pdt_get_dec_bit_depth(const mem_req_info *req)
{
    return (req->bit_depth > 8) ? 10 : 8; /* 8 :a number 10 :a number 8 :a number */
}

static td_s32 pdt_is_share_fs(vfmw_vid_std vid_std)
{
    if (vid_std == VFMW_VP6 || vid_std == VFMW_VP6A || vid_std == VFMW_VP6F || vid_std == VFMW_VP9 ||
        vid_std == VFMW_MPEG2 || vid_std == VFMW_MPEG4 || vid_std == VFMW_H263 || vid_std == VFMW_SORENSON ||
        vid_std == VFMW_VC1) {
        return 1;
    }

    return 0;
}

static td_bool pdt_get_second_path_enable(const mem_req_info *req)
{
    vfmw_unused(req);
    return TD_FALSE;
}

static td_void pdt_get_dec_mem(mem_req_info *req, mem_need_info *need, td_bool is_max_buf_size_en)
{
    const td_u32 align_value = 64;
    td_u32 width, height, bit_depth, dec_num, size_2bit, cal_size_width, cal_size_height;
    vctrl_dflt *dflt = vctrl_get_dflt();

    dec_num = req->need_dec_num + req->extra_fs_num;
    bit_depth = pdt_get_dec_bit_depth(req);
    need->dec_info.bit_depth = (td_u8)bit_depth;

    pdt_get_slot_wh(req);
    if (is_max_buf_size_en == TD_TRUE) {
        width = vfmw_align_up(req->max_dec_width, align_value);
        height = vfmw_align_up(req->max_dec_height, align_value);
    } else {
        width = (dflt->fix_frame_width != -1) ? (td_u32)dflt->fix_frame_width : req->slot_width;
        height = (dflt->fix_frame_height != -1) ? (td_u32)dflt->fix_frame_height : req->slot_height;
    }
    need->dec_info.dec_num = (dflt->fix_frame_num != -1) ? (td_u32)dflt->fix_frame_num : dec_num;

    cal_size_width = width; /* inner alloc type used dec width */
    cal_size_height = height;
    vfmw_check(cal_size_width < VFMW_INT16_MAX);
    need->dec_info.dec_stride = cal_size_width;
    need->dec_info.dec_size = cal_size_width * cal_size_height * 3 / 2; /* 3 2 yuv bit */
    size_2bit = (bit_depth == 10) ? need->dec_info.dec_size / 4 : 0; /* bitdepth 10, 2bit size = size / 4 */

    need->dec_info.dec_size += size_2bit;
    need->dec_info.dec_width = width;
    need->dec_info.dec_height = height;

    if (need->dec_info.compress_en == 1) {
        need->dec_info.head_stride = vfmw_align_up(width, 2048) >> 5; /* 2048 :align 5 :offset */
        vfmw_check(vfmw_align_up(height, 64) < VFMW_MAX_UINT16); /* 64 align */
        vfmw_check(need->dec_info.head_stride < VFMW_MAX_UINT32);
        need->dec_info.head_size = (vfmw_align_up(height, 64) * need->dec_info.head_stride) >> 2; /* 64 2 :a number */
        vfmw_check(need->dec_info.head_size < VFMW_MAX_UINT32);
        need->dec_info.dec_size += need->dec_info.head_size * 3 / 2; /* 3 :a number 2 :a number */
    } else {
        need->dec_info.head_stride = 0;
        need->dec_info.head_size = 0;
    }

#ifdef VFMW_MODULE_LOWDLY_SUPPORT
    need->dec_info.line_num_offset = need->dec_info.dec_size;
    need->dec_info.dec_size += 32; /* 32 :a number */
#endif

    return;
}

static td_void pdt_get_pmv_size(const mem_req_info *req, mem_need_info *need,
    td_u32 width, td_u32 height)
{
    td_u32 mult_coef, divi_coef, mv_width, mv_height;

    mv_width = vfmw_align_up(width, 64); /* 64 :a number */
    mv_height = vfmw_align_up(height, 64); /* 64 :a number */

    if (req->vid_std == VFMW_HEVC) {
        mult_coef = 2;  /* 2 :logic required */
        divi_coef = 16; /* 16 :a number */
    } else {
        mult_coef = 5; /* 5 :logic required */
        divi_coef = 16; /* 16 :logic required */
    }

    need->pmv_size = mv_width * mv_height * mult_coef / divi_coef;
}

static td_void pdt_get_pmv_mem(const mem_req_info *req, mem_need_info *need, td_bool is_max_buf_size_en)
{
    td_u32 width, height;
    td_u32 align_value = 64;
    vctrl_dflt *dflt = vctrl_get_dflt();

    if (is_max_buf_size_en == TD_TRUE) {
        width = vfmw_align_up(req->max_dec_width, align_value);
        height = vfmw_align_up(req->max_dec_height, align_value);
    } else {
        width = (dflt->fix_frame_width != -1) ? (td_u32)dflt->fix_frame_width : req->slot_width;
        height = (dflt->fix_frame_height != -1) ? (td_u32)dflt->fix_frame_height : req->slot_height;
    }

    need->pmv_num = (dflt->fix_frame_num != -1) ? (td_u32)dflt->fix_frame_num : req->need_pmv_num;

    pdt_get_pmv_size(req, need, width, height);

    /* Fix PMV prefetch error */
    need->pmv_size += 256; /* 256 :a number */

    return;
}

static td_void pdt_get_disp_mem(const mem_req_info *req, mem_need_info *need, td_bool is_need_disp)
{
    td_u32 width, height;
    vctrl_dflt *dflt = vctrl_get_dflt();

    width = (dflt->fix_frame_width != -1) ? (td_u32)dflt->fix_frame_width : req->slot_width;
    height = (dflt->fix_frame_height != -1) ? (td_u32)dflt->fix_frame_height : req->slot_height;

    need->disp_info.is_disp_need = is_need_disp;
    if (is_need_disp == TD_TRUE) {
        need->disp_info.is_disp_need = 1;
        need->disp_info.disp_num = req->extra_fs_num + req->need_disp_num;

        if (req->alloc_type == 1) {
            need->disp_info.disp_num += 2; /* 2 :a number */
        }
        need->disp_info.compress_en = TD_FALSE;
        need->disp_info.bit_depth = (td_u8)pdt_get_disp_bit_depth(req, need->disp_info.compress_en);
        /* 3 :a number 2 :a number 8 :a number */
        need->disp_info.disp_size =
            req->disp_slot_width * req->disp_slot_height * 0x3 / 0x2 * need->disp_info.bit_depth / 0x8;
        need->disp_info.disp_width = req->disp_width;
        /* height must not allign */
        need->disp_info.disp_height = req->disp_height;
        need->disp_info.disp_stride = req->disp_slot_width * need->disp_info.bit_depth / 0x8;

        if (need->disp_info.compress_en == 1) {
            need->disp_info.head_stride = vfmw_align_up(width, 2048) >> 5; /* 2048 :a number 5 :a number */
            vfmw_check(vfmw_align_up(height, 64) < VFMW_MAX_UINT16); /* 64 align */
            vfmw_check(need->disp_info.head_stride < VFMW_MAX_UINT32);
            need->disp_info.head_size = (vfmw_align_up(height, 64) * need->disp_info.head_stride) >> 2; /* 64 2 num */
            vfmw_check(need->disp_info.head_size < VFMW_MAX_UINT32);
            need->disp_info.disp_size += need->disp_info.head_size * 3 / 2; /* 3 :a number 2 :a number */
        } else {
            need->disp_info.head_stride = 0;
            need->disp_info.head_size = 0;
        }

#ifdef VFMW_MODULE_LOWDLY_SUPPORT
        need->disp_info.disp_size += 32; /* 32 :a number */
#endif
        if (pdt_is_share_fs(req->vid_std) == 1) {
            need->dec_info.dec_num = need->disp_info.disp_num =
                vfmw_max(need->dec_info.dec_num, need->disp_info.disp_num);
        }
    }

    return;
}

static td_void pdt_get_attach_mem(const mem_req_info *req, mem_need_info *need, td_bool is_need_disp)
{
    td_bool is_need_attach;
    is_need_attach = pdt_need_attach_mem(req->vid_std, need);

    need->attach = ATTACH_NONE;
    if (is_need_attach || need->sec_size != 0) {
        if (is_need_disp == TD_TRUE) {
            need->attach = ATTACH_DISP;
        } else {
            need->attach = ATTACH_DEC;
        }
    }
}

static td_void pdt_set_dec_compress(mem_req_info *req, mem_need_info *need)
{
    vfmw_unused(req);
    need->dec_info.compress_en = 0;
}

td_s32 pdt_get_mem_req(mem_req_info *req, mem_need_info *need)
{
    td_bool is_need_disp;
    mem_need_info need_max;
    mem_req_info req_max;

    vfmw_check_ret(req != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(need != TD_NULL, EXT_FAILURE);

    is_need_disp = pdt_get_second_path_enable(req);

    need->linear_en = TD_TRUE;
    pdt_set_dec_compress(req, need);

    pdt_get_dec_mem(req, need, TD_FALSE);

    pdt_get_pmv_mem(req, need, TD_FALSE);

    /* Secure attachment memory */
    pdt_get_attach_mem(req, need, is_need_disp);

    pdt_get_disp_mem(req, need, is_need_disp);

    if (req->is_max_buf_size_en == TD_TRUE) {
        (td_void)memcpy_s(&need_max, sizeof(mem_need_info), need, sizeof(mem_need_info));
        (td_void)memcpy_s(&req_max, sizeof(mem_req_info), req, sizeof(mem_req_info));
        pdt_get_dec_mem(&req_max, &need_max, TD_TRUE);
        if (need_max.dec_info.dec_size > need->dec_info.dec_size) {
            need->dec_info.dec_size = need_max.dec_info.dec_size;
        }
        pdt_get_pmv_mem(&req_max, &need_max, TD_TRUE);
        if (need_max.pmv_size > need->pmv_size) {
            need->pmv_size = need_max.pmv_size;
        }
    }

    return EXT_SUCCESS;
}

td_s32 pdt_get_dps_num(vfmw_vid_std std, td_s32 *num)
{
    td_s32 dps_num = 1;

    vfmw_check_ret(num != TD_NULL, EXT_FAILURE);

    // vp9 not support parallel
    if (std == VFMW_HEVC || std == VFMW_H264 ||
        std == VFMW_AVS2 || std == VFMW_AVS3 ||
        std == VFMW_AV1 || std == VFMW_MPEG2) {
        dps_num = MAX_DPS_NUM;
    }

    *num = dps_num;

    return EXT_SUCCESS;
}

static td_void pdt_get_dec_cap_new1(vfmw_cap *cap, td_s32 *cnt)
{
    td_s32 std_cnt;

    std_cnt = *cnt;

#ifdef VFMW_USER_SUPPORT
    cap->max_width[std_cnt] = MJPEG_MAX_PIX_WIDTH;
    cap->max_height[std_cnt] = MJPEG_MAX_PIX_HEIGHT;
    cap->max_frm_rate[std_cnt] = VFMW_FRM_RATE_60;
    cap->supported_std[std_cnt++] = VFMW_MJPEG;
#endif

#ifdef VFMW_H264_SUPPORT
    cap->max_width[std_cnt] = H264_MAX_PIX_WIDTH;
    cap->max_height[std_cnt] = H264_MAX_PIX_HEIGHT;
    cap->max_frm_rate[std_cnt] = VFMW_FRM_RATE_60;
    cap->supported_std[std_cnt++] = VFMW_H264;
#endif

#ifdef VFMW_HEVC_SUPPORT
    cap->max_width[std_cnt] = HEVC_MAX_PIX_WIDTH;
    cap->max_height[std_cnt] = HEVC_MAX_PIX_HEIGHT;
    cap->max_frm_rate[std_cnt] = VFMW_FRM_RATE_60;
    cap->supported_std[std_cnt++] = VFMW_HEVC;
#endif

    *cnt = std_cnt;

    return;
}

td_void air_dec_get_cap(vfmw_air_cap *air_cap)
{
    vfmw_check(air_cap != TD_NULL);
    air_cap->is_support_air_dec = 0;
    air_cap->air_version = 0;
}

td_s32 pdt_get_dec_cap(vfmw_cap *cap)
{
    td_s32 std_cnt;

    vfmw_check_ret(cap != TD_NULL, EXT_FAILURE);

    vfmw_check_sec_func(memset_s(cap, sizeof(vfmw_cap), 0, sizeof(vfmw_cap)));

    std_cnt = 0;
    cap->max_chan_num = VFMW_CHAN_NUM;
    cap->max_bit_rate = 50; /* 50 :a number */
    cap->frm_layout_cap = VFMW_FRM_LAYOUT_CAP_LINEAR;
    cap->bit_depth_cap = VFMW_BITDEPTH_CAP_DEFAULT;

    pdt_get_dec_cap_new1(cap, &std_cnt);

    return EXT_SUCCESS;
}

td_void pdt_get_max_width_height(td_s32 chan_id, td_u32 *dec_max_width, td_u32 *dec_max_height)
{
    td_s32 ret;

    ret = vctrl_get_max_dec_width_height(chan_id, dec_max_width, dec_max_height);
    if (ret != EXT_SUCCESS) {
        *dec_max_width = H264_MAX_PIX_WIDTH;
        *dec_max_height = H264_MAX_PIX_HEIGHT;
    }
    return;
}

td_s32 pdt_calc_seg_buf_size(td_s32 chan_id)
{
    td_float mult;
    td_u32 max_w, max_h;
    const td_s32 fhd_seg_size = 2 * 1024 * 1024; /* 2M */

    td_s32 seg_buf_size;

    pdt_get_max_width_height(chan_id, &max_w, &max_h);
    mult = ((td_float)max_w * (td_float)max_h) / (1920 * 1080); /* 1920 1080 FHD */
    seg_buf_size = (td_s32)(fhd_seg_size * mult);

    return seg_buf_size;
}


td_s32 pdt_get_seg_buf_size(const td_void *args)
{
    vfmw_vid_std std = VFMW_STD_START;
    td_s32 size;
    vfmw_chan_option *opt = TD_NULL;

    if (args != TD_NULL) {
        opt = (vfmw_chan_option *)(args);
        std = opt->vid_std;
    }

    switch (std) {
        case VFMW_HEVC:
        case VFMW_AVS2:
        case VFMW_AVS3:
        case VFMW_H264:
            size = pdt_calc_seg_buf_size(opt->chan_id);
            break;
        case VFMW_AV1:
        case VFMW_VP9:
        case VFMW_VP6:
        case VFMW_VP6A:
        case VFMW_VP6F:
        case VFMW_VP8:
        case VFMW_DIVX3:
        case VFMW_REAL8:
        case VFMW_REAL9:
        case VFMW_MJPEG:
            size = 0;
            break;
        default:
            size = VFMW_SEG_BUF_MIN;
    }

    return size;
}

vdh_glb_reg_info *pdt_get_glb_reg(td_void)
{
    return &g_vdh_glb_reg;
}

td_void pdt_init_mmu_reg_addr(td_u8 *vir_addr)
{
    vfmw_unused(vir_addr);
}

static td_s32 pdt_open_hardware(td_void)
{
    UADDR phy_addr;
    td_u8 *vir_addr = TD_NULL;
    vdh_glb_reg_info *glb_reg;

    glb_reg = pdt_get_glb_reg();
    vfmw_check_ret(os_kmap_reg != TD_NULL, EXT_FAILURE);

    /* sys crg */
    phy_addr = SYS_CRG_REG_PHY_ADDR;
    vir_addr = (td_u8 *)os_kmap_reg(phy_addr, SYS_CRG_REGISTER_LEN);
    if (vir_addr == TD_NULL) {
        dprint(PRN_ERROR, "%s map sys crg Fail\n", __func__);
        return EXT_FAILURE;
    }
    glb_reg->sys_crg_phy = phy_addr;
    glb_reg->sys_crg_vir = vir_addr;

    /* vdh crg */
    phy_addr = VDH_CRG_REG_PHY_ADDR;
    vir_addr = (td_u8 *)os_kmap_reg(phy_addr, VDH_CRG_REGISTER_LEN);
    if (vir_addr == TD_NULL) {
        dprint(PRN_ERROR, "%s map vdh crg fail\n", __func__);
        goto err1;
    }
    glb_reg->vdh_crg_phy = phy_addr;
    glb_reg->vdh_crg_vir = vir_addr;

    /* vdh reg */
    phy_addr = VDM_REG_PHY_ADDR;
    vir_addr = (td_u8 *)os_kmap_reg(phy_addr, VDH_REGISTER_LEN);
    if (vir_addr == TD_NULL) {
        dprint(PRN_ERROR, "%s map vdh reg fail\n", __func__);
        goto err2;
    }
    glb_reg->vdh_reg_phy = phy_addr;
    glb_reg->vdh_reg_vir = vir_addr;

    return EXT_SUCCESS;
err2:
    os_kunmap_reg(glb_reg->vdh_crg_vir);
    glb_reg->vdh_crg_vir = TD_NULL;
err1:
    os_kunmap_reg(glb_reg->sys_crg_vir);
    glb_reg->sys_crg_vir = TD_NULL;
    return EXT_FAILURE;
}

static td_s32 pdt_close_hardware(td_void)
{
    vdh_glb_reg_info *glb_reg = TD_NULL;

    glb_reg = pdt_get_glb_reg();

    /* sys crg */
    os_kunmap_reg(glb_reg->sys_crg_vir);
    glb_reg->sys_crg_vir = TD_NULL;

    /* vdh crg */
    os_kunmap_reg(glb_reg->vdh_crg_vir);
    glb_reg->vdh_crg_vir = TD_NULL;

    /* vdh reg */
    os_kunmap_reg(glb_reg->vdh_reg_vir);
    glb_reg->vdh_crg_vir = TD_NULL;

    return EXT_SUCCESS;
}

td_s32 pdt_init(td_void)
{
    td_s32 ret = EXT_SUCCESS;

    if (g_pdt_init != 0) {
        return EXT_SUCCESS;
    }

    ret = pdt_open_hardware();
    if (ret == EXT_SUCCESS) {
        g_pdt_init = 1;
    }

    return ret;
}

td_s32 pdt_exit(td_void)
{
    td_s32 ret = EXT_SUCCESS;

    if (g_pdt_init == 0) {
        return EXT_SUCCESS;
    }

    ret = pdt_close_hardware();

    g_pdt_init = 0;

    return ret;
}
