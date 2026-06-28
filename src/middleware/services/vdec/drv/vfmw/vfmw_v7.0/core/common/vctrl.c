/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */
#include "vctrl.h"
#include "stm_intf.h"
#include "syn_intf.h"
#include "dec_intf.h"
#include "fsp.h"
#include "voque.h"
#include "dps.h"
#include "usrdata.h"
#include "vfmw_pdt.h"
#include "event.h"
#include "vdec_chr.h"

#define VCTRL_CREATE_OPT_SIZE sizeof(vfmw_chan_option)

/* inner struct define */
typedef td_s32 (*vctrl_handler)(td_s32, td_void *);

typedef struct {
    vfmw_cid cmd;
    vctrl_handler handler;
    td_u32 length;
} vctrl_case;

typedef struct {
    vctrl_chan *chan[VFMW_CHAN_NUM];
    vctrl_dflt dflt;
} vctrl_entry_info;

static vctrl_entry_info g_vctrl_entry;

vctrl_dflt *vctrl_get_dflt(td_void)
{
    return &g_vctrl_entry.dflt;
}

STATIC td_void vctrl_init_dflt(td_void)
{
    vctrl_dflt *dflt = vctrl_get_dflt();

    vfmw_check_sec_func(memset_s(dflt, sizeof(vctrl_dflt), -1, sizeof(vctrl_dflt)));

    dflt->tunnel_line_num = 65; /* 65 : vfmw_min tunnel line number */
}

STATIC td_s32 vctrl_prepare_create_chan(td_s32 chan_id, const td_void *args,
    stm_create_info *stm_param, dec_create_info *dec_param)
{
    const vfmw_chan_option *opt = (vfmw_chan_option *)args;
    vctrl_chan *chan = TD_NULL;

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);

    if (g_vctrl_entry.chan[chan_id] != TD_NULL) {
        dprint(PRN_FATAL, "chan[%d] is already created!\n", chan_id);
        return EXT_FAILURE;
    }

    g_vctrl_entry.chan[chan_id] = os_alloc_vir(0, "vctrl_chan", sizeof(vctrl_chan));
    if (g_vctrl_entry.chan[chan_id] == TD_NULL) {
        dprint(PRN_FATAL, "alloc chan[%d] fail!\n", chan_id);
        return EXT_FAILURE;
    }

    chan = g_vctrl_entry.chan[chan_id];
    (td_void)memset_s(chan, sizeof(vctrl_chan), 0, sizeof(vctrl_chan));

    chan->handle = chan;
    chan->id = chan_id;
    chan->unid = (td_u32)os_get_unid();
    chan->start_time = 0;
    chan->recv_time = 0;
    vfmw_check_sec_func(memset_s(&chan->cfg, sizeof(vfmw_chan_cfg), 0, sizeof(vfmw_chan_cfg)));

    stm_param->is_sec = (opt == TD_NULL) ? 0 : ((opt->is_sec_mode != 0) ? 1 : 0);
    stm_param->seg_buf_size = (opt == TD_NULL) ? 0 : opt->seg_buf_size;

    dec_param->adpt_type = (opt == TD_NULL) ? 0 : opt->adpt_type;

    return EXT_SUCCESS;
}

STATIC td_void vctrl_post_create_chan(td_s32 chan_id, const td_void *args)
{
    vctrl_chan *chan = vctrl_get_chan(chan_id);

    if (chan != TD_NULL && args != TD_NULL) {
        *(td_s32 *)args = chan_id;
        vfmw_check_sec_func(memcpy_s(&chan->opt, sizeof(chan->opt), args, sizeof(vfmw_chan_option)));
    }
}

vctrl_chan *vctrl_get_chan(td_s32 chan_id)
{
    if (chan_id < 0 || chan_id >= VFMW_CHAN_NUM) {
        return TD_NULL;
    }

    if (g_vctrl_entry.chan[chan_id] == TD_NULL) {
        return TD_NULL;
    }

    return g_vctrl_entry.chan[chan_id];
}

STATIC td_s32 vctrl_check_mem(td_void)
{
#ifdef VFMW_CHECK_MEM_SUPPORT
    td_s32 ret;
    td_bool mem_err;

    vfmw_check_ret(os_check_mem != TD_NULL, EXT_FAILURE);
    ret = os_check_mem(&mem_err);
    vfmw_check_ret((ret == OSAL_OK) && (mem_err == 0), EXT_FAILURE);
#endif

    return EXT_SUCCESS;
}

STATIC td_s32 vctrl_destroy_chan_com(td_s32 chan_id, vctrl_chan *chan)
{
    td_s32 err_cnt = 0;

    vfmw_unused(chan);

    if (voq_destroy(chan_id) != VOQ_OK) {
        dprint(PRN_FATAL, "VOQ Destroy Error \n");
        err_cnt++;
    }

    if (fsp_destroy(chan_id) != FSP_OK) {
        dprint(PRN_FATAL, "FSP Destroy Error \n");
        err_cnt++;
    }

    if (dps_destroy(chan_id) != DPS_OK) {
        dprint(PRN_FATAL, "DPS Destroy Error \n");
        err_cnt++;
    }

#ifdef VFMW_USD_SUPPORT
    if (usd_destroy(chan_id) != USD_OK) {
        dprint(PRN_FATAL, "UserData Destroy Error \n");
        err_cnt++;
    }
#endif

    vfmw_check_ret(vctrl_check_mem() == EXT_SUCCESS, err_cnt);

    return err_cnt;
}

STATIC td_s32 vctrl_destroy_chan(td_s32 chan_id, td_void *args)
{
    td_s32 ret;
    td_s32 err_cnt = 0;
    vctrl_chan *chan = TD_NULL;

    vfmw_unused(args);

    chan = vctrl_get_chan(chan_id);
    if (chan == TD_NULL || chan->handle == TD_NULL) {
        dprint(PRN_FATAL, "chan or chan->handle is TD_NULL \n");
        return EXT_FAILURE;
    }

    ret = dec_control(chan_id, DEC_CID_DESTROY_CHAN, TD_NULL);
    if (ret != DEC_OK) {
        dprint(PRN_FATAL, "DEC Destroy Error \n");
        err_cnt++;
    }

    ret = syn_control(chan_id, SYN_CID_DESTROY_CHAN, TD_NULL);
    if (ret != SYN_OK) {
        dprint(PRN_FATAL, "SYN Destroy Error \n");
        err_cnt++;
    }

    ret = stm_control(chan_id, STM_CMD_DESTROY_CHAN, TD_NULL);
    if (ret != STM_OK) {
        dprint(PRN_FATAL, "STM Destroy Error \n");
        err_cnt++;
    }

    err_cnt += vctrl_destroy_chan_com(chan_id, chan);

    chan->handle = TD_NULL;

    os_free_vir(chan);
    g_vctrl_entry.chan[chan_id] = TD_NULL;

    if (err_cnt > 0) {
        ret = EXT_FAILURE;
        dprint(PRN_FATAL, "%s chan %d error,  %d moudles failed \n", __func__, chan_id, err_cnt);
    }

    return ret;
}

STATIC td_s32 vctrl_create_chan(td_s32 chan_id, td_void *args)
{
    stm_create_info stm_param = { 0 };
    dec_create_info dec_param = { 0 };

    if (vctrl_prepare_create_chan(chan_id, args, &stm_param, &dec_param) != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    if (fsp_create(chan_id) != FSP_OK) {
        dprint(PRN_FATAL, "fsp_create err\n");
        goto out;
    }

    if (voq_create(chan_id) != VOQ_OK) {
        dprint(PRN_FATAL, "voq_create err\n");
        goto out;
    }

    if (dps_create(chan_id) != DPS_OK) {
        dprint(PRN_FATAL, "dps_create err\n");
        goto out;
    }

#ifdef VFMW_USD_SUPPORT
    if (usd_create(chan_id) != USD_OK) {
        dprint(PRN_FATAL, "usd_create err\n");
        goto out;
    }
#endif

    if (stm_control(chan_id, STM_CMD_CREATE_CHAN, &stm_param) != STM_OK) {
        dprint(PRN_FATAL, "stm_create err\n");
        goto out;
    }

    if (syn_control(chan_id, SYN_CID_CREATE_CHAN, TD_NULL) != SYN_OK) {
        dprint(PRN_FATAL, "syn_create err\n");
        goto out;
    }

    if (dec_control(chan_id, DEC_CID_CREATE_CHAN, &dec_param) != DEC_OK) {
        dprint(PRN_FATAL, "dec_create err\n");
        goto out;
    }

    vctrl_post_create_chan(chan_id, args);

    vfmw_check_ret(vctrl_check_mem() == EXT_SUCCESS, EXT_FAILURE);
    vdec_record_chr_event(chan_id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_CREATE_OK);

    return EXT_SUCCESS;

out:

    if (vctrl_destroy_chan(chan_id, args) != EXT_SUCCESS) {
        dprint(PRN_ERROR, "%s destroy chan fail!\n", __func__);
    }

    return EXT_FAILURE;
}

STATIC td_s32 vctrl_start_chan(td_s32 chan_id, td_void *args)
{
    td_s32 ret;
    vctrl_chan *chan = TD_NULL;

    vfmw_unused(args);
    vfmw_check_ret(os_get_time_us != TD_NULL, EXT_FAILURE);

    chan = vctrl_get_chan(chan_id);
    if (chan == TD_NULL || chan->handle == TD_NULL) {
        dprint(PRN_FATAL, "Vfmw Chan Error \n");
        return EXT_FAILURE;
    }

    ret = stm_control(chan_id, STM_CMD_START_CHAN, TD_NULL);
    if (ret != STM_OK) {
        dprint(PRN_FATAL, "STM Start Error \n");
        return EXT_FAILURE;
    }

    ret = syn_control(chan_id, SYN_CID_START_CHAN, TD_NULL);
    if (ret != SYN_OK) {
        dprint(PRN_FATAL, "SYN Start Error \n");
        return EXT_FAILURE;
    }

    ret = dec_control(chan_id, DEC_CID_START_CHAN, TD_NULL);
    if (ret != DEC_OK) {
        dprint(PRN_FATAL, "DEC Start Error \n");
        return EXT_FAILURE;
    }

    vfmw_check_ret(vctrl_check_mem() == EXT_SUCCESS, EXT_FAILURE);

    if (chan->start_time == 0) {
        chan->start_time = os_get_time_us();
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vctrl_reset_chan(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    vfmw_unused(args);

    ret = dec_control(chan_id, DEC_CID_RESET_CHAN, TD_NULL);
    if (ret != DEC_OK) {
        dprint(PRN_FATAL, "DEC Reset Error \n");
        return EXT_FAILURE;
    }

    ret = syn_control(chan_id, SYN_CID_RESET_CHAN, TD_NULL);
    if (ret != SYN_OK) {
        dprint(PRN_FATAL, "SYN Reset Error \n");
        return EXT_FAILURE;
    }

    ret = stm_control(chan_id, STM_CMD_RESET_CHAN, TD_NULL);
    if (ret != STM_OK) {
        dprint(PRN_FATAL, "STM Reset Error \n");
        return EXT_FAILURE;
    }

    ret = voq_reset(chan_id);
    if (ret != VOQ_OK) {
        dprint(PRN_FATAL, "VOQ Reset Error \n");
        return EXT_FAILURE;
    }

    ret = fsp_reset(chan_id);
    if (ret != FSP_OK) {
        dprint(PRN_FATAL, "FSP Reset Error \n");
        return EXT_FAILURE;
    }

    ret = dps_reset(chan_id);
    if (ret != DPS_OK) {
        dprint(PRN_FATAL, "DPS Reset Error \n");
        return EXT_FAILURE;
    }

#ifdef VFMW_USD_SUPPORT
    ret = usd_reset(chan_id);
    if (ret != USD_OK) {
        dprint(PRN_FATAL, "UserData Reset Error \n");
    }
#endif

    vfmw_check_ret(vctrl_check_mem() == EXT_SUCCESS, EXT_FAILURE);

    return EXT_SUCCESS;
}

#ifdef VFMW_USD_SUPPORT
STATIC td_void vctrl_flush_chan_usd_reset(td_s32 chan_id)
{
    td_s32 ret;

    ret = usd_reset(chan_id);
    if (ret != USD_OK) {
        dprint(PRN_ERROR, "UserData Reset Error \n");
    }
}
#endif

STATIC td_s32 vctrl_flush_chan(td_s32 chan_id, td_void *args)
{
    td_s32 ret;
    vfmw_chan_reset_option rst_opt = {0};
    vfmw_flush_type flush_type;

    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);

    flush_type = *((vfmw_flush_type *)args);
    if (flush_type == VFMW_FLUSH_OUT || flush_type == VFMW_FLUSH_ALL) {
        ret = dec_control(chan_id, DEC_CID_RESET_CHAN, TD_NULL);
        if (ret != DEC_OK) {
            dprint(PRN_FATAL, "DEC Reset Error \n");
            return EXT_FAILURE;
        }

        rst_opt.keep_sps_pps = 1;
        ret = syn_control(chan_id, SYN_CID_RESET_CHAN, &rst_opt);
        if (ret != SYN_OK) {
            dprint(PRN_FATAL, "SYN Reset Error \n");
            return EXT_FAILURE;
        }

        ret = stm_control(chan_id, STM_CMD_RESET_CHAN, TD_NULL);
        if (ret != STM_OK) {
            dprint(PRN_FATAL, "STM Reset Error \n");
            return EXT_FAILURE;
        }

        ret = voq_reset(chan_id);
        if (ret != VOQ_OK) {
            dprint(PRN_FATAL, "VOQ Reset Error \n");
            return EXT_FAILURE;
        }

        ret = fsp_reset(chan_id);
        if (ret != FSP_OK) {
            dprint(PRN_FATAL, "FSP Reset Error \n");
            return EXT_FAILURE;
        }

        ret = dps_reset(chan_id);
        if (ret != DPS_OK) {
            dprint(PRN_FATAL, "DPS Reset Error \n");
            return EXT_FAILURE;
        }

#ifdef VFMW_USD_SUPPORT
        vctrl_flush_chan_usd_reset(chan_id);
#endif
    }

    vfmw_check_ret(vctrl_check_mem() == EXT_SUCCESS, EXT_FAILURE);

    return EXT_SUCCESS;
}

STATIC td_s32 vctrl_flush_post(td_s32 chan_id, td_void *args)
{
    td_s32 ret;
    vctrl_chan *chan = vctrl_get_chan(chan_id);

    vfmw_unused(args);

    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    ret = EXT_FAILURE;

    if (chan->cfg.alloc_type == VFMW_FRM_ALLOC_OUTER) {
        ret = voq_flush(chan_id);
        fsp_flush(chan_id);
    }

    fsp_set_wait_flush(chan_id, 0);

    return (ret == FSP_OK) ? EXT_SUCCESS : EXT_FAILURE;
}

STATIC td_s32 vctrl_config_fsp(const vctrl_chan *chan,
    const vfmw_chan_cfg *chan_cfg)
{
    td_s32 ret;
    fsp_attr fsp_attribute = { 0 };

    ret = fsp_get_attr(chan->id, &fsp_attribute);
    if (ret != FSP_OK) {
        print_func_err(fsp_get_attr, ret);
        return EXT_FAILURE;
    }

    fsp_attribute.perf = (chan_cfg->android_test == 0) ? FSP_HIGH_PERF : FSP_LOW_PERF;
    fsp_attribute.android_test = chan_cfg->android_test;
    fsp_attribute.extra_fs_num = chan_cfg->extra_frame_store_num;
    fsp_attribute.vcmp_mode = chan_cfg->v_cmp_mode;
    fsp_attribute.uvmos_en = chan_cfg->uvmos_enable;
    fsp_attribute.out_err_thr = chan_cfg->chan_err_thr;
    fsp_attribute.disp_bit_depth_mode = chan_cfg->en_bit_depth_cfg;
    fsp_attribute.sec_flag = chan_cfg->sec_flag;
    fsp_attribute.alloc_type = chan_cfg->alloc_type;
    fsp_attribute.frm_layout = chan_cfg->frame_layout;
    fsp_attribute.need_film_grain = chan_cfg->std_ext.av1_ext.need_film_grain;
    fsp_attribute.air_version = chan_cfg->air_dec.air_version;
    fsp_attribute.outer_frm_mode = chan_cfg->outer_frm_mode;
    fsp_attribute.is_max_buf_size_en = chan_cfg->is_max_buf_size_en;
    fsp_attribute.max_dec_width = chan_cfg->max_dec_width;
    fsp_attribute.max_dec_height = chan_cfg->max_dec_height;

    ret = fsp_set_attr(chan->id, &fsp_attribute);
    if (ret != FSP_OK) {
        print_func_err(fsp_set_attr, ret);
        return EXT_FAILURE;
    }
    vdec_record_chr_event(chan->id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_SET_FSP_OK);

    return EXT_SUCCESS;
}

STATIC td_s32 vctrl_config_dps(const vctrl_chan *chan,
    const vfmw_chan_cfg *chan_cfg)
{
    td_s32 ret;
    dps_cfg_info dps_cfg = { 0 };

    dps_cfg.vid_std = chan_cfg->vid_std;
    dps_cfg.max_slc_num = chan_cfg->max_slice_num;
    dps_cfg.alloc_type = chan_cfg->alloc_type;
    dps_cfg.adpt_type = chan->opt.adpt_type;
    ret = dps_config(chan->id, &dps_cfg);
    vfmw_check_ret_print(ret == DPS_OK, EXT_FAILURE, "dps cfg err\n");
    vdec_record_chr_event(chan->id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_SET_DPS_OK);

    return EXT_SUCCESS;
}

STATIC td_s32 vctrl_config_voq(const vctrl_chan *chan,
    const vfmw_chan_cfg *chan_cfg)
{
    td_s32 ret;
    voq_cfg_info voq_cfg = { 0 };

    voq_cfg.frm_fence_enable = chan_cfg->frm_fence_enable;

    ret = voq_config(chan->id, &voq_cfg);
    vfmw_check_ret_print(ret == DPS_OK, EXT_FAILURE, "voq cfg err\n");
    vdec_record_chr_event(chan->id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_SET_VOQ_OK);

    return EXT_SUCCESS;
}

STATIC td_s32 vctrl_config_stm(vctrl_chan *chan, const vfmw_chan_cfg *chan_cfg)
{
    td_s32 ret;
    stm_cfg_info stm_cfg = { 0 };

    stm_cfg.vid_std = chan_cfg->vid_std;
    stm_cfg.priority = chan_cfg->chan_priority;
    stm_cfg.std_ext = chan_cfg->std_ext;
    stm_cfg.sec_flag = chan_cfg->sec_flag;
    stm_cfg.is_tunnel_path = (td_u8)chan_cfg->is_tunneled_path;
    stm_cfg.fill_eop_en = (td_u8)chan_cfg->chan_lowly_enable;
    stm_cfg.half_seg_en = (td_u8)chan_cfg->dec_lowly_enable;
    stm_cfg.adpt_type = chan->opt.adpt_type;

    ret = stm_control(chan->id, STM_CMD_CONFIG_CHAN, &stm_cfg);
    vfmw_check_ret_print(ret == STM_OK, EXT_FAILURE, "stm cfg err\n");

#ifdef VFMW_STREAM_SUPPORT
    if (chan_cfg->private_data != 0) {
        ret = stm_control(chan->id, STM_CMD_CFG_EXT_INTF, uint64_ptr(chan_cfg->private_data));
        vfmw_check_ret_print(ret == STM_OK, EXT_FAILURE, "stm cfg ext err\n");
    }
#endif
    vdec_record_chr_event(chan->id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_SET_STM_OK);

    return EXT_SUCCESS;
}

STATIC td_s32 vctrl_config_syn(const vctrl_chan *chan, const vfmw_chan_cfg *chan_cfg)
{
    td_s32 ret;
    syn_cfg_info syn_cfg = { 0 };

    syn_cfg.adpt_type = chan->opt.adpt_type;
    syn_cfg.vid_std = chan_cfg->vid_std;
    syn_cfg.priority = chan_cfg->chan_priority;
    syn_cfg.dec_mode = chan_cfg->dec_mode;
    syn_cfg.crc_mode = chan_cfg->crc_mode;
    syn_cfg.output_order = chan_cfg->dec_order_output;
    syn_cfg.sed_only_en = (td_u32)chan_cfg->dec_lowly_enable;
    syn_cfg.lowdly_en = chan_cfg->chan_lowly_enable;
    syn_cfg.slc_ldy_en = 0;
    syn_cfg.max_vps_num = chan_cfg->max_vps_num;
    syn_cfg.max_sps_num = chan_cfg->max_sps_num;
    syn_cfg.max_pps_num = chan_cfg->max_pps_num;
    syn_cfg.max_ref_num = chan_cfg->max_ref_frame_num;
    syn_cfg.max_slice_num = chan_cfg->max_slice_num;
    syn_cfg.max_width = chan_cfg->max_width;
    syn_cfg.max_height = chan_cfg->max_height;
    syn_cfg.sec_flag = chan_cfg->sec_flag;
    syn_cfg.out_err_thr = chan_cfg->chan_err_thr;
    syn_cfg.ref_err_thr = chan_cfg->chan_err_thr;
    syn_cfg.air_version = chan_cfg->air_dec.air_version;
    syn_cfg.air_dec_attr.fixed_airdec = chan_cfg->air_dec.fixed_airdec;
    syn_cfg.air_dec_attr.air_version = chan_cfg->air_dec.air_version;
    syn_cfg.air_dec_attr.slice_num = chan_cfg->air_dec.slice_num;
    syn_cfg.air_dec_attr.layer_num = chan_cfg->air_dec.layer_num;
    syn_cfg.air_dec_attr.max_width = chan_cfg->air_dec.max_width;
    syn_cfg.air_dec_attr.max_height = chan_cfg->air_dec.max_height;
    syn_cfg.is_max_buf_size_en = chan_cfg->is_max_buf_size_en;
    syn_cfg.max_dec_width = chan_cfg->max_dec_width;
    syn_cfg.max_dec_height = chan_cfg->max_dec_height;

    ret = syn_control(chan->id, SYN_CID_CONFIG_CHAN, &syn_cfg);
    vfmw_check_ret_print(ret == SYN_OK, EXT_FAILURE, "syn cfg ext err\n");
    vdec_record_chr_event(chan->id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_SET_SYN_OK);

    return EXT_SUCCESS;
}

STATIC td_s32 vctrl_config_dec(const vctrl_chan *chan, const vfmw_chan_cfg *chan_cfg)
{
    td_s32 ret;
    dec_chan_cfg dec_cfg = { 0 };
    vctrl_dflt *dflt = vctrl_get_dflt();
    const vfmw_product_info *pdt_info = pdt_get_product_info();

    dec_cfg.vid_std = chan_cfg->vid_std;
    dec_cfg.priority = chan_cfg->chan_priority;
    dec_cfg.vcmp_mode = chan_cfg->v_cmp_mode;
    dec_cfg.low_delay_en = chan_cfg->module_lowly_enable;
    dec_cfg.uv_order_en = chan_cfg->uv_order_enable;
    dec_cfg.sec_flag = chan_cfg->sec_flag;
    dec_cfg.uvmos_en = chan_cfg->uvmos_enable;
    dec_cfg.out_err_thr = chan_cfg->chan_err_thr;
    dec_cfg.ref_err_thr = chan_cfg->chan_err_thr;
    dec_cfg.tunl_line_num = dflt->tunnel_line_num;
    dec_cfg.disp_bit_depth_mode = chan_cfg->en_bit_depth_cfg;
    dec_cfg.poc_threshold = chan_cfg->poc_threshold;
    dec_cfg.max_core_num = chan_cfg->max_core_num;
    dec_cfg.need_film_grain = chan_cfg->std_ext.av1_ext.need_film_grain;
    dec_cfg.layout = chan_cfg->frame_layout;
    dec_cfg.alloc_type = chan_cfg->alloc_type;
    dec_cfg.air_version = chan_cfg->air_dec.air_version;
    dec_cfg.linear_no_gap_en = chan_cfg->linear_no_gap_en;
    dec_cfg.is_max_buf_size_en = chan_cfg->is_max_buf_size_en;
    dec_cfg.max_dec_width = chan_cfg->max_dec_width;
    dec_cfg.max_dec_height = chan_cfg->max_dec_height;

    if (pdt_info->linear_no_gap_support == TD_FALSE && chan_cfg->linear_no_gap_en == TD_TRUE) {
        dprint(PRN_ALWS, "%s, %d product does not support linear without gap,pls check cfg!\r\n", __func__, __LINE__);
        return EXT_FAILURE;
    }
    ret = dec_control(chan->id, DEC_CID_CONFIG_CHAN, &dec_cfg);
    vfmw_check_ret_print(ret == DEC_OK, EXT_FAILURE, "dec cfg ext err\n");
    vdec_record_chr_event(chan->id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_SET_DEC_OK);

    return EXT_SUCCESS;
}

STATIC td_void vctrl_config_lowldy(vfmw_chan_cfg *chan_cfg)
{
    td_s32 sedldy_en;
    vctrl_dflt *dflt = vctrl_get_dflt();

    if (dflt->mod_lowdelay_en != -1) {
        chan_cfg->module_lowly_enable = (td_u8)dflt->mod_lowdelay_en;
    }

    if (dflt->sedldy_en != -1) {
        chan_cfg->dec_lowly_enable = dflt->sedldy_en;
    }

    sedldy_en = 0;
#ifdef VFMW_H264_SLC_LDY_SUPPORT
    if (chan_cfg->vid_std == VFMW_H264) {
        sedldy_en = chan_cfg->dec_lowly_enable;
    }
#endif

#ifdef VFMW_HEVC_SLC_LDY_SUPPORT
    if (chan_cfg->vid_std == VFMW_HEVC) {
        sedldy_en = chan_cfg->dec_lowly_enable;
    }
#endif

    chan_cfg->dec_lowly_enable = sedldy_en;
}

STATIC td_s32 vctrl_config_air_dec(const vfmw_chan_cfg *cfg)
{
    vfmw_air_cap air_cap = { 0 };

    air_dec_get_cap(&air_cap);

    if (cfg->air_dec.air_version > air_cap.air_version) {
        dprint(PRN_FATAL, "cfg air_dec %d out of supported range [0, %d]\n", cfg->air_dec, air_cap.air_version);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_void vctrl_set_dflt_config(vfmw_chan_cfg * chan_cfg, vctrl_dflt *dflt)
{
    chan_cfg->uvmos_enable = chan_cfg->uvmos_enable;
    chan_cfg->linear_no_gap_en = (dflt->linear_no_gap_en != -1) ?
        (dflt->linear_no_gap_en) : (chan_cfg->linear_no_gap_en);
    chan_cfg->dec_mode = (dflt->dec_mode != -1) ? (vfmw_dec_mode)(dflt->dec_mode) : (chan_cfg->dec_mode);
    chan_cfg->dec_order_output = (dflt->out_order != -1) ? (dflt->out_order) : (chan_cfg->dec_order_output);
    chan_cfg->chan_err_thr = (dflt->err_threshold != -1) ? (dflt->err_threshold) : (chan_cfg->chan_err_thr);
    chan_cfg->v_cmp_mode = (dflt->cmp_mode != -1) ? ((vfmw_cmp)(dflt->cmp_mode)) : (chan_cfg->v_cmp_mode);
    chan_cfg->frame_layout = (dflt->frm_layout != -1) ? ((vfmw_frm_layout)dflt->frm_layout) : (chan_cfg->frame_layout);
    chan_cfg->v_cmp_mode = (chan_cfg->frame_layout == VFMW_FRM_LAYOUT_LINEAR) ? VFMW_CMP_OFF : chan_cfg->v_cmp_mode;
}

STATIC td_s32 vctrl_config_chan(td_s32 chan_id, td_void *args)
{
    vctrl_chan *chan = TD_NULL;
    vfmw_chan_cfg *chan_cfg = TD_NULL;
    vctrl_dflt *dflt = vctrl_get_dflt();

    vfmw_check_ret_print(args != TD_NULL, EXT_FAILURE, "args null\n");
    chan = vctrl_get_chan(chan_id);
    vfmw_check_ret_print(chan != TD_NULL, EXT_FAILURE, "chan null\n");
    vfmw_check_ret_print(chan->handle != TD_NULL, EXT_FAILURE, "handle not null\n");

    chan_cfg = (vfmw_chan_cfg *)args;

    vctrl_set_dflt_config(chan_cfg, dflt);

    vctrl_config_lowldy(chan_cfg);

    chan_cfg->air_dec.air_version = (td_u32)(chan_cfg->air_dec.air_version);
    if (vctrl_config_air_dec(chan_cfg) != EXT_SUCCESS) {
        dprint(PRN_FATAL, "air dec cfg err\n");
        return EXT_FAILURE;
    }

    if (vctrl_config_fsp(chan, chan_cfg) != EXT_SUCCESS) {
        dprint(PRN_FATAL, "fsp cfg err!\n");
        return EXT_FAILURE;
    }

    if (vctrl_config_dps(chan, chan_cfg) != EXT_SUCCESS) {
        dprint(PRN_FATAL, "dps cfg err!\n");
        return EXT_FAILURE;
    }

    if (vctrl_config_voq(chan, chan_cfg) != EXT_SUCCESS) {
        dprint(PRN_FATAL, "voq cfg err!\n");
        return EXT_FAILURE;
    }

    if (vctrl_config_stm(chan, chan_cfg) != EXT_SUCCESS) {
        dprint(PRN_FATAL, "stm cfg err!\n");
        return EXT_FAILURE;
    }

    if (vctrl_config_syn(chan, chan_cfg) != EXT_SUCCESS) {
        dprint(PRN_FATAL, "syn cfg err!\n");
        return EXT_FAILURE;
    }

    if (vctrl_config_dec(chan, chan_cfg) != EXT_SUCCESS) {
        dprint(PRN_FATAL, "dec cfg err!\n");
        return EXT_FAILURE;
    }

    vfmw_check_sec_func(memcpy_s(&chan->cfg, sizeof(chan->cfg), chan_cfg, sizeof(vfmw_chan_cfg)));

    return EXT_SUCCESS;
}

STATIC td_s32 vctrl_get_chan_cfg(td_s32 chan_id, td_void *args)
{
    vctrl_chan *chan = TD_NULL;

    if (args == TD_NULL) {
        dprint(PRN_FATAL, "%s invalid args!\n", __func__);
        return EXT_FAILURE;
    }

    chan = vctrl_get_chan(chan_id);
    if ((chan == TD_NULL) || (chan->handle == TD_NULL)) {
        dprint(PRN_FATAL, "chan %d or handle is invalid!\n", chan_id);
        return EXT_FAILURE;
    }

    vfmw_check_sec_func(memcpy_s(args, sizeof(vfmw_chan_cfg), &chan->cfg, sizeof(chan->cfg)));

    return EXT_SUCCESS;
}

STATIC td_s32 vctrl_stop_chan(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    vfmw_unused(args);

    ret = stm_control(chan_id, STM_CMD_STOP_CHAN, TD_NULL);
    if (ret != STM_OK) {
        dprint(PRN_FATAL, "STM Stop Error \n");
        return EXT_FAILURE;
    }

    ret = syn_control(chan_id, SYN_CID_STOP_CHAN, TD_NULL);
    if (ret != SYN_OK) {
        dprint(PRN_FATAL, "SYN Stop Error \n");
        return EXT_FAILURE;
    }

    ret = dec_control(chan_id, DEC_CID_STOP_CHAN, TD_NULL);
    if (ret != DEC_OK) {
        dprint(PRN_FATAL, "DEC Stop Error \n");
        return EXT_FAILURE;
    }

    vfmw_check_ret(vctrl_check_mem() == EXT_SUCCESS, EXT_FAILURE);

    return EXT_SUCCESS;
}

STATIC td_s32 vctrl_set_stream_intf(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    ret = stm_control(chan_id, STM_CMD_CFG_EXT_INTF, args);
    ret = (ret == STM_OK) ? EXT_SUCCESS : EXT_FAILURE;

    return ret;
}

STATIC td_void vctrl_get_stm_chan_state(vfmw_chan_info *state, const stm_info *stm)
{
    state->buffered_stream_num = stm->fresh_seg_num;
    state->buffered_stream_size = stm->fresh_seg_size;
    state->bit_rate = stm->bit_rate;
    state->stm_raw_new_flag = stm->raw_new_flag;
    state->stm_seg_full = stm->stm_seg_full;
    state->raw_recv_try = stm->raw_recv_try;
    state->raw_recv_ok = stm->raw_recv_ok;
    state->raw_fresh_num = stm->raw_fresh_num;
    state->seg_recv_try = stm->seg_recv_try;
    state->seg_recv_ok = stm->seg_recv_ok;
    state->seg_fresh_num = stm->seg_fresh_num;
    state->eos_flag = stm->eos_flag;
}

STATIC td_void vctrl_get_syn_chan_state(vfmw_chan_info *state, const syn_syntax_info *syn)
{
    state->std = syn->std;
    state->profile = syn->profile;
    state->level = syn->level;
    state->syn_get_seg_try_cnt = syn->get_seg_try_cnt;
    state->syn_get_seg_succ_cnt = syn->get_seg_succ_cnt;
    state->syn_spec_invalid = syn->spec_invalid;
    state->syn_spec_value = syn->spec_value;
}

STATIC td_void vctrl_get_dec_chan_state(vfmw_chan_info *state, const dec_info *dec)
{
    state->error_frame_num = dec->err_frame_num;
    state->dec_task_state = dec->task_state;
    state->dec_is_task_run = dec->is_task_run;
    state->dec_dps_num = dec->dec_dps_num;
    state->dec_load = dec->load;
}

STATIC td_void vctrl_get_fsp_chan_state(vfmw_chan_info *state, const fsp_info *fsp)
{
    state->total_disp_frame_num = fsp->total_frm_num;
    state->wait_disp_frame_num = fsp->wait_disp_frm_num;
    state->pvr_buf_num = fsp->pvr_buf_num;
    state->fsp_has_free_dec = fsp->has_dec_fs_free;
    state->fsp_has_free_pmv = fsp->has_pmv_fs_free;
    state->fsp_has_free_attach = fsp->has_attach_fs_free;
}

STATIC td_s32 vctrl_get_chan_state(td_s32 chan_id, td_void *args)
{
    td_s32 ret;
    vctrl_chan *chan = TD_NULL;
    vfmw_chan_info *state = TD_NULL;
    stm_info stm = { 0 };
    syn_syntax_info syn = { 0 };
    dec_info dec = { 0 };
    fsp_info fsp = { 0 };
    voq_info voq = { 0 };
    dps_fault_check_info dps = { 0 };

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);

    chan = vctrl_get_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL && chan->handle != TD_NULL, EXT_FAILURE);

    state = (vfmw_chan_info *)args;
    vfmw_check_sec_func(memset_s(state, sizeof(vfmw_chan_info), 0, sizeof(vfmw_chan_info)));

    ret = stm_control(chan_id, STM_CMD_GET_INFO, &stm);
    vfmw_check_ret_print(ret == STM_OK, EXT_FAILURE, "%s get stm info failed.\n", __func__);

    ret = syn_control(chan_id, SYN_CID_GET_INFO, &syn);
    vfmw_check_ret_print(ret == SYN_OK, EXT_FAILURE, "%s get syn info failed.\n", __func__);

    ret = dec_control(chan_id, DEC_CID_GET_INFO, &dec);
    vfmw_check_ret_print(ret == DEC_OK, EXT_FAILURE, "%s get dec info failed.\n", __func__);

    ret = fsp_get_info(chan_id, &fsp);
    vfmw_check_ret_print(ret == FSP_OK, EXT_FAILURE, "%s get fsp info failed.\n", __func__);

    ret = voq_get_info(chan_id, &voq);
    vfmw_check_ret_print(ret == VOQ_OK, EXT_FAILURE, "%s get voq info failed.\n", __func__);

    ret = dps_get_info(chan_id, &dps);
    vfmw_check_ret_print(ret == DPS_OK, EXT_FAILURE, "%s get dps info failed.\n", __func__);

    vctrl_get_stm_chan_state(state, &stm);  /* stm */
    vctrl_get_syn_chan_state(state, &syn);  /* syn */
    vctrl_get_dec_chan_state(state, &dec);  /* dec */
    vctrl_get_fsp_chan_state(state, &fsp);  /* fsp */

    /* voq */
    state->total_frame_num = (td_s32)voq.recv_frm_num;
    state->dec_frm_rate = voq.dec_frm_rate * 1000; /* 1000 times */

    /* dps */
    state->dps_has_make_node = dps.has_node_make;

    return EXT_SUCCESS;
}

STATIC td_s32 vctrl_set_frame_buffer_mode(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);

    ret = fsp_set_frame_buf_mode(chan_id, *((vfmw_frm_alloc_type *)args));

    return ret;
}

STATIC td_s32 vctrl_bind_extern_buffer(td_s32 chan_id, td_void *args)
{
    td_s32 ret;
    vfmw_ext_buf *buffer = (vfmw_ext_buf *)args;
    vctrl_chan *chan = TD_NULL;

    vfmw_check_ret(buffer != TD_NULL, EXT_FAILURE);
    chan = vctrl_get_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    ret = fsp_bind_extern_fs(chan_id, buffer);
    if (ret == FSP_OK) {
        if (dec_wakeup(chan_id) != DEC_OK) {
            dprint(PRN_FATAL, "%s dec_wakeup err\n", __func__);
        }

        if ((buffer->ext_info_buf.phy_addr != 0) && (buffer->ext_info_buf.vir_addr == 0)) {
            dprint(PRN_FATAL, "info buffer phy:%x, but vir addr is TD_NULL\n", buffer->ext_info_buf.phy_addr);
        }
    }
    vdec_record_chr_event(chan_id, MT_VDEC_BIND_BUFFER, MT_VDEC_BIND_BUFFER_OK, MT_VDEC_VFMW_BIND_BUFFER_OK);

    return (ret == FSP_OK) ? EXT_SUCCESS : EXT_FAILURE;
}

STATIC td_s32 vctrl_unbind_extern_buffer(td_s32 chan_id, td_void *args)
{
    td_s32 ret;
    vctrl_chan *chan = TD_NULL;
    vfmw_ext_buf *buffer = (vfmw_ext_buf *)args;

    vfmw_check_ret(buffer != TD_NULL, EXT_FAILURE);
    chan = vctrl_get_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    ret = fsp_unbind_extern_fs(chan_id, buffer);

    return (ret == FSP_OK) ? EXT_SUCCESS : EXT_FAILURE;
}

STATIC td_s32 vctrl_bind_extern_buffer_ready(td_s32 chan_id, td_void *args)
{
    td_s32 ret;
    vctrl_chan *chan = TD_NULL;

    vfmw_unused(args);
    chan = vctrl_get_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    ret = fsp_set_out_frm_state(chan_id, FS_OUT_FRM_BIND_READY);
    if (ret != FSP_OK) {
        dprint(PRN_FATAL, "%s set FS_OUT_FRM_BIND_READY failed\n", __func__);
    }

    return (ret == FSP_OK) ? EXT_SUCCESS : EXT_FAILURE;
}

STATIC td_s32 vctrl_unbind_extern_buffer_ready(td_s32 chan_id, td_void *args)
{
    td_s32 ret;
    vctrl_chan *chan = TD_NULL;

    vfmw_unused(args);
    chan = vctrl_get_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    ret = fsp_set_out_frm_state(chan_id, FS_OUT_FRM_UNBIND_READY);
    if (ret != FSP_OK) {
        dprint(PRN_FATAL, "%s set FS_OUT_FRM_UNBIND_READY failed\n", __func__);
    }

    return (ret == FSP_OK) ? EXT_SUCCESS : EXT_FAILURE;
}

STATIC td_s32 vctrl_set_frame_intf(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    ret = fsp_set_ext_intf(chan_id, args);

    return (ret == FSP_OK) ? EXT_SUCCESS : EXT_FAILURE;
}

STATIC td_s32 vctrl_get_dec_cap(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    vfmw_unused(chan_id);
    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);

    ret = pdt_get_attr(ATTR_GET_CAP, args);

    return ret;
}

STATIC td_s32 vctrl_wakeup(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    vfmw_unused(chan_id);
    vfmw_unused(args);

    ret = stm_wake_up(TD_NULL);
    if (ret != STM_OK) {
        dprint(PRN_FATAL, "%s stm wake up error!\n", __func__);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vctrl_release_stream(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    ret = stm_control(chan_id, STM_CMD_FLUSH_CHAN, args);
    ret = (ret == STM_OK) ? EXT_SUCCESS : EXT_FAILURE;

    return ret;
}

STATIC td_s32 vctrl_set_trick_mode(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);

    ret = syn_control(chan_id, SYN_CID_SET_TRICKMODE, args);
    ret = (ret == SYN_OK) ? EXT_SUCCESS : EXT_FAILURE;

    voq_set_speed(chan_id, *((td_s32*)args));

    return ret;
}

STATIC td_s32 vctrl_set_ctrl_info(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);

    ret = syn_control(chan_id, SYN_CID_SET_CTRLINFO, (vfmw_control_info *)args);
    ret = (ret == SYN_OK) ? EXT_SUCCESS : EXT_FAILURE;

    return ret;
}

STATIC td_s32 vctrl_reset_chan_with_option(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    ret = vctrl_stop_chan(chan_id, TD_NULL);
    if (ret != EXT_SUCCESS) {
        dprint(PRN_FATAL, "%s stop chan %d fail!\n", __func__, chan_id);
        return EXT_FAILURE;
    }

    ret = vctrl_reset_chan(chan_id, args);
    if (ret != EXT_SUCCESS) {
        dprint(PRN_FATAL, "%s reset chan %d fail!\n", __func__, chan_id);
        return EXT_FAILURE;
    }

    ret = vctrl_start_chan(chan_id, TD_NULL);
    if (ret != EXT_SUCCESS) {
        dprint(PRN_FATAL, "%s start chan %d fail!\n", __func__, chan_id);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vctrl_set_discard_pic_param(td_s32 chan_id, td_void *args)
{
    vctrl_chan *chan = TD_NULL;

    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);

    chan = vctrl_get_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    dprint(PRN_ERROR, "Not support vctrl_set_discard_pic_param\n");

    return EXT_SUCCESS;
}

STATIC td_s32 vctrl_set_pts_to_seek(td_s32 chan_id, td_void *args)
{
    vctrl_chan *chan = TD_NULL;

    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);

    chan = vctrl_get_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    dprint(PRN_ERROR, "Not support seek pts\n");

    return EXT_SUCCESS;
}

STATIC td_s32 vctrl_inquire_adpt_type(td_s32 chan_id, td_void *args)
{
    vctrl_chan *chan = TD_NULL;
    vfmw_adpt_type *type = (vfmw_adpt_type *)args;

    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);

    chan = vctrl_get_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(chan->handle != TD_NULL, EXT_FAILURE);

    *type = chan->opt.adpt_type;

    return EXT_SUCCESS;
}

#ifdef VFMW_VMM_SUPPORT
td_s32 vctrl_bind_req_mem(td_s32 chan_id, td_void *args)
{
    td_s32 ret;
    vctrl_chan *chan = TD_NULL;

    chan = vctrl_get_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);

    ret = fsp_bind_req_fs(chan_id, (vfmw_mem_report *)args);
    if (ret == FSP_OK) {
        dec_wakeup(chan_id);
    }

    return (ret == FSP_OK) ? EXT_SUCCESS : EXT_FAILURE;
}
#endif

STATIC td_s32 vctrl_bind_attach_mem(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);

    ret = fsp_bind_attach_fs(chan_id, (vfmw_mem_attach *)args);

    return (ret == FSP_OK) ? EXT_SUCCESS : EXT_FAILURE;
}

STATIC td_s32 vctrl_set_es_invalid(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    ret = stm_control(chan_id, STM_CMD_SET_ES_INVALID, args);

    return (ret == STM_OK) ? EXT_SUCCESS : EXT_FAILURE;
}

STATIC td_s32 vctrl_set_event_map(td_s32 chan_id, td_void *args)
{
    td_u64 map;

    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);
    map = *(td_u64 *)args;

    return event_set_map(chan_id, map);
}

STATIC td_s32 vctrl_bind_es_buf(td_s32 chan_id, td_void *args)
{
    vfmw_unused(chan_id);
    vfmw_unused(args);

    /* do not need process below vctrl */
    return EXT_SUCCESS;
}

STATIC td_s32 vctrl_unbind_es_buf(td_s32 chan_id, td_void *args)
{
    vfmw_unused(chan_id);
    vfmw_unused(args);

    /* do not need process below vctrl */
    return EXT_SUCCESS;
}

STATIC vctrl_case g_vctrl_case_table[] = {
    { VFMW_CID_GET_CHAN_STATE,         vctrl_get_chan_state,         sizeof(vfmw_chan_info) },
    { VFMW_CID_CREATE_CHAN,            vctrl_create_chan,            VCTRL_CREATE_OPT_SIZE},
    { VFMW_CID_RESET_CHAN,             vctrl_reset_chan,             0 },
    { VFMW_CID_CFG_CHAN,               vctrl_config_chan,            sizeof(vfmw_chan_cfg) },
    { VFMW_CID_GET_CHAN_CFG,           vctrl_get_chan_cfg,           sizeof(vfmw_chan_cfg) },
    { VFMW_CID_START_CHAN,             vctrl_start_chan,             0 },
    { VFMW_CID_STOP_CHAN,              vctrl_stop_chan,              0 },
    { VFMW_CID_DESTROY_CHAN,           vctrl_destroy_chan,           0 },
    { VFMW_CID_SET_STREAM_INTF,        vctrl_set_stream_intf,        sizeof(vfmw_stream_intf) },
    { VFMW_CID_SET_FRAME_INTF,         vctrl_set_frame_intf,         sizeof(vfmw_fsp_frame_intf) },
    { VFMW_CID_GET_CAPABILITY,         vctrl_get_dec_cap,            sizeof(vfmw_cap) },
    { VFMW_CID_EXTRA_WAKEUP_THREAD,    vctrl_wakeup,                 0 },
    { VFMW_CID_RELEASE_STREAM,         vctrl_release_stream,         0 },
    { VFMW_CID_SET_TRICK_MODE,         vctrl_set_trick_mode,         sizeof(td_s32) },
    { VFMW_CID_SET_CTRL_INFO,          vctrl_set_ctrl_info,          sizeof(vfmw_control_info) },
    { VFMW_CID_RESET_CHAN_WITH_OPTION, vctrl_reset_chan_with_option, sizeof(vfmw_chan_reset_option) },
    { VFMW_CID_SET_PTS_TO_SEEK,        vctrl_set_pts_to_seek,        sizeof(td_s64) },
    { VFMW_CID_SET_DISCARDPICS_PARAM,  vctrl_set_discard_pic_param,  sizeof(vfmw_dispic_param) },
    { VFMW_CID_INQUIRE_ADPT_TYPE,      vctrl_inquire_adpt_type,      sizeof(vfmw_adpt_type) },
#ifdef VFMW_VMM_SUPPORT
    { VFMW_CID_BIND_MEM,               vctrl_bind_req_mem,           sizeof(vfmw_mem_report) },
#endif
    { VFMW_CID_BIND_ATTACH,            vctrl_bind_attach_mem,        sizeof(vfmw_mem_attach) },
    { VFMW_CID_SET_FRAME_BUFFER_MODE,  vctrl_set_frame_buffer_mode,  sizeof(vfmw_frm_alloc_type)},
    { VFMW_CID_BIND_EXTERN_BUFFER,     vctrl_bind_extern_buffer,     sizeof(vfmw_ext_buf)},
    { VFMW_CID_UNBIND_EXTERN_BUFFER,   vctrl_unbind_extern_buffer,   sizeof(vfmw_ext_buf)},

    { VFMW_CID_REPORT_DEMUX_RELEASE,   vctrl_set_es_invalid,         0 },
    { VFMW_CID_SET_EVENT_MAP,          vctrl_set_event_map,          sizeof(td_u64)},
    { VFMW_CID_BIND_ES_BUF,            vctrl_bind_es_buf,            sizeof(vfmw_stream_buf)},
    { VFMW_CID_UNBIND_ES_BUF,          vctrl_unbind_es_buf,          0 },
    { VFMW_CID_FLUSH_CHAN,             vctrl_flush_chan,             sizeof(vfmw_flush_type)},
    { VFMW_CID_FLUSH_POST_PROCESS,     vctrl_flush_post,             0 },
    { VFMW_CID_BIND_EXTERN_BUFFER_READY,    vctrl_bind_extern_buffer_ready,     0},
    { VFMW_CID_UNBIND_EXTERN_BUFFER_READY,  vctrl_unbind_extern_buffer_ready,   0},
    { VFMW_CID_MAX, TD_NULL, 0 },
};

STATIC td_s32 vctrl_case_entry(td_s32 chan_id, vfmw_cid cmd, td_void *args)
{
    td_s32 ret;
    td_u32 index;
    td_u32 case_num;
    vctrl_handler target_handler = TD_NULL;

    case_num = (td_u32)sizeof(g_vctrl_case_table) / (td_u32)sizeof(vctrl_case);
    for (index = 0; index < case_num; index++) {
        if (cmd == g_vctrl_case_table[index].cmd) {
            target_handler = g_vctrl_case_table[index].handler;
            break;
        }
    }

    if (target_handler != TD_NULL) {
        ret = target_handler(chan_id, args);
    } else {
        dprint(PRN_ERROR, "Unsupport cmd %d\n", cmd);
        ret = EXT_FAILURE;
    }

    return ret;
}

td_s32 vctrl_get_require_mem(vfmw_svr_mem_need *mem)
{
    vfmw_check_ret(mem != TD_NULL, EXT_FAILURE);

    stm_get_mem_need_size(mem);
    syn_get_mem_need_size(mem);
    dec_get_mem_need_size(mem);
    fsp_get_mem_need_size(mem);
    voq_get_mem_need_size(mem);
    dps_get_mem_need_size(mem);
#ifdef VFMW_USD_SUPPORT
    usd_get_mem_need_size(mem);
#endif

    return EXT_SUCCESS;
}

td_s32 vctrl_open(td_void)
{
    (td_void)stm_open();
    (td_void)syn_open();
    (td_void)dec_open();
    (td_void)fsp_open();
    (td_void)voq_open();
    (td_void)dps_open();
#ifdef VFMW_USD_SUPPORT
    (td_void)usd_open();
#endif
    vctrl_init_dflt();

    vfmw_check_ret(vctrl_check_mem() == EXT_SUCCESS, EXT_FAILURE);

    return EXT_SUCCESS;
}

td_s32 vctrl_close(td_void)
{
    (td_void)dec_close();
    (td_void)syn_close();
    (td_void)stm_close();
    (td_void)fsp_close();
    (td_void)voq_close();
    (td_void)dps_close();
#ifdef VFMW_USD_SUPPORT
    (td_void)usd_close();
#endif

    vfmw_check_ret(vctrl_check_mem() == EXT_SUCCESS, EXT_FAILURE);

    return EXT_SUCCESS;
}

/* extern funcs */
td_s32 vctrl_init(td_void *args)
{
    td_s32 ret;
    td_u32 id;

    vfmw_unused(args);

    ret = pdt_init();
    vfmw_check_ret(ret == EXT_SUCCESS, EXT_FAILURE);

    for (id = 0; id < VFMW_CHAN_NUM; id++) {
        g_vctrl_entry.chan[id] = TD_NULL;
    }

    ret = fsp_initial();
    if (ret != FSP_OK) {
        dprint(PRN_ERROR, "%s fsp_initial Error \n", __func__);
        return EXT_FAILURE;
    }

    ret = voq_init();
    if (ret != VOQ_OK) {
        dprint(PRN_ERROR, "%s voq_init Error \n", __func__);
        goto err0;
    }

    ret = dps_init();
    if (ret != DPS_OK) {
        dprint(PRN_ERROR, "%s dps_init Error \n", __func__);
        goto err1;
    }

    vfmw_check_goto(vctrl_check_mem() == EXT_SUCCESS, err2);

    return EXT_SUCCESS;
err2:
    dps_exit();
err1:
    voq_exit();
err0:
    fsp_exit();
    return EXT_FAILURE;
}

td_s32 vctrl_exit(const td_void *args)
{
    td_u32 id;
    td_s32 ret;
    td_bool is_power_off;
    td_bool is_apb_off;

    vfmw_unused(args);

    ret = voq_exit();
    if (ret != VOQ_OK) {
        dprint(PRN_ERROR, "%s voq_exit Error \n", __func__);
        return EXT_FAILURE;
    }

    ret = fsp_exit();
    if (ret != FSP_OK) {
        dprint(PRN_ERROR, "%s fsp_exit Error \n", __func__);
        return EXT_FAILURE;
    }

    ret = dps_exit();
    if (ret != DPS_OK) {
        dprint(PRN_ERROR, "%s dps_exit Error \n", __func__);
        return EXT_FAILURE;
    }

    ret = pdt_exit();
    if (ret != EXT_SUCCESS) {
        dprint(PRN_ERROR, "%s pdt_exit Error \n", __func__);
        return EXT_FAILURE;
    }

    ret = pdt_check_power_is_off(&is_power_off);
    if (ret == EXT_SUCCESS && is_power_off == TD_FALSE) {
        dprint(PRN_FATAL, "vfmw power is not close,need to check\n");
        vdec_record_chr_event(-1, MT_VDEC_KEY_ERROR, MT_VDEC_VFMW_KEY_ERROR, MT_VDEC_VFMW_VDH_CLOSE_CLK_ERR);
    }
    ret = pdt_check_apb_is_off(&is_apb_off);
    if (ret == EXT_SUCCESS && is_apb_off == TD_FALSE) {
        dprint(PRN_FATAL, "vfmw apb is not close,need to check\n");
        vdec_record_chr_event(-1, MT_VDEC_KEY_ERROR, MT_VDEC_VFMW_KEY_ERROR, MT_VDEC_VFMW_VDH_CLOSE_CLK_ERR);
    }

    for (id = 0; id < VFMW_CHAN_NUM; id++) {
        if (g_vctrl_entry.chan[id] != TD_NULL) {
            os_free_vir(g_vctrl_entry.chan[id]);
            g_vctrl_entry.chan[id] = TD_NULL;
        }
    }

    vfmw_check_ret(vctrl_check_mem() == EXT_SUCCESS, EXT_FAILURE);

    return EXT_SUCCESS;
}

td_s32 vctrl_suspend(const td_void *args)
{
    td_s32 ret;

    ret = stm_suspend(args);
    if (ret != STM_OK) {
        dprint(PRN_FATAL, "stm_suspend fail!\n");
        return EXT_FAILURE;
    }

    ret = syn_suspend(args);
    if (ret != SYN_OK) {
        dprint(PRN_FATAL, "syn_suspend fail!\n");
        return EXT_FAILURE;
    }

    ret = dec_suspend(args);
    if (ret != DEC_OK) {
        dprint(PRN_FATAL, "dec_suspend fail!\n");
        return EXT_FAILURE;
    }

    vfmw_check_ret(vctrl_check_mem() == EXT_SUCCESS, EXT_FAILURE);

    return EXT_SUCCESS;
}

td_s32 vctrl_resume(const td_void *args)
{
    td_s32 ret;

    ret = stm_resume(args);
    if (ret != STM_OK) {
        dprint(PRN_FATAL, "stm_resume fail!\n");
        return EXT_FAILURE;
    }

    ret = syn_resume(args);
    if (ret != SYN_OK) {
        dprint(PRN_FATAL, "syn_resume fail!\n");
        return EXT_FAILURE;
    }

    ret = dec_resume(args);
    if (ret != DEC_OK) {
        dprint(PRN_FATAL, "dec_resume fail!\n");
        return EXT_FAILURE;
    }

    vfmw_check_ret(vctrl_check_mem() == EXT_SUCCESS, EXT_FAILURE);

    return EXT_SUCCESS;
}

td_s32 vctrl_get_image(td_s32 chan_id, const td_void *image)
{
    td_s32 ret;
    vctrl_chan *chan = TD_NULL;

    if (image == TD_NULL) {
        dprint(PRN_ERROR, "%s pImage is TD_NULL\n", __func__);
        return EXT_FAILURE;
    }
    vfmw_check_ret(os_get_time_us != TD_NULL, EXT_FAILURE);

    ret = voq_receive(chan_id, (vfmw_image *)image);
    if (ret != VOQ_OK) {
        return EXT_FAILURE;
    }

    chan = vctrl_get_chan(chan_id);
    if (chan != TD_NULL && chan->recv_time == 0) {
        chan->recv_time = os_get_time_us();
    }

    return EXT_SUCCESS;
}

td_void vctrl_img_2_rls_info(const vfmw_image *img, vfmw_image_rls *rls_info)
{
    rls_info->image_id = img->image_id;
    rls_info->image_id_1 = img->image_id_1;
    rls_info->is_3d = img->is_3d;
}

td_s32 vctrl_release_image(td_s32 chan_id, const td_void *rls_info)
{
    td_s32 ret;
    vfmw_image_rls *img_rls = (vfmw_image_rls *)rls_info;

    if (img_rls == TD_NULL) {
        dprint(PRN_ERROR, "%s image is TD_NULL\n", __func__);
        return EXT_FAILURE;
    }

    ret = voq_release(chan_id, img_rls);
    if (ret != VOQ_OK) {
        dprint(PRN_WARN, "%s call voq_release failed.\n", __func__);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vctrl_check_image(td_s32 chan_id, const td_void *img)
{
    td_s32 ret;

    ret = voq_check_img(chan_id, (vfmw_image *)img);
    if (ret != VOQ_OK) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vctrl_control(td_s32 chan_id, td_s32 cmd, td_void *args)
{
    td_s32 ret;

    ret = vctrl_case_entry(chan_id, cmd, args);

    return ret;
}

td_void vctrl_set_event_interface(fun_vfmw_event_report evt_rpt)
{
    event_set_callback(evt_rpt);
}

td_s32 vctrl_get_param_len(td_s32 cmd, td_u32 *len)
{
    td_u32 index;
    td_u32 case_num = (td_u32)sizeof(g_vctrl_case_table) / (td_u32)sizeof(vctrl_case);

    vfmw_check_ret(len != TD_NULL, EXT_FAILURE);

    for (index = 0; index < case_num; index++) {
        if (g_vctrl_case_table[index].cmd == (vfmw_cid)cmd) {
            *len = g_vctrl_case_table[index].length;
            break;
        }
    }

    if (index >= case_num) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vctrl_stm_process(td_void)
{
    td_s32 ret;

    ret = stm_process();

    return (ret == STM_ERR) ? EXT_FAILURE : EXT_SUCCESS;
}

td_s32 vctrl_syn_process(td_void)
{
    td_s32 ret;

    ret = syn_process();

    return (ret == SYN_ERR) ? EXT_FAILURE : EXT_SUCCESS;
}

td_s32 vctrl_dec_process(td_void)
{
    td_s32 ret;

    ret = dec_process();

    return (ret == DEC_ERR) ? EXT_FAILURE : EXT_SUCCESS;
}

td_s32 vctrl_set_voq_ext_intf(const voq_ext_intf *intf)
{
    td_s32 ret;

    ret = voq_set_ext_intf(intf);

    return (ret == VOQ_ERR) ? EXT_FAILURE : EXT_SUCCESS;
}

td_s32 vctrl_check_eos(td_s32 chan_id, td_s32 *flag)
{
    td_s32 ret;

    ret = voq_check_eos(chan_id, flag);

    return (ret == VOQ_OK) ? EXT_SUCCESS : EXT_FAILURE;
}

td_s32 vctrl_get_max_dec_width_height(td_s32 chan_id, td_u32 *dec_max_width, td_u32 *dec_max_height)
{
    vctrl_chan *chan = TD_NULL;

    chan = vctrl_get_chan(chan_id);
    vfmw_check_ret_print(chan != TD_NULL, EXT_FAILURE, "chan null\n");
    vfmw_check_ret_print(chan->handle != TD_NULL, EXT_FAILURE, "handle not null\n");
    vfmw_check_ret_print(dec_max_width != TD_NULL && dec_max_height != TD_NULL, EXT_FAILURE, "point null\n");

    if (chan->cfg.is_max_buf_size_en != TD_TRUE) {
        return EXT_FAILURE;
    }

    *dec_max_width = chan->cfg.max_dec_width;
    *dec_max_height = chan->cfg.max_dec_height;
    return EXT_SUCCESS;
}
