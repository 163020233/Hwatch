/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "hevc.h"
#include "fsp.h"
#include "event.h"
#include "voque.h"
#include "dps.h"
#include "vfmw_proc.h"
#include "syn_parse.h"
#include "vfmw_define.h"
#include "vfmw_config.h"

static const td_s32 g_crop_unit_x[4] = { 1, 2, 2, 1 }; /* 4 is array size */
static const td_s32 g_crop_unit_y[4] = { 1, 2, 1, 1 }; /* 4 is array size */

static const td_u32 g_quant_ts_default[4] = { /* 4 is array size */
    0x10101010, 0x10101010, 0x10101010, 0x10101010
};
static td_u8 g_quant_ts_default4x4[16] = { /* 16 is array size */
    16, 16, 16, 16,
    16, 16, 16, 16,
    16, 16, 16, 16,
    16, 16, 16, 16
};

static td_u8 g_quant_intra_default8x8[64] = { /* 64 is array size */
    16, 16, 16, 16, 17, 18, 21, 24,
    16, 16, 16, 16, 17, 19, 22, 25,
    16, 16, 17, 18, 20, 22, 25, 29,
    16, 16, 18, 21, 24, 27, 31, 36,
    17, 17, 20, 24, 30, 35, 41, 47,
    18, 19, 22, 27, 35, 44, 54, 65,
    21, 22, 25, 31, 41, 54, 70, 88,
    24, 25, 29, 36, 47, 65, 88, 115
};

static td_u32 g_quant8_intra_default[16] = { /* 16 is array size */
    0x10101010, 0x18151211, 0x10101010, 0x19161311,
    0x12111010, 0x1d191614, 0x15121010, 0x241f1b18,
    0x18141111, 0x2f29231e, 0x1b161312, 0x41362c23,
    0x1f191615, 0x58463629, 0x241d1918, 0x7358412f
};

static td_u8 g_quant_inter_default8x8[64] = { /* 64 is array size */
    16, 16, 16, 16, 17, 18, 20, 24,
    16, 16, 16, 17, 18, 20, 24, 25,
    16, 16, 17, 18, 20, 24, 25, 28,
    16, 17, 18, 20, 24, 25, 28, 33,
    17, 18, 20, 24, 25, 28, 33, 41,
    18, 20, 24, 25, 28, 33, 41, 54,
    20, 24, 25, 28, 33, 41, 54, 71,
    24, 25, 28, 33, 41, 54, 71, 91
};

static const td_u32 g_quant8_inter_default[16] = { /* 16 is array size */
    0x10101010, 0x18141211, 0x11101010, 0x19181412,
    0x12111010, 0x1c191814, 0x14121110, 0x211c1918,
    0x18141211, 0x29211c19, 0x19181412, 0x3629211c,
    0x1c191814, 0x47362921, 0x211c1918, 0x5b473629
};

static const td_u8 g_hevc_diag_scan4x4_x[16] = { /* 16 is array size */
    0, 0, 1, 0,
    1, 2, 0, 1,
    2, 3, 1, 2,
    3, 2, 3, 3,
};

static const td_u8 g_hevc_diag_scan4x4_y[16] = { /* 16 is array size */
    0, 1, 0, 2,
    1, 0, 3, 2,
    1, 0, 3, 2,
    1, 3, 2, 3,
};

static const td_u8 g_hevc_diag_scan8x8_x[64] = { /* 64 is array size */
    0, 0, 1, 0,
    1, 2, 0, 1,
    2, 3, 0, 1,
    2, 3, 4, 0,
    1, 2, 3, 4,
    5, 0, 1, 2,
    3, 4, 5, 6,
    0, 1, 2, 3,
    4, 5, 6, 7,
    1, 2, 3, 4,
    5, 6, 7, 2,
    3, 4, 5, 6,
    7, 3, 4, 5,
    6, 7, 4, 5,
    6, 7, 5, 6,
    7, 6, 7, 7,
};

static const td_u8 g_hevc_diag_scan8x8_y[64] = { /* 64 is array size */
    0, 1, 0, 2,
    1, 0, 3, 2,
    1, 0, 4, 3,
    2, 1, 0, 5,
    4, 3, 2, 1,
    0, 6, 5, 4,
    3, 2, 1, 0,
    7, 6, 5, 4,
    3, 2, 1, 0,
    7, 6, 5, 4,
    3, 2, 1, 7,
    6, 5, 4, 3,
    2, 7, 6, 5,
    4, 3, 7, 6,
    5, 4, 7, 6,
    5, 7, 6, 7,
};

#define H265_UUID_SIZE                16
const uint8_t g_hevc_splice_uuid[H265_UUID_SIZE] = {
    0x48, 0x53, 0x50, 0x49, 0x43, 0x59, 0x55, 0x56, /* HSPICYUV */
    0x20, 0x26, 0x01, 0x19, 0x24, 0x0b, 0xea, 0xc0  /* 2026/01/19 + version 4 UUID gen random */
};

STATIC td_void hevc_clear_all_nal(hevc_ctx *ctx);
STATIC td_s32 hevc_receive_packet(hevc_ctx *ctx, const hevc_stream_packet *packet);
STATIC td_s32 hevc_clear_curr_nal(hevc_ctx *ctx);
STATIC td_s32 hevc_inquire_slice_property(hevc_ctx *ctx, td_s32 *is_new_pic, td_s32 *is_size_changed);
STATIC td_s32 hevc_have_slice_to_dec(hevc_ctx *ctx);
STATIC td_s32 hevc_dec_one_nal(hevc_ctx *ctx);
STATIC td_s32 hevc_empty_frm_from_dpb(hevc_ctx *ctx);
STATIC td_s32 hevc_flush_output_frm_from_dpb(hevc_ctx *ctx);
STATIC td_s32 hevc_dec_vps(hevc_ctx *ctx);
STATIC td_s32 hevc_dec_sps(hevc_ctx *ctx);
STATIC td_s32 hevc_dec_pps(hevc_ctx *ctx);
STATIC td_s32 hevc_dec_sei(hevc_ctx *ctx);
STATIC td_s32 hevc_dec_slice(hevc_ctx *ctx);
STATIC td_s32 hevc_clear_curr_slice(hevc_ctx *ctx);
STATIC td_u32 hevc_u_v(bs_info *bs_ptr, td_s32 v, td_char *name);
STATIC td_s32 hevc_dec_ptl(hevc_ctx *ctx, hevc_profile_tier_level *ptl, td_s32 profile_present_flag,
                           td_s32 max_num_sub_layers_minus1);
STATIC td_u32 hevc_ue_v(bs_info *bs_ptr, td_char *name);
STATIC td_s32 hevc_dec_hrd_param(const hevc_ctx *ctx, hevc_hrd_param_set *hrd, td_u8 common_inf_present_flag,
                                 td_u32 max_num_sub_layers_minus1);
STATIC td_s32 hevc_more_rbsp_data(const hevc_ctx *ctx);
STATIC td_s32 hevc_dec_scaling_list_data(const hevc_ctx *ctx, hevc_scaling_list *scaling_list);
STATIC td_s32 hevc_dec_short_term_ref_pic_set(const hevc_ctx *ctx, const hevc_seq_param_set *sps,
                                              hevc_short_term_rpset *short_term_rpset, td_u32 idx);
STATIC td_s32 hevc_dec_vui_param(hevc_ctx *ctx, hevc_vui_param_set *vui, const hevc_seq_param_set *sps);
STATIC td_s32 hevc_se_v(bs_info *bs_ptr, td_char *name);
STATIC td_s32 hevc_sei_message(hevc_ctx *ctx, hevc_sei_param_set *sei, hevc_seq_param_set *sps);
STATIC td_s32 hevc_sei_more_rbsp_data(const hevc_ctx *ctx);
STATIC td_s32 hevc_dec_check(hevc_ctx *ctx);
STATIC td_s32 hevc_write_pic_msg(hevc_ctx *ctx);
STATIC td_s32 hevc_dec_post_proc(hevc_ctx *ctx, td_bool is_slc);
STATIC td_u8 hevc_is_eos_nal(const hevc_ctx *ctx);
STATIC td_s32 hevc_get_dar(td_s32 ar_idc, td_s32 sar_width, td_s32 sar_height, td_s32 img_width,
                           td_s32 img_height);
STATIC td_s32 hevc_sei_payload(hevc_ctx *ctx, hevc_sei_param_set *sei, hevc_seq_param_set *sps,
                               td_u32 payload_type, td_u32 payload_size);
STATIC td_s32 hevc_sei_buffer_period(hevc_ctx *ctx, hevc_sei_buf_period *buf, hevc_hrd_param_set *hrd);
STATIC td_s32 hevc_sei_picture_timming(hevc_ctx *ctx, hevc_sei_pic_timming *pic, const hevc_vui_param_set *vui);
STATIC td_s32 hevc_sei_usr_dat_reg_itu_t_t35(hevc_ctx *ctx, hevc_sei_user_data_reg_itu_t_t35 *itu_t35,
                                             td_u32 payload_size);
STATIC td_s32 hevc_sei_usr_dat_unreg(hevc_ctx *ctx, hevc_sei_user_data_unreg *data, td_u32 payload_size);
STATIC td_s32 hevc_sei_recovery_point_info(const hevc_ctx *ctx, hevc_sei_recovery_point *rec);
STATIC td_s32 hevc_sei_dec_unit_info(const hevc_ctx *ctx, hevc_sei_decoding_unit_info *dec,
    const hevc_hrd_param_set *hrd);
STATIC td_s32 hevc_sei_frm_packing_arrangement(const hevc_ctx *ctx, hevc_sei_frame_packing_arrangement *frm);
STATIC td_s32 hevc_sei_tone_mapping(const hevc_ctx *ctx, hevc_sei_tone_mapping_info *ton);
STATIC td_s32 hevc_sei_disp_orientation(const hevc_ctx *ctx, hevc_sei_display_orientation *dis);
STATIC td_s32 hevc_sei_structure_of_pictures(const hevc_ctx *ctx, hevc_sei_sop_info *sop, td_u32 payload_size);
STATIC td_s32 hevc_sei_active_param_sets(const hevc_ctx *ctx, hevc_sei_active_parameter_set *act);
STATIC td_s32 hevc_sei_temporal_sub_layer_zero_index(const hevc_ctx *ctx, hevc_sei_temporal_sub_layer *tem);
STATIC td_s32 hevc_sei_scalable_nesting_info(hevc_ctx *ctx, hevc_sei_param_set *sei,
                                             hevc_seq_param_set *sps, td_u32 payloadsize);
STATIC td_s32 hevc_sei_region_refresh(const hevc_ctx *ctx, hevc_sei_region_refresh_info *reg);
STATIC td_s32 hevc_sei_mastering_disp_colour_vol(const hevc_ctx *ctx, hevc_sei_mastering_display_colour_volume *reg);
STATIC td_s32 hevc_sei_decoded_pic_hash(const hevc_ctx *ctx, hevc_sei_decoded_picture_hash *dec);
STATIC td_u32 hevc_sei_payload_extension_present(const hevc_ctx *ctx, td_u32 payload_size);
STATIC td_u32 hevc_sei_more_data_in_payload(const hevc_ctx *ctx, td_u32 payload_size);
STATIC td_s32 hevc_output_one_frm_from_dpb(hevc_ctx *ctx, td_u32 pos);
STATIC td_s32 hevc_clear_dpb(hevc_ctx *ctx);
STATIC td_void hevc_remove_dummy_frame(hevc_ctx *ctx);
STATIC td_void hevc_apply_reference_picture_set(hevc_ctx *ctx);
STATIC td_s32 hevc_remove_unused_frame_store(hevc_ctx *ctx);
STATIC td_s32 hevc_create_lost_picture(hevc_ctx *ctx, td_s32 lost_pos);
STATIC td_void hevc_update_ref_list(hevc_ctx *ctx);
STATIC td_void hevc_update_lt_ref_list(hevc_ctx *ctx);
STATIC td_s32 hevc_init_pic(hevc_ctx *ctx);
STATIC td_s32 hevc_set_scaling_list(hevc_ctx *ctx);
STATIC td_void hevc_calc_stream_bits(hevc_ctx *ctx);
STATIC td_s32 hevc_dec_list(hevc_ctx *ctx);
STATIC td_s32 hevc_write_slice_msg(hevc_ctx *ctx);
STATIC td_u8 hevc_get_rap_pic_flag(hevc_ctx *ctx);
STATIC td_u8 hevc_get_idr_pic_flag(hevc_ctx *ctx);
STATIC td_s32 hevc_vps_sps_pps_check(const hevc_ctx *ctx);
STATIC td_s32 hevc_dec_ref_pic_lists_modification(const hevc_ctx *ctx, const hevc_slice_segment_header *slice,
    hevc_ref_pic_lists_moddification *rp_list_modification, td_s32 num_rps_curr_temp_list);
STATIC td_s32 hevc_dec_pred_weight_table(hevc_ctx *ctx);
STATIC td_s32 hevc_read_byte_alignment(const hevc_ctx *ctx);
STATIC td_s32 hevc_sei_color_remapping(const hevc_ctx *ctx, hevc_sei_color_remapping_info *remapping);
STATIC td_s32 hevc_sei_content_light_level_information(const hevc_ctx *ctx,
    hevc_sei_content_light_level_info *reg);
STATIC td_s32 hevc_sei_hlghdr_comapitibility_info(const hevc_ctx *ctx, hevc_sei_hlg_compatible_info *reg);
STATIC td_s32 hevc_check_ref_pics(const hevc_ctx *ctx, td_s32 *lost_poc);
STATIC td_s32 hevc_check_list_x(const hevc_ctx *ctx);
STATIC td_s32 hevc_clear_curr_pic(hevc_ctx *ctx);
STATIC td_s32 hevc_get_pic_stream_size(const hevc_ctx *ctx);
STATIC td_s32 hevc_get_rerange_flag(hevc_ctx *ctx, td_s32 old_width, td_s32 old_height,
                                    td_s32 old_log2_ctb_size_y, td_s32 dpb_size_changed);
STATIC td_void hevc_get_back_pic_from_vo_queue(hevc_ctx *ctx);
STATIC td_s32 hevc_clear_all(hevc_ctx *ctx, td_s32 flag);
STATIC td_s32 hevc_alloc_frame_store(hevc_ctx *ctx);
STATIC td_s32 hevc_init_list_x(hevc_ctx *ctx);
STATIC td_void hevc_clear_all_slice(hevc_ctx *ctx);
STATIC td_s32 hevc_arrange_vahb_mem(hevc_ctx *ctx, td_u32 img_width, td_u32 img_height,
                                    td_u32 img_width_in_ctb, td_u32 img_height_in_ctb,
                                    td_u32 img_num, td_u32 pmv_blk_num, td_u32 bit_depth);
STATIC td_void hevc_unmark_frame_store_ref(const hevc_ctx *ctx, hevc_frame_store *frame_store);
STATIC td_void hevc_remove_frame_store_out_dpb(hevc_ctx *ctx, td_u32 pos);
STATIC td_s32 hevc_is_out_dpb(const hevc_ctx *ctx, const hevc_frame_store *frame_store);
STATIC td_s32 hevc_insert_frm_in_dpb(hevc_ctx *ctx, td_u32 pos, hevc_curr_pic *curr_pic);
STATIC td_void hevc_get_image_para(hevc_ctx *ctx, const hevc_frame_store *frame_store);
STATIC td_s32 hevc_check_frame_store(const hevc_ctx *ctx, hevc_frame_store *frame_store);
STATIC td_s32 hevc_output_frm_from_dpb(hevc_ctx *ctx);
STATIC td_u32 hevc_frame_store_ref_flg(const hevc_frame_store *fs);
STATIC td_s32 hevc_dec_recycle_image(const hevc_ctx *ctx, td_u32 pic_id);
STATIC hevc_pic_type hevc_is_new_pic(hevc_ctx *ctx);
STATIC td_u8 *hevc_get_scaling_list_default_address(td_u32 size_id, td_u32 matrix_id);
STATIC td_void hevc_set_default_scaling_list(hevc_ctx *ctx);
STATIC td_void hevc_set_default_scaling_list(hevc_ctx *ctx);
STATIC td_void hevc_config_max_para(syn_cfg_info *extra_data);
STATIC td_s32 hevc_flush_dpb(hevc_ctx *ctx);
STATIC td_void hevc_cal_scaling_list(hevc_ctx *ctx, const td_u8 *scaling_list_coef, td_u32 list_size, td_u32 matrix_id);
STATIC td_s32 hevc_init_dpb(hevc_ctx *ctx, td_u32 rerange_flag);
STATIC td_s32 hevc_output_frm_to_vo(hevc_ctx *ctx, hevc_frame_store *frame_store);
STATIC td_void hevc_simple_slide_dpb(hevc_ctx *ctx);

STATIC td_void *hevc_get_dyn_mem(hevc_ctx *ctx, td_u32 len)
{
    td_void *addr = TD_NULL;
    td_s32 cnt;
    mem_record mem = { 0 };

    for (cnt = 0; cnt < HEVC_MEM_NUM; cnt++) {
        if (ctx->mem_list[cnt].mem_addr == TD_NULL) {
            break;
        }
    }

    if (cnt >= HEVC_MEM_NUM || len > VFMW_MAX_UINT32) {
        dprint(PRN_ERROR, "%s mem_list full error\n", __func__);
        return TD_NULL;
    }

    mem.mem_name = "h265_dyn_mem";
    mem.length = vfmw_align_up(len, VFMW_CTX_ALIGN);
    mem.is_cached = 1;
    mem.type = CACHE_MEM_TYPE;
    mem.unid = (td_u32)ctx->chan_id;
    os_malloc(&mem); /* "H265_Mem" */

    addr = (td_void *)(mem.vir_addr);
    if (addr == TD_NULL) {
        dprint(PRN_ERROR, "%s mem_list alloc error\n", __func__);
        return TD_NULL;
    }

    vfmw_check_sec_func(memset_s(addr, mem.length, 0, mem.length));
    ctx->mem_list[cnt].mem_addr = addr;
    ctx->mem_list[cnt].mem_size = mem.length;
    ctx->hevc_ctx_size += mem.length;

    return addr;
}

STATIC td_void hevc_print_dpb_state(const hevc_ctx *ctx)
{
    fsp_logic *logic_fs = TD_NULL;
    fsp_phy *dec_fs = TD_NULL;
    td_u32 j;

    dprint(PRN_ERROR, "--------------- PrintDPBState START ---------------\n");

    dprint(PRN_ERROR, "dpb: size:%d, used_size:%d, negative:%d, positive:%d, longterm:%d\n",
           ctx->dpb.size, ctx->dpb.used_size, ctx->dpb.negative_ref_frames_in_buffer,
           ctx->dpb.positive_ref_frames_in_buffer, ctx->dpb.ltref_frames_in_buffer);

    for (j = 0; j < ctx->dpb.used_size; j++) {
        dprint(PRN_ERROR, "fs[%d] non_existing:%d state:%d is_ref:%d displayed:%d poc:%d "
               "long_term:%d, fsp_ref:%d fsp_disp:%d logic_id:%d\n",
               j, ctx->dpb.fs[j]->non_existing, ctx->dpb.fs[j]->frame_store_state,
               ctx->dpb.fs[j]->is_reference, ctx->dpb.fs[j]->is_displayed,
               ctx->dpb.fs[j]->poc, ctx->dpb.fs[j]->frame.is_long_term,
               fsp_get_logic(ctx->chan_id, ctx->dpb.fs[j]->logic_fs_id),
               fsp_get_ref(ctx->chan_id, ctx->dpb.fs[j]->logic_fs_id),
               fsp_get_disp(ctx->chan_id, ctx->dpb.fs[j]->logic_fs_id),
               ctx->dpb.fs[j]->logic_fs_id);
    }

    dprint(PRN_ERROR, "\n");

    for (j = 0; j < ctx->dpb.used_size; j++) {
        logic_fs = fsp_get_logic(ctx->chan_id, ctx->dpb.fs[j]->logic_fs_id);
        dec_fs = fsp_get_dec_fs(ctx->chan_id, ctx->dpb.fs[j]->logic_fs_id);
        dprint(PRN_ERROR, "fs[%d]\t", j);

        if (logic_fs != TD_NULL) {
            dprint(PRN_ERROR, "LogicTick:%d\t", logic_fs->tick);
            dprint(PRN_ERROR, "IsRef:%d\t", logic_fs->is_ref);
            dprint(PRN_ERROR, "DispState:%d\t", logic_fs->disp_state);
        } else {
            dprint(PRN_ERROR, "LogicFs:Null\t");
        }

        if (dec_fs != TD_NULL) {
            dprint(PRN_ERROR, "DecTick:%d\t", dec_fs->tick);
        } else {
            dprint(PRN_ERROR, "DecFs:Null\t");
        }

        dprint(PRN_ERROR, "\n");
    }

    dprint(PRN_ERROR, "--------------- PrintDPBState END ---------------\n");

    return;
}

/***********************************************************************
*   函数原型 :  td_s32 hevc_is_slice_unit()
*   功能描述 :  检测当前nal unit是否是一个slice
************************************************************************/
STATIC INLINE td_s8 hevc_is_slice_unit(td_u32 nal_unit_type)
{
    return ((nal_unit_type == NAL_UNIT_CODED_SLICE_TRAIL_R) \
            ||  (nal_unit_type == NAL_UNIT_CODED_SLICE_TRAIL_N) \
            ||  (nal_unit_type == NAL_UNIT_CODED_SLICE_TLA_R) \
            ||  (nal_unit_type == NAL_UNIT_CODED_SLICE_TSA_N) \
            ||  (nal_unit_type == NAL_UNIT_CODED_SLICE_STSA_R) \
            ||  (nal_unit_type == NAL_UNIT_CODED_SLICE_STSA_N) \
            ||  (nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_LP) \
            ||  (nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_RADL) \
            ||  (nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP) \
            ||  (nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL) \
            ||  (nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP) \
            ||  (nal_unit_type == NAL_UNIT_CODED_SLICE_CRA) \
            ||  (nal_unit_type == NAL_UNIT_CODED_SLICE_RADL_N) \
            ||  (nal_unit_type == NAL_UNIT_CODED_SLICE_RADL_R) \
            ||  (nal_unit_type == NAL_UNIT_CODED_SLICE_RASL_N) \
            ||  (nal_unit_type == NAL_UNIT_CODED_SLICE_RASL_R))
           ? 1 : 0;
}

/************************************************************************
*   函数原型 :  td_s32 hevc_is_i_unit()
*   功能描述 :  检测该帧是否指示I unit type
*************************************************************************/
STATIC INLINE td_s8 hevc_is_i_unit(td_u32 nal_unit_type)
{
    return (nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL \
            || nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP   \
            || nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP   \
            || nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_RADL \
            || nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_LP   \
            || nal_unit_type == NAL_UNIT_CODED_SLICE_CRA)
           ? 1 : 0;
}

/*************************************************************************
*   函数原型 :  td_s32 hevc_is_flush_unit()
*   功能描述 :  检测该帧是否指示Flush dpb
*************************************************************************/
STATIC INLINE td_s32 hevc_is_flush_unit(td_u32 nal_unit_type)
{
    return (nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL \
            || nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP   \
            || nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP   \
            || nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_RADL \
            || nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_LP)
           ? 1 : 0;
}

#ifdef HEVC_PVR_SUPPORT
/*************************************************************************
*   函数原型 :  td_s32 hevc_is_pvr_flush_pic()
*   功能描述 :  pvr检测该帧是否当成IDR帧
*************************************************************************/
STATIC INLINE td_s32 hevc_is_pvr_flush_pic(const hevc_ctx *ctx, td_u32 pic_type)
{
    return ((pic_type == HEVC_I_FRAME) &&
            (ctx->extra_data_ptr->pvr_info.b_frm_ref_flag == 1) &&
            (hevc_is_flush_unit(ctx->curr_slice.nal_unit_type) == 1) &&
            (ctx->extra_data_ptr->pvr_info.continuous_flag == 0) &&
            (!(ctx->extra_data_ptr->fast_forward_backward_speed >= -1024 && /* 1024 is times */
               ctx->extra_data_ptr->fast_forward_backward_speed <= 1024))) ? 1 : 0; /* 1024 is times */
}
#endif

STATIC td_void hevc_init_frame_store_para(hevc_ctx *ctx)
{
    td_u32 i;

    /* Init Frame store */
    for (i = 0; i < HEVC_MAX_FRAME_STORE; i++) {
        ctx->frame_store[i].non_existing = 0;                /* a real pic; */
        ctx->frame_store[i].frame_store_state = FS_NOT_USED;
        ctx->frame_store[i].is_reference = 0;
        ctx->frame_store[i].poc = 0;
        ctx->frame_store[i].is_displayed = 0;
        ctx->frame_store[i].pmv_address_idc = HEVC_MAX_PMV_STORE;
        ctx->frame_store[i].apc_idc = HEVC_APC_SIZE;

        ctx->frame_store[i].frame.frame_store = &ctx->frame_store[i];
        ctx->frame_store[i].frame.structure = 0; /* g_frame is frame, will be change when field paired */
    }

    return;
}

STATIC td_void hevc_init_dpb_para(hevc_ctx *ctx)
{
    td_u32 i;

    /* ctx->dpb refresh every stream */
    for (i = 0; i < HEVC_MAX_DPB_NUM; i++) {
        ctx->dpb.fs[i] = ctx->dpb.fs_negative_ref[i] = ctx->dpb.fs_positive_ref[i] = ctx->dpb.fs_ltref[i] = TD_NULL;
    }

    ctx->dpb.used_size = 0;
    ctx->dpb.max_long_term_pic_idx = 0;
    ctx->dpb.ltref_frames_in_buffer = 0;
    ctx->dpb.negative_ref_frames_in_buffer = 0;
    ctx->dpb.positive_ref_frames_in_buffer = 0;
    ctx->dpb.size = HEVC_MAX_DPB_NUM;

    return;
}

STATIC td_void hevc_init_scaling_list_para(hevc_ctx *ctx)
{
    ctx->scaling_list_size[0] = 16; /* 16 is spec define */
    ctx->scaling_list_size[1] = 64; /* 64 is spec define */
    ctx->scaling_list_size[2] = 256; /* 2 is idx, 256 is spec define */
    ctx->scaling_list_size[3] = 1025; /* 3 is idx, 1025 is spec define */

    ctx->scaling_list_size_x[0] = 4; /* 4 is spec define */
    ctx->scaling_list_size_x[1] = 8; /* 8 is spec define */
    ctx->scaling_list_size_x[2] = 16; /* 2 is idx, 16 is spec define */
    ctx->scaling_list_size_x[3] = 32; /* 3 is idx, 32 is spec define */

    ctx->scaling_list_num[0] = 6; /* 6 is spec define */
    ctx->scaling_list_num[1] = 6; /* 6 is spec define */
    ctx->scaling_list_num[2] = 6; /* 2 is idx, 6 is spec define */
    ctx->scaling_list_num[3] = 2; /* 3 is idx, 2 is spec define */

    return;
}

STATIC td_void hevc_init_dec_para(hevc_ctx *ctx)
{
    td_u32 i;

    for (i = 0; i < HEVC_MAX_NUM_LAYERS; i++) {
        ctx->no_output_of_prior_pics_flag[i] = HEVC_FALSE;
        ctx->no_rasl_output_flag[i] = HEVC_TRUE;
        ctx->last_display_poc[i] = -HEVC_MAX_INT;
        ctx->poc_random_access[i] = HEVC_MAX_INT;
    }

    /* nal array refresh */
    hevc_clear_all_nal(ctx);

    hevc_init_frame_store_para(ctx);

    hevc_init_dpb_para(ctx);

    /* pmv slot refresh every stream */
    for (i = 0; i < HEVC_MAX_PMV_STORE; i++) {
        ctx->pmv_store_used_flag[i] = 0;
    }

    /* list refresh every stream */
    for (i = 0; i < HEVC_MAX_LIST_SIZE; i++) {
        ctx->list_x[0][i] = ctx->list_x[1][i] = TD_NULL;
    }

    /* init some global var */
    ctx->curr_nal = TD_NULL;

    /* refresh some statistic variable */
    ctx->num_slice_para = 0;
    ctx->max_bytes_received = 0;
    ctx->curr_msg_slot_num = HEVC_SLICE_SLOT_START_NUM - 1;

    /* init ctx->curr_slice */
    vfmw_check_sec_func(memset_s(&ctx->curr_slice,
                                 sizeof(hevc_slice_segment_header), 0, sizeof(hevc_slice_segment_header)));

    ctx->curr_slice.slice_type = HEVC_I_SLICE;
    ctx->curr_slice.new_pic_type = IS_NEW_PIC;

    ctx->p_pic_found = 0;
    ctx->new_sequence = HEVC_TRUE;

    ctx->allow_start_dec = 0;
    ctx->total_fs_num = HEVC_MAX_FRAME_STORE; /* change while ctx->dpb.size change */
    ctx->total_pmv_num = HEVC_MAX_PMV_STORE;  /* change while ctx->dpb.size change */

    ctx->bit_depth_y = 8; /* 8 is bit depth */
    ctx->bit_depth_c = 8; /* 8 is bit depth */
    ctx->prev_rap_is_bla = HEVC_FALSE;
    ctx->prev_slice_poc = 0;
    ctx->last_slice_poc = 0;
    ctx->frame_rate = 0;

    hevc_init_scaling_list_para(ctx);

    vfmw_check_sec_func(memset_s(&ctx->bs, sizeof(bs_info), 0, sizeof(bs_info)));

    ctx->bs_ptr = &ctx->bs;

    ctx->bs_ptr->ops.is_sec = ctx->is_sec;

    return;
}

STATIC td_s32 hevc_shvc_frame_output(const hevc_ctx *ctx, const hevc_frame_store *fs, vfmw_image *imag)
{
#ifdef VFMW_MV_HEVC_SUPPORT
    td_s32 output_needed = 1;

    if (ctx->is_multi_layers == 1) {
        if (ctx->extra_data_ptr->frame_output_layer == BASE_LAYER) {
            if (fs->view_id != 0) {
                output_needed = 0;
                fsp_set_disp(ctx->chan_id, fs->logic_fs_id, 0);
            }
        } else if (ctx->extra_data_ptr->frame_output_layer == ENHANCED_LAYER) {
            if (fs->view_id != 1) {
                output_needed = 0;
                fsp_set_disp(ctx->chan_id, fs->logic_fs_id, 0);
            }
        }
    }

    return output_needed;
#else
    vfmw_unused(ctx);
    vfmw_unused(fs);
    vfmw_unused(imag);
    return 1;
#endif
}

STATIC td_u8 hevc_get_multi_layer_ext_sps_flag(const hevc_ctx *ctx, const hevc_seq_param_set *sps)
{
#ifdef VFMW_MV_HEVC_SUPPORT
    if (ctx->is_multi_layers == 1 &&
        ctx->curr_nal->nal_layer_id != 0 &&
        sps->sps_ext_or_max_sub_layers_minus1 == 7) { /* 7 sps ext vfmw_max layers */
        return 1;
    } else {
        return 0;
    }
#else
    vfmw_unused(ctx);
    vfmw_unused(sps);
    return 0;
#endif
}

#ifdef VFMW_MV_HEVC_SUPPORT
STATIC td_void hevc_mark_inter_layer_ref_list(hevc_ctx *ctx)
{
    if (ctx->is_multi_layers == 1) {
        td_u32 i;

        for (i = 0; i < ctx->dpb.negative_inter_layer_ref_frames; i++) {
            ctx->dpb.fs_negative_inter_layer_ref[i]->frame.is_long_term = 0;
            ctx->dpb.fs_negative_inter_layer_ref[i]->frame.is_short_term = 1;
        }

        for (i = 0; i < ctx->dpb.positive_inter_layer_ref_frames; i++) {
            ctx->dpb.fs_positive_inter_layer_ref[i]->frame.is_long_term = 0;
            ctx->dpb.fs_positive_inter_layer_ref[i]->frame.is_short_term = 1;
        }
    }
    return;
}

STATIC td_void hevc_parse_slice_ext_flag(hevc_ctx *ctx, const hevc_pic_param_set *pps)
{
    if (ctx->is_multi_layers == 1) {
        td_s32 i;

        i = 0;
        if (pps->num_extra_slice_header_bits > i) {
            i++;
            hevc_u_v(ctx->bs_ptr, 1, "discardable_flag");
        }

        if (pps->num_extra_slice_header_bits > i) {
            i++;
            hevc_u_v(ctx->bs_ptr, 1, "cross_layer_bla_flag");
        }

        if (pps->num_extra_slice_header_bits > i) {
            i++;
            hevc_u_v(ctx->bs_ptr, 1, "poc_reset_flag");
        }

        for (; i < pps->num_extra_slice_header_bits; i++) {
            hevc_u_v(ctx->bs_ptr, 1, "slice_reserved_flag[i]");
        }
    }

    return;
}

STATIC td_u8 hevc_is_base_layer_only(const hevc_ctx *ctx)
{
    if (ctx->extra_data_ptr->frame_output_layer == BASE_LAYER) {
        return 1;
    } else {
        return 0;
    }
}

STATIC td_u8 hevc_is_multi_layer_valid(const hevc_ctx *ctx)
{
    if (ctx->is_multi_layers == 1 &&
        hevc_is_base_layer_only(ctx) == 0) {
        return 1;
    } else {
        return 0;
    }
}

STATIC td_u32 hevc_ceil_log2(td_u32 value)
{
    td_u32 ceil_log2 = 0;

    while (value > (1<< ceil_log2)) {
        ceil_log2++;
    }

    return ceil_log2;
}

td_u32 hevc_check_vps_ext_over_lap(const hevc_vps_extension_set *vps_ext)
{
    td_u32 over_lap = 0;

    if (vps_ext == NULL) {
        dprint(PRN_ERROR, "%s vps_ext = NULL\n", __func__);
    } else {
        over_lap |= (td_u32)(vps_ext->head_reserved != 0);
        over_lap |= (td_u32)(vps_ext->tail_reserved != 0) << 1;
    }
    return over_lap;
}

STATIC td_void hevc_set_multi_layers(hevc_ctx *ctx, const hevc_video_param_set *vps)
{
    if (vps->vps_max_layers_minus1 > 0 && vps->vps_max_layers_minus1 < 63 && vps->vps_max_layer_id > 0) { /* 63 max */
        ctx->is_multi_layers = 1;
    } else {
        ctx->is_multi_layers = 0;
    }
}

STATIC td_void hevc_derive_ext_dpb_size(const hevc_ctx *ctx, hevc_seq_param_set *sps)
{
    td_u32 i;
    td_u32 max_dec_pic_buffering = 0;
    hevc_video_param_set *vps = ctx->vps[sps->video_parameter_set_id];

    sps->reorder_num = 0;
    for (i = 0; i <= sps->sps_max_sub_layers_minus1; i++) {
        sps->max_dec_pic_buffering[i] = (td_u32)vps->vps_max_dec_pic_buffering[i];
        sps->num_reorder_pics[i] = (td_u32)vps->vps_num_reorder_pics[i];
        sps->max_latency_increase[i]  = (td_u32)vps->vps_max_latency_increase[i];
        sps->reorder_num = (sps->num_reorder_pics[i] > sps->reorder_num) ? sps->num_reorder_pics[i] : sps->reorder_num;

        max_dec_pic_buffering = vfmw_max(sps->max_dec_pic_buffering[i], max_dec_pic_buffering);
    }
    sps->dpb_size = max_dec_pic_buffering;
}

STATIC td_s32 hevc_get_num_ref_layer_pics(const hevc_ctx *ctx, const hevc_video_param_set *vps)
{
    td_s32 i, j, k;
    td_s32 num_ref_layer_pics;
    td_s32 curr_layer_id;
    const hevc_vps_extension_set *vps_ext = &(vps->vps_extension);

    curr_layer_id = (td_s32)ctx->curr_nal->nal_layer_id;
    if (curr_layer_id < 0 || curr_layer_id >= HEVC_MAX_NUM_LAYERS) {
        dprint(PRN_ERROR, "line %d func %s curr_layer_id = %d overrun error\n", __LINE__, __func__, curr_layer_id);
        return 0;
    }

    for (i = 0, j = 0; i < vps_ext->num_direct_ref_layers[curr_layer_id]; i++) {
        k = vps_ext->layer_idx_in_vps[vps_ext->ref_layer_id[curr_layer_id][i]];
        if ((vps_ext->sub_layers_vps_max_minus1[k] >= (td_s32)ctx->curr_nal->nuh_temporal_id) &&
            (ctx->curr_nal->nuh_temporal_id == 0 ||
            vps_ext->max_tid_il_ref_pics_plus1[k][vps_ext->layer_idx_in_vps[curr_layer_id]])) {
            j++;
        }
    }
    num_ref_layer_pics = j;

    return num_ref_layer_pics;
}

STATIC td_void hevc_parse_active_ref_layer_pics(hevc_ctx *ctx, const hevc_video_param_set *vps,
    hevc_slice_extension_set *slice_ext)
{
    td_u32 num_inter_layer_ref_pics_minus1;
    td_s32 curr_layer_id = (td_s32)ctx->curr_nal->nal_layer_id;

    if (!vps->vps_extension.max_one_active_ref_layer_flag) {
        num_inter_layer_ref_pics_minus1 = hevc_u_v(ctx->bs_ptr,
            hevc_ceil_log2(vps->vps_extension.num_direct_ref_layers[curr_layer_id]),
            "num_inter_layer_ref_pics_minus1");
        if (num_inter_layer_ref_pics_minus1 > HEVC_MAX_NUM_LAYERS - 1) {
            dprint(PRN_ERROR, "%d:num_inter_layer_ref_pics_minus1(%d)not support", __LINE__,
                num_inter_layer_ref_pics_minus1);
            num_inter_layer_ref_pics_minus1 = 0;
        }
        slice_ext->num_active_ref_layer_pics = (td_s32)(num_inter_layer_ref_pics_minus1 + 1);
    } else {
        slice_ext->num_active_ref_layer_pics = 1;
    }
}

STATIC td_void hevc_parse_inter_layer_pred_idc(hevc_ctx *ctx, const hevc_video_param_set *vps,
    hevc_slice_extension_set *slice_ext)
{
    td_s32 i;
    td_s32 curr_layer_id = (td_s32)ctx->curr_nal->nal_layer_id;

    for (i = 0; i < slice_ext->num_active_ref_layer_pics; i++) {
        slice_ext->inter_layer_pred_layer_idc[i] = hevc_u_v(ctx->bs_ptr,
            hevc_ceil_log2(vps->vps_extension.num_direct_ref_layers[curr_layer_id]), "inter_layer_pred_layer_idc");
    }
}

STATIC td_s32 hevc_parse_ref_layers(hevc_ctx *ctx, const hevc_video_param_set *vps,
    td_u32 *inter_layer_idc_pre_flag)
{
    td_s32 curr_layer_id;
    td_u8 inter_layer_pred_enabled_flag;
    hevc_slice_extension_set *slc_ext = &(ctx->curr_slice.slice_extension);

    curr_layer_id = (td_s32)ctx->curr_nal->nal_layer_id;
    if (curr_layer_id > 0 && !(vps->vps_extension.all_ref_layers_active_flag) &&
        vps->vps_extension.num_direct_ref_layers[curr_layer_id]) {
        inter_layer_pred_enabled_flag = hevc_u_v(ctx->bs_ptr, 1, "inter_layer_pred_enabled_flag");
        hevc_parse_active_ref_layer_pics(ctx, vps, slc_ext);

        if (inter_layer_pred_enabled_flag && vps->vps_extension.num_direct_ref_layers[curr_layer_id] &&
            slc_ext->num_active_ref_layer_pics != vps->vps_extension.num_direct_ref_layers[curr_layer_id]) {
            if (slc_ext->num_active_ref_layer_pics > HEVC_MAX_NUM_LAYERS) {
                dprint(PRN_ERROR, "Error0: Parse num_active_ref_layer_pics %d > HEVC_MAX_NUM_LAYERS %d, not support\n",
                    slc_ext->num_active_ref_layer_pics, HEVC_MAX_NUM_LAYERS);
                    return HEVC_DEC_ERR;
            }

            *inter_layer_idc_pre_flag = 1;
            hevc_parse_inter_layer_pred_idc(ctx, vps, slc_ext);
        }  else if (vps->vps_extension.num_direct_ref_layers[curr_layer_id] == 1) {
            slc_ext->num_active_ref_layer_pics = 1; /* 1 if it has no vps extension */
        } else {
            slc_ext->num_active_ref_layer_pics = 0;
        }
    } else if (vps->vps_extension.all_ref_layers_active_flag == 1) {
        slc_ext->num_active_ref_layer_pics = hevc_get_num_ref_layer_pics(ctx, vps);
    } else {
        slc_ext->num_active_ref_layer_pics = 0;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_parse_slice_extension(hevc_ctx *ctx, const hevc_video_param_set *vps)
{
    td_s32 i, ret;
    td_u32 inter_layer_pred_layer_idc_present_flag = 0;
    hevc_slice_extension_set *slc_ext = &(ctx->curr_slice.slice_extension);

    ret = hevc_parse_ref_layers(ctx, vps, &inter_layer_pred_layer_idc_present_flag);
    if (ret != HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

    if (slc_ext->num_active_ref_layer_pics > HEVC_MAX_NUM_LAYERS) {
        dprint(PRN_ERROR, "Error1: Parse num_active_ref_layer_pics %d > HEVC_MAX_NUM_LAYERS %d, not support\n",
            slc_ext->num_active_ref_layer_pics, HEVC_MAX_NUM_LAYERS);
        return HEVC_DEC_ERR;
    }

    for (i = 0; i < slc_ext->num_active_ref_layer_pics; i++) {
        if (!inter_layer_pred_layer_idc_present_flag) {
            slc_ext->inter_layer_pred_layer_idc[i] = (td_u32)i;
        }

        if (slc_ext->inter_layer_pred_layer_idc[i] > HEVC_MAX_NUM_LAYERS) {
            dprint(PRN_ERROR, "Parse inter_layer_pred_layer_idc[%d] %d > HEVC_MAX_NUM_LAYERS %d, not support",
                i, slc_ext->inter_layer_pred_layer_idc[i], HEVC_MAX_NUM_LAYERS);
            return HEVC_DEC_ERR;
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_u32 hevc_ref_pic_set_zero_flag(hevc_vps_extension_set *vps_ext, td_s32 curr_layer_id, td_s32 ref_layer_id)
{
    td_u32 ref_pic_set_zero_flag;

    ref_pic_set_zero_flag = (vps_ext->view_id_val[curr_layer_id] <= vps_ext->view_id_val[0] &&
                             vps_ext->view_id_val[curr_layer_id] <= vps_ext->view_id_val[ref_layer_id]) ||
                            (vps_ext->view_id_val[curr_layer_id] >= vps_ext->view_id_val[0] &&
                             vps_ext->view_id_val[curr_layer_id] >= vps_ext->view_id_val[ref_layer_id]);
    return ref_pic_set_zero_flag;
}

STATIC td_void hevc_update_inter_layer_ref_list(hevc_ctx *ctx)
{
    td_u32 m = 0;
    td_u32 n = 0;
    td_s32 i, j, curr_layer_id, ref_layer_id;
    td_u32 ref_pic_set_zero_flag;
    td_u32 inter_layer_pred_layer_idc;
    hevc_vps_extension_set *vps_ext = TD_NULL;

    curr_layer_id = (td_s32)ctx->curr_slice.view_id;
    if (curr_layer_id == 0) {
        ctx->dpb.negative_inter_layer_ref_frames = 0;
        ctx->dpb.positive_inter_layer_ref_frames = 0;
        return;
    }
    vfmw_alert(ctx->curr_vps != TD_NULL);
    vps_ext = &ctx->curr_vps->vps_extension;

    for (i = 0; i < ctx->curr_slice.slice_extension.num_active_ref_layer_pics; i++) {
        inter_layer_pred_layer_idc = ctx->curr_slice.slice_extension.inter_layer_pred_layer_idc[i];
        vfmw_check(((curr_layer_id >= 0 && curr_layer_id < HEVC_MAX_NUM_LAYERS) &&
            inter_layer_pred_layer_idc < HEVC_MAX_NUM_LAYERS));

        ref_layer_id = vps_ext->ref_layer_id[curr_layer_id][inter_layer_pred_layer_idc];
        vfmw_check_break((ref_layer_id >= 0 && ref_layer_id < HEVC_MAX_NUM_LAYERS));

        ref_pic_set_zero_flag = hevc_ref_pic_set_zero_flag(vps_ext, curr_layer_id, ref_layer_id);

        for (j = 0; (td_u32)j < ctx->dpb.used_size; j++) {
            /* view_id is layer_id */
            if ((ctx->dpb.fs[j]->poc != ctx->curr_slice.poc) || (ctx->dpb.fs[j]->view_id != ref_layer_id)) {
                continue;
            }

            ctx->dpb.fs[j]->is_reference = 1;
            fsp_set_ref(ctx->chan_id, ctx->dpb.fs[j]->logic_fs_id, 1);

            ctx->dpb.fs[j]->frame.is_short_term = 0;
            ctx->dpb.fs[j]->frame.is_long_term = 1;
            ctx->dpb.fs[j]->frame.is_inter_layer = 1;

            if (ref_pic_set_zero_flag) {
                ctx->dpb.fs_negative_inter_layer_ref[m++] = ctx->dpb.fs[j];
            } else {
                ctx->dpb.fs_positive_inter_layer_ref[n++] = ctx->dpb.fs[j];
            }
            break;
        }
    }

    ctx->dpb.negative_inter_layer_ref_frames = m;
    ctx->dpb.positive_inter_layer_ref_frames = n;
    while (m < ctx->dpb.size) {
        ctx->dpb.fs_negative_inter_layer_ref[m++] = TD_NULL;
    }
    while (n < ctx->dpb.size) {
        ctx->dpb.fs_positive_inter_layer_ref[n++] = TD_NULL;
    }

    return;
}

STATIC td_void hevc_set_necessary_layer_flags(const hevc_video_param_set *vps,
                                              hevc_vps_extension_set *vps_ext,
                                              td_u32 ols_idx, td_u32 is_layer_idx, td_s32 is_idx)
{
    td_u32 curr_layer_id;
    td_u32 ref_layer_id;
    td_u32 rls_layer_idx;
    td_u32 layer_idx_in_vps;

    if (ols_idx >= HEVC_MAX_VPS_OUTPUTLAYER_SETS || is_layer_idx >= HEVC_MAX_NUM_LAYERS ||
        is_idx >= HEVC_MAX_VPS_OP_SETS_PLUS1) {
        dprint(PRN_ERROR, "%s ols_idx(%d)/is_layer_idx(%d)/is_idx(%d) out of range.\n", __func__,
            ols_idx, is_layer_idx, is_idx);
        return;
    }

    vps_ext->necessary_layer_flag[ols_idx][is_layer_idx] = 1;
    curr_layer_id = vps->layer_set_layer_id_list[ols_idx][is_layer_idx];
    for (rls_layer_idx = 0; rls_layer_idx < is_layer_idx; rls_layer_idx++) {
        ref_layer_id = vps->layer_set_layer_id_list[is_idx][rls_layer_idx];
        layer_idx_in_vps = (td_u32)vps_ext->layer_idx_in_vps[curr_layer_id];
        if (layer_idx_in_vps >= HEVC_MAX_NUM_LAYERS || ref_layer_id >= HEVC_MAX_NUM_LAYERS) {
            dprint(PRN_ERROR, "line %d func %s layer_idx_in_vps = %d or ref_layer_id = %d overrun error\n",
                __LINE__, __func__, layer_idx_in_vps, ref_layer_id);
            break;
        }
        if (vps_ext->direct_dependency_flag[layer_idx_in_vps][vps_ext->layer_idx_in_vps[ref_layer_id]]) {
            vps_ext->necessary_layer_flag[ols_idx][rls_layer_idx] = 1;
        }
    }
}

STATIC td_void hevc_derive_necessary_layer_flags(hevc_ctx *ctx, const hevc_video_param_set *vps,
    hevc_vps_extension_set *vps_ext, td_u32 idx)
{
    td_u32 ols_idx;
    td_u32 is_layer_idx;
    td_u32 num_layers;
    td_s32 is_idx;

    for (ols_idx = 0; ols_idx < (td_u32)vps_ext->num_output_layer_sets; ols_idx++) {
        is_idx = vps_ext->layer_set_idx_for_ols_minus1[ols_idx] + 1;

        if (is_idx >= HEVC_MAX_VPS_OUTPUTLAYER_SETS) {
            dprint(PRN_ERROR, "%s is_idx(%d) out of range.\n", __func__, is_idx);
            return;
        }

        num_layers = vps->num_layers_in_id_list[is_idx];
        for (is_layer_idx = 0; is_layer_idx < num_layers; is_layer_idx++) {
            if (ols_idx >= HEVC_MAX_VPS_OUTPUTLAYER_SETS || is_layer_idx >= HEVC_MAX_NUM_LAYERS) {
                dprint(PRN_ERROR, "%s ols_idx(%d)/is_layer_idx(%d) out of range.\n", __func__, ols_idx, is_layer_idx);
                return;
            }

            vps_ext->necessary_layer_flag[ols_idx][is_layer_idx] = 0;
            if (vps_ext->output_layer_flag[ols_idx][is_layer_idx]) {
                hevc_set_necessary_layer_flags(vps, vps_ext, ols_idx, is_layer_idx, is_idx);
            }
        }
    }
}

STATIC td_void hevc_derive_target_layer_id_list(hevc_ctx *ctx, const hevc_video_param_set *vps,
    const hevc_vps_extension_set *vps_ext, td_s32 ols_idx)
{
    td_s32 is_idx;
    td_u32 j;

    if (ols_idx < vps_ext->num_layer_sets) {
        is_idx = ols_idx;
    } else {
        is_idx = vps_ext->layer_set_idx_for_ols_minus1[ols_idx] + 1;
    }
    if (is_idx >= HEVC_MAX_VPS_OUTPUTLAYER_SETS || is_idx < 0) {
        dprint(PRN_ERROR, "line %d func %s is_idx = %d overrun error\n", __LINE__, __func__, is_idx);
        return;
    }

    for (j = 0; j < vps->num_layers_in_id_list[is_idx]; j++) {
        if (vps_ext->necessary_layer_flag[is_idx][j]) {
        }

        if (vps_ext->output_layer_flag[ols_idx][j]) {
        }
    }

    return;
}

STATIC td_s32 hevc_parse_chroma_qp_offset_list(const hevc_ctx *ctx)
{
    td_u32 i;
    td_u32 chroma_qp_offset_list_enabled_flag;

    hevc_u_v(ctx->bs_ptr, 1, "cross_component_prediction_enable_flag");
    chroma_qp_offset_list_enabled_flag = hevc_ue_v(ctx->bs_ptr, "chorma_qp_offset_list_len_minus_1");
    if (chroma_qp_offset_list_enabled_flag > 5) {  /* 5 :protocal required */
        dprint(PRN_ERROR, "chroma_qp_offset_list_len_minus1 %d > 5\n", chroma_qp_offset_list_enabled_flag);
        return HEVC_DEC_ERR;
    }

    for (i = 0; i < chroma_qp_offset_list_enabled_flag; i++) {
        hevc_se_v(ctx->bs_ptr, "cb_qp_offset_list");
        hevc_se_v(ctx->bs_ptr, "cr_qp_offset_list");
    }

    hevc_ue_v(ctx->bs_ptr, "log2_sao_offset_scale_luma");
    hevc_ue_v(ctx->bs_ptr, "log2_sao_offset_scale_chroma");

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_init_pps_ext_param_0(hevc_pps_extension_set *pps_ext)
{
    pps_ext->scaled_ref_layer_left_offset[0] = 0;
    pps_ext->scaled_ref_layer_top_offset[0] = 0;
    pps_ext->scaled_ref_layer_right_offset[0] = 0;
    pps_ext->scaled_ref_layer_bottom_offset[0] = 0;
    pps_ext->ref_region_left_offset[0] = 0;
    pps_ext->ref_region_top_offset[0] = 0;
    pps_ext->ref_region_right_offset[0] = 0;
    pps_ext->ref_region_bottom_offset[0] = 0;
    pps_ext->phase_hor_luma[0] = 0;
    pps_ext->phase_ver_luma[0] = 0;
    pps_ext->phase_hor_chroma_plus8[0] = 8; /* 8 is protocal required */
    pps_ext->phase_ver_chroma_plus8[0] = 8; /* 8 is protocal required */
}

STATIC td_s32 hevc_parse_multilayer_extension_param(const hevc_ctx *ctx, hevc_pps_extension_set *pps_ext,
    td_u32 *num_ref_loc_offsets)
{
    hevc_u_v(ctx->bs_ptr, 1, "poc_reset_info_present_flag");
    pps_ext->pps_infer_scaling_list_flag = (td_u8)hevc_u_v(ctx->bs_ptr, 1, "pps_infer_scaling_list_flag");
    if (pps_ext->pps_infer_scaling_list_flag) {
        pps_ext->pps_scaling_list_ref_layer_id = hevc_u_v(ctx->bs_ptr, 0x6, "pps_scaling_list_ref_layer_id");
        if (pps_ext->pps_scaling_list_ref_layer_id >= HEVC_MAX_NUM_LAYERS) {
            dprint(PRN_ERROR, "pps_scaling_list_ref_layer_id %d > HEVC_MAX_NUM_LAYERS %d\n",
                pps_ext->pps_scaling_list_ref_layer_id, HEVC_MAX_NUM_LAYERS);
            return HEVC_DEC_ERR;
        }
    } else {
        pps_ext->pps_scaling_list_ref_layer_id = 0;
    }

    *num_ref_loc_offsets = hevc_ue_v(ctx->bs_ptr, "num_ref_loc_offsets");
    if (*num_ref_loc_offsets >= HEVC_EXT_MAX_REF_LOC_OFFSET) {
        dprint(PRN_ERROR, "num_ref_loc_offsets %d > HEVC_EXT_MAX_REF_LOC_OFFSET %d\n",
            *num_ref_loc_offsets, HEVC_EXT_MAX_REF_LOC_OFFSET);
        return HEVC_DEC_ERR;
    }

    hevc_init_pps_ext_param_0(pps_ext);

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_parse_ref_offset(const hevc_ctx *ctx, hevc_pps_extension_set *pps_ext, td_u32 idx)
{
    td_u32 scaled_ref_layer_offset_present_flag, ref_region_offset_present_flag;

    scaled_ref_layer_offset_present_flag = hevc_u_v(ctx->bs_ptr, 1, "scaled_ref_layer_offset_present_flag");
    if (scaled_ref_layer_offset_present_flag) {
        pps_ext->scaled_ref_layer_left_offset[idx] = hevc_se_v(ctx->bs_ptr, "scaled_ref_layer_left_offset");
        pps_ext->scaled_ref_layer_top_offset[idx] = hevc_se_v(ctx->bs_ptr, "scaled_ref_layer_top_offset");
        pps_ext->scaled_ref_layer_right_offset[idx] = hevc_se_v(ctx->bs_ptr, "scaled_ref_layer_right_offset");
        pps_ext->scaled_ref_layer_bottom_offset[idx] = hevc_se_v(ctx->bs_ptr, "scaled_ref_layer_bottom_offset");
    } else {
        pps_ext->scaled_ref_layer_left_offset[idx] = 0;
        pps_ext->scaled_ref_layer_top_offset[idx] = 0;
        pps_ext->scaled_ref_layer_right_offset[idx] = 0;
        pps_ext->scaled_ref_layer_bottom_offset[idx] = 0;
    }

    ref_region_offset_present_flag = hevc_u_v(ctx->bs_ptr, 1, "ref_region_offset_present_flag");
    if (ref_region_offset_present_flag) {
        pps_ext->ref_region_left_offset[idx] = hevc_se_v(ctx->bs_ptr, "ref_region_left_offset");
        pps_ext->ref_region_top_offset[idx] = hevc_se_v(ctx->bs_ptr, "ref_region_top_offset");
        pps_ext->ref_region_right_offset[idx] = hevc_se_v(ctx->bs_ptr, "ref_region_right_offset");
        pps_ext->ref_region_bottom_offset[idx] = hevc_se_v(ctx->bs_ptr, "ref_region_bottom_offset");
    } else {
        pps_ext->ref_region_left_offset[idx] = 0;
        pps_ext->ref_region_top_offset[idx] = 0;
        pps_ext->ref_region_right_offset[idx] = 0;
        pps_ext->ref_region_bottom_offset[idx] = 0;
    }
}

STATIC td_void hevc_parse_resample_phase_set(const hevc_ctx *ctx, hevc_pps_extension_set *pps_ext, td_u32 idx, td_u32 i)
{
    pps_ext->resample_phase_set_present_flag[i] = hevc_u_v(ctx->bs_ptr, 1, "resample_phase_set_present_flag");
    if (pps_ext->resample_phase_set_present_flag[i]) {
        pps_ext->phase_hor_luma[idx] = hevc_ue_v(ctx->bs_ptr, "phase_hor_luma");
        pps_ext->phase_ver_luma[idx] = hevc_ue_v(ctx->bs_ptr, "phase_ver_luma");
        pps_ext->phase_hor_chroma_plus8[idx] = hevc_ue_v(ctx->bs_ptr, "phase_hor_chroma_plus8");
        pps_ext->phase_ver_chroma_plus8[idx] = hevc_ue_v(ctx->bs_ptr, "phase_ver_chroma_plus8");
    } else {
        pps_ext->phase_hor_luma[idx] = 0;
        pps_ext->phase_ver_luma[idx] = 0;
        pps_ext->phase_hor_chroma_plus8[idx] = 8; /* 8 is protocal required */
        pps_ext->phase_ver_chroma_plus8[idx] = 8; /* 8 is protocal required */
    }
}

STATIC td_s32 hevc_parse_pps_extension(const hevc_ctx *ctx, hevc_pic_param_set *pps)
{
    td_u32 i;
    td_u32 idx;
    td_u8 pps_range_extensions_flag = 0;
    td_u8 pps_multilayer_extension_flag = 0;
    td_u8 pps_extension_5bits = 0;
    td_u32 num_ref_loc_offsets = 0;
    hevc_pps_extension_set *pps_ext = &(pps->pps_extension);

    if (ctx->is_multi_layers == 1) {
        pps_range_extensions_flag = (td_u8)hevc_u_v(ctx->bs_ptr, 1, "pps_range_extensions_flag");
        pps_multilayer_extension_flag = (td_u8)hevc_u_v(ctx->bs_ptr, 1, "pps_multilayer_extension_flag");
        hevc_u_v(ctx->bs_ptr, 1, "pps_3d_extension_flag");
        pps_extension_5bits = (td_u8)hevc_u_v(ctx->bs_ptr, 5, "pps_extension_flag"); /* protocal required 5 bits */
    }

    if (pps_range_extensions_flag == 1) {
        if (pps->transform_skip_enabled_flag) {
            hevc_ue_v(ctx->bs_ptr, "transformable_skip_flag_enabled_flag");
        }

        if (hevc_parse_chroma_qp_offset_list(ctx) != HEVC_DEC_NORMAL) {
            return HEVC_DEC_ERR;
        }
    }

    if (pps_multilayer_extension_flag) {
        if (hevc_parse_multilayer_extension_param(ctx, pps_ext, &num_ref_loc_offsets) != HEVC_DEC_NORMAL) {
            return HEVC_DEC_ERR;
        }
    }

    for (i = 0; i < num_ref_loc_offsets; i++) {
        idx = hevc_u_v(ctx->bs_ptr, 6, "ref_loc_offset_layer[i]"); /* 6 :protocal required */
        if (idx >= HEVC_EXT_MAX_REF_LOC_OFFSET) {
            dprint(PRN_ERROR, "ref_loc_offset_layer_id %d > HEVC_EXT_MAX_REF_LOC_OFFSET %d\n",
                idx, HEVC_EXT_MAX_REF_LOC_OFFSET);
            return HEVC_DEC_ERR;
        }

        hevc_parse_ref_offset(ctx, pps_ext, idx);
        hevc_parse_resample_phase_set(ctx, pps_ext, idx, i);
    }
    hevc_u_v(ctx->bs_ptr, 1, "colour_mapping_enabled_flag");

    if (pps_extension_5bits) {
        while (hevc_more_rbsp_data(ctx) == HEVC_TRUE) {
            pps->pps_extension_data_flag = hevc_u_v(ctx->bs_ptr, 1, "pps_extension_data_flag");
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_derive_sps_rep_format(const hevc_ctx *ctx, hevc_seq_param_set *sps)
{
    td_u32 idx;
    td_u32 update_rep_format_flag;
    hevc_vps_extension_set *vps_ext = &ctx->vps[sps->video_parameter_set_id]->vps_extension;

    update_rep_format_flag = hevc_u_v(ctx->bs_ptr, 1, "update_rep_format_flag");
    if (update_rep_format_flag) {
        idx = hevc_u_v(ctx->bs_ptr, 8, "sps_rep_formaty_idx"); /* protocal required 8 bits */
    } else {
        idx = vps_ext->vps_rep_format_idx[vps_ext->layer_idx_in_vps[ctx->curr_nal->nal_layer_id]];
    }
    if (idx >= HEVC_MAX_NUM_LAYERS) {
        dprint(PRN_ERROR, "sps_rep_format_idx %u > HEVC_MAX_NUM_LAYERS %d\n", idx, HEVC_MAX_NUM_LAYERS);
        return HEVC_DEC_ERR;
    }

    sps->chroma_format_idc = vps_ext->rep_format[idx].chroma_format_vps_idc;
    sps->separate_colour_plane_flag = vps_ext->rep_format[idx].separate_colour_plane_vps_flag;
    sps->pic_width_in_luma_samples = vps_ext->rep_format[idx].pic_width_vps_in_luma_samples;
    sps->pic_height_in_luma_samples = vps_ext->rep_format[idx].pic_height_vps_in_luma_samples;
    sps->bit_depth_luma = vps_ext->rep_format[idx].bit_depth_vps_luma_minus8 + 8; /* 8 :protocal required */
    sps->bit_depth_chroma = vps_ext->rep_format[idx].bit_depth_vps_chroma_minus8 + 8; /* 8 :protocal required */
    sps->conf_win_left_offset = vps_ext->rep_format[idx].conf_win_vps_left_offset;
    sps->conf_win_right_offset = vps_ext->rep_format[idx].conf_win_vps_right_offset;
    sps->conf_win_top_offset = vps_ext->rep_format[idx].conf_win_vps_top_offset;
    sps->conf_win_bottom_offset = vps_ext->rep_format[idx].conf_win_vps_bottom_offset;

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_parse_rep_format(const hevc_ctx *ctx,
    const hevc_rep_format_set *pre_rep_format, hevc_rep_format_set *rep_format)
{
    td_u8 chroma_and_bit_depth_vps_present_flag;
    td_u8 conformance_window_vps_flag;

    rep_format->pic_width_vps_in_luma_samples =
        hevc_u_v(ctx->bs_ptr, 16, "pic_width_vps_in_luma_samples"); /* protocal required 16 bits */
    rep_format->pic_height_vps_in_luma_samples =
        hevc_u_v(ctx->bs_ptr, 16, "pic_height_vps_in_luma_samples"); /* protocal required 16 bits */
    chroma_and_bit_depth_vps_present_flag = hevc_u_v(ctx->bs_ptr, 1, "chroma_and_bit_depth_vps_present_flag");
    if (chroma_and_bit_depth_vps_present_flag) {
        rep_format->chroma_format_vps_idc =
            hevc_u_v(ctx->bs_ptr, 2, "chroma_format_vps_idc"); /* protocal required 2 bits */
        if (rep_format->chroma_format_vps_idc == 0x03) {
            rep_format->separate_colour_plane_vps_flag = hevc_u_v(ctx->bs_ptr, 1, "separate_colour_plane_vps_flag");
        } else if (pre_rep_format != NULL) {
            rep_format->separate_colour_plane_vps_flag = pre_rep_format->separate_colour_plane_vps_flag;
        } else {
            rep_format->separate_colour_plane_vps_flag = 0;
        }
        rep_format->bit_depth_vps_luma_minus8 =
            hevc_u_v(ctx->bs_ptr, 4, "bit_depth_vps_luma_minus8");  /* protocal required 4 bits */
        rep_format->bit_depth_vps_chroma_minus8 =
            hevc_u_v(ctx->bs_ptr, 4, "bit_depth_vps_chroma_minus8");  /* protocal required 4 bits */
    } else if (pre_rep_format != NULL) {
        rep_format->chroma_format_vps_idc = pre_rep_format->chroma_format_vps_idc;
        rep_format->separate_colour_plane_vps_flag = pre_rep_format->separate_colour_plane_vps_flag;
        rep_format->bit_depth_vps_luma_minus8 = pre_rep_format->bit_depth_vps_chroma_minus8;
        rep_format->bit_depth_vps_chroma_minus8 = pre_rep_format->bit_depth_vps_chroma_minus8;
    }

    conformance_window_vps_flag = hevc_u_v(ctx->bs_ptr, 1, "conformance_window_vps_flag");
    if (conformance_window_vps_flag) {
        rep_format->conf_win_vps_left_offset = hevc_ue_v(ctx->bs_ptr, "conf_win_vps_left_offset");
        rep_format->conf_win_vps_right_offset = hevc_ue_v(ctx->bs_ptr, "conf_win_vps_right_offset");
        rep_format->conf_win_vps_top_offset = hevc_ue_v(ctx->bs_ptr, "conf_win_vps_top_offset");
        rep_format->conf_win_vps_bottom_offset = hevc_ue_v(ctx->bs_ptr, "conf_win_vps_bottom_offset");
    } else {
        rep_format->conf_win_vps_left_offset = 0;
        rep_format->conf_win_vps_right_offset = 0;
        rep_format->conf_win_vps_top_offset = 0;
        rep_format->conf_win_vps_bottom_offset = 0;
    }

    if (rep_format->bit_depth_vps_luma_minus8 > 8 || rep_format->bit_depth_vps_chroma_minus8 > 8) { /* 8 : bit depth */
        dprint(PRN_ERROR, "bit_depth_vps_luma_minus8(%d) or bit_depth_vps_chroma_minus8(%d) out of range [0,8]\n",
            rep_format->bit_depth_vps_luma_minus8, rep_format->bit_depth_vps_chroma_minus8);
        /* force to continue */
        rep_format->bit_depth_vps_luma_minus8 = 0;
        rep_format->bit_depth_vps_chroma_minus8 = 0;
    }
}

STATIC td_void hevc_derive_vps_ext_dpb(const hevc_ctx *ctx, const hevc_video_param_set *vps)
{
    td_u32 i, j, k;
    td_u32 idx;
    td_u32 sub_layer_flag_info_present_flag;
    td_u32 sub_layer_dpb_info_present_flag;
    const hevc_vps_extension_set *vps_ext = &(vps->vps_extension);

    sub_layer_dpb_info_present_flag = 0;
    for (i = 1; i < vps_ext->num_output_layer_sets; i++) {
        sub_layer_flag_info_present_flag = hevc_u_v(ctx->bs_ptr, 1, "sub_layer_flag_info_present_flag");
        idx = (i < vps_ext->num_layer_sets) ? i : ((td_u32)vps_ext->layer_set_idx_for_ols_minus1[i] + 1);
        for (j = 0; j < vps_ext->max_sub_layers_in_layer_set_minus1[idx]; j++) {
            if (j > 0 && sub_layer_flag_info_present_flag) {
                sub_layer_dpb_info_present_flag = hevc_u_v(ctx->bs_ptr, 1, "sub_layer_dpb_info_present_flag");
            } else if (j == 0) {
                sub_layer_dpb_info_present_flag = 1;
            } else {
                sub_layer_dpb_info_present_flag = 0;
            }
        }

        if (!sub_layer_dpb_info_present_flag) {
            continue;
        }

        for (k = 0; k < vps->num_layers_in_id_list[idx]; k++) {
            if (vps_ext->necessary_layer_flag[i][k] &&
                (vps->vps_base_layer_internal_flag || (vps->layer_set_layer_id_list[idx][k] != 0))) {
                hevc_ue_v(ctx->bs_ptr, "max_vps_dec_pic_buffering");
            }
            hevc_ue_v(ctx->bs_ptr, "max_vps_num_reorder_pics");
            hevc_ue_v(ctx->bs_ptr, "max_vps_latency_increase_plus1");
        }
    }

    return;
}

STATIC td_void hevc_parse_video_signal_info(const hevc_ctx *ctx)
{
    hevc_u_v(ctx->bs_ptr, 3, "video_vps_format");  /* protocal required 3 bits */
    hevc_u_v(ctx->bs_ptr, 1, "video_full_range_vps_flag");
    hevc_u_v(ctx->bs_ptr, 8, "colour_primaries_vps");  /* protocal required 8 bits */
    hevc_u_v(ctx->bs_ptr, 8, "transfer_characteristics_vps");  /* protocal required 8 bits */
    hevc_u_v(ctx->bs_ptr, 8, "matrix_coeffs_vps");  /* protocal required 8 bits */
}

STATIC td_void hevc_parse_bsp_schedules(const hevc_ctx *ctx, td_u32 num_partitions_in_scheme_minus1,
    td_u32 vps_num_hrd_params, td_u32 num_bsp_schedules_minus1)
{
    td_u32 j, k;

    for (j = 0; j <= num_bsp_schedules_minus1; j++) {
        for (k = 0; k <= num_partitions_in_scheme_minus1; k++) {
            hevc_u_v(ctx->bs_ptr, hevc_ceil_log2(vps_num_hrd_params), "bsp_hrd_idx");
            hevc_ue_v(ctx->bs_ptr, "bsp_sched_idx");
        }
    }
}

STATIC td_s32 hevc_process_bsp_partiton_parameters(const hevc_ctx *ctx, const hevc_video_param_set *vps,
    td_u32 vps_num_hrd_params, td_u32 num_output_layer_idx, const hevc_vps_extension_set *vps_ext)
{
    td_u32 i, j, k, h, r, t;
    td_u32 pos;
    td_u32 num_signalled_partitioning_schemes;
    td_u32 num_bsp_schedules_minus1;
    td_u32 num_partitions_in_scheme_minus1[HEVC_MAX_PARTITION_NUM + 1];

    h = num_output_layer_idx;
    if (h >= HEVC_MAX_VPS_OUTPUTLAYER_SETS) {
        dprint(PRN_ERROR, "line %d func %s idx = %d overrun error\n", __LINE__, __func__, h);
        return HEVC_DEC_ERR;
    }
    if (vps->num_layers_in_id_list[h] > 2) { /* 2 :vfmw_max layer */
        num_partitions_in_scheme_minus1[0] = vps->num_layers_in_id_list[h] - 1;
    } else {
        num_partitions_in_scheme_minus1[0] = 1;
    }

    num_signalled_partitioning_schemes = hevc_ue_v(ctx->bs_ptr, "num_signalled_partitioning_schemes");
    if (num_signalled_partitioning_schemes > HEVC_MAX_PARTITION_NUM) {
        dprint(PRN_ERROR, "OPL %d num_signalled_partitioning_schemes %d out of range [0,%d]\n", h,
            num_signalled_partitioning_schemes, HEVC_MAX_PARTITION_NUM);
        return HEVC_DEC_ERR;
    }

    for (j = 1; j < num_signalled_partitioning_schemes + 1; j++) {
        num_partitions_in_scheme_minus1[j] = hevc_ue_v(ctx->bs_ptr, "num_partitions_in_scheme_minus1");
        if (num_partitions_in_scheme_minus1[j] > vps->num_layers_in_id_list[h] - 1) {
            dprint(PRN_ERROR, "num_partitions_in_scheme_minus1[%d] %d is out of range!\n", j, num_partitions_in_scheme_minus1[j]);
            return HEVC_DEC_ERR;
        }
        for (k = 0; k <= num_partitions_in_scheme_minus1[j]; k++) {
            pos = (h < vps_ext->num_layer_sets) ? h : ((td_u32)vps_ext->layer_set_idx_for_ols_minus1[h] + 1);
            for (r = 0; r < vps->num_layers_in_id_list[pos]; r++) {
                hevc_u_v(ctx->bs_ptr, 1, "layer_included_in_partition_flag");
            }
        }
    }

    for (i = 0; i < num_signalled_partitioning_schemes + 1; i++) {
        pos = (h < vps_ext->num_layer_sets) ? h : ((td_u32)vps_ext->layer_set_idx_for_ols_minus1[h] + 1);
        for (t = 0; t <= vps_ext->max_sub_layers_in_layer_set_minus1[pos]; t++) {
            num_bsp_schedules_minus1 = hevc_ue_v(ctx->bs_ptr, "num_bsp_schedules_minus1");
            if (num_bsp_schedules_minus1 > 31) { /* 31 :protocal required */
                dprint(PRN_ERROR, "OPL %d Partition %d Tid %dnum_bsp_schedules_minus1 %d out of range [0,31]\n",
                    h, i, t, num_bsp_schedules_minus1);
                return HEVC_DEC_ERR;
            }

            hevc_parse_bsp_schedules(ctx, num_partitions_in_scheme_minus1[i],
                vps_num_hrd_params, num_bsp_schedules_minus1);
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_parse_bsp_partiton_parameters(const hevc_ctx *ctx, const hevc_video_param_set *vps,
    td_u32 vps_num_hrd_params)
{
    td_u32 h;
    const hevc_vps_extension_set *vps_ext = &(vps->vps_extension);

    for (h = 1; h < vps_ext->num_output_layer_sets; h++) {
        if (hevc_process_bsp_partiton_parameters(ctx, vps, vps_num_hrd_params, h, vps_ext) != HEVC_DEC_NORMAL) {
            return HEVC_DEC_ERR;
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_parse_bsp_hrd_param(hevc_ctx *ctx, const hevc_video_param_set *vps,
    const hevc_vps_vui_bsp_hrdpara_set *vps_vui_bsp)
{
    td_u32 i;
    td_s32 ret;
    td_u32 vps_num_add_hrd_params;
    td_u32 num_sub_layer_hrd_minus1;
    td_u32 vps_num_hrd_params;
    td_u32 cprms_add_present_flag;

    vfmw_check_sec_func(memset_s(&(ctx->hrd_param), sizeof(ctx->hrd_param), 0, sizeof(hevc_hrd_param_set)));

    vps_num_add_hrd_params = hevc_ue_v(ctx->bs_ptr, "vps_num_add_hrd_params");
    vps_num_hrd_params = vps->vps_num_hrd_parameters + vps_num_add_hrd_params;
    if (vps_num_hrd_params > HEVC_MAX_VPS_OP_SETS_PLUS1) {
        dprint(PRN_ERROR, "vps_num_hrd_params %d out of range [0,%d]\n",
            vps_num_hrd_params, HEVC_MAX_VPS_OP_SETS_PLUS1);
        return HEVC_DEC_ERR;
    }

    for (i = vps->vps_num_hrd_parameters; i < vps_num_hrd_params; i++) {
        if (i > 0) {
            cprms_add_present_flag = hevc_u_v(ctx->bs_ptr, 1, "cprms_add_present_flag");
        } else {
            cprms_add_present_flag = 1;
        }

        num_sub_layer_hrd_minus1 = hevc_ue_v(ctx->bs_ptr, "num_sub_layer_hrd_minus1");
        if (num_sub_layer_hrd_minus1 > HEVC_MAX_TEMPLAYER - 1) {
            dprint(PRN_ERROR, "hrd %d num_sub_layer_hrd_minus1 %d out of range [0,%d]\n",
                i, num_sub_layer_hrd_minus1, HEVC_MAX_TEMPLAYER - 1);
            return HEVC_DEC_ERR;
        }

        (td_void)hevc_dec_hrd_param(ctx, &(ctx->hrd_param), cprms_add_present_flag, num_sub_layer_hrd_minus1);
    }

    if (vps_num_hrd_params <= 0) {
        return HEVC_DEC_NORMAL;
    }

    ret = hevc_parse_bsp_partiton_parameters(ctx, vps, vps_num_hrd_params);
    if (ret != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "HEVCC_ParseBspPartitonParameters failed.\n");
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_parse_rate(const hevc_ctx *ctx, const hevc_video_param_set *vps)
{
    td_u32 i, j;
    td_u32 bit_rate_present_vps_flag;
    td_u32 pic_rate_present_vps_flag;
    td_u32 bit_rate_present_flag = 0;
    td_u32 pic_rate_present_flag = 0;
    td_s32 vps_max_sub_layers_minus1 = vps->vps_max_sub_layers - 1;

    bit_rate_present_vps_flag = hevc_u_v(ctx->bs_ptr, 1, "bit_rate_present_vps_flag");
    pic_rate_present_vps_flag = hevc_u_v(ctx->bs_ptr, 1, "pic_rate_present_vps_flag");
    if ((bit_rate_present_vps_flag == 0) && (pic_rate_present_vps_flag == 0)) {
        return;
    }

    for (i = vps->vps_base_layer_internal_flag ? 0 : 1; i <= vps->vps_num_layer_sets_minus1; i++) {
        for (j = 0; j <= (td_u32)vps_max_sub_layers_minus1; j++) {
            if (bit_rate_present_vps_flag) {
                bit_rate_present_flag = hevc_u_v(ctx->bs_ptr, 1, "bit_rate_present_flag");
            }

            if (pic_rate_present_vps_flag) {
                pic_rate_present_flag = hevc_u_v(ctx->bs_ptr, 1, "pic_rate_present_flag");
            }

            if (bit_rate_present_flag) {
                hevc_u_v(ctx->bs_ptr, 16, "avg_bit_rate");  /* protocal required 16 bits */
                hevc_u_v(ctx->bs_ptr, 16, "max_bit_rate");  /* protocal required 16 bits */
            }

            if (pic_rate_present_flag) {
                hevc_u_v(ctx->bs_ptr, 2, "constant_pic_rate_idc");  /* protocal required 2 bits */
                hevc_u_v(ctx->bs_ptr, 2, "avg_pic_rate");  /* protocal required 2 bits */
            }
        }
    }
}

STATIC td_void hevc_parse_tile_flag(const hevc_ctx *ctx, const hevc_vps_extension_set *vps_ext)
{
    td_u32 i, j, k;
    td_u8  tiles_in_use_flag[HEVC_MAX_NUM_LAYERS] = {0};

    for (i = 0; i <= vps_ext->max_layers_minus1; i++) {
        tiles_in_use_flag[i] = hevc_u_v(ctx->bs_ptr, 1, "tiles_in_use_flag");
        if (tiles_in_use_flag[i]) {
            hevc_u_v(ctx->bs_ptr, 1, "loop_filter_not_across_tiles_flag");
        }
    }
    for (i = 1; i <= vps_ext->max_layers_minus1; i++) {
        for (j = 0; j < vps_ext->num_direct_ref_layers[vps_ext->layer_id_in_nuh[i]]; j++) {
            k = vps_ext->layer_idx_in_vps[vps_ext->ref_layer_id[vps_ext->layer_id_in_nuh[i]][j]];
            if (tiles_in_use_flag[i] && tiles_in_use_flag[k]) {
                hevc_u_v(ctx->bs_ptr, 1, "tile_boundaries_aligned_flag");
            }
        }
    }
}

STATIC td_void hevc_parse_wpp_flag(const hevc_ctx *ctx, const hevc_vps_extension_set *vps_ext)
{
    td_u32 i;
    td_u32 wpp_not_in_use_flag;

    wpp_not_in_use_flag = hevc_u_v(ctx->bs_ptr, 1, "wpp_not_in_use_flag");
    if (!wpp_not_in_use_flag) {
        for (i = 0; i <= vps_ext->max_layers_minus1; i++) {
            hevc_u_v(ctx->bs_ptr, 1, "wpp_in_use_flag");
        }
    }

    hevc_u_v(ctx->bs_ptr, 1, "single_layer_for_non_irap_flag");
    hevc_u_v(ctx->bs_ptr, 1, "higher_layer_irap_skip_flag");
}

STATIC td_void hevc_parse_ctu_offset_puls1(const hevc_ctx *ctx, td_u32 ctu_based_offset_enabled_flag)
{
    if (ctu_based_offset_enabled_flag) {
        hevc_ue_v(ctx->bs_ptr, "min_horizontal_ctu_offset_plus1");
    }
}

STATIC td_void hevc_parse_ctu_base_offset(hevc_ctx *ctx, const hevc_video_param_set *vps,
    const hevc_vps_extension_set *vps_ext)
{
    td_u32 i, j;
    td_u32 min_spatial_segment_offset_plus1 = 0;
    td_u32 ctu_based_offset_enabled_flag;

    for (i = 1; i <= vps_ext->max_layers_minus1; i++) {
        for (j = 0; j < vps_ext->num_direct_ref_layers[vps_ext->layer_id_in_nuh[i]]; j++) {
            if (vps->vps_base_layer_internal_flag || vps_ext->ref_layer_id[vps_ext->layer_id_in_nuh[i]][j] > 0) {
                min_spatial_segment_offset_plus1 = hevc_ue_v(ctx->bs_ptr, "min_spatial_segment_offset_plus1");
            }

            if (min_spatial_segment_offset_plus1 > 0) {
                ctu_based_offset_enabled_flag = hevc_u_v(ctx->bs_ptr, 1, "ctu_based_offset_enabled_flag");
                hevc_parse_ctu_offset_puls1(ctx, ctu_based_offset_enabled_flag);
            }
        }
    }
}

STATIC td_void hevc_parse_base_layer_compatibility_flag(hevc_ctx *ctx, const hevc_vps_extension_set *vps_ext)
{
    td_u32 i;

    for (i = 1; i <= vps_ext->max_layers_minus1; i++) {
        if (vps_ext->num_direct_ref_layers[vps_ext->layer_id_in_nuh[i]] == 0) {
            hevc_u_v(ctx->bs_ptr, 1, "base_layer_parameter_set_compatibility_flag");
        }
    }
}

STATIC td_s32 hevc_parse_vui(hevc_ctx *ctx, const hevc_video_param_set *vps, const hevc_vps_vui_set *vps_vui)
{
    td_u32 i;
    td_u32 cross_layer_pic_type_aligned_flag;
    td_u32 cross_layer_irap_aligned_flag = 0;
    td_u32 video_signal_info_idx_present_flag;
    td_u32 tiles_not_in_use_flag;
    td_u32 ilp_restricted_ref_layers_flag;
    td_u32 vps_vui_bsp_hrd_present_flag;
    td_u32 vps_num_video_signal_info_minus1;
    const hevc_vps_extension_set *vps_ext = &(vps->vps_extension);

    cross_layer_pic_type_aligned_flag = hevc_u_v(ctx->bs_ptr, 1, "cross_layer_pic_type_aligned_flag");
    if (!cross_layer_pic_type_aligned_flag) {
        cross_layer_irap_aligned_flag = hevc_u_v(ctx->bs_ptr, 1, "cross_layer_irap_aligned_flag");
    }

    if (cross_layer_irap_aligned_flag) {
        hevc_u_v(ctx->bs_ptr, 1, "all_layers_idr_aligned_flag");
    }

    hevc_parse_rate(ctx, vps);

    video_signal_info_idx_present_flag = hevc_u_v(ctx->bs_ptr, 1, "video_signal_info_idx_present_flag");
    if (video_signal_info_idx_present_flag) {
        vps_num_video_signal_info_minus1 = hevc_u_v(ctx->bs_ptr, 0x4, "vps_num_video_signal_info_minus1");
    } else {
        vps_num_video_signal_info_minus1 = vps_ext->max_layers_minus1;
    }

    for (i = 0; i <= vps_num_video_signal_info_minus1; i++) {
        hevc_parse_video_signal_info(ctx);
    }

    if (video_signal_info_idx_present_flag && (vps_num_video_signal_info_minus1 > 0)) {
        for (i = vps->vps_base_layer_internal_flag ? 0 : 1; i <= vps_ext->max_layers_minus1; i++) {
            hevc_u_v(ctx->bs_ptr, 4, "vps_video_signal_info_idx");  /* protocal required 4 bits */
        }
    }

    tiles_not_in_use_flag = hevc_u_v(ctx->bs_ptr, 1, "tiles_not_in_use_flag");
    if (!tiles_not_in_use_flag) {
        hevc_parse_tile_flag(ctx, vps_ext);
    }

    hevc_parse_wpp_flag(ctx, vps_ext);

    ilp_restricted_ref_layers_flag = hevc_u_v(ctx->bs_ptr, 1, "ilp_restricted_ref_layers_flag");
    if (ilp_restricted_ref_layers_flag) {
        hevc_parse_ctu_base_offset(ctx, vps, vps_ext);
    }

    vps_vui_bsp_hrd_present_flag = hevc_u_v(ctx->bs_ptr, 1, "vps_vui_bsp_hrd_present_flag");
    if (vps_vui_bsp_hrd_present_flag) {
        hevc_parse_bsp_hrd_param(ctx, vps, &vps_vui->vps_vui_bsp_hrdpara);
    }

    hevc_parse_base_layer_compatibility_flag(ctx, vps_ext);

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_derive_shvc_type(hevc_ctx *ctx, td_u32 first_layer, td_u32 layer_num)
{
    td_u32 m, n;
    const hevc_vps_extension_set *vps_ext = TD_NULL;

    vfmw_alert(ctx->curr_vps != TD_NULL);
    vps_ext = &ctx->curr_vps->vps_extension;

    m = first_layer;
    for (n = m + 1; n < layer_num; n++) {
        if ((vps_ext->rep_format[m].pic_width_vps_in_luma_samples ==
            vps_ext->rep_format[n].pic_width_vps_in_luma_samples) &&
            (vps_ext->rep_format[m].pic_height_vps_in_luma_samples ==
            vps_ext->rep_format[n].pic_height_vps_in_luma_samples)) {
            continue;
        }

        if (ctx->is_shvc_type == 1) {
            continue;
        }

        ctx->is_shvc_type = 1;
    }

    return;
}

STATIC td_void hevc_check_layer_info(hevc_ctx *ctx)
{
    td_s32 m;
    hevc_vps_extension_set *vps_ext = TD_NULL;

    vfmw_alert(ctx->curr_vps != TD_NULL);
    vps_ext = &ctx->curr_vps->vps_extension;

    if (vps_ext->vps_num_rep_formats_minus1 <= 0 && vps_ext->rep_format_equal_flag != 1) {
        return;
    }

    ctx->layer_num = vfmw_min(vps_ext->vps_num_rep_formats_minus1 + 1, HEVC_MAX_NUM_LAYERS);

    if (vps_ext->rep_format_equal_flag == 1) {
        ctx->layer_num = HEVC_MAX_NUM_LAYERS;
    }

    for (m = 0; (td_u32)m < ctx->layer_num; m++) {
        hevc_derive_shvc_type(ctx, m, ctx->layer_num);
        if (ctx->layer_width[m]  != vps_ext->rep_format[m].pic_width_vps_in_luma_samples ||
            ctx->layer_height[m] != vps_ext->rep_format[m].pic_height_vps_in_luma_samples) {
            ctx->layer_width[m] = vps_ext->rep_format[m].pic_width_vps_in_luma_samples;
            ctx->layer_height[m] = vps_ext->rep_format[m].pic_height_vps_in_luma_samples;
            ctx->is_mem_arrange_done = 0;
        }
        ctx->max_width = vfmw_max(ctx->layer_width[0], ctx->layer_width[1]);
        ctx->max_height = vfmw_max(ctx->layer_height[0], ctx->layer_height[1]);
    }

    return;
}

STATIC td_void hevc_derive_layer_list(const hevc_ctx *ctx, hevc_video_param_set *vps)
{
    td_s32 i, j;
    td_u32 num;
    td_u8 layer_id_included_flag;
    vfmw_check_sec_func(memset_s(vps->layer_set_layer_id_list, sizeof(vps->layer_set_layer_id_list),
        0, sizeof(vps->layer_set_layer_id_list)));
    vfmw_check_sec_func(memset_s(vps->num_layers_in_id_list, sizeof(vps->num_layers_in_id_list),
        0, sizeof(vps->num_layers_in_id_list)));

    for (i = 1; i <= vps->vps_num_layer_sets_minus1; i++) {
        num = 0;
        for (j = 0; j <= vps->vps_max_layer_id; j++) {
            layer_id_included_flag = (td_u8)hevc_u_v(ctx->bs_ptr, 1, "layer_id_included_flag");
            if (layer_id_included_flag && num < HEVC_MAX_NUM_LAYERS) {
                vps->layer_set_layer_id_list[i][num++] = (td_u32)j;
            }
        }
        vps->num_layers_in_id_list[i] = num;
    }

    return;
}

STATIC td_void hevc_parse_vps_direct_dependency(hevc_ctx *ctx, const hevc_video_param_set *vps,
    const hevc_vps_extension_set *vps_ext, td_s32 direct_dep_type_len_minus2)
{
    td_s32 i, j;

    for (i = vps->vps_base_layer_internal_flag ?
        1 : 2; (i <= vps_ext->max_layers_minus1) && (i < HEVC_MAX_NUM_LAYERS); i++) { /* 2 :protocal required */
        for (j = vps->vps_base_layer_internal_flag ? 0 : 1; j < i; j++) {
            if (vps_ext->direct_dependency_flag[i][j]) {
               hevc_u_v(ctx->bs_ptr, direct_dep_type_len_minus2 + 2, "direct_dependency_type"); /* 2 : bitsnum */
            }
        }
    }
}

STATIC td_s32 hevc_derive_vps_ext_max_layer_minus1(const hevc_video_param_set *vps, hevc_vps_extension_set *vps_ext)
{
    vps_ext->max_layers_minus1 = vps->vps_max_layers_minus1;
    if (vps_ext->max_layers_minus1 < 0 || vps_ext->max_layers_minus1 > HEVC_MAX_NUM_LAYERS - 1) {
        dprint(PRN_ERROR, "%s: vps_max_layers_minus1 %d out of range[0,%d].\n", __func__,
            vps_ext->max_layers_minus1, HEVC_MAX_NUM_LAYERS - 1);
        vps_ext->max_layers_minus1 = 0;
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_init_dim_bit_offset(td_s32 dim_bit_offset[HEVC_MAX_NUM_SCALABILITY_TYPES], td_u32 size1,
    const td_s32 dimension_id_len_minus1[HEVC_MAX_NUM_SCALABILITY_TYPES], td_u32 size2, td_u8 splitting_flag,
    td_s32 scalability_types_num)
{
    td_s32 i, j;

    for (i = 0; i <= scalability_types_num; i++) {
        if (splitting_flag && (i == scalability_types_num)) {
            dim_bit_offset[i] = 6; /* 6 :protocal required */
        } else {
            for (j = 0; j <= i - 1; j++) {
                dim_bit_offset[i] += (dimension_id_len_minus1[j] + 1);
            }
        }
    }
}

STATIC td_void hevc_parse_dimension_id(const hevc_ctx *ctx, hevc_vps_extension_set *vps_ext, td_u8 splitting_flag,
    td_s32 scalability_types_num, td_s32 dimension_id[HEVC_MAX_NUM_LAYERS][HEVC_MAX_NUM_SCALABILITY_TYPES])
{
    td_u8 vps_nuh_layer_id_present_flag;
    td_s32 i, j;
    td_s32 dim_bit_offset[HEVC_MAX_NUM_SCALABILITY_TYPES] = {0};
    td_s32 dimension_id_len_minus1[HEVC_MAX_NUM_SCALABILITY_TYPES] = {0};

    for (i = 0; i < scalability_types_num - (splitting_flag ? 1 : 0); i++) {
        dimension_id_len_minus1[i] =
            (td_s32)(hevc_u_v(ctx->bs_ptr, 3, "dimension_id_len_minus1[]") + 1); /* 3 : bitsnum */
    }

    hevc_init_dim_bit_offset(dim_bit_offset, HEVC_MAX_NUM_SCALABILITY_TYPES, dimension_id_len_minus1,
        HEVC_MAX_NUM_SCALABILITY_TYPES, splitting_flag, scalability_types_num);

    if (splitting_flag) {
        dimension_id_len_minus1[scalability_types_num - 1] =
            5 - dim_bit_offset[scalability_types_num - 1]; /* 5 :protocal required */
    }

    vps_nuh_layer_id_present_flag = hevc_u_v(ctx->bs_ptr, 1, "vps_nuh_layer_id_present_flag");
    for (i = 1; i <= vps_ext->max_layers_minus1; i++) {
        if (vps_nuh_layer_id_present_flag) {
            /* protocal required 6 bits */
            vps_ext->layer_id_in_nuh[i] = (td_s32)hevc_u_v(ctx->bs_ptr, 6, "layer_id_in_nuh");
            if (vps_ext->layer_id_in_nuh[i] >= HEVC_MAX_NUM_LAYERS) {
                dprint(PRN_ERROR, "%d layer_id_in_nuh[%d](%d) is not supported.\n", __LINE__, i,
                    ctx->curr_nal->nuh_temporal_id);
                vps_ext->layer_id_in_nuh[i] = 0;
            }
        } else {
           vps_ext->layer_id_in_nuh[i] = i;
        }

        vps_ext->layer_idx_in_vps[vps_ext->layer_id_in_nuh[i]] = i;
        if (!splitting_flag) {
            for (j = 0; j < scalability_types_num; j++) {
                dimension_id[i][j] = (td_s32)hevc_u_v(ctx->bs_ptr, dimension_id_len_minus1[j], "dimension_id");
            }
        } else {
            for (j = 0; j < scalability_types_num; j++) {
                dimension_id[i][j] = (td_s32)(((td_u32)vps_ext->layer_id_in_nuh[i] &
                    (td_u32)((1 << (td_u32)dim_bit_offset[j + 1]) - 1)) >> (td_u32)dim_bit_offset[j]);
            }
        }
    }
}

STATIC td_void hevc_parse_scalability_view_order(const hevc_ctx *ctx, hevc_vps_extension_set *vps_ext)
{
    td_u8 new_view_flag;
    td_u8 splitting_flag;
    td_s32 num_of_views = 1;
    td_s32 i, j, k, idx;
    td_s32 scalability_types_num = 0;
    td_u32 view_id_len;
    td_s32 view_order_idx[HEVC_MAX_NUM_LAYERS] = {0};
    td_s32 scalability_id[HEVC_MAX_NUM_SCALABILITY_TYPES] = {0};
    td_s32 dimension_id[HEVC_MAX_NUM_LAYERS][HEVC_MAX_NUM_SCALABILITY_TYPES] = {{0, 0}};
    td_s32 scalability_mask_flag[HEVC_MAX_NUM_SCALABILITY_TYPES] = {0};

    splitting_flag = (td_u8)hevc_u_v(ctx->bs_ptr, 1, "splitting_flag");
    for (i = 0; i < HEVC_MAX_NUM_SCALABILITY_TYPES; i++) {
        scalability_mask_flag[i] = (td_s32)hevc_u_v(ctx->bs_ptr, 1, "scalability_mask_flag");
        scalability_types_num += (scalability_mask_flag[i] ? 1 : 0);
    }

    hevc_parse_dimension_id(ctx, vps_ext, splitting_flag, scalability_types_num, dimension_id);

    view_id_len = hevc_u_v(ctx->bs_ptr, 4, "view_id_len");  /* protocal required 4 bits */

    for (i = 0; i <= vps_ext->max_layers_minus1; i++) {
        idx = vps_ext->layer_id_in_nuh[i];
        view_order_idx[idx] = scalability_mask_flag[0];
        for (k = 0, j = 0; k < 16; k++) { /* 16 : scalability_mask_flag num */
            if (scalability_mask_flag[k]) {
                scalability_id[k] = dimension_id[i][j++];
            }
        }
        view_order_idx[idx] = scalability_id[1];

        if (i <= 0) {
            continue;
        }

        new_view_flag = 1;
        for (j = 0; j < i; j++) {
            if (view_order_idx[idx] == view_order_idx[vps_ext->layer_id_in_nuh[j]]) {
                new_view_flag = 0;
            }
        }
        num_of_views += (new_view_flag ? 1 : 0);
    }

    if (view_id_len > 0) {
        for (i = 0; i < num_of_views; i++) {
            vps_ext->view_id_val[i] = hevc_u_v(ctx->bs_ptr, (td_s32)view_id_len, "view_id_val");
        }
    }
}

STATIC td_void hevc_parse_vps_num_direct_ref_layers(hevc_ctx *ctx, hevc_vps_extension_set *vps_ext)
{
    td_s32 i, j, k;
    td_s32 idx;

    for (i = 1; i <= vps_ext->max_layers_minus1; i++) {
        k = vps_ext->layer_id_in_nuh[i];
        idx = 0;
        for (j = 0; j < i; j++) {
            vps_ext->direct_dependency_flag[i][j] = hevc_u_v(ctx->bs_ptr, 1, "direct_dependency_flag[i][j]");
            if (vps_ext->direct_dependency_flag[i][j]) {
                vps_ext->ref_layer_id[k][idx++] = vps_ext->layer_id_in_nuh[j];
            }
        }
        vps_ext->num_direct_ref_layers[k] = idx;
    }
}

STATIC td_s32 hevc_parse_vps_num_independent_layers(hevc_ctx *ctx, td_s32 layer_idx,
    const td_s32 *num_layers_in_tree_partion, td_u32 *num_add_layer_sets)
{
    td_s32 i, j;
    td_s32 num_independent_layers;

    num_independent_layers = layer_idx;
    if (num_independent_layers > 1) {
        *num_add_layer_sets = hevc_ue_v(ctx->bs_ptr, "num_add_layer_sets");
        if (*num_add_layer_sets >= HEVC_MAX_VPS_OP_SETS_PLUS1) {
            dprint(PRN_ERROR, "%s %d: num_add_layer_sets %d >= HEVC_MAX_VPS_OP_SETS_PLUS1 %d\n", __func__, __LINE__,
                   *num_add_layer_sets, HEVC_MAX_VPS_OP_SETS_PLUS1);
            return HEVC_DEC_ERR;
        }
    } else {
        *num_add_layer_sets = 0;
    }

    for (i = 0; i < *num_add_layer_sets; i++) {
        for (j = 1; j < num_independent_layers; j++) {
            hevc_u_v(ctx->bs_ptr, hevc_ceil_log2(num_layers_in_tree_partion[j] + 1), "highest_layer_idx_plus1");
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_parse_vps_num_layers(hevc_ctx *ctx, const hevc_video_param_set *vps, hevc_vps_extension_set *vps_ext)
{
    td_s32 i, j, k, cur_layer_id;
    td_s32 idx;
    td_u32 num_add_layer_sets = 0;
    td_s32 recursive_ref_layer_flag[HEVC_MAX_NUM_LAYERS][HEVC_MAX_NUM_LAYERS] = {{0, 0}};
    td_s32 num_predicted_layers[HEVC_MAX_NUM_LAYERS] = {0};
    td_s32 num_layers_in_tree_partion[HEVC_MAX_NUM_LAYERS] = {0};

    vfmw_check_sec_func(memset_s(recursive_ref_layer_flag, sizeof(recursive_ref_layer_flag),
        0, sizeof(recursive_ref_layer_flag)));

    k = 0;
    for (i = 0; i <= vps_ext->max_layers_minus1; i++) {
        cur_layer_id = vps_ext->layer_id_in_nuh[i];
        vfmw_check_break(cur_layer_id >= 0 && cur_layer_id < HEVC_MAX_NUM_LAYERS);

        for (j = 0; j < vps_ext->num_direct_ref_layers[cur_layer_id]; j++) {
            idx = vps_ext->ref_layer_id[cur_layer_id][j];
            recursive_ref_layer_flag[cur_layer_id][idx] = 1;
            for (k = 0; k < HEVC_MAX_NUM_LAYERS; k++) {
                recursive_ref_layer_flag[cur_layer_id][k] =
                    (recursive_ref_layer_flag[cur_layer_id][k] || recursive_ref_layer_flag[idx][k]);
            }
        }
    }

    for (i = 0; i < vps_ext->max_layers_minus1; i++) {
        cur_layer_id = vps_ext->layer_id_in_nuh[i];
        vfmw_check_break(cur_layer_id >= 0 && cur_layer_id < HEVC_MAX_NUM_LAYERS);

        num_predicted_layers[cur_layer_id] = k;
    }

    for (i = 0, k = 0; i <= vps_ext->max_layers_minus1; i++) {
        cur_layer_id = vps_ext->layer_id_in_nuh[i];
        vfmw_check_break(cur_layer_id >= 0 && cur_layer_id < HEVC_MAX_NUM_LAYERS);

        if (vps_ext->num_direct_ref_layers[cur_layer_id] == 0) {
            num_layers_in_tree_partion[k] = 1;
            for (j = 0; j < num_predicted_layers[cur_layer_id]; j++) {
                (num_layers_in_tree_partion[k])++;
            }
            k++;
        }
    }

    if (hevc_parse_vps_num_independent_layers(ctx, k, num_layers_in_tree_partion, &num_add_layer_sets) !=
        HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

    vps_ext->num_layer_sets = vps->vps_num_layer_sets_minus1 + (td_s32)num_add_layer_sets + 1;

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_parse_vps_max_sub_layer_params(hevc_ctx *ctx, const hevc_video_param_set *vps,
    hevc_vps_extension_set *vps_ext)
{
    td_u32 i, j, k, m;
    td_u32 vps_sub_layers_max_minus1_present_flag;

    vps_sub_layers_max_minus1_present_flag = hevc_u_v(ctx->bs_ptr, 1, "vps_sub_layers_max_minus1_present_flag");
    if (vps_sub_layers_max_minus1_present_flag) {
        for (i = 0; i <= (td_u32)vps_ext->max_layers_minus1; i++) {
            vps_ext->sub_layers_vps_max_minus1[i] =
                (td_s32)hevc_u_v(ctx->bs_ptr, 3, "sub_layers_vps_max_minus1"); /* protocal required 3 bits */
        }
    } else {
        for (i = 0; i <= (td_u32)vps_ext->max_layers_minus1; i++) {
            vps_ext->sub_layers_vps_max_minus1[i] = vps->vps_max_sub_layers - 1;
        }
    }
    for (i = 0; i <= vps->vps_num_layer_sets_minus1; i++) {
        k = 0;
        for (j = 0; j < vps->num_layers_in_id_list[i]; j++) {
            m = vps->layer_set_layer_id_list[i][j];
            k = (k > (td_u32)vps_ext->sub_layers_vps_max_minus1[vps_ext->layer_idx_in_vps[m]]) ?
                k : (td_u32)vps_ext->sub_layers_vps_max_minus1[vps_ext->layer_idx_in_vps[m]];
        }
        vps_ext->max_sub_layers_in_layer_set_minus1[i] = (td_s32)k;
    }
}

STATIC td_void heve_parse_vps_max_tid_il_ref_pic(hevc_ctx *ctx, hevc_vps_extension_set *vps_ext)
{
    td_u32 i, j;
    td_u8 max_tid_ref_present_flag;

    max_tid_ref_present_flag = hevc_u_v(ctx->bs_ptr, 1, "max_tid_ref_present_flag");
    if (max_tid_ref_present_flag) {
        for (i = 0; i < (td_u32)vps_ext->max_layers_minus1; i++) {
            for (j = i + 1; j <= (td_u32)vps_ext->max_layers_minus1; j++) {
                vps_ext->max_tid_il_ref_pics_plus1[i][j] = (td_s32)((vps_ext->direct_dependency_flag[j][i]) ?
                    hevc_u_v(ctx->bs_ptr, 3, "max_tid_il_ref_pics_plus1") : 7); /* 3 bits, 7 protocal required */
            }
        }
    } else {
        for (i = 0; i < (td_u32)vps_ext->max_layers_minus1; i++) {
            for (j = i + 1; j <= (td_u32)vps_ext->max_layers_minus1; j++) {
                vps_ext->max_tid_il_ref_pics_plus1[i][j] = 7; /* 7 :protocal required */
            }
        }
    }
}

STATIC td_s32 hevc_parse_vps_ext_num_ptl(hevc_ctx *ctx, const hevc_video_param_set *vps,
    hevc_vps_extension_set *vps_ext, td_u32 *vps_num_profile_tier_level_minus1,
    hevc_profile_tier_level *profile_tier_level)
{
    td_u8 vps_profile_present_flag;
    td_s32 i;
    td_u32 offsetval;

    vps_ext->all_ref_layers_active_flag = hevc_u_v(ctx->bs_ptr, 1, "all_ref_layers_active_flag");
    *vps_num_profile_tier_level_minus1 = hevc_ue_v(ctx->bs_ptr, "vps_num_profile_tier_level_minus1");
    if (*vps_num_profile_tier_level_minus1 > 63) { /* 63: max range */
        dprint(PRN_ERROR, "vps_non_vui_extension_length %d is out of range!\n", *vps_num_profile_tier_level_minus1);
        return HEVC_DEC_ERR;
    }
    offsetval = (vps_ext->max_layers_minus1 > 0 &&
        vps->vps_base_layer_internal_flag) ? 2 : 1; /* 2 :protocal required */

    for (i = vps->vps_base_layer_internal_flag ?
        2 : 1; i <= *vps_num_profile_tier_level_minus1; i++) { /* 2 :protocal required */
        vps_profile_present_flag = hevc_u_v(ctx->bs_ptr, 1, "vps_profile_present_flag"); /* 2 :protocal required */
        hevc_dec_ptl(ctx, profile_tier_level, vps_profile_present_flag, vps->vps_max_sub_layers - 1);
        offsetval++;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_parse_vps_num_output_layer_sets(const hevc_ctx *ctx, hevc_vps_extension_set *vps_ext,
    td_s32 num_add_olss)
{
    vps_ext->num_output_layer_sets = num_add_olss + vps_ext->num_layer_sets;
    if (vps_ext->num_output_layer_sets < 0 || vps_ext->num_output_layer_sets > HEVC_MAX_VPS_OUTPUTLAYER_SETS) {
        dprint(PRN_ERROR,
            "%s %d: num_output_layer_sets %d >= HEVC_MAX_VPS_OP_SETS_PLUS1 %d, num_add_olss %d, num_layer_sets %d\n",
            __func__, __LINE__,
            vps_ext->num_output_layer_sets, HEVC_MAX_VPS_OUTPUTLAYER_SETS, num_add_olss, vps_ext->num_layer_sets);
        vps_ext->num_output_layer_sets = 0;
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_get_vps_output_layer_flag(const hevc_video_param_set *vps, hevc_vps_extension_set *vps_ext,
    td_u32 default_output_layer_idc, td_s32 output_layer_sets, td_s32 idx)
{
    td_s32 j;

    if (output_layer_sets >= HEVC_MAX_VPS_OUTPUTLAYER_SETS || output_layer_sets < 0) {
        dprint(PRN_ERROR, "line %d func %s output_layer_sets = %d overrun error\n", __LINE__, __func__,
            output_layer_sets);
        return;
    }

    for (j = 0; j < vps->num_layers_in_id_list[idx]; j++) {
        switch (default_output_layer_idc) {
            case 0: /* 0 output_layer_idc type */
                vps_ext->output_layer_flag[output_layer_sets][j] = 1;
                break;
            case 1: /* 1 output_layer_idc type */
                vps_ext->output_layer_flag[output_layer_sets][j] = (j == (vps->num_layers_in_id_list[idx] - 1)) ? 1 : 0;
                break;
            default:
                break;
        }
    }
}

STATIC td_void hevc_parse_set_idx_ols(const hevc_ctx *ctx, hevc_vps_extension_set *vps_ext, td_s32 idx)
{
    if (vps_ext->num_layer_sets > 2 && idx >= vps_ext->num_layer_sets) { /* 2 :vfmw_max layer set */
        vps_ext->layer_set_idx_for_ols_minus1[idx] = (td_s32)hevc_u_v(ctx->bs_ptr,
            hevc_ceil_log2(vps_ext->num_layer_sets - 1), "layer_set_idx_for_ols_minus1[i]");
    } else {
        vps_ext->layer_set_idx_for_ols_minus1[idx] = 0;
    }
}

STATIC td_s32 hevc_parse_vps_num_layer_info(hevc_ctx *ctx, const hevc_video_param_set *vps,
    hevc_vps_extension_set *vps_ext, td_u32 default_output_layer_idc, td_u32 vps_num_profile_tier_level_minus1)
{
    td_s32 i, j;
    td_s32 idx;
    td_s32 target_output_layer_id_list;
    td_s32 num_layers_in_opt_layer_set;

    target_output_layer_id_list = 0;
    vps_ext->layer_set_idx_for_ols_minus1[0] = -1;
    for (i = 1; i < vps_ext->num_output_layer_sets; i++) {
        hevc_parse_set_idx_ols(ctx, vps_ext, i);

        idx = (i < vps_ext->num_layer_sets) ? i : (vps_ext->layer_set_idx_for_ols_minus1[i] + 1);
        if (idx < 0 || idx >= HEVC_MAX_VPS_OP_SETS_PLUS1) {
            dprint(PRN_ERROR, "line %d func %s idx = %d overrun error\n", __LINE__, __func__, idx);
            return HEVC_DEC_ERR;
        }

        if ((i > vps->vps_num_layer_sets_minus1) || (default_output_layer_idc == 2)) { /* 2 :protocal required */
            for (j = 0; j < vps->num_layers_in_id_list[idx]; j++) {
                vps_ext->output_layer_flag[i][j] = hevc_u_v(ctx->bs_ptr, 1, "output_layer_flag");
            }
        } else {
            hevc_get_vps_output_layer_flag(vps, vps_ext, default_output_layer_idc, i, idx);
        }

        hevc_derive_necessary_layer_flags(ctx, vps, vps_ext, i);
        hevc_derive_target_layer_id_list(ctx, vps, vps_ext, i);
        num_layers_in_opt_layer_set = 0;
        for (j = 0; j < vps->num_layers_in_id_list[idx]; j++) {
            if (!vps_ext->output_layer_flag[i][j]) {
                continue;
            }

            target_output_layer_id_list = vps->layer_set_layer_id_list[idx][j];
            num_layers_in_opt_layer_set++;
        }

        for (j = 0; j < vps->num_layers_in_id_list[idx]; j++) {
            if (vps_ext->necessary_layer_flag[i][j] && vps->vps_num_layer_sets_minus1 > 0) {
                hevc_u_v(ctx->bs_ptr, (td_s32)hevc_ceil_log2(vps_num_profile_tier_level_minus1 + 1),
                    "profile_level_tier_idx");
            }
        }

        if (target_output_layer_id_list >= HEVC_MAX_NUM_LAYERS) {
            dprint(PRN_ERROR, "line %d func %s target_output_layer_id_list = %d overrun error\n", __LINE__, __func__,
                target_output_layer_id_list);
            return HEVC_DEC_ERR;
        }

        if ((num_layers_in_opt_layer_set == 1) && (vps_ext->num_direct_ref_layers[target_output_layer_id_list] > 0)) {
            hevc_u_v(ctx->bs_ptr, 1, "alt_output_layer_flag");
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_parse_vps_rep_format_idx(hevc_ctx *ctx, hevc_video_param_set *vps, hevc_vps_extension_set *vps_ext)
{
    td_s32 i;
    td_u8 rep_format_idx_present_flag = 0;

    if (vps_ext->vps_num_rep_formats_minus1 > 0) {
        rep_format_idx_present_flag = hevc_u_v(ctx->bs_ptr, 1, "rep_format_idx_present_flag");
    }

    for (i = 0; i < (td_s32)vps->vps_base_layer_internal_flag; i++) {
        vps_ext->vps_rep_format_idx[i] = 0;
    }

    if (rep_format_idx_present_flag) {
        for (i = vps->vps_base_layer_internal_flag ? 1 : 0; i <= vps_ext->max_layers_minus1; i++) {
            vps_ext->vps_rep_format_idx[i] = hevc_u_v(ctx->bs_ptr,
                hevc_ceil_log2(vps_ext->vps_num_rep_formats_minus1 + 1), "vps_rep_format_idx");
        }
    } else {
        for (i = vps->vps_base_layer_internal_flag ? 1 : 0; i <= vps_ext->max_layers_minus1; i++) {
            vps_ext->vps_rep_format_idx[i] =
                ((td_u32)i < vps_ext->vps_num_rep_formats_minus1) ? (td_u32)i : vps_ext->vps_num_rep_formats_minus1;
        }
    }
}

STATIC td_s32 hevc_derive_vps_rep_format(hevc_ctx *ctx, hevc_video_param_set *vps, hevc_vps_extension_set *vps_ext)
{
    td_s32 i;
    hevc_rep_format_set *rep_format = NULL;
    hevc_rep_format_set *pre_rep_format = NULL;
    hevc_rep_format_set tmp_rep_format;

    vps_ext->vps_num_rep_formats_minus1 = hevc_ue_v(ctx->bs_ptr, "vps_num_rep_formats_minus1");
    if (vps_ext->vps_num_rep_formats_minus1 > HEVC_VPS_MAX_MAX_REP_FORMAT_MINUS_ONE - 1) {
        dprint(PRN_ERROR, "vps_num_rep_formats_minus1 %d > 255.\n", vps_ext->vps_num_rep_formats_minus1);
        return HEVC_DEC_ERR;
    }

    for (i = 0; (td_u32)i <= vps_ext->vps_num_rep_formats_minus1; i++) {
        if (i < HEVC_MAX_NUM_LAYERS)  {
            pre_rep_format = (i == 0) ? NULL : &(vps->vps_extension.rep_format[i - 1]);
            rep_format = &(vps->vps_extension.rep_format[i]);
        } else {
            pre_rep_format = &(vps->vps_extension.rep_format[0]);
            rep_format = &tmp_rep_format;
        }

        hevc_parse_rep_format(ctx, pre_rep_format, rep_format);
        if (vps_ext->vps_num_rep_formats_minus1 == 0) {
            vfmw_check_sec_func(memcpy_s(&(vps->vps_extension.rep_format[1]), sizeof(hevc_rep_format_set),
                &(vps->vps_extension.rep_format[0]), sizeof(vps->vps_extension.rep_format[0])));
            vps_ext->rep_format_equal_flag = 1;
        }
    }

    hevc_parse_vps_rep_format_idx(ctx, vps, vps_ext);

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_parse_poc_lsb_not_present_flag(hevc_ctx *ctx, hevc_vps_extension_set *vps_ext)
{
    td_s32 i;

    for (i = 1; i <= vps_ext->max_layers_minus1; i++) {
        if (vps_ext->num_direct_ref_layers[vps_ext->layer_id_in_nuh[i]] == 0) {
           vps_ext->poc_lsb_not_present_flag[i] = hevc_u_v(ctx->bs_ptr, 1, "poc_lsb_not_present_flag");
        }
    }
}

STATIC td_void hevc_parse_direct_dependency_all_layers_type(hevc_ctx *ctx, const hevc_video_param_set *vps,
    const hevc_vps_extension_set *vps_ext)
{
    td_u8 direct_dependency_all_layers_flag;
    td_s32 direct_dep_type_len_minus2;

    direct_dep_type_len_minus2 = (td_s32)hevc_ue_v(ctx->bs_ptr, "direct_dep_type_len_minus2");
    direct_dependency_all_layers_flag = hevc_u_v(ctx->bs_ptr, 1, "direct_dependency_all_layers_flag");
    if (direct_dependency_all_layers_flag) {
        hevc_u_v(ctx->bs_ptr, direct_dep_type_len_minus2 + 2, "direct_dependency_all_layers_type"); /* 2 required */
    } else {
        hevc_parse_vps_direct_dependency(ctx, vps, vps_ext, direct_dep_type_len_minus2);
    }
}

STATIC td_void hevc_parse_vps_ext_vui_info(hevc_ctx *ctx, const hevc_video_param_set *vps,
    const hevc_vps_extension_set *vps_ext)
{
    td_u32 i;
    td_u32 vps_non_vui_extension_length;
    td_u32 vps_vui_present_flag;

    vps_non_vui_extension_length = hevc_ue_v(ctx->bs_ptr, "vps_non_vui_extension_length");
    if (vps_non_vui_extension_length > 4096) {  /* 4096 max length */
        dprint(PRN_ERROR, "vps_non_vui_extension_length %d is out of range!\n", vps_non_vui_extension_length);
        return;
    }

    for (i = 1; i <= vps_non_vui_extension_length; i++) {
        hevc_u_v(ctx->bs_ptr, 8, "vps_non_vui_extension_data_byte"); /* 8 protocal required */
    }

    vps_vui_present_flag = hevc_u_v(ctx->bs_ptr, 1, "vps_vui_present_flag");
    if (vps_vui_present_flag) {
        bs_skip_to_next_byte(ctx->bs_ptr);
        hevc_parse_vui(ctx, vps, &vps_ext->vui);
    }

    return;
}

STATIC td_s32 hevc_parse_vps_extension(hevc_ctx *ctx, hevc_video_param_set *vps)
{
    td_s32 num_add_olss;
    td_u32 default_output_layer_idc;
    td_u32 vps_num_profile_tier_level_minus1 = 0;
    hevc_profile_tier_level *profile_tier_level = &vps->profile_tier_level;
    hevc_vps_extension_set *vps_ext = &vps->vps_extension;

    vfmw_check_sec_func(memset_s(vps_ext, sizeof(vps->vps_extension), 0, sizeof(hevc_vps_extension_set)));

    if (hevc_derive_vps_ext_max_layer_minus1(vps, vps_ext) != HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

    if (vps_ext->max_layers_minus1 > 0 && vps->vps_base_layer_internal_flag) {
        hevc_dec_ptl(ctx, profile_tier_level, 0, vps->vps_max_sub_layers - 1);
    }

    hevc_parse_scalability_view_order(ctx, vps_ext);

    hevc_parse_vps_num_direct_ref_layers(ctx, vps_ext);

    if (hevc_parse_vps_num_layers(ctx, vps, vps_ext) != HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

    hevc_parse_vps_max_sub_layer_params(ctx, vps, vps_ext);

    heve_parse_vps_max_tid_il_ref_pic(ctx, vps_ext);

    if (hevc_parse_vps_ext_num_ptl(ctx, vps, vps_ext, &vps_num_profile_tier_level_minus1,
        profile_tier_level) != HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

    num_add_olss = 0;
    default_output_layer_idc = 0;
    if (vps_ext->num_layer_sets > 1) {
        num_add_olss = (td_s32)hevc_ue_v(ctx->bs_ptr, "num_add_olss");
        default_output_layer_idc = hevc_u_v(ctx->bs_ptr, 2, "default_output_layer_idc"); /* protocal required 2 bits */
    }

    if (hevc_parse_vps_num_output_layer_sets(ctx, vps_ext, num_add_olss) != HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

    if (hevc_parse_vps_num_layer_info(ctx, vps, vps_ext, default_output_layer_idc,
        vps_num_profile_tier_level_minus1) != HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

    if (hevc_derive_vps_rep_format(ctx, vps, vps_ext) != HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

    vps_ext->max_one_active_ref_layer_flag = hevc_u_v(ctx->bs_ptr, 1, "max_one_active_ref_layer_flag");
    hevc_u_v(ctx->bs_ptr, 1, "vps_poc_lsb_aligned_flag");

    hevc_parse_poc_lsb_not_present_flag(ctx, vps_ext);

    hevc_derive_vps_ext_dpb(ctx, vps);

    hevc_parse_direct_dependency_all_layers_type(ctx, vps, vps_ext);

    hevc_parse_vps_ext_vui_info(ctx, vps, vps_ext);

    return HEVC_DEC_NORMAL;
}

STATIC hevc_scaling_list *hevc_get_infer_scaling_list(const hevc_ctx *ctx)
{
    td_u32 ref_id;
    hevc_seq_param_set *ref_sps = NULL;
    hevc_pic_param_set *ref_pps = NULL;
    hevc_pps_extension_set *pps_ext = &(ctx->curr_pps->pps_extension);
    hevc_scaling_list *scaling_list = NULL;

    if (pps_ext->pps_infer_scaling_list_flag == 1) {
        ref_id = pps_ext->pps_scaling_list_ref_layer_id;
        ref_sps = (ctx->sps[ctx->curr_sps_id[ref_id]]);
        ref_pps = (ctx->pps[ctx->curr_pps_id[ref_id]]);

        if (ref_pps->pic_scaling_list_data_present_flag == 1) {
            scaling_list = &(ref_pps->scaling_list);
        } else if (ref_sps->sps_scaling_list_data_present_flag == 1) {
            scaling_list = &(ref_sps->scaling_list);
        }
    }  else if (ctx->curr_sps->sps_extension.sps_infer_scaling_list_flag == 1 &&
                ctx->curr_sps->sps_scaling_list_data_present_flag == 0 &&
                ctx->curr_pps->pic_scaling_list_data_present_flag == 0 &&
                pps_ext->pps_infer_scaling_list_flag == 0) {
        ref_id = ctx->curr_sps->sps_extension.sps_scaling_list_ref_layer_id;
        ref_sps = (ctx->sps[ctx->curr_sps_id[ref_id]]);

        if (ref_sps->sps_scaling_list_data_present_flag == 1) {
            scaling_list = &(ref_sps->scaling_list);
        }
    } else if (ctx->curr_pps->pic_scaling_list_data_present_flag) {
        scaling_list = &(ctx->curr_pps->scaling_list);
    } else if (ctx->curr_sps->sps_scaling_list_data_present_flag) {
        scaling_list = &(ctx->curr_sps->scaling_list);
    }

    return scaling_list;
}

STATIC td_s32 hevc_arrange_shvc_mem(hevc_ctx *ctx, td_u32 bit_depth)
{
    td_s32 ret = FSP_ERR;
    td_u8 layer_id;
    td_s32 dec_fs_num;
    td_s32 pmv_num;
    td_u32 layer_width, layer_height;
    td_u32 imag_width = 0;
    td_u32 imag_height = 0;
    td_u32 disp_width;
    td_u32 disp_height;
    fsp_cfg fsp_inst_cfg;
    hevc_seq_param_set *sps = ctx->curr_sps;

    disp_width = disp_height = 0;

    if (ctx->layer_num < 0) {
        dprint(PRN_FATAL, "%s layer num %d invalid!\n", __func__, ctx->layer_num);
        return ret;
    }

    for (layer_id = 0; layer_id < ctx->layer_num; layer_id++) {
        layer_width = (ctx->layer_width[layer_id]  + sps->max_cu_width - 1) & (~(sps->max_cu_width - 1));
        layer_height = (ctx->layer_height[layer_id] + 15) & (~15); /* 15: for align */

        imag_width = vfmw_max(imag_width, layer_width);
        imag_height = vfmw_max(imag_height, layer_height);
        disp_width = vfmw_max(imag_width, layer_width);
        disp_height = vfmw_max(imag_height, layer_height);
    }
    ctx->max_width = imag_width;
    ctx->max_height = imag_height;

    dec_fs_num = (td_s32)ctx->dpb.size;
    pmv_num = dec_fs_num;

    vfmw_check_sec_func(memset_s(&fsp_inst_cfg, sizeof(fsp_inst_cfg), 0, sizeof(fsp_cfg)));

    fsp_inst_cfg.vid_std = VFMW_HEVC;
    fsp_inst_cfg.bit_depth = bit_depth;
    fsp_inst_cfg.dec_width = imag_width;
    fsp_inst_cfg.dec_height = imag_height;
    fsp_inst_cfg.disp_width = disp_width;
    fsp_inst_cfg.disp_height = disp_height;
    fsp_inst_cfg.need_disp_num = 5; /* 5 is need disp num */
    fsp_inst_cfg.need_dec_num = (td_u32)((ctx->extra_data_ptr->dec_mode == VFMW_DEC_MODE_I) ? 1 : dec_fs_num);
    fsp_inst_cfg.need_pmv_num = (td_u32)((ctx->extra_data_ptr->dec_mode == VFMW_DEC_MODE_I) ? 1 : pmv_num);
    fsp_inst_cfg.frame_mbs_only = 1;

    ret = fsp_config(ctx->chan_id, &fsp_inst_cfg);
    if (ret != FSP_OK) {
        return SYN_PARSE_RESOURCE_INVALID;
    }

    return SYN_OK;
}
#endif

#if defined(VFMW_HEVC_SLC_LDY_SUPPORT)
STATIC td_s32 hevc_ldy_try_succ(hevc_ctx *ctx);

STATIC td_bool hevc_is_frame_end_nal(td_u32 nal_type)
{
    return ((nal_type == NAL_UNIT_VPS) ||
            /* H50_FUNNY_192_F180 No Right */
            /* (nal_type == NAL_UNIT_SPS) || */
            /* H50_FUNNY_1920_F200 No Right */
            /* (nal_type == NAL_UNIT_PPS) || */
            (nal_type == NAL_UNIT_EOPIC) ||
            (nal_type == NAL_UNIT_EOSTREAM));
}

STATIC td_void hevc_save_ldy_info(hevc_ctx *ctx)
{
    ctx->ldy_info.valid = 1;
    ctx->ldy_info.poc_cra = ctx->poc_cra;
    ctx->ldy_info.poc_random = ctx->poc_random_access[ctx->curr_slice.view_id];
    ctx->ldy_info.new_sequence = ctx->new_sequence;
    ctx->ldy_info.prev_rap_is_bla = ctx->prev_rap_is_bla;
    ctx->ldy_info.no_rasl_out_flag = ctx->no_rasl_output_flag[0];
}

STATIC td_void hevc_bak_ldy_info(hevc_ctx *ctx)
{
    if (ctx->ldy_info.valid == 1) {
        ctx->ldy_info.valid = 0;
        ctx->poc_cra = ctx->ldy_info.poc_cra;
        ctx->poc_random_access[ctx->curr_slice.view_id] = ctx->ldy_info.poc_random;
        ctx->new_sequence = ctx->ldy_info.new_sequence;
        ctx->prev_rap_is_bla = ctx->ldy_info.prev_rap_is_bla;
        ctx->no_rasl_output_flag[0] = ctx->ldy_info.no_rasl_out_flag;
    }
}

/* check last slice by tile and slice to aviod vdh die */
STATIC td_s32 hevc_can_not_end_ctb(const hevc_ctx *ctx)
{
    td_s32 cnt, slice_num, tile_start_y;
    td_s32 tile_end_y = 0;
    hevc_seq_param_set *sps = TD_NULL;
    hevc_pic_param_set *pps = TD_NULL;
    dec_slice_param_h265 *slice = TD_NULL;
    dec_slice_param_h265 *independ = TD_NULL;
    const vfmw_product_info *pdt = pdt_get_product_info();

    tile_start_y = 0;
    sps = ctx->curr_sps;
    pps = ctx->curr_pps;
    slice_num = ctx->curr_dps->slice_num;
    slice = (dec_slice_param_h265 *)ctx->curr_dps->slice_head;

    for (cnt = 0; cnt < slice_num && slice != TD_NULL; cnt++) {
        if (slice->dependent_slice_segment_flag == 0) {
            independ = slice;
        }

        if (slice->next_slice == TD_NULL) {
            break;
        }

        slice = slice->next_slice;
    }

    vfmw_check_ret_no_print(sps != TD_NULL, 1);
    vfmw_check_ret_no_print(pps != TD_NULL, 1);
    vfmw_check_ret_no_print(slice != TD_NULL, 1);
    vfmw_check_ret_no_print(independ != TD_NULL, 1);
    vfmw_check_ret_no_print(sps->ctb_num_width != 0, 1);

    if (pdt->pdt_board == BOARD_ES &&
        pps->num_tile_columns > 4) { /* more than 4 vdh die */
        return 1;
    }

    /* (1)no tile */
    vfmw_check_ret_no_print(!(pps->tiles_enabled_flag == 0 || pps->num_tile_rows <= 1), 0);

    for (cnt = 0; cnt < pps->num_tile_rows; cnt++) {
        tile_start_y = tile_end_y;
        tile_end_y += pps->row_bd[cnt];
    }

    /* (2)indepent slice in last tile line */
    if (slice->slice_segment_address / sps->ctb_num_width >= (td_u32)tile_start_y) {
        if (slice->dependent_slice_segment_flag == 0 ||
            independ->slice_segment_address / sps->ctb_num_width >= (td_u32)tile_start_y) {
            return 0;
        }
    }

    return 1;
}

VFMW_ACC_SLICE_H265 STATIC td_s32 hevc_check_nal(hevc_ctx *ctx)
{
    td_s32 ret;
    td_u8 *bs_buffer = TD_NULL;
    td_u32 bs_length, next_nal_type;
    td_u32 first_slice_flag = 0;

    vfmw_check_ret(ctx->curr_nal != TD_NULL, HEVC_DEC_ERR);

    bs_buffer = ctx->curr_nal->stream[0].stream_buffer;
    bs_length = ctx->curr_nal->stream[0].bitstream_length;
    bs_init(ctx->bs_ptr, bs_buffer, bs_length);
    bs_skip(ctx->bs_ptr, 24); /* 24 is bit num */
    if (bs_residual_bits(ctx->bs_ptr) < 16) { /* 16 is bit num */
        return HEVC_DEC_ERR;
    }

    bs_skip(ctx->bs_ptr, 1);
    next_nal_type = (td_u32)bs_get(ctx->bs_ptr, 6); /* 6 is bit num */
    bs_skip(ctx->bs_ptr, 9); /* 9 is bit num */

    if (hevc_is_slice_unit(next_nal_type) == 1 && bs_residual_bits(ctx->bs_ptr) > 0) {
        first_slice_flag = (td_u32)bs_get(ctx->bs_ptr, 1);
    }
    if (ctx->num_slice_para > 0 && (hevc_is_frame_end_nal(next_nal_type) == TD_TRUE || first_slice_flag)) {
        ctx->half_nal_succ = 0;
        ctx->have_nal_not_decoded = 1;
        ret = hevc_ldy_try_succ(ctx);
        if (ret == HEVC_DEC_NORMAL) {
            return SYN_PARSE_PRE_FRMDONE;
        } else if (ret == HEVC_DEC_ERR) {
            ctx->have_nal_not_decoded = 0;
            return HEVC_DEC_ERR;
        }
    } else if (hevc_is_slice_unit(next_nal_type) != 1) {
        return HEVC_DEC_ERR;
    }

    /* slc ldy do not try half */
    if (ctx->ldy_mode == LDY_SLC) {
        return HEVC_DEC_ERR;
    }

    ctx->half_nal_succ = 0;
    ctx->half_nal_try = TRY_HALF;
    hevc_save_ldy_info(ctx);

    return HEVC_DEC_NORMAL;
}

VFMW_ACC_SLICE_H265 STATIC td_s32 hevc_check_half_nal(hevc_ctx *ctx)
{
    td_s32 ret;
    td_s32 chan_id = ctx->chan_id;

    vfmw_check_ret(ctx->extra_data_ptr != TD_NULL, HEVC_DEC_ERR);

    ctx->no_next_slice = 0;
    ctx->half_nal_try = TRY_IDLE;

    if (ctx->seg_cut_state == SEG_CUT_HALF) {
        if (ctx->half_nal_en == 0 || ctx->half_nal_succ == 1) { /* slice end may have invalid bytes */
            return HEVC_DEC_ERR;
        }

        if (ctx->extra_data_ptr->crc_mode == 0 && stm_control(chan_id, STM_CMD_GET_SEG_NUM, TD_NULL) > 0) {
            return HEVC_DEC_ERR;
        }

        ret = hevc_check_nal(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            /* return err or prev frm done */
            return ret;
        }
    }

    if (ctx->seg_cut_state == SEG_CUT_REPEAT) {
        if (ctx->half_nal_succ == 1) {
            ctx->half_nal_succ = 0;
            return HEVC_DEC_ERR;
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_is_need_try_dec(const hevc_ctx *ctx)
{
    if (ctx->curr_sps == TD_NULL ||
        ctx->num_slice_para == 0 ||
        ctx->curr_sps->ctb_num_width <= 2 || /* 2 is ctb num */
        ctx->try_slice_num == ctx->num_slice_para) {
        return 0;
    }

    if (ctx->ldy_mode == LDY_SED &&
        hevc_can_not_end_ctb(ctx) == 1) {
        return 0;
    }

    if (ctx->ldy_mode == LDY_SLC &&
        ctx->first_slc_ctb != 0) {
        return 0;
    }

    if (ctx->half_nal_try != TRY_IDLE && ctx->curr_nal != TD_NULL) {
        return 1;
    }

    if (ctx->half_nal_try == TRY_IDLE &&
        stm_control(ctx->chan_id, STM_CMD_GET_SEG_NUM, TD_NULL) <= 0) {
        return 1;
    }

    if (ctx->extra_data_ptr != TD_NULL && ctx->extra_data_ptr->crc_mode != 0) {
        if (os_get_rand() % 3 == 0) { /* 3 is vfmw_max num */
            return 1;
        }
    }

    return 0;
}

STATIC td_s32 hevc_set_dec_param(hevc_ctx *ctx)
{
    td_s32 chan_id = ctx->chan_id;

    if (ctx->half_nal_en == 0) {
        return HEVC_DEC_ERR;
    }

    if (hevc_is_need_try_dec(ctx) == 1) {
        if (hevc_dec_check(ctx) != HEVC_DEC_NORMAL) {
            ctx->curr_nal = TD_NULL;
            report_decsyntax_err(ctx->chan_id);
            return HEVC_DEC_ERR;
        }

        ctx->curr_dps->sed_state = SED_IDLE;
        ctx->curr_dps->ldy_mode = (td_s32)ctx->ldy_mode;
        ctx->try_slice_num = ctx->num_slice_para;

        if (ctx->ldy_mode == LDY_SED) {
            ctx->curr_dps->sed_state = SED_TRY;
            if (ctx->half_nal_try == TRY_IDLE) {
                ctx->half_nal_try = TRY_NORM;
            } else {
                ctx->half_nal_try = TRY_SLICE;
                ctx->have_nal_not_decoded = 1;
            }
        }

        if (hevc_write_pic_msg(ctx) != HEVC_DEC_NORMAL) {
            return HEVC_DEC_ERR;
        }

        return SYN_PARSE_PRE_FRMDONE;
    } else if (ctx->half_nal_try == TRY_HALF) {
        ctx->half_nal_try = TRY_IDLE;
        ctx->num_slice_para--;
        hevc_bak_ldy_info(ctx);
        hevc_clear_curr_nal(ctx);
        dps_delete_slice(chan_id, ctx, ctx->curr_dps);
    }

    return HEVC_DEC_ERR;
}
#endif

#ifdef VFMW_HEVC_SLC_LDY_SUPPORT
STATIC td_s32 hevc_ldy_try_succ(hevc_ctx *ctx)
{
    td_s32 ret;

    ret = hevc_dec_check(ctx);
    if (ret != HEVC_DEC_NORMAL) {
        report_decsyntax_err(ctx->chan_id);
        return HEVC_DEC_ERR;
    }

    ctx->no_next_slice = 1;
    ctx->curr_pic.is_ref_idc = 1;
    if (ctx->curr_dps != TD_NULL) {
        ctx->curr_dps->sed_state = SED_IDLE;
    }
    ctx->half_nal_try = TRY_IDLE;
    ctx->try_slice_num = 0;

    ret = hevc_write_pic_msg(ctx);
    if (ret != HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

    ret = hevc_dec_post_proc(ctx, TD_FALSE);
    if (ret != HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

VFMW_ACC_SLICE_H265 STATIC td_s32 hevc_dec_proc_pre_nal(hevc_ctx *ctx)
{
    td_s32 ret;
    td_s32 chan_id = ctx->chan_id;

    if (ctx->half_nal_try == TRY_SLICE) {
        /* prev half slice sed try succ */
        if (ctx->curr_dps != TD_NULL && ctx->curr_dps->sed_state == SED_SUCC) {
            ret = hevc_dec_post_proc(ctx, TD_FALSE);
            if (ret != HEVC_DEC_NORMAL) {
                return HEVC_DEC_ERR;
            }

            ctx->half_nal_succ = 1;
            ctx->no_next_slice = 1;
            ctx->curr_pic.is_ref_idc = 1;
            ctx->curr_dps->sed_state = SED_IDLE;
            ctx->half_nal_try = TRY_IDLE;
            ctx->try_slice_num = 0;
            return SYN_PARSE_NOT_FRMDONE;
        }

        if (ctx->try_slice_num > 0) {
            ctx->try_slice_num--;
        }
        if (ctx->num_slice_para > 0) {
            ctx->num_slice_para--;
        }
        hevc_bak_ldy_info(ctx);
        hevc_clear_curr_nal(ctx);
        dps_delete_slice(chan_id, ctx, ctx->curr_dps);

        return SYN_PARSE_NOT_FRMDONE;
    } else if (ctx->half_nal_try == TRY_NORM) {
        /* sed try succ */
        if (ctx->curr_dps != TD_NULL && ctx->curr_dps->sed_state == SED_SUCC) {
            ctx->no_next_slice = 1;
            ctx->curr_pic.is_ref_idc = 1;

            ret = hevc_dec_post_proc(ctx, TD_FALSE);
            if (ret != HEVC_DEC_NORMAL) {
                return HEVC_DEC_ERR;
            }

            ctx->curr_dps->sed_state = SED_IDLE;
            ctx->half_nal_try = TRY_IDLE;
            ctx->try_slice_num = 0;
        }

        return SYN_PARSE_NOT_FRMDONE;
    }

    return HEVC_DEC_NORMAL;
}
#endif

STATIC td_s32 hevc_init_stream(hevc_ctx *ctx, const hevc_stream_packet *packet)
{
    td_s32 ret = HEVC_DEC_ERR;
    if (ctx->have_nal_not_decoded != 1) {
        if ((ctx->extra_data_ptr->dynamic_fs_en == 1) &&
            (ctx->extra_data_ptr->wait_fs_flag == 1) &&
            (ctx->scd_wrap_around == 1)) {
            ctx->curr_nal = ctx->last_nal;
            ctx->scd_wrap_around = 0;
        } else {
            ret = hevc_receive_packet(ctx, packet);
            ctx->last_nal = ctx->curr_nal;
        }
    } else {
        ret = (ctx->curr_nal != TD_NULL) ? HEVC_DEC_NORMAL : HEVC_DEC_ERR;
        if (ret == HEVC_DEC_ERR) {
            dprint(PRN_ERROR, "hevc_init_stream: ctx->curr_nal is null.\n");
            return HEVC_DEC_ERR;
        }
    }

    ctx->have_nal_not_decoded = 0;

    return ret;
}

STATIC td_void hevc_report_fake_frame(hevc_ctx *ctx)
{
    const td_u32 mb_size = 16;

    if (ctx->curr_sps == TD_NULL) {
        return;
    }
    if ((ctx->total_pic_num != 0) &&
        (ctx->is_resolution_changed == 1) &&
        (ctx->curr_sps->pic_width_in_luma_samples != ctx->fake_frm_width_in_mb * mb_size ||
         ctx->curr_sps->pic_height_in_luma_samples != ctx->fake_frm_height_in_mb * mb_size)) {
        ctx->fake_frm_width_in_mb = ctx->curr_sps->pic_width_in_luma_samples / mb_size;
        ctx->fake_frm_height_in_mb = ctx->curr_sps->pic_height_in_luma_samples / mb_size;
        report_fake_frame(ctx->chan_id);
    }
}

STATIC td_s32 hevc_process_size_change(hevc_ctx *ctx)
{
    if (ctx->no_output_of_prior_pics_flag[ctx->curr_nal->nal_layer_id] == HEVC_TRUE) {
        if (hevc_empty_frm_from_dpb(ctx) != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "%s call hevc_empty_frm_from_dpb failed\n", __func__);
        }

        if (hevc_flush_output_frm_from_dpb(ctx) != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "%s call hevc_flush_output_frm_from_dpb failed\n", __func__);
        }
        /* If the VO queue fails to be inserted, the value of ctx->curr_nal is set to null.
           If no result is returned, a core dump occurs. */
        if (ctx->curr_nal == TD_NULL) {
            dprint(PRN_ERROR, "ctx->curr_nal is null!\n");
            return HEVC_DEC_ERR;
        }
    }

    /* inquire voque new image num */
    if (fsp_get_frame_buf_mode(ctx->chan_id) == VFMW_FRM_ALLOC_OUTER) {
        td_u32 readnum = 0;
        td_u32 newnum = 0;
        voq_get_queue_img_num(ctx->chan_id, &readnum, &newnum);

        if (newnum > 0) {
            ctx->have_nal_not_decoded = 1;
            return SYN_PARSE_RESOURCE_INVALID;
        }
    }

    hevc_report_fake_frame(ctx);

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_multislc_ldy_process(hevc_ctx *ctx)
{
    td_s32 ret;

    /*
        默认不是最后一个slice，除非遇到新帧或者流结束，才会设置为最后一个slice
        如果不是新图，且是独立slice，那么可以提前将前一组的送到VDH解码
        如果slc_lcy_cfg_cnt=15的话，就需要等待把所有的slice全部接收完才能送给VDH
        is_get_new_slice 避免重复送两次同一个slice导致都走进这个流程
        slice_num > 0 避免单slice场景访问slice_list挂死
        此时如果seg数量较少，则先送去dec现场解码
    */
    if (ctx->curr_dps->ldy_mode == LDY_MULTI_SLC &&
        stm_control(ctx->chan_id, STM_CMD_GET_SEG_NUM, TD_NULL) <= 0 &&
        ctx->curr_slice.new_pic_type != IS_SKIP_PIC &&
        ctx->curr_slice.first_slice_segment_in_pic_flag == 0 &&
        ctx->curr_slice.dependent_slice_segment_flag == 0 &&
        ctx->curr_dps->process_cnt < 15 &&  // 15: max slice group - 1
        ctx->curr_dps->is_get_new_slice == TD_TRUE &&
        ctx->curr_dps->slice_num > 0) {
        (td_void)hevc_write_pic_msg(ctx);
        ctx->have_nal_not_decoded = 1;

        if (ctx->curr_dps->process_cnt == 0) {
            ret = hevc_dec_post_proc(ctx, TD_TRUE);
            if (ret != HEVC_DEC_NORMAL) {
                dprint(PRN_FATAL, "%s %d process hevc_dec_post_proc error!\n", __func__, __LINE__);
                return HEVC_DEC_ERR;
            }
        }

        return SYN_PARSE_PRE_FRMDONE;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_pre_parse_slice(hevc_ctx *ctx, td_s32 *is_size_changed, td_u32 nal_header)
{
    td_s32 is_new_pic = 0;
    td_s32 ret;
    td_void *lst_slc = TD_NULL;

    if (hevc_inquire_slice_property(ctx, &is_new_pic, is_size_changed) != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "%s error.\n", __func__);
        hevc_clear_curr_nal(ctx);
        if (nal_header != 0x00000100) {
            report_decsyntax_err(ctx->chan_id);
        }
        return HEVC_DEC_ERR;
    }

    if ((is_new_pic == 1) && (hevc_have_slice_to_dec(ctx) == HEVC_DEC_NORMAL)) {
        ctx->curr_nal->nal_bitoffset = 0;
        ctx->curr_nal->nal_unit_type = NAL_UNIT_EOPIC;
        ctx->have_nal_not_decoded = 1;
        ctx->is_insert_eopic_nal = 1;
        // 需要再插入eop之前更新slice的参数
        lst_slc = ctx->curr_dps->slice_list[ctx->curr_dps->slice_num - 1];
        if (lst_slc == TD_NULL) {
            dprint(PRN_FATAL, "%s %d slc param is null!\n", __func__, __LINE__);
            return HEVC_DEC_ERR;
        }
        ctx->curr_dps->is_rcv_lst_slc = TD_TRUE;
        ctx->curr_dps->last_slice = lst_slc;
        if (hevc_dec_one_nal(ctx) == SYN_PARSE_ERR) {
            return SYN_PARSE_PRE_FRMERR;
        }

        ctx->new_pic_counter++;
        return SYN_PARSE_PRE_FRMDONE;
    }

    ret = hevc_multislc_ldy_process(ctx);
    if (ret != HEVC_DEC_NORMAL) {
        return ret;
    }

    if (*is_size_changed == 1) {
        if (hevc_process_size_change(ctx) == SYN_PARSE_RESOURCE_INVALID) {
            return SYN_PARSE_RESOURCE_INVALID;
        }
    }

    return HEVC_DEC_NORMAL;
}
#if defined(VFMW_HEVC_SLC_LDY_SUPPORT)
STATIC td_s32 hevc_decode_slc_ldy(hevc_ctx *ctx)
{
    td_s32 ret;

    ret = hevc_dec_proc_pre_nal(ctx);
    if (ret != HEVC_DEC_NORMAL) {
        ctx->curr_nal = TD_NULL;
        ctx->half_nal_try = TRY_IDLE;
        return ret;
    }

    ret = hevc_check_half_nal(ctx);
    if (ret == SYN_PARSE_PRE_FRMDONE) {
        return ret;
    }

    if (ret != HEVC_DEC_NORMAL) {
        hevc_clear_curr_nal(ctx);
        ret = hevc_set_dec_param(ctx);
        if (ret == SYN_PARSE_PRE_FRMDONE) {
            return SYN_PARSE_PRE_FRMDONE;
        }

        ctx->have_nal_not_decoded = 0;
        return HEVC_DEC_ERR;
    }

    // for multi slice
    if (ctx->ldy_mode == LDY_MULTI_SLC) {
        ctx->curr_dps->ldy_mode = (td_s32)ctx->ldy_mode;
    }
    return HEVC_DEC_NORMAL;
}
#endif
STATIC td_s32 hevc_decode_pre_parse_nal(hevc_ctx *ctx, td_u32 nal_header)
{
    td_s32 ret;
    td_s32 is_size_changed;

    if (hevc_is_slice_unit(ctx->curr_nal->nal_unit_type) == 1) {
        ret = hevc_pre_parse_slice(ctx, &is_size_changed, nal_header);
        if (ret != HEVC_DEC_NORMAL) {
            return ret;
        }
        if (ctx->curr_nal == TD_NULL) {
            return HEVC_DEC_ERR;
        }
    } else {
        ctx->curr_nal->nal_bitoffset = 0;
        bs_init(ctx->bs_ptr, (td_u8 *)((ctx->curr_nal->stream[0].stream_buffer)),
                ctx->curr_nal->stream[0].bitstream_length);
        bs_skip(ctx->bs_ptr, 24); /* 24 is bit num */

        ctx->curr_nal->forbidden_zero_bit = (td_u32)bs_get(ctx->bs_ptr, 1);
        ctx->curr_nal->nal_unit_type = (td_u32)bs_get(ctx->bs_ptr, 6); /* 6 is bit num */
        ctx->curr_nal->nuh_reserved_zero_6bits = (td_u32)bs_get(ctx->bs_ptr, 6); /* 6 is bit num */
        ctx->curr_nal->nuh_temporal_id = (td_u32)(bs_get(ctx->bs_ptr, 3) - 1); /* 3 is bit num */
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_parse_nalu_head(hevc_ctx *ctx)
{
    ctx->curr_nal->forbidden_zero_bit = (td_u32)bs_get(ctx->bs_ptr, 1);
    ctx->curr_nal->nal_unit_type = (td_u32)bs_get(ctx->bs_ptr, 6); /* 6 is bit num */
    ctx->curr_nal->nal_layer_id = (td_u32)bs_get(ctx->bs_ptr, 6); /* 6 is bit num */
    if (ctx->curr_nal->nal_layer_id > HEVC_MAX_NUM_LAYERS - 1) {
        dprint(PRN_ERROR, "nal_layer_id = %d out of range[0, %d].\n", ctx->curr_nal->nal_layer_id,
            HEVC_MAX_NUM_LAYERS - 1);
        hevc_clear_curr_nal(ctx);
        return HEVC_DEC_ERR;
    }

#ifdef VFMW_MV_HEVC_SUPPORT
    if ((ctx->curr_nal->nal_layer_id > 0) && (hevc_is_base_layer_only(ctx) == 1)) {
        hevc_clear_curr_nal(ctx);
        return HEVC_DEC_ERR;
    }
#endif

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_decode_naltype(hevc_ctx *ctx)
{
    td_s32 ret;
    td_u32 nal_header;

    /* start code(3 byte) + nalu header(2 byte), this condition means nalu doesn't include valid stream */
    if (ctx->curr_nal->stream[0].bitstream_length <= 5) { /* 5 is bit num */
        hevc_clear_curr_nal(ctx);
        return HEVC_DEC_ERR;
    }

    ctx->curr_nal->nal_bitoffset = 0;

    bs_init(ctx->bs_ptr, (td_u8 *)(((ctx->curr_nal->stream[0].stream_buffer))),
            ctx->curr_nal->stream[0].bitstream_length);

    /* read first 32bit bs data : 0x00 0x00 0x01 0xxx */
    nal_header = (td_u32)bs_show(ctx->bs_ptr, 32); /* 32 is bit num */

    /* skip start code : 0x00 0x00 0x01 */
    bs_skip(ctx->bs_ptr, 24); /* 24 is bit num */

    if (hevc_parse_nalu_head(ctx) != HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

    ctx->curr_nal->nuh_temporal_id = (td_u32)(bs_get(ctx->bs_ptr, 3) - 1); /* 3 is bit num */

    ret = hevc_decode_pre_parse_nal(ctx, nal_header);
    if (ret != HEVC_DEC_NORMAL) {
        return ret;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_decode_packet(hevc_ctx *ctx, const hevc_stream_packet *packet)
{
    td_s32 ret;

    if (packet == TD_NULL) {
        dprint(PRN_ERROR, "hevc_decode_packet TD_NULL == packet!\n");
        return HEVC_DEC_ERR;
    }
    ctx->advanced_cfg = ((td_u32)ctx->extra_data_ptr->dec_mode << 16) & 0x70000; /* 16 is bit ofs */
    ctx->ref_error_thr = (td_u32)ctx->extra_data_ptr->ref_err_thr;
    ctx->out_error_thr = (td_u32)ctx->extra_data_ptr->out_err_thr;

    ret = hevc_init_stream(ctx, packet);
    if (ret == HEVC_DEC_NORMAL) {
#if defined(VFMW_HEVC_SLC_LDY_SUPPORT)
        ret = hevc_decode_slc_ldy(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            return ret;
        }
#endif
        ret = hevc_decode_naltype(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            return ret;
        }

        ret = hevc_dec_one_nal(ctx);
        if ((ret == SYN_PARSE_CURR_FRMDONE) || (ret == SYN_PARSE_ERR) || (ret == SYN_PARSE_RESOURCE_INVALID)) {
            return ret;
        } else if (ret != HEVC_DEC_NORMAL) {
            return SYN_PARSE_NOT_FRMDONE;
        }

        ctx->curr_pic.current_nal_size = 0;

#if defined(VFMW_HEVC_SLC_LDY_SUPPORT)
        if (ctx->half_nal_en == 1 && ctx->curr_slice.new_pic_type != IS_SKIP_PIC) {
            ret = hevc_set_dec_param(ctx);
            if (ret == SYN_PARSE_PRE_FRMDONE) {
                return SYN_PARSE_PRE_FRMDONE;
            }
        }
#endif
    }

    return SYN_PARSE_NOT_FRMDONE;
}

STATIC td_s32 hevc_dec_eop(hevc_ctx *ctx)
{
    td_s32 ret;
    td_u32 code0 = 0;
    td_u32 code1 = 0;
    td_void *lst_slc = TD_NULL;

    ctx->seg_type = STM_SEG_EOP;

    /* 真正的NAL_UNIT_RESERVED_VCL_N10类型nal,不进行特殊处理, */
    /* 否则为每一帧结尾插入的特殊nal */
    if (ctx->is_insert_eopic_nal != 1) {
        code0 = (td_u32)bs_get(ctx->bs_ptr, 32); /* 32 is bit num */
        code1 = (td_u32)bs_get(ctx->bs_ptr, 32); /* 32 is bit num */
    }

    if ((code0 == VFMW_STREAM_END_CODE1 && code1 == VFMW_STREAM_END_CODE2) ||
        (ctx->is_insert_eopic_nal == 1)) {
        report_meet_newfrm(ctx->chan_id);

        if (ctx->curr_pic.state == HEVC_PIC_EMPTY) {
            return HEVC_DEC_NORMAL;
        }

        ret = hevc_dec_check(ctx); /* 解码前检查 */
        if (ret != HEVC_DEC_NORMAL) {
            report_decsyntax_err(ctx->chan_id);
            return SYN_PARSE_ERR;
        }

        ctx->curr_dps->sed_state = SED_IDLE;

        // 需要再插入eop之前更新slice的参数
        if (ctx->curr_dps->slice_num > 0) {
            lst_slc = ctx->curr_dps->slice_list[ctx->curr_dps->slice_num - 1];
            if (lst_slc != TD_NULL) {
                ctx->curr_dps->is_rcv_lst_slc = TD_TRUE;
                ctx->curr_dps->last_slice = lst_slc;
            }
        }
        ret = hevc_write_pic_msg(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            return SYN_PARSE_ERR;
        }
        ret = hevc_dec_post_proc(ctx, TD_FALSE);
        if (ret != HEVC_DEC_NORMAL) {
            return SYN_PARSE_ERR;
        }

        return SYN_PARSE_CURR_FRMDONE;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_eob(hevc_ctx *ctx)
{
    ctx->curr_pic.big_frame_eop = 1;

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_eos(hevc_ctx *ctx)
{
    td_s32 ret;
    td_void *lst_slc = TD_NULL;

    ctx->seg_type = STM_SEG_EOS;
    if (hevc_is_eos_nal(ctx) == 1) {
        ctx->is_stream_end_flag = 1;
        report_meet_newfrm(ctx->chan_id);

        if (ctx->curr_pic.state == HEVC_PIC_EMPTY) {
            return HEVC_DEC_NORMAL;
        }

        ret = hevc_dec_check(ctx); /* 解码前检查 */
        if (ret != HEVC_DEC_NORMAL) {
            report_decsyntax_err(ctx->chan_id);
            return SYN_PARSE_ERR;
        }

        ctx->curr_dps->sed_state = SED_IDLE;

        // 需要再插入eos之前更新slice的参数
        if (ctx->curr_dps->slice_num > 0) {
            lst_slc = ctx->curr_dps->slice_list[ctx->curr_dps->slice_num - 1];
            if (lst_slc != TD_NULL) {
                ctx->curr_dps->is_rcv_lst_slc = TD_TRUE;
                ctx->curr_dps->last_slice = lst_slc;
            }
        }
        ret = hevc_write_pic_msg(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            return SYN_PARSE_ERR;
        }

        ret = (td_s32)hevc_dec_post_proc(ctx, TD_FALSE);
        if (ret != HEVC_DEC_NORMAL) {
            return SYN_PARSE_ERR;
        }

        return SYN_PARSE_CURR_FRMDONE;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_other_slice(hevc_ctx *ctx)
{
    td_s32 ret;
    vfmw_dec_err_type err_type;

    switch (ctx->curr_nal->nal_unit_type) {
        case NAL_UNIT_VPS:
            ctx->seg_type = STM_SEG_VPS;
            ret = hevc_dec_vps(ctx);
            hevc_clear_curr_nal(ctx);

            if (ret != HEVC_DEC_NORMAL) {
                dprint(PRN_ERROR, "VPS decode error.\n");
                err_type = SYNTAX_ERR_VPS;
                goto err;
            }
            break;

        case NAL_UNIT_SPS:
            ctx->seg_type = STM_SEG_SPS;
            ret = hevc_dec_sps(ctx);
            hevc_clear_curr_nal(ctx);

            if (ret != HEVC_DEC_NORMAL) {
                dprint(PRN_ERROR, "SPS decode error.\n");
                err_type = SYNTAX_ERR_VPS;
                goto err;
            }
            break;

        case NAL_UNIT_PPS:
            ctx->seg_type = STM_SEG_PPS;
            ret = hevc_dec_pps(ctx);
            hevc_clear_curr_nal(ctx);

            if (ret != HEVC_DEC_NORMAL) {
                dprint(PRN_ERROR, "PPS decode error.\n");
                err_type = SYNTAX_ERR_VPS;
                goto err;
            }
            break;

        default:
            break;
    }

    return HEVC_DEC_NORMAL;
err:
    report_decsyntax_err(ctx->chan_id);
    report_dec_err(ctx->chan_id, ctx->stm_info.pts, err_type);
    return HEVC_DEC_ERR;
}

STATIC td_s32 hevc_dec_non_slice(hevc_ctx *ctx)
{
    td_s32 ret = HEVC_DEC_NORMAL;

    switch (ctx->curr_nal->nal_unit_type) {
        case NAL_UNIT_VPS:
        case NAL_UNIT_SPS:
        case NAL_UNIT_PPS:
            ret = hevc_dec_other_slice(ctx);
            break;

        case NAL_UNIT_PREFIX_SEI:
        case NAL_UNIT_SUFFIX_SEI:
            ctx->seg_type = STM_SEG_SEI;
            ret = hevc_dec_sei(ctx);
            hevc_clear_curr_nal(ctx);

            if (ret != HEVC_DEC_NORMAL) {
                dprint(PRN_ERROR, "sei decode error.\n");
                report_decsyntax_err(ctx->chan_id);
                return HEVC_DEC_ERR;
            }
            break;

        case NAL_UNIT_EOPIC:
            ret = hevc_dec_eop(ctx);
            if (ctx->is_insert_eopic_nal != 1) {
                hevc_clear_curr_nal(ctx);
            }
            ctx->is_insert_eopic_nal = 0;
            break;

        case NAL_UNIT_END_OF_BIG_FRAME:
            hevc_dec_eob(ctx);
            ret = hevc_dec_eop(ctx);
            hevc_clear_curr_nal(ctx);
            break;

        case NAL_UNIT_EOSTREAM:
            ret = hevc_dec_eos(ctx);
            hevc_clear_curr_nal(ctx);
            break;

        default:
            dprint(PRN_ERROR, "***** NAL: UNSUPPORT, nal_unit_type=%d\n", ctx->curr_nal->nal_unit_type);

            if (ctx->curr_nal->nal_unit_type > NAL_UNIT_UNSPECIFIED_63) {
                report_unsupport(ctx->chan_id);
                report_decsyntax_err(ctx->chan_id);
                dprint(PRN_ERROR, "report unsupport nal_unit_type:%d.\n", ctx->curr_nal->nal_unit_type);
            }

            hevc_clear_curr_nal(ctx);
            break;
    }

    return ret;
}

STATIC td_s32 hevc_dec_one_nal(hevc_ctx *ctx)
{
    td_s32 ret;
    td_u32 i;

    for (i = 0; i < ctx->curr_nal->nal_segment; i++) {
        ctx->max_bytes_received += ctx->curr_nal->stream[i].bitstream_length;
    }

    ctx->curr_nal->nal_used_segment = 0; /* decode from first segment */

    if (ctx->curr_nal->nal_unit_type == NAL_UNIT_EOS || ctx->curr_nal->nal_unit_type == NAL_UNIT_EOB) {
        ctx->new_sequence = HEVC_TRUE;
    }

    ctx->curr_nal->nal_bitoffset += 16; /* used bits when decode nal, 使用了16 bit */

    if (hevc_is_slice_unit(ctx->curr_nal->nal_unit_type) == 1) {
        ctx->seg_type = STM_SEG_SLICE;
        ret = hevc_dec_slice(ctx);
        if (ret == SYN_PARSE_RESOURCE_INVALID) {
            ctx->have_nal_not_decoded = 1;
            return SYN_PARSE_RESOURCE_INVALID;
        } else if (ret != HEVC_DEC_NORMAL) {
            hevc_clear_curr_slice(ctx);
            report_decsyntax_err(ctx->chan_id);
            return HEVC_DEC_ERR;
        }
    } else {
        ret = hevc_dec_non_slice(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            return ret;
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_process_vps_num_layer_sets(const hevc_ctx *ctx, hevc_video_param_set *vps)
{
#ifndef VFMW_MV_HEVC_SUPPORT
    td_s32 i, j;
#endif

    vps->vps_num_layer_sets_minus1 = (td_s32)hevc_ue_v(ctx->bs_ptr, "vps_num_layer_sets_minus1");
    if (vps->vps_num_layer_sets_minus1 < 0 || vps->vps_num_layer_sets_minus1 > (HEVC_MAX_VPS_OP_SETS_PLUS1 - 1)) {
        dprint(PRN_ERROR, "vps_num_layer_sets_minus1(%d) out of range(0, %d), force to 0.\n",
               vps->vps_num_layer_sets_minus1, (HEVC_MAX_VPS_OP_SETS_PLUS1 - 1));
        vps->vps_num_layer_sets_minus1 = 0;
    }

#ifdef VFMW_MV_HEVC_SUPPORT
    hevc_derive_layer_list(ctx, vps);
#else
    for (i = 1; i <= vps->vps_num_layer_sets_minus1; i++) {
        /* Operation point set */
        for (j = 0; j <= vps->vps_max_layer_id; j++) {
            /* delete snprintf for cpu load */
            vps->layer_id_included_flag[i][j] = (td_s32)hevc_u_v(ctx->bs_ptr, 1, "layer_id_included_flag");
        }
    }
#endif

    return;
}

STATIC td_s32 hevc_process_vps_timing_info(const hevc_ctx *ctx, hevc_video_param_set *vps)
{
    td_s32 ret;
    td_s32 i;

    vps->vps_timing_info_present_flag = hevc_u_v(ctx->bs_ptr, 1, "vps_timing_info_present_flag");

    if (vps->vps_timing_info_present_flag) {
        vps->vps_num_units_in_tick = (td_s32)hevc_u_v(ctx->bs_ptr, 32, "vps_num_units_in_tick"); /* 32 is bit num */
        vps->vps_time_scale = (td_s32)hevc_u_v(ctx->bs_ptr, 32, "vps_time_scale"); /* 32 is bit num */
        vps->vps_poc_proportional_to_timing_flag = hevc_u_v(ctx->bs_ptr, 1, "vps_poc_proportional_to_timing_flag");

        if (vps->vps_poc_proportional_to_timing_flag) {
            vps->vps_num_ticks_poc_diff_one_minus1 =
                (td_s32)hevc_ue_v(ctx->bs_ptr, "vps_num_ticks_poc_diff_one_minus1");
        }

        vps->vps_num_hrd_parameters = (td_s32)hevc_ue_v(ctx->bs_ptr, "vps_num_hrd_parameters");

        if (vps->vps_num_hrd_parameters < 0 || vps->vps_num_hrd_parameters > HEVC_MAX_VPS_OP_SETS_PLUS1) {
            dprint(PRN_ERROR, "vps_num_hrd_parameters(%d) out of range(0,%d).\n",
                   vps->vps_num_hrd_parameters, HEVC_MAX_VPS_OP_SETS_PLUS1);
            return HEVC_DEC_ERR;
        }

        if (vps->vps_num_hrd_parameters > 0) {
            vps->cprms_present_flag[0] = HEVC_TRUE;
        }

        for (i = 0; i < vps->vps_num_hrd_parameters; i++) {
            vps->hrd_layer_set_idx[i] = (td_s32)hevc_ue_v(ctx->bs_ptr, "hrd_layer_set_idx");

            if (i > 0) {
                vps->cprms_present_flag[i] = (td_s32)hevc_u_v(ctx->bs_ptr, 1, "cprms_present_flag");
            }

            ret = hevc_dec_hrd_param(ctx, &(vps->hrd_parameters), vps->cprms_present_flag[i],
                                     vps->vps_max_sub_layers_minus1 - 1);
            if (ret != HEVC_DEC_NORMAL) {
                dprint(PRN_ERROR, "VPS hevc_dec_hrd_param error.\n");
                return HEVC_DEC_ERR;
            }
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_process_vps_extension(hevc_ctx *ctx, hevc_video_param_set *vps)
{
    vps->vps_extension_flag = hevc_u_v(ctx->bs_ptr, 1, "vps_extension_flag");

#ifdef VFMW_MV_HEVC_SUPPORT
    if (ctx->is_multi_layers == 1) {
        td_s32 ret;
        bs_skip_to_next_byte(ctx->bs_ptr);

        ret = hevc_parse_vps_extension(ctx, vps);
        if (ret != HEVC_DEC_NORMAL) {
           dprint(PRN_ERROR, "HEVC VPSExtension error.\n");
           return HEVC_DEC_ERR;
        }
        vps->vps_extension_flag = hevc_u_v(ctx->bs_ptr, 1, "vps_extension2_flag");
    }

    if (vps->vps_extension_flag) {
        while (hevc_more_rbsp_data(ctx) == HEVC_TRUE) {
            vps->vps_extension_data_flag = hevc_u_v(ctx->bs_ptr, 1, "");
        }
    }
#else
    while (hevc_more_rbsp_data(ctx) == HEVC_TRUE) {
        vps->vps_extension_data_flag = hevc_u_v(ctx->bs_ptr, 1, "");
    }
#endif

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_process_vps_sub_layers(const hevc_ctx *ctx, hevc_video_param_set *vps)
{
    td_s32 i;
    td_u32 tmp;

    vps->vps_sub_layer_ordering_info_present_flag =
        hevc_u_v(ctx->bs_ptr, 1, "vps_sub_layer_ordering_info_present_flag");

    for (i = 0; i <= vps->vps_max_sub_layers_minus1 - 1; i++) {
        tmp = hevc_ue_v(ctx->bs_ptr, "vps_max_dec_pic_buffering_minus1[i]");
        if (tmp > HEVC_MAX_UINT - 1) {
            dprint(PRN_ERROR, "vps_max_dec_pic_buffering_minus1 overflow!\n");
            return HEVC_DEC_ERR;
        }
        vps->vps_max_dec_pic_buffering[i] = (td_s32)(tmp + 1);
        vps->vps_num_reorder_pics[i] = (td_s32)hevc_ue_v(ctx->bs_ptr, "vps_num_reorder_pics[i]");
        vps->vps_max_latency_increase[i] = (td_s32)hevc_ue_v(ctx->bs_ptr, "vps_max_latency_increase_plus1[i]");

        if (!vps->vps_sub_layer_ordering_info_present_flag) {
            for (i++; i <= vps->vps_max_sub_layers_minus1 - 1; i++) {
                vps->vps_max_dec_pic_buffering[i] = vps->vps_max_dec_pic_buffering[0];
                vps->vps_num_reorder_pics[i] = vps->vps_num_reorder_pics[0];
                vps->vps_max_latency_increase[i] = vps->vps_max_latency_increase[0];
            }
            break;
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_dec_vps_base_layer(const hevc_ctx *ctx, hevc_video_param_set *vps)
{
#ifdef VFMW_MV_HEVC_SUPPORT
    td_u8 vps_base_layer_available_flag;

    vps->vps_base_layer_internal_flag  = hevc_u_v(ctx->bs_ptr, 1, "vps_base_layer_internal_flag");
    vps_base_layer_available_flag = hevc_u_v(ctx->bs_ptr, 1, "vps_base_layer_available_flag");
    if (vps_base_layer_available_flag != 0x1) {
        dprint(PRN_ERROR, "vps_base_layer_available_flag(%d) not equal 0x1.\n", vps_base_layer_available_flag);
    }
#else
    vps->vps_reserved_three_2bits = (td_s32)hevc_u_v(ctx->bs_ptr, 2, "vps_reserved_three_2bits"); /* 2 is bit num */

    if (vps->vps_reserved_three_2bits != 0x3) {
        dprint(PRN_ERROR, "vps_reserved_three_2bits(%d) not equal 0x3.\n", vps->vps_reserved_three_2bits);
    }
#endif
}

STATIC td_s32 hevc_dec_vps_max_layer_id(const hevc_ctx *ctx, hevc_video_param_set *vps)
{
    vps->vps_max_layer_id = (td_s32)hevc_u_v(ctx->bs_ptr, 6, "vps_max_layer_id"); /* 6 is bit num */
    if (vps->vps_max_layer_id < 0 || vps->vps_max_layer_id > (HEVC_MAX_VPS_NUH_RESERVED_ZERO_LAYER_ID_PLUS1 - 1)) {
        dprint(PRN_ERROR, "vps_max_layer_id(%d) out of range(0,%d).\n",
               vps->vps_max_layer_id, (HEVC_MAX_VPS_NUH_RESERVED_ZERO_LAYER_ID_PLUS1 - 1));
        vps->vps_max_layer_id = HEVC_MAX_NUM_LAYERS - 1;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_process_vps(hevc_ctx *ctx, hevc_video_param_set *vps)
{
    td_s32 ret;
    td_s32 vps_max_sub_layers_minus1;

    hevc_dec_vps_base_layer(ctx, vps);

    vps->vps_max_layers_minus1 = (td_s32)hevc_u_v(ctx->bs_ptr, 6, "vps_max_layers_minus1"); /* 6 is bit num */

    if (vps->vps_max_layers_minus1 < 0 || vps->vps_max_layers_minus1 > 63) { /* 63 is vfmw_max layer */
        dprint(PRN_ERROR, "vps_max_layers_minus1 out of range(0,63).\n");
        return HEVC_DEC_ERR;
    }

    vps_max_sub_layers_minus1 = (td_s32)hevc_u_v(ctx->bs_ptr, 3, "vps_max_sub_layers_minus1"); /* 3 is bit num */
    if (vps_max_sub_layers_minus1 < 0 || vps_max_sub_layers_minus1 > (HEVC_MAX_TEMPLAYER - 1)) {
        dprint(PRN_ERROR, "vps_max_sub_layers_minus1=%d\n", vps_max_sub_layers_minus1);
        dprint(PRN_ERROR, "vps_max_sub_layers_minus1 out of range(0,%d).\n", HEVC_MAX_TEMPLAYER);
        return HEVC_DEC_ERR;
    }

    vps->vps_max_sub_layers = vps_max_sub_layers_minus1 + 1;
    vps->vps_max_sub_layers_minus1 = vps_max_sub_layers_minus1 + 1;
    vps->vps_temporal_id_nesting_flag = hevc_u_v(ctx->bs_ptr, 1, "vps_temporal_id_nesting_flag");

    vps->vps_reserved_0xffff_16bits = (td_s32)hevc_u_v(ctx->bs_ptr, 16, "vps_reserved_ffff_16bits"); /* 16 is bit num */

    if (vps->vps_reserved_0xffff_16bits != 0xffff) {
        dprint(PRN_ERROR, "vps_reserved_0xffff_16bits not equal 0xffff.\n");
        return HEVC_DEC_ERR;
    }

    ret = hevc_dec_ptl(ctx, &(vps->profile_tier_level), 1, vps_max_sub_layers_minus1);
    if (ret != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "VPS hevc_dec_ptl error.\n");
        return HEVC_DEC_ERR;
    }

    ret = hevc_process_vps_sub_layers(ctx, vps);
    if (ret != HEVC_DEC_NORMAL) {
        return ret;
    }

    if (hevc_dec_vps_max_layer_id(ctx, vps) != HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

#ifdef VFMW_MV_HEVC_SUPPORT
    hevc_set_multi_layers(ctx, vps);
#endif

    hevc_process_vps_num_layer_sets(ctx, vps);

    ret = hevc_process_vps_timing_info(ctx, vps);
    if (ret != HEVC_DEC_NORMAL) {
        return ret;
    }

    ret = hevc_process_vps_extension(ctx, vps);
    if (ret != HEVC_DEC_NORMAL) {
        return ret;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_vps(hevc_ctx *ctx)
{
    td_u32 vps_video_parameter_set_id;
    hevc_video_param_set *vps_tmp = TD_NULL;

    ctx->vps_update_flag = 1;
    vps_video_parameter_set_id = hevc_u_v(ctx->bs_ptr, 4, "vps_video_parameter_set_id"); /* 4 is bit num */
    if (vps_video_parameter_set_id >= (td_u32)(ctx->max_vps_num)) {
        dprint(PRN_ERROR, "vps->vps_video_parameter_set_id out of range(0,15).\n");
        return HEVC_DEC_ERR;
    }

    if (ctx->vps[vps_video_parameter_set_id] == TD_NULL) {
        vps_tmp = hevc_get_dyn_mem(ctx, sizeof(hevc_video_param_set));
        if (vps_tmp == TD_NULL) {
            dprint(PRN_ERROR, "hevc_get_dyn_mem VPS Fail\n");
            return HEVC_DEC_ERR;
        }

        ctx->vps[vps_video_parameter_set_id] = vps_tmp;
    }

    if (ctx->vps[vps_video_parameter_set_id]->valid) { /* 曾经解对过该id对应的VPS */
        if (ctx->tmp_vps == TD_NULL) {
            ctx->tmp_vps = hevc_get_dyn_mem(ctx, sizeof(hevc_video_param_set));
            if (ctx->tmp_vps == TD_NULL) {
                dprint(PRN_ERROR, "h265 alloc TmpVPS fail\n");
                return HEVC_DEC_ERR;
            }
        }

        ctx->tmp_vps->video_parameter_set_id = (td_s32)vps_video_parameter_set_id;

        if (HEVC_DEC_NORMAL != hevc_process_vps(ctx, ctx->tmp_vps)) {
            dprint(PRN_ERROR, "VPS[%d] decode error0.\n", vps_video_parameter_set_id);
            return HEVC_DEC_ERR;
        }

        ctx->tmp_vps->is_refresh = 1;
        ctx->tmp_vps->valid = 1;

        /* exchange tmp_vps and ctx->vps[vpsid] */
        vps_tmp = ctx->vps[vps_video_parameter_set_id];
        ctx->vps[vps_video_parameter_set_id] = ctx->tmp_vps;
        ctx->tmp_vps = vps_tmp;
    } else {
        ctx->vps[vps_video_parameter_set_id]->video_parameter_set_id = (td_s32)vps_video_parameter_set_id;

        if (HEVC_DEC_NORMAL != hevc_process_vps(ctx, (ctx->vps[vps_video_parameter_set_id]))) {
            dprint(PRN_ERROR, "VPS[%d] decode error1.\n", vps_video_parameter_set_id);
            ctx->vps[vps_video_parameter_set_id]->is_refresh = 1;
            ctx->vps[vps_video_parameter_set_id]->valid = 0;
            return HEVC_DEC_ERR;
        }

        ctx->vps[vps_video_parameter_set_id]->is_refresh = 1;
        ctx->vps[vps_video_parameter_set_id]->valid = 1;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_u8 hevc_check_width_height_valid(hevc_ctx *ctx, hevc_seq_param_set *sps, td_u32 *pic_size)
{
    sps->pic_width_in_luma_samples = hevc_ue_v(ctx->bs_ptr, "pic_width_in_luma_samples");

    sps->pic_height_in_luma_samples = hevc_ue_v(ctx->bs_ptr, "pic_height_in_luma_samples");

    if (sps->pic_width_in_luma_samples < HEVC_MIN_PIX_WIDTH ||
        sps->pic_width_in_luma_samples > HEVC_MAX_PIX_WIDTH) {
        report_unsupport_spec(ctx->chan_id, VFMW_SPEC_RESOLUTION, sps->pic_width_in_luma_samples);
        ctx->spec_invalid = SPEC_WIDTH_INVALID;
        ctx->spec_value = sps->pic_width_in_luma_samples;
        dprint(PRN_ERROR, "report unsupport pic_width_in_luma:%d.\n", sps->pic_width_in_luma_samples);
        return 0;
    }

    if (sps->pic_height_in_luma_samples < HEVC_MIN_PIX_HEIGHT ||
        sps->pic_height_in_luma_samples > HEVC_MAX_PIX_HEIGHT) {
        report_unsupport_spec(ctx->chan_id, VFMW_SPEC_RESOLUTION, sps->pic_height_in_luma_samples);
        ctx->spec_invalid = SPEC_HEIGHT_INVALID;
        ctx->spec_value = sps->pic_height_in_luma_samples;
        dprint(PRN_ERROR, "report unsupport pic_height_in_luma:%d.\n", sps->pic_height_in_luma_samples);
        return 0;
    }

    *pic_size = sps->pic_width_in_luma_samples * sps->pic_height_in_luma_samples;

    return 1;
}

STATIC td_s32 hevc_get_pic_size_by_level_idc(const hevc_seq_param_set *sps)
{
    td_s32 size;

    switch (sps->profile_tier_level.general_level_idc) {
        case 0: /* 0 is idc */
        case 30: /* 30 is idc */
            size = 36864; /* 36864 pic size */
            break;

        case 60: /* 60 is idc */
            size = 122880; /* 122880 pic size */
            break;

        case 63: /* 63 is idc */
            size = 245760; /* 245760 pic size */
            break;

        case 90: /* 90 is idc */
            size = 552960; /* 552960 pic size */
            break;

        case 93: /* 93 is idc */
            size = 983040; /* 983040 pic size */
            break;

        case 120: /* 120 is idc */
        case 123: /* 123 is idc */
            size = 2228224; /* 2228224 pic size */
            break;

        case 150: /* 150 is idc */
        case 153: /* 153 is idc */
        case 156: /* 156 is idc */
            size = 8912896; /* 8912896 pic size */
            break;

        case 180: /* 180 is idc */
        case 183: /* 183 is idc */
        case 186: /* 186 is idc */
            size = 35651584; /* 35651584 pic size */
            break;

        default:
            dprint(PRN_ERROR, "ERROR Unkown level: %d\n", sps->profile_tier_level.general_level_idc);
            return HEVC_LEVEL_ERR;
    }

    return size;
}

STATIC td_u32 hevc_get_dpb_size(td_u32 pic_size_from_resolution, td_u32 pic_size_from_level_idc)
{
    td_u32 dpb_size;

    if (pic_size_from_resolution <= (pic_size_from_level_idc / 4)) { /* 4 is num */
        dpb_size = vfmw_min(4 * HEVC_MAX_DPB_PIC_BUF, 16); /* 4 16 is num */
    } else if (pic_size_from_resolution <= (pic_size_from_level_idc / 2)) { /* 2 is num */
        dpb_size = vfmw_min(2 * HEVC_MAX_DPB_PIC_BUF, 16); /* 2 16 is num */
    } else if (pic_size_from_resolution <= ((pic_size_from_level_idc * 3) / 4)) { /* 3 4 is num */
        dpb_size = vfmw_min((4 * HEVC_MAX_DPB_PIC_BUF) / 3, 16); /* 4 3 16 is num */
    } else {
        dpb_size = HEVC_MAX_DPB_PIC_BUF;
    }

    return dpb_size;
}

STATIC td_u8 hevc_check_bit_depth_luma(hevc_ctx *ctx, hevc_seq_param_set *sps)
{
    td_u32 bit_depth_luma;
    td_u32 bit_depth_luma_valid = 1;

    bit_depth_luma = hevc_ue_v(ctx->bs_ptr, "bit_depth_luma_minus8");
    if (bit_depth_luma > HEVC_MAX_UINT - 8) { /* 8 is base */
        dprint(PRN_ERROR, "bit_depth_luma_minus8 overflow!\n");
        return 0;
    }

    bit_depth_luma += 8; /* 8 is base */

    if (bit_depth_luma < 8 || bit_depth_luma > 14) { /* 8 14 is bit num */
        dprint(PRN_ERROR, "report unsupport bit_depth_luma(%d).\n", bit_depth_luma);
        report_unsupport(ctx->chan_id);
        ctx->spec_invalid = SPEC_BIT_INVALID;
        ctx->spec_value = bit_depth_luma;
        bit_depth_luma_valid = 0;
    } else if (bit_depth_luma > HEVC_MAX_BIT_DEPTH) {
        dprint(PRN_ERROR, "report unsupport bit_depth_luma(%d).\n", bit_depth_luma);
        report_unsupport(ctx->chan_id);
        ctx->spec_invalid = SPEC_BIT_INVALID;
        ctx->spec_value = bit_depth_luma;
        bit_depth_luma = HEVC_MAX_BIT_DEPTH;
    }

    sps->bit_depth_luma = bit_depth_luma;

    return bit_depth_luma_valid;
}

STATIC td_u8 hevc_check_bit_depth_chroma(hevc_ctx *ctx, hevc_seq_param_set *sps)
{
    td_u32 bit_depth_chroma;
    td_u8 bit_depth_chroma_valid = 1;

    bit_depth_chroma = hevc_ue_v(ctx->bs_ptr, "bit_depth_luma_minus8");
    if (bit_depth_chroma > HEVC_MAX_UINT - 8) { /* 8 is base */
        dprint(PRN_ERROR, "bit_depth_chroma_minus8 overflow!\n");
        return 0;
    }

    bit_depth_chroma += 8; /* 8 is base */
    if (bit_depth_chroma < 8 || bit_depth_chroma > 14) { /* 8 14 is bit num */
        dprint(PRN_ERROR, "report unsupport bit_depth_chroma(%d).\n", bit_depth_chroma);
        report_unsupport(ctx->chan_id);
        ctx->spec_invalid = SPEC_BIT_INVALID;
        ctx->spec_value = bit_depth_chroma;
        bit_depth_chroma_valid = 0;
    } else if (bit_depth_chroma > HEVC_MAX_BIT_DEPTH) {
        dprint(PRN_ERROR, "report unsupport bit_depth_chroma(%d).\n", bit_depth_chroma);
        report_unsupport(ctx->chan_id);
        ctx->spec_invalid = SPEC_BIT_INVALID;
        ctx->spec_value = bit_depth_chroma;
        bit_depth_chroma = HEVC_MAX_BIT_DEPTH;
    }

    sps->bit_depth_chroma = bit_depth_chroma;

    return bit_depth_chroma_valid;
}

STATIC td_s32 hevc_process_sps_conform_window(const hevc_ctx *ctx, hevc_seq_param_set *sps)
{
    td_s32 temp_data;

    sps->conformance_window_flag = hevc_u_v(ctx->bs_ptr, 1, "conformance_window_flag");

    if (sps->conformance_window_flag) {
        sps->conf_win_left_offset = hevc_ue_v(ctx->bs_ptr, "conf_win_left_offset");
        vfmw_check_ret(sps->conf_win_left_offset < VFMW_MAX_UINT32, HEVC_DEC_ERR);
        vfmw_check_ret(g_crop_unit_x[sps->chroma_format_idc] < VFMW_MAX_SINT32, HEVC_DEC_ERR);
        sps->conf_win_left_offset = sps->conf_win_left_offset * g_crop_unit_x[sps->chroma_format_idc];
        sps->conf_win_right_offset = hevc_ue_v(ctx->bs_ptr, "conf_win_right_offset");
        vfmw_check_ret(sps->conf_win_right_offset < VFMW_MAX_UINT32, HEVC_DEC_ERR);
        sps->conf_win_right_offset = sps->conf_win_right_offset * g_crop_unit_x[sps->chroma_format_idc];

        temp_data = (td_s32)(sps->pic_width_in_luma_samples - sps->conf_win_left_offset - sps->conf_win_right_offset);
        if (temp_data < 0) {
            dprint(PRN_ERROR, "conf_win_left_offset+conf_win_right_offset out of range.\n");
            return HEVC_DEC_ERR;
        }

        sps->conf_win_top_offset = hevc_ue_v(ctx->bs_ptr, "conf_win_top_offset");
        vfmw_check_ret(sps->conf_win_top_offset < VFMW_MAX_UINT32, HEVC_DEC_ERR);
        vfmw_check_ret(g_crop_unit_y[sps->chroma_format_idc] < VFMW_MAX_SINT32, HEVC_DEC_ERR);
        sps->conf_win_top_offset = sps->conf_win_top_offset * g_crop_unit_y[sps->chroma_format_idc];
        sps->conf_win_bottom_offset = hevc_ue_v(ctx->bs_ptr, "conf_win_bottom_offset");
        vfmw_check_ret(sps->conf_win_bottom_offset < VFMW_MAX_UINT32, HEVC_DEC_ERR);
        sps->conf_win_bottom_offset = sps->conf_win_bottom_offset * g_crop_unit_y[sps->chroma_format_idc];

        temp_data = (td_s32)(sps->pic_height_in_luma_samples - sps->conf_win_top_offset -
                             sps->conf_win_bottom_offset);
        vfmw_check_ret_print(!(temp_data < 0), HEVC_DEC_ERR,
                             "conf_win_top_offset+conf_win_bottom_offset out of range.\n");
    } else {
        sps->conf_win_left_offset = 0;
        sps->conf_win_right_offset = 0;
        sps->conf_win_top_offset = 0;
        sps->conf_win_bottom_offset = 0;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_process_sps_sub_layer(const hevc_ctx *ctx,
    hevc_seq_param_set *sps, td_u32 *max_dec_pic_buffering)
{
    td_u32 i;

    sps->sps_sub_layer_ordering_info_present_flag = hevc_u_v(ctx->bs_ptr, 1, "ordering_info_present_flag");
    sps->reorder_num = 0;
    for (i = 0; i <= sps->sps_max_sub_layers_minus1; i++) {
        sps->max_dec_pic_buffering[i] = hevc_ue_v(ctx->bs_ptr, "sps_max_dec_pic_buffering_minus1");
        if (sps->max_dec_pic_buffering[i] > VFMW_MAX_UINT32 - 1) {
            return HEVC_DEC_ERR;
        }
        sps->max_dec_pic_buffering[i] += 1;
        sps->num_reorder_pics[i] = hevc_ue_v(ctx->bs_ptr, "sps_num_reorder_pics");
        sps->reorder_num = (sps->num_reorder_pics[i] > sps->reorder_num) ?
            sps->num_reorder_pics[i] : sps->reorder_num;
        *max_dec_pic_buffering = vfmw_max(sps->max_dec_pic_buffering[i], sps->max_dec_pic_buffering[0]);
        *max_dec_pic_buffering = vfmw_max(*max_dec_pic_buffering, sps->num_reorder_pics[i]);

        /* sps->dpb_size = vfmw_min(sps->dpb_size, sps->max_dec_pic_buffering[i]);   不用加上num_reorder_pics */
        sps->max_latency_increase[i] = hevc_ue_v(ctx->bs_ptr, "sps_max_latency_increase_plus1");

        if (!sps->sps_sub_layer_ordering_info_present_flag) {
            for (i++; i <= sps->sps_max_sub_layers_minus1; i++) {
                sps->max_dec_pic_buffering[i] = sps->max_dec_pic_buffering[0];
                sps->num_reorder_pics[i] = sps->num_reorder_pics[0];
                sps->max_latency_increase[i] = sps->max_latency_increase[0];
            }
            break;
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_process_sps_dpb_size(hevc_ctx *ctx, hevc_seq_param_set *sps)
{
    td_u32 max_dec_pic_buffering = 0;
    td_u32 pic_width_in_smp, pic_height_in_smp;
    td_s32 max_ref_num, max_width, max_height;
    td_u32 temp_dpb_size = 0;

#ifdef VFMW_MV_HEVC_SUPPORT
    if (hevc_get_multi_layer_ext_sps_flag(ctx, sps) == 1) {
        hevc_derive_ext_dpb_size(ctx, sps);
    } else {
#endif
        if (hevc_process_sps_sub_layer(ctx, sps, &max_dec_pic_buffering) != HEVC_DEC_NORMAL) {
            return HEVC_DEC_ERR;
        }

        if (max_dec_pic_buffering == (sps->dpb_size + 1)) {
            temp_dpb_size = max_dec_pic_buffering;
        } else {
            temp_dpb_size = vfmw_min(sps->dpb_size, max_dec_pic_buffering);
        }
        sps->dpb_size = vfmw_min(temp_dpb_size, 16); /* 16 is ref num */
#ifdef VFMW_MV_HEVC_SUPPORT
    }
#endif
    ctx->true_dpb_size = (td_s32)sps->dpb_size;

    max_ref_num = ctx->extra_data_ptr->max_ref_num;
    max_width = ctx->extra_data_ptr->max_width;
    max_height = ctx->extra_data_ptr->max_height;

    pic_width_in_smp = sps->pic_width_in_luma_samples;
    pic_height_in_smp = sps->pic_height_in_luma_samples;
    if (sps->dpb_size > (td_u32)(max_ref_num + 1)) {
        /* 此处直接返回error可以避免后续码流如果分辨率超大或者参考帧个数过多时重复初始化DPB导致不必要的麻烦 */
        return HEVC_DEC_ERR;
    }

    if ((pic_width_in_smp * pic_height_in_smp) > ((td_u32)max_width * (td_u32)max_height)) {
        /* 此处直接返回error可以避免后续码流如果分辨率超大或者参考帧个数过多时重复初始化DPB导致不必要的麻烦 */
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_check_sps_valid(const hevc_ctx *ctx, hevc_seq_param_set *sps)
{
    td_u8 flag;

    sps->log2_min_luma_coding_block_size_minus3 = hevc_ue_v(ctx->bs_ptr, "log2_min_coding_block_size_minus3");

    flag = (sps->log2_min_luma_coding_block_size_minus3 > 3); /* 3 is vfmw_max size */
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "log2_min_luma_coding_block_size_minus3 out of range(0,3).\n");

    sps->log2_diff_max_min_luma_coding_block_size = hevc_ue_v(ctx->bs_ptr, "log2_diff_max_min_coding_block_size");

    flag = (sps->log2_diff_max_min_luma_coding_block_size > 3); /* 3 is vfmw_max size */
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "log2_diff_max_min_luma_coding_block_size out of range(0,3).\n");

    sps->log2_min_cb_sizey = sps->log2_min_luma_coding_block_size_minus3 + 3; /* 3 is num */
    sps->log2_ctb_sizey = sps->log2_min_cb_sizey + sps->log2_diff_max_min_luma_coding_block_size;

    flag = (sps->log2_ctb_sizey < 4 || sps->log2_ctb_sizey > 6); /* 4 6 is range */
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "log2_ctb_sizey out of range(4,6).\n");

    sps->min_cb_sizey = 1 << sps->log2_min_cb_sizey;
    sps->ctb_sizey = 1 << sps->log2_ctb_sizey;
    sps->max_cu_width = 1 << sps->log2_ctb_sizey;
    sps->max_cu_height = 1 << sps->log2_ctb_sizey;
    sps->ctb_num_width = (sps->pic_width_in_luma_samples % sps->max_cu_width) ?
                         (sps->pic_width_in_luma_samples / sps->max_cu_width + 1) :
                         (sps->pic_width_in_luma_samples / sps->max_cu_width);
    sps->ctb_num_height = (sps->pic_height_in_luma_samples % sps->max_cu_height) ?
                          (sps->pic_height_in_luma_samples / sps->max_cu_height + 1) :
                          (sps->pic_height_in_luma_samples / sps->max_cu_height);

    sps->log2_min_transform_block_size_minus2 = hevc_ue_v(ctx->bs_ptr, "log2_min_transform_block_size_minus2");

    flag = (sps->log2_min_transform_block_size_minus2 > 3); /* 3 is vfmw_max size */
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "log2_min_transform_block_size_minus2 out of range(0,3).\n");

    sps->log2_diff_max_min_transform_block_size = hevc_ue_v(ctx->bs_ptr, "log2_diff_max_min_transform_block_size");

    flag = (sps->log2_diff_max_min_transform_block_size > 3); /* 3 is vfmw_max size */
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "log2_diff_max_min_transform_block_size out of range(0,3).\n");

    sps->quadtree_tu_log2_min_size = sps->log2_min_transform_block_size_minus2 + 2; /* 2 is num */

    flag = (sps->quadtree_tu_log2_min_size >= sps->log2_min_cb_sizey);
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "quadtree_tu_log2_min_size not less than log2_min_cb_sizey.\n");

    sps->quadtree_tu_log2_max_size = sps->quadtree_tu_log2_min_size + sps->log2_diff_max_min_transform_block_size;

    flag = (sps->quadtree_tu_log2_max_size > 5 || /* 5 is vfmw_max size */
            sps->quadtree_tu_log2_max_size > sps->log2_ctb_sizey);
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "quadtree_tu_log2_max_size greater than Min( CtbLog2SizeY, 5 ).\n");

    sps->max_transform_hierarchy_depth_inter = hevc_ue_v(ctx->bs_ptr, "max_transform_hierarchy_depth_inter");

    flag = (sps->max_transform_hierarchy_depth_inter > sps->log2_ctb_sizey - sps->quadtree_tu_log2_min_size);
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR,
                         "max_transform_hierarchy_depth_inter out of range(0,CtbLog2SizeY-Log2MinTrafoSize).\n");

    sps->max_transform_hierarchy_depth_intra = hevc_ue_v(ctx->bs_ptr, "max_transform_hierarchy_depth_intra");

    flag = (sps->max_transform_hierarchy_depth_intra > sps->log2_ctb_sizey - sps->quadtree_tu_log2_min_size);
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR,
                         "max_transform_hierarchy_depth_intra out of range(0,CtbLog2SizeY-Log2MinTrafoSize).\n");

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_process_sps_pcm_enable(const hevc_ctx *ctx, hevc_seq_param_set *sps)
{
    td_u8 flag;

    sps->pcm_enabled_flag = hevc_u_v(ctx->bs_ptr, 1, "pcm_enabled_flag");

    if (sps->pcm_enabled_flag) {
        sps->pcm_bit_depth_luma = hevc_u_v(ctx->bs_ptr, 4, "bit_depth_minus1") + 1; /* 4 is bit num */
        sps->pcm_bit_depth_chroma = hevc_u_v(ctx->bs_ptr, 4, "bit_depth_minus1") + 1; /* 4 is bit num */

        sps->log2_min_pcm_coding_block_size_minus3 =
            hevc_ue_v(ctx->bs_ptr, "log2_min_pcm_luma_coding_block_size_minus3");

        if (sps->log2_min_pcm_coding_block_size_minus3 > 2) { /* 2 is vfmw_max val */
            dprint(PRN_ERROR, "log2_min_pcm_coding_block_size_minus3(%d) out of range[0,2].\n",
                   sps->log2_min_pcm_coding_block_size_minus3);
        }

        sps->log2_diff_max_min_pcm_coding_block_size =
            hevc_ue_v(ctx->bs_ptr, "log2_diff_max_min_pcm_luma_coding_block_size");

        if (sps->log2_diff_max_min_pcm_coding_block_size > 2) { /* 2 is vfmw_max val */
            dprint(PRN_ERROR, "log2_diff_max_min_pcm_coding_block_size(%d) out of range[0,2].\n",
                   sps->log2_diff_max_min_pcm_coding_block_size);
        }

        if (sps->pcm_log2_min_size > (VFMW_MAX_UINT32 - sps->log2_diff_max_min_pcm_coding_block_size) ||
            sps->log2_min_pcm_coding_block_size_minus3 > (VFMW_MAX_UINT32 - 3)) { /* 3 is a number */
            return HEVC_DEC_ERR;
        }

        sps->pcm_log2_min_size = sps->log2_min_pcm_coding_block_size_minus3 + 3; /* 3 is num */
        sps->pcm_log2_max_size = sps->pcm_log2_min_size + sps->log2_diff_max_min_pcm_coding_block_size;

        flag = (sps->pcm_log2_max_size > 5 || sps->pcm_log2_max_size > sps->log2_ctb_sizey); /* 5 is vfmw_max num */
        vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "pcm_log2_max_size greater than Min( CtbLog2SizeY, 5 ).\n");

        sps->pcm_loop_filter_disable_flag = hevc_u_v(ctx->bs_ptr, 1, "pcm_loop_filter_disable_flag");
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_process_sps_ref_set(const hevc_ctx *ctx, hevc_seq_param_set *sps)
{
    td_u32 i;
    td_u8 flag;
    td_s32 ret;

    sps->num_short_term_ref_pic_sets = hevc_ue_v(ctx->bs_ptr, "num_short_term_ref_pic_sets");

    flag = (sps->num_short_term_ref_pic_sets > HEVC_MAX_STRPS_NUM);
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "num_short_term_ref_pic_sets out of range[0,%d].\n", HEVC_MAX_STRPS_NUM);

    for (i = 0; i < sps->num_short_term_ref_pic_sets; i++) {
        /* get short term reference picture sets */
        ret = hevc_dec_short_term_ref_pic_set(ctx, sps, &(sps->short_term_ref_pic_set[i]), i);
        vfmw_check_ret_print((ret == HEVC_DEC_NORMAL), HEVC_DEC_ERR, "SPS hevc_dec_short_term_ref_pic_set error.\n");
    }

    sps->long_term_ref_pics_present_flag = hevc_u_v(ctx->bs_ptr, 1, "long_term_ref_pics_present_flag");

    if (sps->long_term_ref_pics_present_flag) {
        sps->num_long_term_ref_pic_sps = hevc_ue_v(ctx->bs_ptr, "num_long_term_ref_pic_sps");

        flag = (sps->num_long_term_ref_pic_sps > HEVC_MAX_LSB_NUM - 1);
        vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "num_long_term_ref_pic_sps out of range[0,32].\n");

        for (i = 0; i < sps->num_long_term_ref_pic_sps; i++) {
            sps->lt_ref_pic_poc_lsb_sps[i] =
                hevc_u_v(ctx->bs_ptr, sps->max_pic_order_cnt_lsb, "lt_ref_pic_poc_lsb_sps");
            sps->used_by_curr_pic_lt_sps_flag[i] = hevc_u_v(ctx->bs_ptr, 1, "used_by_curr_pic_lt_sps_flag");
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_process_sps_scaling_and_adapt(hevc_ctx *ctx, hevc_seq_param_set *sps)
{
    td_s32 ret;

    sps->scaling_list_enabled_flag = hevc_u_v(ctx->bs_ptr, 1, "scaling_list_enabled_flag");

    if (sps->scaling_list_enabled_flag) {
#ifdef VFMW_MV_HEVC_SUPPORT
        if (hevc_get_multi_layer_ext_sps_flag(ctx, sps) == 1) {
            sps->sps_extension.sps_infer_scaling_list_flag = hevc_u_v(ctx->bs_ptr, 1, "sps_infer_scaling_list_flag");
        } else {
            sps->sps_extension.sps_infer_scaling_list_flag = 0;
        }

        if (sps->sps_extension.sps_infer_scaling_list_flag) {
            sps->sps_extension.sps_scaling_list_ref_layer_id =
                hevc_u_v(ctx->bs_ptr, 6, "sps_scaling_list_ref_layer_id"); /* 6 protocal required */
            sps->sps_scaling_list_data_present_flag = 0;
        } else
#endif
        {
            sps->sps_scaling_list_data_present_flag = hevc_u_v(ctx->bs_ptr, 1, "sps_scaling_list_data_present_flag");
        }

        if (sps->sps_scaling_list_data_present_flag) {
            ret = hevc_dec_scaling_list_data(ctx, &(sps->scaling_list));
            vfmw_check_ret_print((ret == HEVC_DEC_NORMAL), HEVC_DEC_ERR, "SPS hevc_dec_scaling_list_data error.\n");
        }
    }

    sps->amp_enabled_flag = hevc_u_v(ctx->bs_ptr, 1, "amp_enabled_flag");
    sps->sample_adaptive_offset_enabled_flag = hevc_u_v(ctx->bs_ptr, 1, "sample_adaptive_offset_enabled_flag");

    /* 逻辑不支持图像高度64以下没有sao的码流 */
    if (sps->sample_adaptive_offset_enabled_flag == 0 && sps->pic_height_in_luma_samples <= 64) { /* 64 is min wid */
        dprint(PRN_ERROR, "SPS sao is off and pic height(%d) is too small.(Logic report unsupport!).\n",
               sps->pic_height_in_luma_samples);
        ctx->spec_invalid = SPEC_LOGIC_UNSUPPORT;
        report_unsupport(ctx->chan_id);
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_process_sps_depth(hevc_ctx *ctx, hevc_seq_param_set *sps)
{
    sps->quadtree_tu_max_depth_inter = sps->max_transform_hierarchy_depth_inter + 1;
    sps->quadtree_tu_max_depth_intra = sps->max_transform_hierarchy_depth_intra + 1;

    ctx->add_cu_depth = 0;

    while (((td_u32)(sps->max_cu_width >> sps->log2_diff_max_min_luma_coding_block_size))
           >  (td_u32)(1 << (sps->quadtree_tu_log2_min_size + ctx->add_cu_depth))) {
        ctx->add_cu_depth++;
    }

    sps->max_cu_depth = sps->log2_diff_max_min_luma_coding_block_size + ctx->add_cu_depth;

    /* BB: these parameters may be removed completly and replaced by the fixed values */
    sps->min_tr_depth = 0;
    sps->max_tr_depth = 1;

    return;
}

STATIC td_void hevc_parse_sps_range_extension(const hevc_ctx *ctx)
{
    hevc_u_v(ctx->bs_ptr, 1, "transform_skip_context_enabled_flag");
    hevc_u_v(ctx->bs_ptr, 1, "implicit_rdpcm_enabled_flag");
    hevc_u_v(ctx->bs_ptr, 1, "explicit_rdpcm_enabled_flag");
    hevc_u_v(ctx->bs_ptr, 1, "extended_precision_processing_flag");
    hevc_u_v(ctx->bs_ptr, 1, "intra_smoothing_disabled_flag");
    hevc_u_v(ctx->bs_ptr, 1, "high_precision_offsets_enabled_flag");
    hevc_u_v(ctx->bs_ptr, 1, "persistent_rice_adaptation_enabled_flag");
    hevc_u_v(ctx->bs_ptr, 1, "cabac_bypass_alignment_enabled_flag");
}

STATIC td_void hevc_parse_sps_extension(const hevc_ctx *ctx, hevc_seq_param_set *sps)
{
    td_u8 sps_extension_present_flag, sps_range_extensions_flag;
    td_u8 sps_multilayer_extension_flag;
    td_u8 sps_extension_bit, sps_air_extension_flag;
    td_u32 i;

    sps_extension_present_flag = hevc_u_v(ctx->bs_ptr, 1, "sps_extension_present_flag");
    if (sps_extension_present_flag == 0) {
#ifdef HEVC_AIR_SUPPORT
        if (ctx->extra_data_ptr->air_dec_attr.fixed_airdec == 1) {
            sps->air_parameters.air_version = ctx->extra_data_ptr->air_version;
            sps->air_parameters.air_slice_num = ctx->extra_data_ptr->air_dec_attr.slice_num;
            sps->air_parameters.air_layer_num = ctx->extra_data_ptr->air_dec_attr.layer_num;
            sps->air_parameters.air_big_width = ctx->extra_data_ptr->air_dec_attr.max_width;
            sps->air_parameters.air_big_height = ctx->extra_data_ptr->air_dec_attr.max_height;
        }
#endif
        return;
    }

    sps_range_extensions_flag = hevc_u_v(ctx->bs_ptr, 1, "sps_range_extension_flag");
    sps_multilayer_extension_flag = hevc_u_v(ctx->bs_ptr, 1, "sps_multilayer_extension_flag");
    hevc_u_v(ctx->bs_ptr, 1, "sps_3d_extension_flag");
    hevc_u_v(ctx->bs_ptr, 1, "sps_scc_extension_bit");
    sps_extension_bit = hevc_u_v(ctx->bs_ptr, 1, "sps_extension_bit");
    sps_extension_bit = hevc_u_v(ctx->bs_ptr, 1, "sps_extension_bit");
    sps_extension_bit = hevc_u_v(ctx->bs_ptr, 1, "sps_extension_bit");

    sps_air_extension_flag = hevc_u_v(ctx->bs_ptr, 1, "sps_air_extension_flag");

    if (sps_range_extensions_flag) {
        hevc_parse_sps_range_extension(ctx);
    }

    if (sps_multilayer_extension_flag) {
        hevc_u_v(ctx->bs_ptr, 1, "inter_view_mv_vert_constraint_flag");
    }

    if (sps_air_extension_flag && ctx->extra_data_ptr->air_version != AIR_DEC_UNSUPPORT) {
        sps->air_parameters.air_version = hevc_ue_v(ctx->bs_ptr, "air_version");
        sps->air_parameters.air_slice_num = hevc_ue_v(ctx->bs_ptr, "air_slice_num");
        sps->air_parameters.air_layer_num = hevc_ue_v(ctx->bs_ptr, "air_layer_num");
        sps->air_parameters.air_big_width = hevc_ue_v(ctx->bs_ptr, "air_original_width");
        sps->air_parameters.air_big_height = hevc_ue_v(ctx->bs_ptr, "air_original_height");
        vfmw_check(sps->air_parameters.air_slice_num < 32); /* 32 is shift num */
        for (i = 0; i < sps->air_parameters.air_slice_num; i++) {
            sps->air_parameters.air_integrality |= (1 << i);
        }
    }

    if (sps_extension_bit) {
        while (hevc_more_rbsp_data(ctx) == HEVC_TRUE) {
            hevc_u_v(ctx->bs_ptr, 1, "sps_extension_data_flag");
        }
    }

    return;
}

STATIC td_s32 hevc_process_sps_last_part(hevc_ctx *ctx, hevc_seq_param_set *sps)
{
    td_s32 ret;
    td_s32 img_width, img_height;

    sps->sps_temporal_mvp_enable_flag = hevc_u_v(ctx->bs_ptr, 1, "sps_temporal_mvp_enable_flag");
    sps->sps_strong_intra_smoothing_enable_flag = hevc_u_v(ctx->bs_ptr, 1, "sps_strong_intra_smoothing_enable_flag");
    sps->vui_parameters_present_flag = hevc_u_v(ctx->bs_ptr, 1, "vui_parameters_present_flag");

    vfmw_check_sec_func(memset_s(&sps->vui_parameters, sizeof(hevc_vui_param_set), 0, sizeof(hevc_vui_param_set)));
    sps->vui_parameters.matrix_coeffs = 2; /* 2 means unspicified for ctx test */
    if (sps->vui_parameters_present_flag) {
        ret = hevc_dec_vui_param(ctx, &(sps->vui_parameters), sps);
        vfmw_check_ret_print((ret == HEVC_DEC_NORMAL), HEVC_DEC_ERR, "SPS hevc_dec_vui_param error.\n");
    }

    img_width = (td_s32)sps->pic_width_in_luma_samples;

    vfmw_check_ret(sps->pic_height_in_luma_samples <= HEVC_MAX_PIX_HEIGHT, HEVC_DEC_ERR);
    vfmw_check_ret(sps->vui_parameters.field_seq_flag <= 1, HEVC_DEC_ERR);

    img_height = (td_s32)(sps->pic_height_in_luma_samples * (1 + sps->vui_parameters.field_seq_flag));
    sps->vui_parameters.aspect_ratio = (td_u32)hevc_get_dar(sps->vui_parameters.aspect_ratio_idc,
        sps->vui_parameters.sar_width, sps->vui_parameters.sar_height, img_width, img_height);

    hevc_parse_sps_extension(ctx, sps);

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_parse_sps_frame_base_info(hevc_ctx *ctx, hevc_seq_param_set *sps)
{
    td_s32 ret, size;
    td_u32 pic_size;
    td_u8 width_height_valid, bit_depth_luma_valid, bit_depth_chroma_valid;

    width_height_valid = bit_depth_luma_valid = bit_depth_chroma_valid = 1;
    if (hevc_get_multi_layer_ext_sps_flag(ctx, sps) == 1)  {
#ifdef VFMW_MV_HEVC_SUPPORT
        ret = hevc_derive_sps_rep_format(ctx, sps);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "hevc_derive_sps_rep_format failed.\n");
            return HEVC_DEC_ERR;
        }
#endif
    } else {
        sps->chroma_format_idc = hevc_ue_v(ctx->bs_ptr, "chroma_format_idc");
        if ((sps->chroma_format_idc != 1) && (sps->chroma_format_idc != 0)) {
            dprint(PRN_ERROR, "report unsupport chroma_format_idc(%d).", sps->chroma_format_idc);
            ctx->spec_invalid = SPEC_PIX_FMT_INVALID;
            ctx->spec_value = sps->chroma_format_idc;
            report_unsupport(ctx->chan_id);
            return HEVC_DEC_ERR;
        }

        width_height_valid = hevc_check_width_height_valid(ctx, sps, &pic_size);
        vfmw_check_ret(width_height_valid != 0, HEVC_DEC_ERR);

        size = hevc_get_pic_size_by_level_idc(sps);
        vfmw_check_ret(size != HEVC_LEVEL_ERR, HEVC_LEVEL_ERR);

        sps->dpb_size = hevc_get_dpb_size(pic_size, size);

        ret = hevc_process_sps_conform_window(ctx, sps);
        vfmw_check_ret(ret != HEVC_DEC_ERR, HEVC_DEC_ERR);

        bit_depth_luma_valid = hevc_check_bit_depth_luma(ctx, sps);
        vfmw_check_ret(bit_depth_luma_valid != 0, HEVC_DEC_ERR);

        bit_depth_chroma_valid = hevc_check_bit_depth_chroma(ctx, sps);
        vfmw_check_ret(bit_depth_chroma_valid != 0, HEVC_DEC_ERR);
    }

    ctx->bit_depth_y = sps->bit_depth_luma;
    ctx->bit_depth_c = sps->bit_depth_chroma;

    vfmw_check_ret(sps->bit_depth_luma < VFMW_MAX_UINT32, HEVC_DEC_ERR);
    vfmw_check_ret(sps->bit_depth_chroma < VFMW_MAX_UINT32, HEVC_DEC_ERR);

    sps->qp_bdoffset_y = (sps->bit_depth_luma - 8) * 6; /* 8 6 is num */
    sps->qp_bdoffset_c = (sps->bit_depth_chroma - 8) * 6; /* 8 6 is num */

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_process_sps(hevc_ctx *ctx, hevc_seq_param_set *sps)
{
    td_s32 ret;
    td_u32 log2_max_pic_order_cnt_lsb_minus4;
    td_u8 flag;

    /* in the first version we only support chroma_format_idc equal to 1 (4:2:0),
    so separate_colour_plane_flag cannot appear in the bitstream */
    if (hevc_parse_sps_frame_base_info(ctx, sps) != HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

    log2_max_pic_order_cnt_lsb_minus4 = hevc_ue_v(ctx->bs_ptr, "log2_max_pic_order_cnt_lsb_minus4");

    flag = (log2_max_pic_order_cnt_lsb_minus4 > 12); /* 12 is vfmw_max val */
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "log2_max_pic_order_cnt_lsb_minus4 out of range[0,12].\n");

    sps->max_pic_order_cnt_lsb = log2_max_pic_order_cnt_lsb_minus4 + 4; /* 4 is num */
    sps->bits_for_poc = sps->max_pic_order_cnt_lsb;

    ret = hevc_process_sps_dpb_size(ctx, sps);
    vfmw_check_ret(ret != HEVC_DEC_ERR, HEVC_DEC_ERR);

    ret = hevc_check_sps_valid(ctx, sps);
    vfmw_check_ret(ret != HEVC_DEC_ERR, HEVC_DEC_ERR);

    hevc_process_sps_depth(ctx, sps);

    ret = hevc_process_sps_scaling_and_adapt(ctx, sps);
    vfmw_check_ret(ret != HEVC_DEC_ERR, HEVC_DEC_ERR);

    ret = hevc_process_sps_pcm_enable(ctx, sps);
    vfmw_check_ret(ret != HEVC_DEC_ERR, HEVC_DEC_ERR);

    ret = hevc_process_sps_ref_set(ctx, sps);
    vfmw_check_ret(ret != HEVC_DEC_ERR, HEVC_DEC_ERR);

    ret = hevc_process_sps_last_part(ctx, sps);
    vfmw_check_ret(ret != HEVC_DEC_ERR, HEVC_DEC_ERR);

    sps->is_refresh = 1;

    /* non air_dec stream */
    if (sps->air_parameters.air_version == 0) {
        ctx->extra_data_ptr->air_version = 0;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_refresh_sps(hevc_seq_param_set *sps, td_u32 vps_id, td_u32 sps_sub_layers,
    td_u8 sps_tmp_id_flag, td_u32 sps_id, td_u32 sps_ext)
{
    sps->video_parameter_set_id = vps_id;
    sps->sps_max_sub_layers_minus1 = sps_sub_layers;
    sps->sps_temporal_id_nesting_flag = sps_tmp_id_flag;
    sps->seq_parameter_set_id = sps_id;
#ifdef VFMW_MV_HEVC_SUPPORT
    sps->sps_ext_or_max_sub_layers_minus1 = sps_ext;
#else
    vfmw_unused(sps_ext);
#endif
}

STATIC td_s32 hevc_dec_sps_process(hevc_ctx *ctx, td_u32 vps_id, td_u32 sps_sub_layers,
                                   td_u8 sps_tmp_id_flag, td_u32 sps_id, td_u32 sps_ext)
{
    hevc_seq_param_set *sps_tmp = TD_NULL;

    if (ctx->sps[sps_id] == TD_NULL) {
        sps_tmp = hevc_get_dyn_mem(ctx, sizeof(hevc_seq_param_set));
        if (sps_tmp == TD_NULL) {
            dprint(PRN_ERROR, "hevc_get_dyn_mem SPS Fail\n");
            return HEVC_DEC_ERR;
        }

        ctx->sps[sps_id] = sps_tmp;
    }

    if (ctx->sps[sps_id]->valid) { /* 曾经解对过该id对应的SPS */
        hevc_refresh_sps(ctx->tmp_sps, vps_id, sps_sub_layers, sps_tmp_id_flag, sps_id, sps_ext);
        if (hevc_process_sps(ctx, ctx->tmp_sps) != HEVC_DEC_NORMAL) {
            ctx->sei_sps = ctx->sps[sps_id];
            dprint(PRN_ERROR, "SPS[%d] decode error0.\n", sps_id);
            return HEVC_DEC_ERR;
        }

        ctx->tmp_sps->is_refresh = 1;
        ctx->tmp_sps->valid = 1;
        ctx->sei_sps = ctx->tmp_sps;

        sps_tmp = ctx->sps[sps_id];
        ctx->sps[sps_id] = ctx->tmp_sps;
        ctx->tmp_sps = sps_tmp;
    } else {
        sps_tmp = ctx->sps[sps_id];
        hevc_refresh_sps(sps_tmp, vps_id, sps_sub_layers, sps_tmp_id_flag, sps_id, sps_ext);
        vfmw_check_sec_func(memcpy_s(&(sps_tmp->profile_tier_level), sizeof(sps_tmp->profile_tier_level),
                                     &(ctx->tmp_sps->profile_tier_level), sizeof(hevc_profile_tier_level)));

        if (hevc_process_sps(ctx, sps_tmp) != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "SPS[%d] decode error.\n", sps_id);
            sps_tmp->is_refresh = 1;
            sps_tmp->valid = 0;
            ctx->sei_sps = sps_tmp;
            return HEVC_DEC_ERR;
        }

        sps_tmp->is_refresh = 1;
        sps_tmp->valid = 1;
        ctx->sei_sps = sps_tmp;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_get_sps_max_sub_layers(const hevc_ctx *ctx, td_u32 *sps_max_sub_layers_minus1,
    td_u32 *sps_ext_or_max_sub_layers_minus1)
{
    if (ctx->curr_nal->nal_layer_id == 0) {
        *sps_max_sub_layers_minus1 = hevc_u_v(ctx->bs_ptr, 3, "sps_max_sub_layers_minus1"); /* 3 is bit num */
        if (*sps_max_sub_layers_minus1 > (HEVC_MAX_TEMPLAYER - 1)) {
            dprint(PRN_ERROR, "sps_max_sub_layers_minus1 out of range(0,%d).\n", HEVC_MAX_TEMPLAYER);
            return HEVC_DEC_ERR;
        }
    } else {
#ifdef VFMW_MV_HEVC_SUPPORT
        *sps_max_sub_layers_minus1 = 0;
        *sps_ext_or_max_sub_layers_minus1 = hevc_u_v(ctx->bs_ptr,
            3, "sps_ext_or_max_sub_layers_minus1"); /* 3 protocal required */
        if (*sps_ext_or_max_sub_layers_minus1 != 7) { /* 7 protocal required */
            *sps_max_sub_layers_minus1 = *sps_ext_or_max_sub_layers_minus1;
        }
#else
        vfmw_unused(sps_ext_or_max_sub_layers_minus1);
#endif
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_sps_base_info(hevc_ctx *ctx, td_u32 *sps_max_sub_layers_minus1,
    td_u8 *sps_temporal_id_nesting_flag, td_u32 video_parameter_set_id, td_u32 sps_ext_or_max_sub_layers_minus1)
{
    td_s32 ret;

#ifdef VFMW_MV_HEVC_SUPPORT
    if (ctx->vps[video_parameter_set_id] == TD_NULL) {
        dprint(PRN_ERROR, "%s vps is null with this video_parameter_set_id = %d havn't be decoded\n",
             __func__, video_parameter_set_id);
        return HEVC_DEC_ERR;
    }
    if (ctx->curr_nal->nal_layer_id != 0 && sps_ext_or_max_sub_layers_minus1 == 7) { /* 7 protocal required */
        *sps_max_sub_layers_minus1 = ctx->vps[video_parameter_set_id]->vps_max_sub_layers - 1;
        if (*sps_max_sub_layers_minus1 > 0) {
            *sps_temporal_id_nesting_flag = ctx->vps[video_parameter_set_id]->vps_temporal_id_nesting_flag;
        }  else {
            *sps_temporal_id_nesting_flag = 1;
        }
    } else {
#else
    vfmw_unused(video_parameter_set_id);
    vfmw_unused(sps_ext_or_max_sub_layers_minus1);
#endif
        *sps_temporal_id_nesting_flag = hevc_u_v(ctx->bs_ptr, 1, "sps_temporal_id_nesting_flag");

        if (ctx->tmp_sps == TD_NULL) {
            ctx->tmp_sps = hevc_get_dyn_mem(ctx, sizeof(hevc_seq_param_set));
            if (ctx->tmp_sps == TD_NULL) {
                dprint(PRN_ERROR, "h265 alloc tmp_sps fail\n");
                return HEVC_DEC_ERR;
            }
        }

        ret = hevc_dec_ptl(ctx, &(ctx->tmp_sps->profile_tier_level), 1, *sps_max_sub_layers_minus1);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "SPS hevc_dec_ptl error.\n");
            return HEVC_DEC_ERR;
        }
#ifdef VFMW_MV_HEVC_SUPPORT
    }
#endif

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_sps(hevc_ctx *ctx)
{
    td_s32 ret;
    td_u32 video_parameter_set_id, sps_max_sub_layers_minus1, sps_seq_parameter_set_id;
    td_u8 sps_temporal_id_nesting_flag = 0;
    td_u32 sps_ext_or_max_sub_layers_minus1 = 0;

    video_parameter_set_id = hevc_u_v(ctx->bs_ptr, 4, "sps_video_parameter_set_id"); /* 4 is bit num */
    if (video_parameter_set_id >= (td_u32)(ctx->max_vps_num)) {
        dprint(PRN_ERROR, "sps_video_parameter_set_id out of range(0,%d).\n", ctx->max_vps_num);
        return HEVC_DEC_ERR;
    }

    sps_max_sub_layers_minus1 = 0;
    if (hevc_get_sps_max_sub_layers(ctx, &sps_max_sub_layers_minus1, &sps_ext_or_max_sub_layers_minus1) !=
        HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

    ret = hevc_dec_sps_base_info(ctx, &sps_max_sub_layers_minus1, &sps_temporal_id_nesting_flag,
                                 video_parameter_set_id, sps_ext_or_max_sub_layers_minus1);
    if (ret != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "SPS hevc_dec_ptl error.\n");
        return HEVC_DEC_ERR;
    }

    sps_seq_parameter_set_id = hevc_ue_v(ctx->bs_ptr, "sps_seq_parameter_set_id");
    if (sps_seq_parameter_set_id >= (td_u32)(ctx->max_sps_num) || sps_seq_parameter_set_id >= HEVC_MAX_SPS_NUM ||
        ctx->max_sps_num < 0) {
        dprint(PRN_ERROR, "sps_seq_parameter_set_id %d out of range(0,%d).\n",
               sps_seq_parameter_set_id, ctx->max_sps_num);
        return HEVC_DEC_ERR;
    }

    ret = hevc_dec_sps_process(ctx, video_parameter_set_id, sps_max_sub_layers_minus1, sps_temporal_id_nesting_flag,
                               sps_seq_parameter_set_id, sps_ext_or_max_sub_layers_minus1);
    if (ret != HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_get_pps_detail(const hevc_ctx *ctx, hevc_pic_param_set *pps)
{
    td_u8 flag;
    hevc_seq_param_set *sps = TD_NULL;

    sps = ctx->sps[pps->seq_parameter_set_id];

    pps->dependent_slice_segments_enabled_flag = hevc_u_v(ctx->bs_ptr, 1, "dependent_slice_segments_enabled_flag");
    pps->output_flag_present_flag = hevc_u_v(ctx->bs_ptr, 1, "output_flag_present_flag");
    pps->num_extra_slice_header_bits = hevc_u_v(ctx->bs_ptr, 3, "num_extra_slice_header_bits"); /* 3 is bit num */
    pps->sign_data_hiding_flag = hevc_u_v(ctx->bs_ptr, 1, "sign_data_hiding_flag");
    pps->cabac_init_present_flag = hevc_u_v(ctx->bs_ptr, 1, "cabac_init_present_flag");

    pps->num_ref_idx_l0_default_active = (td_s32)hevc_ue_v(ctx->bs_ptr, "num_ref_idx_l0_default_active_minus1");
    vfmw_check_ret(pps->num_ref_idx_l0_default_active <= 14, HEVC_DEC_ERR); /* 14 is vfmw_max val */
    vfmw_check_ret(pps->num_ref_idx_l0_default_active >= 0, HEVC_DEC_ERR);
    pps->num_ref_idx_l0_default_active += 1;

    pps->num_ref_idx_l1_default_active = (td_s32)(1 + hevc_ue_v(ctx->bs_ptr, "num_ref_idx_l1_default_active_minus1"));

    flag = (pps->num_ref_idx_l1_default_active < 0 || pps->num_ref_idx_l1_default_active > 15); /* 15 is vfmw_max val */
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "num_ref_idx_l1_default_active out of range[0,15].\n");

    pps->pic_init_qp = 26 + hevc_se_v(ctx->bs_ptr, "init_qp_minus26"); /* 26 is num */

    flag = (pps->pic_init_qp < (td_s32)(-(sps->qp_bdoffset_y)) || pps->pic_init_qp > 51); /* 51 is vfmw_max val */
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "pic_init_qp(%d) out of range[-%d,51].\n",
                         pps->pic_init_qp, (sps->qp_bdoffset_y));

    pps->constrained_intra_pred_flag = hevc_u_v(ctx->bs_ptr, 1, "constrained_intra_pred_flag");
    pps->transform_skip_enabled_flag = hevc_u_v(ctx->bs_ptr, 1, "transform_skip_enabled_flag");
    pps->cu_qp_delta_enabled_flag = hevc_u_v(ctx->bs_ptr, 1, "cu_qp_delta_enabled_flag");

    if (pps->cu_qp_delta_enabled_flag) {
        pps->diff_cu_qp_delta_depth = (td_s32)hevc_ue_v(ctx->bs_ptr, "diff_cu_qp_delta_depth");
    } else {
        pps->diff_cu_qp_delta_depth = 0;
    }

    flag = (pps->diff_cu_qp_delta_depth < 0 || pps->diff_cu_qp_delta_depth > 3); /* 3 is vfmw_max val */
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "diff_cu_qp_delta_depth out of range[0,3].\n");

    pps->max_cu_qp_delta_depth = pps->diff_cu_qp_delta_depth;

    pps->pic_cb_qp_offset = hevc_se_v(ctx->bs_ptr, "pps_cb_qp_offset");

    flag = (pps->pic_cb_qp_offset < -12 || pps->pic_cb_qp_offset > 12); /* 12 is range */
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "pic_cb_qp_offset out of range[-12,12].\n");

    pps->pic_cr_qp_offset = hevc_se_v(ctx->bs_ptr, "pps_cr_qp_offset");

    flag = (pps->pic_cr_qp_offset < -12 || pps->pic_cr_qp_offset > 12); /* 12 is range */
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "pic_cr_qp_offset out of range[-12,12].\n");

    pps->pic_slice_chroma_qp_offsets_present_flag = hevc_u_v(ctx->bs_ptr, 1, "chroma_qp_offsets_present_flag");
    pps->weighted_pred_flag = hevc_u_v(ctx->bs_ptr, 1, "weighted_pred_flag");
    pps->weighted_bipred_flag = hevc_u_v(ctx->bs_ptr, 1, "weighted_bipred_flag");
    pps->transquant_bypass_enable_flag = hevc_u_v(ctx->bs_ptr, 1, "transquant_bypass_enable_flag");
    pps->tiles_enabled_flag = hevc_u_v(ctx->bs_ptr, 1, "tiles_enabled_flag");
    pps->entropy_coding_sync_enabled_flag = hevc_u_v(ctx->bs_ptr, 1, "entropy_coding_sync_enabled_flag");

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_get_num_tile(hevc_ctx *ctx, hevc_pic_param_set *pps)
{
    td_u8 flag;

    pps->num_tile_columns = (td_s32)hevc_ue_v(ctx->bs_ptr, "num_tile_columns_minus1");

    flag = (td_u8)(pps->num_tile_columns > HEVC_MAX_TILE_COLUMNS - 1 || pps->num_tile_columns < 0);
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "num_tile_columns(%d) out of range(0,%d).\n",
                         pps->num_tile_columns, HEVC_MAX_TILE_COLUMNS - 1);
    pps->num_tile_columns += 1;

    pps->num_tile_rows = (td_s32)hevc_ue_v(ctx->bs_ptr, "num_tile_rows_minus1");

    flag = (td_u8)(pps->num_tile_rows > HEVC_MAX_TILE_ROWS - 1 || pps->num_tile_rows < 0);
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "num_tile_rows out(%d) of range(0,%d).\n",
                         pps->num_tile_rows, HEVC_MAX_TILE_ROWS - 1);
    pps->num_tile_rows += 1;

    /* 逻辑限制column x row 为 20 x 22 */
    if (pps->num_tile_columns > HEVC_LOGIC_TILE_COLUMNS_LIMIT) {
        dprint(PRN_ERROR, "Logic limit(%d), report unsupport num_tile_columns(%d).\n",
               HEVC_LOGIC_TILE_COLUMNS_LIMIT, pps->num_tile_columns);
        ctx->spec_invalid = SPEC_LOGIC_UNSUPPORT;
        report_unsupport(ctx->chan_id);
        return HEVC_DEC_ERR;
    }

    if (pps->num_tile_rows > HEVC_LOGIC_TILE_ROWS_LIMIT) {
        dprint(PRN_ERROR, "Logic limit(%d), report unsupport num_tile_rows(%d).\n",
               HEVC_LOGIC_TILE_ROWS_LIMIT, pps->num_tile_rows);
        ctx->spec_invalid = SPEC_LOGIC_UNSUPPORT;
        report_unsupport(ctx->chan_id);
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_process_pps_uniform_spacing(const hevc_ctx *ctx, hevc_pic_param_set *pps)
{
    td_u8 flag;
    td_s32 i;
    hevc_seq_param_set *sps = TD_NULL;

    sps = ctx->sps[pps->seq_parameter_set_id];

    pps->uniform_spacing_flag = hevc_u_v(ctx->bs_ptr, 1, "uniform_spacing_flag");

    if (!pps->uniform_spacing_flag) {
        for (i = 0; i < (pps->num_tile_columns - 1); i++) {
            pps->column_width[i] = (td_s32)hevc_ue_v(ctx->bs_ptr, "column_width_minus1") + 1;

            /* 宽度约束 */
            flag = (pps->column_width[i] < 0 || (td_u32)(pps->column_width[i]) > sps->ctb_num_width);
            vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "column_width[%d](%d) out of range (0 %d)\n",
                                 i, pps->column_width[i], sps->ctb_num_width);
        }

        for (i = 0; i < (pps->num_tile_rows - 1); i++) {
            pps->row_height[i] = (td_s16)(hevc_ue_v(ctx->bs_ptr, "row_height_minus1") + 1);

            /* 高度约束 */
            flag = (pps->row_height[i] < 0 || (td_u32)(pps->row_height[i]) > sps->ctb_num_height);
            vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "row_height[%d](%d) out of range (0 %d)\n",
                                 i, pps->row_height[i], sps->ctb_num_height);
        }
    }

    if (pps->uniform_spacing_flag) {
        for (i = 0; i < pps->num_tile_columns; i++) {
            pps->column_bd[i] = (td_s16)((td_u32)(i + 1) * sps->ctb_num_width / (td_u32)pps->num_tile_columns -
                                ((td_u32)i * sps->ctb_num_width) / (td_u32)pps->num_tile_columns);
        }

        for (i = 0; i < pps->num_tile_rows; i++) {
            pps->row_bd[i] = (td_s16)((td_u32)(i + 1) * sps->ctb_num_height / (td_u32)pps->num_tile_rows -
                             ((td_u32)i * sps->ctb_num_height) / (td_u32)pps->num_tile_rows);
        }
    } else {
        pps->column_bd[pps->num_tile_columns - 1] = (td_s16)sps->ctb_num_width;

        for (i = 0; i < pps->num_tile_columns - 1; i++) {
            pps->column_bd[i] = pps->column_width[i];
            pps->column_bd[pps->num_tile_columns - 1] -= pps->column_bd[i];
        }

        flag = (td_u8)(pps->column_bd[pps->num_tile_columns - 1] <= 0);
        vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "column_bd[%d](%d) <= 0, invalid!\n",
                             pps->num_tile_columns - 1, pps->column_bd[pps->num_tile_columns - 1]);

        pps->row_bd[pps->num_tile_rows - 1] = (td_s16)sps->ctb_num_height;

        for (i = 0; i < pps->num_tile_rows - 1; i++) {
            pps->row_bd[i] = pps->row_height[i];
            pps->row_bd[pps->num_tile_rows - 1] -= pps->row_bd[i];
        }

        flag = (td_u8)(pps->row_bd[pps->num_tile_rows - 1] <= 0);
        vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "row_bd[%d](%d) <= 0, invalid!\n",
                             pps->num_tile_rows - 1, pps->row_bd[pps->num_tile_rows - 1]);
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_pps_tile_enable(hevc_ctx *ctx, hevc_pic_param_set *pps, const hevc_seq_param_set *sps)
{
    td_s32 ret;

    if (pps->tiles_enabled_flag) {
        ret = hevc_get_num_tile(ctx, pps);
        if (ret != HEVC_DEC_NORMAL) {
            return ret;
        }

        ret = hevc_process_pps_uniform_spacing(ctx, pps);
        if (ret != HEVC_DEC_NORMAL) {
            return ret;
        }

        if (pps->num_tile_columns != 1 || pps->num_tile_rows != 1) {
            pps->loop_filter_across_tiles_enabled_flag =
                hevc_u_v(ctx->bs_ptr, 1, "loop_filter_across_tiles_enabled_flag");
        }
    } else {
        pps->column_bd[0] = sps->ctb_num_width;
        pps->row_bd[0] = sps->ctb_num_height;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_process_pps_last_part(const hevc_ctx *ctx, hevc_pic_param_set *pps, const hevc_seq_param_set *sps)
{
    td_s32 ret;
    td_u8 flag;

    pps->loop_filter_across_slices_enabled_flag = hevc_u_v(ctx->bs_ptr, 1, "loop_filter_across_slices_enabled_flag");
    pps->deblocking_filter_control_present_flag = hevc_u_v(ctx->bs_ptr, 1, "deblocking_filter_control_present_flag");

    if (pps->deblocking_filter_control_present_flag) {
        pps->deblocking_filter_override_enabled_flag = hevc_u_v(ctx->bs_ptr, 1, "filter_override_enabled_flag");
        pps->pic_disable_deblocking_filter_flag = hevc_u_v(ctx->bs_ptr, 1, "pps_disable_deblocking_filter_flag");

        if (!pps->pic_disable_deblocking_filter_flag) {
            pps->pps_beta_offset_div2 = hevc_se_v(ctx->bs_ptr, "pps_beta_offset_div2");

            if (pps->pps_beta_offset_div2 < -6 || pps->pps_beta_offset_div2 > 6) { /* 6 is range */
                dprint(PRN_ERROR, "pps_beta_offset_div2(%d) out of range(-6,6).\n", pps->pps_beta_offset_div2);
                pps->pps_beta_offset_div2 = 0; /* 赋值强解 */
                /* 钳位继续解 */
            }

            pps->pps_tc_offset_div2 = hevc_se_v(ctx->bs_ptr, "pps_tc_offset_div2");

            if (pps->pps_tc_offset_div2 < -6 || pps->pps_tc_offset_div2 > 6) { /* 6 is range */
                dprint(PRN_ERROR, "pps_tc_offset_div2(%d) out of range(-6,6).\n", pps->pps_tc_offset_div2);
                pps->pps_tc_offset_div2 = 0; /* 赋值强解 */
                /* 钳位继续解 */
            }
        }
    }

    pps->pic_scaling_list_data_present_flag = hevc_u_v(ctx->bs_ptr, 1, "pps_scaling_list_data_present_flag");

    if (pps->pic_scaling_list_data_present_flag) {
        ret = hevc_dec_scaling_list_data(ctx, &(pps->scaling_list));
        vfmw_check_ret_print(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR, "PPS hevc_dec_scaling_list_data error.\n");
    }

    pps->lists_modification_present_flag = hevc_u_v(ctx->bs_ptr, 1, "lists_modification_present_flag");
    pps->log2_parallel_merge_level = hevc_ue_v(ctx->bs_ptr, "log2_parallel_merge_level_minus2"); /* 2 is num */

    flag = (pps->log2_parallel_merge_level > sps->log2_ctb_sizey - 2); /* 2 is vfmw_min level */
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "log2_parallel_merge_level(%d) out of range(2,log2_ctb_sizey).\n",
                         pps->log2_parallel_merge_level);
    pps->log2_parallel_merge_level += 2; /* 2 is vfmw_min level */

    pps->slice_segment_header_extension_present_flag =
        hevc_u_v(ctx->bs_ptr, 1, "slice_segment_header_extension_present_flag");
    pps->pps_extension_flag = hevc_u_v(ctx->bs_ptr, 1, "pps_extension_flag");

    if (pps->pps_extension_flag) {
#ifdef VFMW_MV_HEVC_SUPPORT
        if (hevc_parse_pps_extension(ctx, pps) != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "HEVC_ParsePPSExtension %d failed.\n", pps->log2_parallel_merge_level);
            return HEVC_DEC_ERR;
        }
#else
        while (hevc_more_rbsp_data(ctx) == HEVC_TRUE) {
            pps->pps_extension_data_flag = hevc_u_v(ctx->bs_ptr, 1, "");
        }
#endif
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_process_pps(hevc_ctx *ctx, hevc_pic_param_set *pps)
{
    td_s32 i, ret;
    td_u8 flag;
    hevc_seq_param_set *sps = TD_NULL;

    pps->loop_filter_across_tiles_enabled_flag = 1;
    pps->num_tile_columns = 1;
    pps->num_tile_rows = 1;
    pps->seq_parameter_set_id = (td_s32)hevc_ue_v(ctx->bs_ptr, "pps_seq_parameter_set_id");

    flag = (pps->seq_parameter_set_id < 0 || pps->seq_parameter_set_id >= (ctx->max_sps_num));
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "pic_parameter_set_id(%d) out of range[0,15].\n",
                         pps->seq_parameter_set_id);

    flag = (ctx->sps[pps->seq_parameter_set_id] == TD_NULL);
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "SPS(%d) is null, it haven't decode.\n", pps->seq_parameter_set_id);

    flag = (ctx->sps[pps->seq_parameter_set_id]->valid == 0);
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "SPS(%d) haven't decode.\n", pps->seq_parameter_set_id);

    sps = ctx->sps[pps->seq_parameter_set_id];
    ret = hevc_get_pps_detail(ctx, pps);
    if (ret != HEVC_DEC_NORMAL) {
        return ret;
    }

    ret = hevc_pps_tile_enable(ctx, pps, sps);
        if (ret != HEVC_DEC_NORMAL) {
        return ret;
    }

    /* 逻辑不支持tile宽度<=64、tile高度<64的码流 */
    for (i = 0; i < pps->num_tile_columns; i++) {
        if ((td_u32)pps->column_bd[i] * sps->max_cu_width <= 64 && /* 64 is min wid */
            sps->pic_width_in_luma_samples > 64) { /* 64 is min wid */
            dprint(PRN_ERROR, "PPS tile width(%d) is too small.(Logic report unsupport.)\n",
                (td_u32)pps->column_bd[i] * sps->max_cu_width);
            ctx->spec_invalid = SPEC_LOGIC_UNSUPPORT;
            report_unsupport(ctx->chan_id);
            return HEVC_DEC_ERR;
        }
    }

    for (i = 0; i < pps->num_tile_rows; i++) {
        if ((td_u32)pps->row_bd[i] * sps->max_cu_width < 64) { /* 64 is vfmw_min wid */
            dprint(PRN_ERROR, "PPS tile height(%d) is too small.(Logic report unsupport.)\n",
                (td_u32)pps->row_bd[i] * sps->max_cu_width);
            ctx->spec_invalid = SPEC_LOGIC_UNSUPPORT;
            report_unsupport(ctx->chan_id);
            return HEVC_DEC_ERR;
        }
    }

    ret = hevc_process_pps_last_part(ctx, pps, sps);
    vfmw_check_ret(ret != HEVC_DEC_ERR, HEVC_DEC_ERR);

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_pps(hevc_ctx *ctx)
{
    td_u32 pic_parameter_set_id;

    hevc_pic_param_set *pps_tmp = TD_NULL;

    pic_parameter_set_id = hevc_ue_v(ctx->bs_ptr, "pps_pic_parameter_set_id");
    if (pic_parameter_set_id >= (td_u32)(ctx->max_pps_num) || (pic_parameter_set_id >= HEVC_MAX_PPS_NUM) ||
        ctx->max_pps_num < 0) {
        dprint(PRN_ERROR, "pic_parameter_set_id(%d) out of range(0,%d).\n", pic_parameter_set_id, ctx->max_pps_num);
        return HEVC_DEC_ERR;
    }

    if (ctx->pps[pic_parameter_set_id] == TD_NULL) {
        pps_tmp = hevc_get_dyn_mem(ctx, sizeof(hevc_pic_param_set));
        if (pps_tmp == TD_NULL) {
            dprint(PRN_ERROR, "hevc_get_dyn_mem PPS Fail\n");
            return HEVC_DEC_ERR;
        }
        ctx->pps[pic_parameter_set_id] = pps_tmp;
    }

    if (ctx->pps[pic_parameter_set_id]->valid) { /* 曾经解对过该id对应的PPS */
        if (ctx->tmp_pps == TD_NULL) {
            ctx->tmp_pps = hevc_get_dyn_mem(ctx, sizeof(hevc_pic_param_set));
            if (ctx->tmp_pps == TD_NULL) {
                dprint(PRN_ERROR, "h265 alloc tmp_pps fail\n");
                return HEVC_DEC_ERR;
            }
        }

        ctx->tmp_pps->pic_parameter_set_id = (td_s32)pic_parameter_set_id;

        if (hevc_process_pps(ctx, ctx->tmp_pps) != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "PPS[%d] decode error.\n", pic_parameter_set_id);
            return HEVC_DEC_ERR;
        }

        ctx->tmp_pps->is_refresh = 1;
        ctx->tmp_pps->valid = 1;

        pps_tmp = ctx->pps[pic_parameter_set_id];
        ctx->pps[pic_parameter_set_id] = ctx->tmp_pps;
        ctx->tmp_pps = pps_tmp;
    } else {
        ctx->pps[pic_parameter_set_id]->pic_parameter_set_id = (td_s32)pic_parameter_set_id;

        if (hevc_process_pps(ctx, (ctx->pps[pic_parameter_set_id])) != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "PPS[%d] decode error.\n", pic_parameter_set_id);
            ctx->pps[pic_parameter_set_id]->is_refresh = 1;
            ctx->pps[pic_parameter_set_id]->valid = 0;
            return HEVC_DEC_ERR;
        }

        ctx->pps[pic_parameter_set_id]->is_refresh = 1;
        ctx->pps[pic_parameter_set_id]->valid = 1;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_sei(hevc_ctx *ctx)
{
    hevc_sei_param_set *sei = TD_NULL;
    hevc_seq_param_set *sps = TD_NULL;
    td_u32 rbsp_trailing_bits;
    td_s32 ret;

#ifdef VFMW_MV_HEVC_SUPPORT
    if (ctx->is_multi_layers == 1) {
        return HEVC_DEC_NORMAL;
    }
#endif

    if (ctx->sei_sps == TD_NULL) {
        dprint(PRN_ERROR, "H265 DecSei sei_sps = TD_NULL\n");
        return HEVC_DEC_ERR;
    }

    sei = &ctx->sei;
    sps = ctx->sei_sps;

    do {
        ret = hevc_sei_message(ctx, sei, sps);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "sei hevc_sei_message error.\n");
            return HEVC_DEC_ERR;
        }

        if (bs_is_byte_aligned(ctx->bs_ptr) == 0) {
            dprint(PRN_ERROR, "sei bs_is_byte_aligned error.\n");
            return HEVC_DEC_ERR;
        }
    } while (hevc_sei_more_rbsp_data(ctx) == HEVC_TRUE);

    if (ret == HEVC_DEC_NORMAL && ((bs_pos(ctx->bs_ptr) + 8) <= (ctx->bs_ptr->bs_len * 8))) { /* 8 is bit num */
        rbsp_trailing_bits = hevc_u_v(ctx->bs_ptr, 8, "rbsp_trailing_bits"); /* 8 is bit num */
        if (rbsp_trailing_bits != 0x80) {
            dprint(PRN_ERROR, "sei rbsp_trailing_bits(%d) not equal 0x80.\n", rbsp_trailing_bits);
            return HEVC_DEC_ERR;
        }
    }
    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sei_message(hevc_ctx *ctx, hevc_sei_param_set *sei, hevc_seq_param_set *sps)
{
    td_u32 payload_type;
    td_u32 payload_size;
    td_s32 ret;

    payload_type = 0;

    while (bs_show(ctx->bs_ptr, 8) == 0xFF) { /* 8 is bit num */
        bs_skip(ctx->bs_ptr, 8); /* 8 is bit num */
        payload_type += 255; /* 255 is byte num */

        /* TV产品遇到一条码流，发现SEI只有14bit,但因解析时
           没有做nal有效长度的限制，从而导致概率性访问到无效地址挂死。
           当前解决办法:加一个有效长度判断条件，加8是因为后面还有8个bit的消耗
           建议最终解决办法:将这块容错放到码流解析函数里去。
        */
        if ((bs_pos(ctx->bs_ptr) + 8) > (ctx->bs_ptr->bs_len * 8)) { /* 8 is bit num */
            dprint(PRN_ERROR, "line:%d Cosume total_pos(%d > %d)!!!\n", __LINE__,
                   bs_pos(ctx->bs_ptr), ctx->bs_ptr->bs_len);
            return HEVC_DEC_ERR;
        }
    }

    sei->last_payload_type_byte = hevc_u_v(ctx->bs_ptr, 8, "last_payload_type_byte"); /* 8 is bit num */
    payload_type += sei->last_payload_type_byte;

    payload_size = 0;

    while (bs_show(ctx->bs_ptr, 8) == 0xFF) { /* 8 is bit num */
        bs_skip(ctx->bs_ptr, 8); /* 8 is bit num */
        payload_size += 255; /* 255 is byte num */

        /* TV产品遇到一条码流，发现SEI只有14bit,但因解析时
           没有做nal有效长度的限制，从而导致概率性访问到无效地址挂死。
           当前解决办法:加一个有效长度判断条件，加8是因为后面还有8个bit的消耗
           建议最终解决办法:将这块容错放到码流解析函数里去。
        */
        if ((bs_pos(ctx->bs_ptr) + 8) > ctx->bs_ptr->bs_len * 8) { /* 8 is bit num */
            dprint(PRN_ERROR, "line:%d Cosume total_pos(%d > %d)!!!\n", __LINE__,
                   bs_pos(ctx->bs_ptr), ctx->bs_ptr->bs_len);
            return HEVC_DEC_ERR;
        }
    }

    sei->last_payload_size_byte = hevc_u_v(ctx->bs_ptr, 8, "last_payload_size_byte"); /* 8 is bit num */
    payload_size += sei->last_payload_size_byte;

    if (payload_size * 8 + (td_u32)(bs_pos(ctx->bs_ptr)) > (td_u32)(ctx->bs_ptr->bs_len) * 8) { /* 8 is bit num */
        dprint(PRN_ERROR, "%s payload size(%d) > bs_len(%d)\n", __func__, payload_size, ctx->bs_ptr->bs_len);
        return HEVC_DEC_ERR;
    }

    ret = hevc_sei_payload(ctx, sei, sps, payload_type, payload_size);
    if (ret != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "hevc_sei_message hevc_sei_payload error.\n");
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_parse_other_prefix_sei(hevc_ctx *ctx, hevc_sei_param_set *sei,
    hevc_seq_param_set *sps, td_u32 payload_type, td_u32 payload_size)
{
    td_u32 i;
    td_s32 ret;

    switch (payload_type) {
        case 130: /* 130 is type val */
            ret = hevc_sei_dec_unit_info(ctx, &(sei->decoding_unit_info), &(sps->vui_parameters.hrd_parameters));
            break;

        case 131: /* 131 is type val */
            ret = hevc_sei_temporal_sub_layer_zero_index(ctx, &(sei->temporal_sub_layer));
            break;

        case 133: /* 133 is type val */
            ret = hevc_sei_scalable_nesting_info(ctx, sei, sps, payload_size);
            break;

        case 134: /* 134 is type val */
            ret = hevc_sei_region_refresh(ctx, &(sei->region_refresh_info));
            break;

        case 137: /* 137 is type val */
            ret = hevc_sei_mastering_disp_colour_vol(ctx, &(sei->mastering_display_colour_volume));
            break;

        case 142: /* 142 is type val */
            ret = hevc_sei_color_remapping(ctx, &(sei->color_remapping_info));
            break;

        case 144: /* 144 is type val */
            ret = hevc_sei_content_light_level_information(ctx, &(sei->content_light_level));
            break;

        case 147: /* 147 is type val */
            ret = hevc_sei_hlghdr_comapitibility_info(ctx, &(sei->hdr_compatible_info));
            break;

        default:
            for (i = 0; i < payload_size; i++) {
                hevc_u_v(ctx->bs_ptr, 8, "unknown prefix sei payload byte"); /* 8 is bit num */
            }
            ret = HEVC_DEC_NORMAL;
            break;
    }

    return ret;
}

STATIC td_s32 hevc_parse_prefix_sei(hevc_ctx *ctx, hevc_sei_param_set *sei, hevc_seq_param_set *sps,
    td_u32 payload_type, td_u32 payload_size)
{
    td_s32 ret;

    switch (payload_type) {
        case 0: /* 0 is type val */
            ret = hevc_sei_buffer_period(ctx, &(sei->buf_period), &(sps->vui_parameters.hrd_parameters));
            break;

        case 1: /* 1 is type val */
            ret = hevc_sei_picture_timming(ctx, &(sei->pic_timming), &(sps->vui_parameters));
            break;

        case 4: /* 4 is type val */
            ret = hevc_sei_usr_dat_reg_itu_t_t35(ctx, &(sei->itu_t35), payload_size);
            break;

        case 5: /* 5 is type val */
            ret = hevc_sei_usr_dat_unreg(ctx, &(sei->user_data_unreg), payload_size);
            break;

        case 6: /* 6 is type val */
            ret = hevc_sei_recovery_point_info(ctx, &(sei->recovery_point));
            break;

        case 23: /* 23 is type val */
            ret = hevc_sei_tone_mapping(ctx, &(sei->tone_mapping_info));
            break;

        case 45: /* 45 is type val */
            ret = hevc_sei_frm_packing_arrangement(ctx, &(sei->frame_packing));
            break;

        case 47: /* 47 is type val */
            ret = hevc_sei_disp_orientation(ctx, &(sei->display_orientation));
            break;

        case 128: /* 128 is type val */
            ret = hevc_sei_structure_of_pictures(ctx, &(sei->sop_info), payload_size);
            break;

        case 129: /* 129 is type val */
            ret = hevc_sei_active_param_sets(ctx, &(sei->active_param_sets));
            break;

        default:
            ret = hevc_parse_other_prefix_sei(ctx, sei, sps, payload_type, payload_size);
            break;
    }

    return ret;
}

STATIC td_s32 hevc_parse_suffix_sei(hevc_ctx *ctx, hevc_sei_param_set *sei, const hevc_seq_param_set *sps,
    td_u32 payload_type, td_u32 payload_size)
{
    td_u32 i;
    td_s32 ret;

    vfmw_unused(sps);
    switch (payload_type) {
        case 5: /* 5 is type val */
            ret = hevc_sei_usr_dat_unreg(ctx, &(sei->user_data_unreg), payload_size);
            break;

        case 132: /* 132 is type val */
            ret = hevc_sei_decoded_pic_hash(ctx, &(sei->decoded_picture_hash));
            break;

        default:

            for (i = 0; i < payload_size; i++) {
                hevc_u_v(ctx->bs_ptr, 8, "unknown suffix sei payload byte"); /* 8 is bit num */
            }
            ret = HEVC_DEC_NORMAL;
            break;
    }

    return ret;
}

STATIC td_s32 hevc_sei_payload(hevc_ctx *ctx, hevc_sei_param_set *sei, hevc_seq_param_set *sps,
    td_u32 payload_type, td_u32 payload_size)
{
    td_s32 ret;

    if (ctx->curr_nal->nal_unit_type == NAL_UNIT_PREFIX_SEI) {
        ret = hevc_parse_prefix_sei(ctx, sei, sps, payload_type, payload_size);
    } else {
        ret = hevc_parse_suffix_sei(ctx, sei, sps, payload_type, payload_size);
    }

    if (ret != HEVC_DEC_NORMAL) {
        return ret;
    }

    if (hevc_sei_more_data_in_payload(ctx, payload_size) == 1) {
        if (hevc_sei_payload_extension_present(ctx, payload_size) == 1) {
            sei->reserved_payload_extension_data = hevc_ue_v(ctx->bs_ptr, "reserved_payload_extension_data");
        }

        sei->payload_bit_equal_to_one = hevc_u_v(ctx->bs_ptr, 1, "payload_bit_equal_to_one");

        while (bs_is_byte_aligned(ctx->bs_ptr) == 0) {
            sei->payload_bit_equal_to_zero = hevc_u_v(ctx->bs_ptr, 1, "payload_bit_equal_to_zero");
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_u32 hevc_sei_more_data_in_payload(const hevc_ctx *ctx, td_u32 payload_size)
{
    td_s32 is_payload_size;

    if (payload_size > VFMW_MAX_UINT32 / 8) { /* 8 is a number */
        return 0;
    }

    is_payload_size = ((td_u32)(bs_pos(ctx->bs_ptr) - ctx->bs_ptr->history_pos) ==
                       (payload_size * 8)) ? 1 : 0; /* 8 is bit num */
    if (bs_is_byte_aligned(ctx->bs_ptr) == 1 && (is_payload_size == 0)) {
        return 0;
    } else {
        return 1;
    }
}

STATIC td_u32 hevc_sei_payload_extension_present(const hevc_ctx *ctx, td_u32 payload_size)
{
    td_u32 pos;
    bs_info bs = {0};
    bs_info *bs_ptr = &bs;

    vfmw_check_sec_func(memcpy_s(bs_ptr, sizeof(bs), ctx->bs_ptr, sizeof(bs_info)));

    pos = (td_u32)(bs_pos(ctx->bs_ptr) - bs_ptr->history_pos);
    if (pos < 8 * payload_size) { /* 8 is bit num */
        if (bs_show(bs_ptr, 1) != 0) {
            bs_skip(bs_ptr, 1);
            pos = (td_u32)(bs_pos(ctx->bs_ptr) - bs_ptr->history_pos);

            while ((!bs_show(bs_ptr, 1)) && (pos < 8 * payload_size)) { /* 8 is bit num */
                bs_skip(bs_ptr, 1);
                pos = (td_u32)(bs_pos(ctx->bs_ptr) - bs_ptr->history_pos);
            }

            if (pos >= 8 * payload_size) { /* 8 is bit num */
                return 0;
            } else {
                return 1;
            }
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

STATIC td_s32 hevc_sei_decoded_pic_hash(const hevc_ctx *ctx, hevc_sei_decoded_picture_hash *dec)
{
    td_u32 i;
    td_u32 j;
    td_u32 val;

    if (ctx == TD_NULL || dec == TD_NULL || ctx->bs_ptr == TD_NULL) {
        return HEVC_DEC_ERR;
    }

    dec->hash_type = hevc_u_v(ctx->bs_ptr, 8, "hash_type"); /* 8 is bit num */
    dec->method = (hevc_sei_method)dec->hash_type;

    for (j = 0; j < 3; j++) { /* 3 is vfmw_max num */
        if (dec->method == MD5) {
            for (i = 0; i < 16; i++) { /* 16 is vfmw_max num */
                dec->digest[j][i] = hevc_u_v(ctx->bs_ptr, 8, "picture_md5"); /* 8 is bit num */
            }
        } else if (dec->method == CRC) {
            val = hevc_u_v(ctx->bs_ptr, 16, "picture_crc"); /* 16 is bit num */
            dec->digest[j][0] = (val >> 8) & 0xFF; /* 8 is bit ofs */
            dec->digest[j][1] = val & 0xFF;
        } else if (dec->method == CHECKSUM) {
            val = hevc_u_v(ctx->bs_ptr, 32, "picture_checksum"); /* 32 is bit num */
            dec->digest[j][0] = (val >> 24) & 0xff; /* 24 is bit ofs */
            dec->digest[j][1] = (val >> 16) & 0xff; /* 16 is bit ofs */
            dec->digest[j][2] = (val >> 8) & 0xff; /* 2 is idx, 8 is bit ofs */
            dec->digest[j][3] = val & 0xff; /* 3 is idx */
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sei_scalable_nesting_info(hevc_ctx *ctx, hevc_sei_param_set *sei,
    hevc_seq_param_set *sps, td_u32 payloadsize)
{
    td_u32 i;
    td_s32 ret;
    hevc_sei_scalable_nesting *sca = TD_NULL;

    vfmw_unused(payloadsize);
    sca = &(sei->scalable_nesting);
    sca->bitstream_subset_flag = hevc_u_v(ctx->bs_ptr, 1, "bitstream_subset_flag");
    sca->nesting_op_flag = hevc_u_v(ctx->bs_ptr, 1, "nesting_op_flag");

    if (sca->nesting_op_flag) {
        sca->default_op_flag = hevc_u_v(ctx->bs_ptr, 1, "default_op_flag");
        sca->nesting_num_ops_minus1 = hevc_ue_v(ctx->bs_ptr, "nesting_num_ops_minus1");

        if (sca->nesting_num_ops_minus1 >= HEVC_MAX_NESTING_NUM_LAYER) {
            dprint(PRN_ERROR, "nesting_num_ops_minus1 out of range(0,%d)\n", HEVC_MAX_NESTING_NUM_LAYER);
            return HEVC_DEC_ERR;
        }

        for (i = sca->default_op_flag; i <= sca->nesting_num_ops_minus1; i++) {
            sca->nesting_max_temporal_id_plus1[i] =
                hevc_u_v(ctx->bs_ptr, 3, "nesting_max_temporal_id_plus1"); /* 3 is bit num */
            sca->nesting_op_idx[i] = hevc_ue_v(ctx->bs_ptr, "nesting_op_idx");
        }
    } else {
        sca->all_layers_flag = hevc_u_v(ctx->bs_ptr, 1, "all_layers_flag");

        if (!(sca->all_layers_flag)) {
            sca->nesting_no_op_max_temporal_id_plus1 =
                hevc_u_v(ctx->bs_ptr, 3, "nesting_no_op_max_temporal_id_plus1"); /* 3 is bit num */
            sca->nesting_num_layers_minus1 = hevc_ue_v(ctx->bs_ptr, "nesting_num_layers_minus1");

            if (sca->nesting_num_layers_minus1 > 63) { /* 63 is vfmw_max val */
                dprint(PRN_ERROR, "nesting_num_layers_minus1 out of range(0,63)\n");
                return HEVC_DEC_ERR;
            }

            for (i = 0; i <= sca->nesting_num_layers_minus1; i++) {
                sca->nesting_layer_id[i] = hevc_u_v(ctx->bs_ptr, 6, "nesting_num_layers_minus1"); /* 6 is bit num */
            }
        }
    }

    bs_skip_to_next_byte(ctx->bs_ptr);

    sca->caller_owns_seis = HEVC_FALSE;

    do {
        ret = hevc_sei_message(ctx, sei, sps);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "sei hevc_sei_message error.\n");
            return HEVC_DEC_ERR;
        }
    } while (hevc_more_rbsp_data(ctx) == HEVC_TRUE);

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sei_structure_of_pictures(const hevc_ctx *ctx, hevc_sei_sop_info *sop, td_u32 payload_size)
{
    td_u32 i;

    vfmw_unused(payload_size);
    sop->sop_seq_parameter_set_id = hevc_ue_v(ctx->bs_ptr, "sop_seq_parameter_set_id");
    sop->num_entries_in_sop_minus1 = hevc_ue_v(ctx->bs_ptr, "num_entries_in_sop_minus1");

    if (sop->num_entries_in_sop_minus1 > 1024) { /* 1024 is vfmw_max val */
        dprint(PRN_ERROR, "num_entries_in_sop_minus1 out of range(0,1024)\n");
        return HEVC_DEC_ERR;
    }

    for (i = 0; i < sop->num_entries_in_sop_minus1; i++) {
        sop->sop_vcl_nut[i] = hevc_u_v(ctx->bs_ptr, 6, "sop_vcl_nut"); /* 6 is bit num */
        sop->sop_temporal_id[i] = hevc_u_v(ctx->bs_ptr, 3, "sop_temporal_id"); /* 3 is bit num */

        if ((sop->sop_vcl_nut[i] != NAL_UNIT_CODED_SLICE_IDR_W_RADL) &&
            (sop->sop_vcl_nut[i] != NAL_UNIT_CODED_SLICE_IDR_N_LP)) {
            sop->sop_short_term_rps_idx[i] = hevc_ue_v(ctx->bs_ptr, "sop_short_term_rps_idx");
        }

        if (i > 0) {
            sop->sop_poc_delta[i] = hevc_se_v(ctx->bs_ptr, "sop_poc_delta");
        }
    }

    bs_skip_to_next_byte(ctx->bs_ptr);

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_sei_tone_mapping_model4(const hevc_ctx *ctx, hevc_sei_tone_mapping_info *ton)
{
    ton->camera_iso_speed_idc = hevc_u_v(ctx->bs_ptr, 8, "camera_iso_speed_idc"); /* 8 is bit num */

    if (ton->camera_iso_speed_idc == 255) { /* 255 Extended_ISO */
        ton->camera_iso_speed_value =
            hevc_u_v(ctx->bs_ptr, 32, "camera_iso_speed_value"); /* 32 is bit num */
    }

    ton->exposure_compensation_value_sign_flag =
        hevc_u_v(ctx->bs_ptr, 1, "exposure_compensation_value_sign_flag");
    ton->exposure_compensation_value_numerator =
        hevc_u_v(ctx->bs_ptr, 16, "exposure_compensation_value_numerator"); /* 16 is bit num */
    ton->exposure_compensation_value_denom_idc =
        hevc_u_v(ctx->bs_ptr, 16, "exposure_compensation_value_denom_idc"); /* 16 is bit num */
    ton->ref_screen_luminance_white =
        hevc_u_v(ctx->bs_ptr, 32, "ref_screen_luminance_white"); /* 32 is bit num */
    ton->extended_range_white_level =
        hevc_u_v(ctx->bs_ptr, 32, "extended_range_white_level"); /* 32 is bit num */
    ton->nominal_black_level_code_value =
        hevc_u_v(ctx->bs_ptr, 16, "nominal_black_level_code_value"); /* 16 is bit num */
    ton->nominal_white_level_code_value =
        hevc_u_v(ctx->bs_ptr, 16, "nominal_white_level_code_value"); /* 16 is bit num */
    ton->exposure_compensation_value_numerator =
        hevc_u_v(ctx->bs_ptr, 16, "exposure_compensation_value_numerator"); /* 16 is bit num */
    ton->extended_white_level_code_value =
        hevc_u_v(ctx->bs_ptr, 16, "extended_white_level_code_value"); /* 16 is bit num */

    return;
}

STATIC td_s32 hevc_get_tone_map(const hevc_ctx *ctx, hevc_sei_tone_mapping_info *ton)
{
    ton->tone_map_id = hevc_ue_v(ctx->bs_ptr, "tone_map_id");
    ton->tone_map_cancel_flag = hevc_u_v(ctx->bs_ptr, 1, "tone_map_cancel_flag");

    if (!ton->tone_map_cancel_flag) {
        ton->tone_map_persistence_flag = hevc_u_v(ctx->bs_ptr, 1, "tone_map_persistence_flag");
        ton->coded_data_bit_depth = hevc_u_v(ctx->bs_ptr, 8, "coded_data_bit_depth"); /* 8 is bit num */
        ton->target_bit_depth = hevc_u_v(ctx->bs_ptr, 8, "target_bit_depth"); /* 8 is bit num */

        if (ton->target_bit_depth > 8) { /* 应该为1-16,为了节约内存，只支持到8. */
            dprint(PRN_ERROR, "Unsupport: target_bit_depth(%d) > 8\n", ton->target_bit_depth);
            return HEVC_DEC_ERR;
        }

        ton->tone_map_model_id = hevc_ue_v(ctx->bs_ptr, "tone_map_model_id");
        return HEVC_DEC_NORMAL;
    }

    return HEVC_DEC_ERR;
}

STATIC td_s32 hevc_sei_tone_mapping(const hevc_ctx *ctx, hevc_sei_tone_mapping_info *ton)
{
    td_u32 i;
    td_u32 num;
    td_s32 ret;

    ret = hevc_get_tone_map(ctx, ton);
    vfmw_check_ret(ret != HEVC_DEC_ERR, HEVC_DEC_ERR);

    switch (ton->tone_map_model_id) {
        case 0: /* 0 is model id */
            ton->min_value = hevc_u_v(ctx->bs_ptr, 32, "min_value"); /* 32 is bit num */
            ton->max_value = hevc_u_v(ctx->bs_ptr, 32, "max_value"); /* 32 is bit num */
            break;

        case 1: /* 1 is model id */
            ton->sigmoid_midpoint = hevc_u_v(ctx->bs_ptr, 32, "sigmoid_midpoint"); /* 32 is bit num */
            ton->sigmoid_width = hevc_u_v(ctx->bs_ptr, 32, "sigmoid_width"); /* 32 is bit num */
            break;

        case 2: /* 2 is model id */
            num = 1 << (ton->target_bit_depth);
            vfmw_check_ret(num <= 257, HEVC_DEC_ERR); /* 257: array size */
            for (i = 0; i < num; i++) {
                ton->start_of_coded_interval[i] = hevc_u_v(ctx->bs_ptr,
                    vfmw_align_up(ton->coded_data_bit_depth, 8), "start_of_coded_interval"); /* 8 is align */
            }

            ton->start_of_coded_interval[num] = 1 << ton->coded_data_bit_depth;
            break;

        case 3: /* 3 is model id */
            ton->num_pivots = hevc_u_v(ctx->bs_ptr, 16, "num_pivots"); /* 16 is bit num */

            if (ton->num_pivots > 256) { /* 256 is vfmw_max val */
                dprint(PRN_ERROR, "ton->num_pivots > 256\n");
                return HEVC_DEC_ERR;
            }

            for (i = 0; i < ton->num_pivots; i++) {
                ton->coded_pivot_value[i] = hevc_u_v(ctx->bs_ptr,
                    vfmw_align_up(ton->coded_data_bit_depth, 8), "coded_pivot_value"); /* 8 is align */
                ton->target_pivot_value[i] = hevc_u_v(ctx->bs_ptr,
                    vfmw_align_up(ton->target_bit_depth, 8), "target_pivot_value"); /* 8 is align */
            }
            break;

        case 4: /* 4 is model id */
            hevc_sei_tone_mapping_model4(ctx, ton);
            break;

        default:
            dprint(PRN_ERROR, "Unsupported tone map model id %d\n", ton->tone_map_model_id);
            break;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sei_region_refresh(const hevc_ctx *ctx, hevc_sei_region_refresh_info *reg)
{
    if (ctx == TD_NULL || reg == TD_NULL || ctx->bs_ptr == TD_NULL) {
        return HEVC_DEC_ERR;
    }

    reg->refreshed_region_flag = hevc_u_v(ctx->bs_ptr, 1, "refreshed_region_flag");

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sei_mastering_disp_colour_vol(const hevc_ctx *ctx,
    hevc_sei_mastering_display_colour_volume *reg)
{
    td_u32 i;

    if (ctx == TD_NULL || reg == TD_NULL || ctx->bs_ptr == TD_NULL) {
        return HEVC_DEC_ERR;
    }

    reg->is_avalible = 1;

    for (i = 0; i < 3; i++) { /* 3 is vfmw_max num */
        reg->display_primaries_x[i] = hevc_u_v(ctx->bs_ptr, 16, "display_primaries_x"); /* 16 is bit num */
        reg->display_primaries_y[i] = hevc_u_v(ctx->bs_ptr, 16, "display_primaries_y"); /* 16 is bit num */
    }

    reg->white_point_x = hevc_u_v(ctx->bs_ptr, 16, "white_point_x"); /* 16 is bit num */
    reg->white_point_y = hevc_u_v(ctx->bs_ptr, 16, "white_point_y"); /* 16 is bit num */
    reg->max_display_mastering_luminance = hevc_u_v(ctx->bs_ptr, 32, "max_display_luminance"); /* 32 is bit num */
    reg->min_display_mastering_luminance = hevc_u_v(ctx->bs_ptr, 32, "min_display_luminance"); /* 32 is bit num */

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sei_temporal_sub_layer_zero_index(const hevc_ctx *ctx,
    hevc_sei_temporal_sub_layer *tem)
{
    if (ctx == TD_NULL || tem == TD_NULL || ctx->bs_ptr == TD_NULL) {
        return HEVC_DEC_ERR;
    }

    tem->temporal_sub_layer_zero_idx = hevc_u_v(ctx->bs_ptr, 1, "temporal_sub_layer_zero_idx");
    tem->irap_pic_id = hevc_u_v(ctx->bs_ptr, 1, "irap_pic_id");
    bs_skip_to_next_byte(ctx->bs_ptr);

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sei_disp_orientation(const hevc_ctx *ctx, hevc_sei_display_orientation *dis)
{
    if (ctx == TD_NULL || dis == TD_NULL || ctx->bs_ptr == TD_NULL) {
        return HEVC_DEC_ERR;
    }

    dis->display_orientation_cancel_flag = hevc_u_v(ctx->bs_ptr, 1, "display_orientation_cancel_flag");

    if (!dis->display_orientation_cancel_flag) {
        dis->hor_flip = hevc_u_v(ctx->bs_ptr, 1, "hor_flip");
        dis->ver_flip = hevc_u_v(ctx->bs_ptr, 1, "ver_flip");
        dis->anticlockwise_rotation = hevc_u_v(ctx->bs_ptr, 16, "anticlockwise_rotation"); /* 16 is bit num */
        dis->display_orientation_persistence_flag = hevc_u_v(ctx->bs_ptr, 1, "display_orientation_persistence_flag");
    }

    bs_skip_to_next_byte(ctx->bs_ptr);

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sei_frm_packing_arrangement(const hevc_ctx *ctx, hevc_sei_frame_packing_arrangement *frm)
{
    frm->frame_packing_arrangement_id = hevc_ue_v(ctx->bs_ptr, "frame_packing_arrangement_id");
    frm->frame_packing_arrangement_cancel_flag = hevc_u_v(ctx->bs_ptr, 1, "frame_packing_arrangement_cancel_flag");

    if (!frm->frame_packing_arrangement_cancel_flag) {
        frm->frame_packing_arrangement_type = hevc_u_v(ctx->bs_ptr, 7, "frame_packing_type"); /* 7 is bit num */

        if ((frm->frame_packing_arrangement_type <= 2) || /* 2 is vfmw_min val */
            (frm->frame_packing_arrangement_type >= 6)) { /* 6 is vfmw_max val */
            dprint(PRN_ERROR, "frame_packing_arrangement_type(%d) out of range\n", frm->frame_packing_arrangement_type);
            return HEVC_DEC_ERR;
        }

        frm->quincunx_sampling_flag = hevc_u_v(ctx->bs_ptr, 1, "quincunx_sampling_flag");
        frm->content_interpretation_type = hevc_u_v(ctx->bs_ptr, 6, "content_interpretation_type"); /* 6 is bit num */
        frm->spatial_flipping_flag = hevc_u_v(ctx->bs_ptr, 1, "spatial_flipping_flag");
        frm->frame0_flipped_flag = hevc_u_v(ctx->bs_ptr, 1, "frame0_flipped_flag");
        frm->field_views_flag = hevc_u_v(ctx->bs_ptr, 1, "field_views_flag");
        frm->current_frame_is_frame0_flag = hevc_u_v(ctx->bs_ptr, 1, "current_frame_is_frame0_flag");
        frm->frame0_self_contained_flag = hevc_u_v(ctx->bs_ptr, 1, "frame0_self_contained_flag");
        frm->frame1_self_contained_flag = hevc_u_v(ctx->bs_ptr, 1, "frame1_self_contained_flag");

        if ((frm->quincunx_sampling_flag == 0) && (frm->frame_packing_arrangement_type != 5)) { /* 5 is type */
            frm->frame0_grid_position_x = hevc_u_v(ctx->bs_ptr, 4, "frame0_grid_position_x"); /* 4 is bit num */
            frm->frame0_grid_position_y = hevc_u_v(ctx->bs_ptr, 4, "frame0_grid_position_y"); /* 4 is bit num */
            frm->frame1_grid_position_x = hevc_u_v(ctx->bs_ptr, 4, "frame1_grid_position_x"); /* 4 is bit num */
            frm->frame1_grid_position_y = hevc_u_v(ctx->bs_ptr, 4, "frame1_grid_position_y"); /* 4 is bit num */
        }

        frm->frame_packing_arrangement_reserved_byte =
            hevc_u_v(ctx->bs_ptr, 8, "frame_packing_arrangement_reserved_byte"); /* 8 is bit num */
        frm->frame_packing_arrangement_persistence_flag =
            hevc_u_v(ctx->bs_ptr, 1, "frame_packing_arrangement_persistence_flag");
    }

    frm->upsampled_aspect_ratio_flag = hevc_u_v(ctx->bs_ptr, 1, "upsampled_aspect_ratio_flag");

    bs_skip_to_next_byte(ctx->bs_ptr);

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sei_recovery_point_info(const hevc_ctx *ctx, hevc_sei_recovery_point *rec)
{
    if (ctx == TD_NULL || rec == TD_NULL || ctx->bs_ptr == TD_NULL) {
        return HEVC_DEC_ERR;
    }

    rec->recovery_poc_cnt = hevc_se_v(ctx->bs_ptr, "recovery_poc_cnt");
    rec->exact_match_flag = hevc_u_v(ctx->bs_ptr, 1, "exact_match_flag");
    rec->broken_link_flag = hevc_u_v(ctx->bs_ptr, 1, "broken_link_flag");
    bs_skip_to_next_byte(ctx->bs_ptr);

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sei_dec_unit_info(const hevc_ctx *ctx, hevc_sei_decoding_unit_info *dec,
    const hevc_hrd_param_set *hrd)
{
    if (ctx == TD_NULL || dec == TD_NULL || hrd == TD_NULL || ctx->bs_ptr == TD_NULL) {
        return HEVC_DEC_ERR;
    }

    dec->decoding_unit_idx = (td_s32)hevc_ue_v(ctx->bs_ptr, "decoding_unit_idx");

    if (hrd->sub_pic_cpb_params_in_pic_timing_sei_flag) {
        dec->du_spt_cpb_removal_delay_increment = (td_s32)hevc_u_v(ctx->bs_ptr,
            hrd->du_cpb_removal_delay_increment_length_minus1 + 1, "du_spt_cpb_removal_delay_increment");
    } else {
        dec->du_spt_cpb_removal_delay_increment = 0;
    }

    dec->dpb_output_du_delay_present_flag = hevc_u_v(ctx->bs_ptr, 1, "dpb_output_du_delay_present_flag");

    if (dec->dpb_output_du_delay_present_flag) {
        dec->pic_spt_dpb_output_du_delay =
            (td_s32)hevc_u_v(ctx->bs_ptr, hrd->dpb_output_delay_du_length_minus1 + 1, "pic_spt_dpb_output_du_delay");
    }

    bs_skip_to_next_byte(ctx->bs_ptr);

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sei_active_param_sets(const hevc_ctx *ctx, hevc_sei_active_parameter_set *act)
{
    td_s32 i;
    act->active_video_parameter_set_id = (td_s32)hevc_u_v(ctx->bs_ptr, 4, "active_vps_id"); /* 4 is bit num */
    act->self_contained_cvs_flag = hevc_u_v(ctx->bs_ptr, 1, "self_contained_cvs_flag");
    act->no_parameter_set_update_flag = hevc_u_v(ctx->bs_ptr, 1, "no_parameter_set_update_flag");
    act->num_sps_ids_minus1 = (td_s32)hevc_ue_v(ctx->bs_ptr, "num_sps_ids_minus1");

    if (act->num_sps_ids_minus1 > 15) { /* 15 is vfmw_max val */
        dprint(PRN_ERROR, "act->num_sps_ids_minus1 > 15\n");
        return HEVC_DEC_ERR;
    }

    for (i = 0; i <= act->num_sps_ids_minus1; i++) {
        act->active_seq_parameter_set_id[i] = (td_s32)hevc_ue_v(ctx->bs_ptr, "active_seq_parameter_set_id");
    }

    bs_skip_to_next_byte(ctx->bs_ptr);

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_sei_parse_buf_nal_vcl_inf(const hevc_ctx *ctx, hevc_sei_buf_period *buf,
    const hevc_hrd_param_set *hrd)
{
    td_u32 i;
    td_u32 j;
    td_s32 flag;

    for (j = 0; j < 2; j++) { /* 2 is vfmw_max num */
        flag =  (((j == 0) && (hrd->nal_hrd_parameters_present_flag)) ||
            ((j == 1) && (hrd->vcl_hrd_parameters_present_flag)));
        if (!flag) {
            continue;
        }

        for (i = 0; i < (hrd->hrd_sublayer[0].cpb_cnt_minus1 + 1); i++) {
            buf->nal_vcl_inf[i][j].initial_cpb_removal_delay = hevc_u_v(ctx->bs_ptr,
                hrd->initial_cpb_removal_delay_length_minus1 + 1, "nal_initial_cpb_removal_delay");
            buf->nal_vcl_inf[i][j].initial_cpb_removal_offset = hevc_u_v(ctx->bs_ptr,
                hrd->initial_cpb_removal_delay_length_minus1 + 1, "nal_initial_cpb_removal_offset");

            if (hrd->sub_pic_hrd_params_present_flag || buf->irap_cpb_params_present_flag) {
                buf->nal_vcl_inf[i][j].initial_alt_cpb_removal_delay = hevc_u_v(ctx->bs_ptr,
                    hrd->initial_cpb_removal_delay_length_minus1 + 1, "nal_initial_alt_cpb_removal_delay");
                buf->nal_vcl_inf[i][j].initial_alt_cpb_removal_offset = hevc_u_v(ctx->bs_ptr,
                    hrd->initial_cpb_removal_delay_length_minus1 + 1, "nal_initial_cpb_alt_removal_offset");
            }
        }
    }

    return;
}

STATIC td_s32 hevc_sei_buffer_period(hevc_ctx *ctx, hevc_sei_buf_period *buf, hevc_hrd_param_set *hrd)
{
    ctx->pts_parser.bp_num++;
    ctx->pts_parser.last_gop_size = ctx->pts_parser.sei_timing_num_sub;

    buf->bp_seq_parameter_set_id = hevc_ue_v(ctx->bs_ptr, "bp_seq_parameter_set_id");

    if ((buf->bp_seq_parameter_set_id) > 15) { /* 15 is vfmw_max val */
        dprint(PRN_ERROR, "Wrong bp_seq_parameter_set_id = %d, should be 0 to 15!\n", buf->bp_seq_parameter_set_id);
        return HEVC_DEC_ERR;
    }

    if (!hrd->sub_pic_hrd_params_present_flag) {
        buf->irap_cpb_params_present_flag = hevc_u_v(ctx->bs_ptr, 1, "irap_cpb_params_present_flag");
    }

    if (buf->irap_cpb_params_present_flag) {
        buf->cpb_delay_offset = hevc_u_v(ctx->bs_ptr, hrd->au_cpb_removal_delay_length_minus1 + 1, "cpb_delay_offset");
        buf->dpb_delay_offset = hevc_u_v(ctx->bs_ptr, hrd->dpb_output_delay_length_minus1 + 1, "dpb_delay_offset");
    }

    buf->concatenation_flag = hevc_u_v(ctx->bs_ptr, 1, "concatenation_flag");
    buf->au_cpb_removal_delay_delta_minus1 =
        hevc_u_v(ctx->bs_ptr, hrd->au_cpb_removal_delay_length_minus1 + 1, "au_cpb_removal_delay_delta_minus1") + 1;

    if (hrd->hrd_sublayer[0].cpb_cnt_minus1 > 31) { /* 31 is vfmw_max val */
        dprint(PRN_ERROR, "hevc_sei_buffer_period cpb_cnt_minus1(%d) out of range(0,31).\n",
               hrd->hrd_sublayer[0].cpb_cnt_minus1);
        hrd->hrd_sublayer[0].cpb_cnt_minus1 = 0; /* 赋值强解 */
    }

    hevc_sei_parse_buf_nal_vcl_inf(ctx, buf, hrd);

    bs_skip_to_next_byte(ctx->bs_ptr);

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sei_pic_num_decoding_units_minus1(const hevc_ctx *ctx, hevc_sei_pic_timming *pic,
    const hevc_hrd_param_set *hrd)
{
    td_u32 i;

    pic->num_decoding_units_minus1 = hevc_ue_v(ctx->bs_ptr, "num_decoding_units_minus1");
    pic->du_common_cpb_removal_delay_flag = hevc_u_v(ctx->bs_ptr, 1, "du_common_cpb_removal_delay_flag");

    if (pic->du_common_cpb_removal_delay_flag) {
        pic->du_common_cpb_removal_delay_increment_minus1 = hevc_u_v(ctx->bs_ptr,
            hrd->du_cpb_removal_delay_increment_length_minus1 + 1, "du_common_cpb_removal_delay_minus1");
    }

    if (pic->num_decoding_units_minus1 > 255) { /* 255 is vfmw_max val */
        dprint(PRN_ERROR, "pic->num_decoding_units_minus1 > 255\n");
        return HEVC_DEC_ERR;
    }

    for (i = 0; i <= pic->num_decoding_units_minus1; i++) {
        pic->num_nalus_in_du_minus1[i] = hevc_ue_v(ctx->bs_ptr, "num_nalus_in_du_minus1");

        if (!pic->du_common_cpb_removal_delay_flag) {
            pic->du_cpb_removal_delay_increment_minus1[i] = hevc_u_v(ctx->bs_ptr,
                hrd->du_cpb_removal_delay_increment_length_minus1 + 1, "increment_length_minus1");
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sei_picture_timming(hevc_ctx *ctx, hevc_sei_pic_timming *pic, const hevc_vui_param_set *vui)
{
    const hevc_hrd_param_set *hrd = &(vui->hrd_parameters);

    if (vui->frame_field_info_present_flag) {
        pic->pic_struct = hevc_u_v(ctx->bs_ptr, 4, "pic_struct"); /* 4 is bit num */
        pic->source_scan_type = hevc_u_v(ctx->bs_ptr, 2, "source_scan_type"); /* 2 is bit num */
        pic->duplicate_flag = hevc_u_v(ctx->bs_ptr, 1, "duplicate_flag");
    }

    if (hrd->nal_hrd_parameters_present_flag || hrd->vcl_hrd_parameters_present_flag) {
        pic->au_cpb_removal_delay_minus1 = hevc_u_v(ctx->bs_ptr,
            hrd->au_cpb_removal_delay_length_minus1 + 1, "au_cpb_removal_delay_minus1") + 1;
        pic->pic_dpb_output_delay = hevc_u_v(ctx->bs_ptr,
            hrd->dpb_output_delay_length_minus1 + 1, "pic_dpb_output_delay");

        if (hrd->sub_pic_hrd_params_present_flag) {
            pic->pic_dpb_output_du_delay = hevc_u_v(ctx->bs_ptr,
                hrd->dpb_output_delay_du_length_minus1 + 1, "pic_dpb_output_du_delay");
        }

        if (hrd->sub_pic_hrd_params_present_flag && hrd->sub_pic_cpb_params_in_pic_timing_sei_flag) {
            if (hevc_sei_pic_num_decoding_units_minus1(ctx, pic, hrd) != HEVC_DEC_NORMAL) {
                return HEVC_DEC_ERR;
            }
        }
    }

    bs_skip_to_next_byte(ctx->bs_ptr);

    return HEVC_DEC_NORMAL;
}

STATIC td_bool hevc_sei_usr_dat_unreg_unsupport_uuid(hevc_sei_user_data_unreg *data)
{
    td_u8 unsupport_uuid1[UUID_ISO_IEC_11578_LEN]  = {0xfa, 0x5e, 0xfe, 0x40, 0xa6, 0xaf, 0x11, 0xdd,
        0xa6, 0x59, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b};
    td_u8 unsupport_uuid2[UUID_ISO_IEC_11578_LEN]  = {0x68, 0x55, 0x98, 0x4e, 0x49, 0x9c, 0x45, 0xc5,
        0x8e, 0x5b, 0xf2, 0x7b, 0xd1, 0xd4, 0xac, 0xe6};

    if (os_memcmp(data->uuid_iso_iec_11578, unsupport_uuid1, UUID_ISO_IEC_11578_LEN) == 0) {
        return TD_TRUE;
    }

    if (os_memcmp(data->uuid_iso_iec_11578, unsupport_uuid2, UUID_ISO_IEC_11578_LEN) == 0) {
        return TD_TRUE;
    }
    return TD_FALSE;
}

STATIC td_bool hevc_sei_usr_dat_unreg_unsupport_userdata(td_u8 *user_data, td_u32 len)
{
    const td_u8 unsupport_userdata1[9]  = {0x44, 0x69, 0x76, 0x58, 0x20, 0x50, 0x6c, 0x75, 0x73}; /* 9 datalen */
    const td_u8 unsupport_userdata2[8]  = {0x44, 0x69, 0x76, 0x58, 0x50, 0x6c, 0x75, 0x73};    /* 8 datalen */
    const td_u8 unsupport_userdata3[9]  = {0x44, 0x69, 0x76, 0x58, 0x20, 0x48, 0x45, 0x56, 0x43};  /* 9 datalen */

    vfmw_unused(len);
    if (os_memcmp(user_data, unsupport_userdata1, sizeof(unsupport_userdata1)) == 0) {
        return TD_TRUE;
    }

    if (os_memcmp(user_data, unsupport_userdata2, sizeof(unsupport_userdata2)) == 0) {
        return TD_TRUE;
    }

    if (os_memcmp(user_data, unsupport_userdata3, sizeof(unsupport_userdata3)) == 0) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

STATIC td_s32 h265_parse_splice_info(hevc_ctx *ctx, td_u8 *user_data, td_u32 user_data_len)
{
    td_u32 data;
    td_u8 version;

    if (user_data_len < sizeof(h265_unregistered_splice_sei_v1)) {
        dprint(PRN_ERROR, "[%s][%d] user_data_len %d < %d is not support splice info\n", __func__, __LINE__,
            user_data_len, sizeof(h265_unregistered_splice_sei_v1));
        return HEVC_DEC_ERR;
    }

    version = user_data[0];
    ctx->splice_info.version = version;
    if (version == VFMW_SPLICE_VERSION_V1) {
        ctx->splice_info.format_type = ((user_data[1] & 0x3) == 0) ? VFMW_SPLICE_FORMAT_AYUV : VFMW_SPLICE_FORMAT_MAX;
        ctx->splice_info.packing_type =
            (((user_data[1] >> 2) & 0x3) == 0) ? VFMW_SPLICE_PACKING_TAB : VFMW_SPLICE_PACKING_MAX;
        data = user_data[3] & 0xf;
        data = data << 8 | user_data[2];
        ctx->splice_info.ori_width = data;
        data = user_data[4];
        data = data << 4 | ((user_data[3] & 0xf0) >> 4);
        ctx->splice_info.ori_height = data;
    } else {
        dprint(PRN_ERROR, "[%s][%d] version %d is not support\n", __func__, __LINE__, version);
        return HEVC_DEC_ERR;
    }

    if (ctx->splice_info.ori_width == 0 ||
        ctx->splice_info.ori_width > HEVC_MAX_PIX_WIDTH ||
        ctx->splice_info.ori_height == 0 ||
        ctx->splice_info.ori_height > HEVC_MAX_PIX_HEIGHT) {
        dprint(PRN_ERROR, "[%s][%d] ori_width %d max:%d | ori_height %d max:%d\n", __func__, __LINE__,
            ctx->splice_info.ori_width, HEVC_MAX_PIX_WIDTH, ctx->splice_info.ori_height, HEVC_MAX_PIX_HEIGHT);
        return HEVC_DEC_ERR;
    }

    if (ctx->splice_info.format_type == VFMW_SPLICE_FORMAT_AYUV &&
        ctx->splice_info.packing_type == VFMW_SPLICE_PACKING_TAB) {
        ctx->splice_info.type = VFMW_SPLICE_AYUV_TAB;
    } else {
        ctx->splice_info.type = VFMW_SPLICE_NONE;
        dprint(PRN_ERROR, "[%s][%d] format_type %d packing_type %d is not support!\n", __func__, __LINE__,
            ctx->splice_info.format_type, ctx->splice_info.packing_type);
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_bool hevc_check_splice_info(hevc_sei_user_data_unreg *data)
{
    if (os_memcmp(data->uuid_iso_iec_11578, g_hevc_splice_uuid, sizeof(g_hevc_splice_uuid)) == 0) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

STATIC td_s32 hevc_sei_usr_dat_unreg(hevc_ctx *ctx, hevc_sei_user_data_unreg *data, td_u32 payload_size)
{
    td_u32 i;
    td_s32 ret;
    td_u8 user_data_tmp;
    td_u8 user_data[UNREG_PLAYLOAD_LEN] = { 0 };
    td_u32 user_data_type_id, unsupport = 0;

    vfmw_check_ret(payload_size >= H265_UUID_SIZE, HEVC_DEC_ERR);

    for (i = 0; i < UUID_ISO_IEC_11578_LEN; i++) { /* 16 is array size */
        data->uuid_iso_iec_11578[i] = hevc_u_v(ctx->bs_ptr, 8, "uuid_iso_iec_11578"); /* 8 is bit num */
    }

    if (hevc_sei_usr_dat_unreg_unsupport_uuid(data) == TD_TRUE) {
        unsupport = 1;
        dprint(PRN_ERROR, "report unsupport codec.\n");
        report_unsupport(ctx->chan_id);
    }

    data->user_data_length = payload_size - H265_UUID_SIZE;

    if (!(data->user_data_length)) {
        data->user_data_payload_byte = 0;
        return HEVC_DEC_NORMAL;
    }

    for (i = 0; i < data->user_data_length; i++) {
        user_data_tmp = hevc_u_v(ctx->bs_ptr, 8, "user_data"); /* 8 is bit num */
        if (i < UNREG_PLAYLOAD_LEN) {
            user_data[i] = user_data_tmp;
        }
    }

    if (unsupport == 0) {
        if (hevc_sei_usr_dat_unreg_unsupport_userdata(user_data, UNREG_PLAYLOAD_LEN) == TD_TRUE) {
            dprint(PRN_ERROR, "report unsupport codec.\n");
            report_unsupport(ctx->chan_id);
        }
    }

    if (hevc_check_splice_info(data) == TD_TRUE) {
        ret = h265_parse_splice_info(ctx, user_data, vfmw_min(data->user_data_length, UNREG_PLAYLOAD_LEN));
        vfmw_check_ret(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR);
    }

    if (data->user_data_length > AFD_INFO_MIN_LENGTH_IN_BYTES) {
        user_data_type_id = *(td_u32 *)user_data;
        if (user_data_type_id == AFD_IDENTIFIER) {
            ctx->curr_afd_info.active_format_flag =
                (user_data[4] == AFD_ACTIVE_FORMAT_FLAG_BYTE_VALUE) ? 1 : 0; /* 4 is idx */
            if (ctx->curr_afd_info.active_format_flag) {
                ctx->curr_afd_info.active_format = (user_data[5] & 0x0F); /* 5 is idx */
            }
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_check_list0(const hevc_ctx *ctx)
{
    td_u32 i;
    td_s32 ret = 0;
    fsp_logic *logic_fs = TD_NULL;

    if (ctx->curr_slice.list_x_size[0] > 0) {
        for (i = 0; i < ctx->curr_slice.list_x_size[0]; i++) {
            if (ctx->list_x[0][i] == TD_NULL) {
                ret = 1;
                dprint(PRN_ERROR, "list_x[0][%d] = TD_NULL, list_x_size[0] = %d\n", i, ctx->curr_slice.list_x_size[0]);
                break;
            }

            if (ctx->list_x[0][i]->frame_store == TD_NULL) {
                ret = 1;
                dprint(PRN_ERROR, "list_x[0][%d]->frame_store = TD_NULL, list_x_size[0] = %d\n",
                       i, ctx->curr_slice.list_x_size[0]);
                break;
            }

            logic_fs = fsp_get_logic(ctx->chan_id, ctx->list_x[0][i]->frame_store->logic_fs_id);
            if (logic_fs == TD_NULL) {
                ret = 1;
                dprint(PRN_ERROR, "list_x[0][%d] frame_stor, logic_f invalid\n", i);
                break;
            }
        }
    }

    return ret;
}

/* 检查参考列表是否有错误 */
VFMW_ACC_SLICE_H265 STATIC td_s32 hevc_is_ref_list_wrong(const hevc_ctx *ctx)
{
    td_u32 i;
    td_s32 ret;
    fsp_logic *logic_fs = TD_NULL;

    /* 检查list 0 */
    ret = hevc_check_list0(ctx);
    /* 若list 0检查无误，而且list 1非空，继续检查list 1 */
    if ((ret == 0) && (ctx->curr_slice.list_x_size[0] > 0)) {
        for (i = 0; i < ctx->curr_slice.list_x_size[1]; i++) {
            if (ctx->list_x[1][i] == TD_NULL) {
                ret = 1;
                dprint(PRN_ERROR, "list_x[1][%d] = TD_NULL, list_x_size[1] = %d\n", i, ctx->curr_slice.list_x_size[0]);
                break;
            }

            if (ctx->list_x[1][i]->frame_store == TD_NULL) {
                ret = 1;
                dprint(PRN_ERROR, "list_x[1][%d]->frame_store = TD_NULL, list_x_size[1] = %d\n",
                       i, ctx->curr_slice.list_x_size[0]);
                break;
            }

            logic_fs = fsp_get_logic(ctx->chan_id, ctx->list_x[1][i]->frame_store->logic_fs_id);
            if (logic_fs == TD_NULL) {
                ret = 1;
                dprint(PRN_ERROR, "list_x[i][%d] logic_fs invalid\n", i);
                break;
            }
        }
    }

    ret = (ret == 1) ? HEVC_DEC_ERR : HEVC_DEC_NORMAL;

    return ret;
}

/* clear one reference frame from dpb, long ref->short ref */
STATIC td_s32 hevc_discard_ref_from_dpb(hevc_ctx *ctx)
{
#ifdef VFMW_INDUSTRY_FEATURES1_SUPPORT
    td_u32 i;

    hevc_remove_dummy_frame(ctx);
    if (ctx->dpb.used_size < ctx->dpb.size) {
        return HEVC_DEC_NORMAL;
    }

    for (i = 0; i < ctx->dpb.used_size; i++) {
        if (ctx->dpb.fs[i]->is_reference != 0 && ctx->dpb.fs[i]->frame.is_short_term == 1) {
            hevc_unmark_frame_store_ref(ctx, ctx->dpb.fs[i]);
            return HEVC_DEC_NORMAL;
        }
    }

    for (i = 0; i < ctx->dpb.used_size; i++) {
        if (ctx->dpb.fs[i]->is_reference != 0 && ctx->dpb.fs[i]->frame.is_long_term == 1) {
            hevc_unmark_frame_store_ref(ctx, ctx->dpb.fs[i]);
            return HEVC_DEC_NORMAL;
        }
    }
#else
    vfmw_unused(ctx);
#endif
    return HEVC_DEC_ERR;
}

STATIC td_s32 hevc_get_unref_pic_with_min_poc(const hevc_ctx *ctx, td_s32 *pos)
{
    td_u32 i;
    td_s32 poc = 0x7fffffff;

    *pos = -1;

    if (ctx->dpb.used_size > HEVC_MAX_DPB_NUM) {
        dprint(PRN_ERROR, "%s dpb.used_size(%d) > 17.\n", __func__, ctx->dpb.used_size);
        return HEVC_DEC_ERR;
    }

    for (i = 0; i < ctx->dpb.used_size; i++) {
        if ((poc > ctx->dpb.fs[i]->poc)
            &&  (ctx->dpb.fs[i]->is_displayed == 0)
            &&  (ctx->dpb.fs[i]->is_reference == 0)) {
            poc = ctx->dpb.fs[i]->poc;
            *pos = i;
        }
    }

    if (*pos == -1) {
        dprint(PRN_ERROR, "%s can't find such pic.\n", __func__);
        return HEVC_DEC_ERR;
    } else {
        dprint(PRN_WARN, "%s find pos=%d, poc=%d.\n", __func__, *pos, poc);
        return HEVC_DEC_NORMAL;
    }
}

STATIC td_s32 hevc_increase_dpb_size(hevc_ctx *ctx)
{
    td_s32 total_fs_num;
    td_s32 max_occpied_num;

    if (ctx->dpb.size >= HEVC_MAX_DPB_NUM) {
        dprint(PRN_ERROR, "%s dpb.size(%d) already reached vfmw_max(%d).\n", __func__, ctx->dpb.size, HEVC_MAX_DPB_NUM);
        ctx->dpb.size = HEVC_MAX_DPB_NUM;
        return HEVC_DEC_ERR;
    }

    total_fs_num = fsp_get_total_num(ctx->chan_id);

    max_occpied_num = fsp_get_post_num(ctx->chan_id, ctx->curr_pic.pic_width, ctx->curr_pic.pic_height);

    total_fs_num -= max_occpied_num;
    total_fs_num = total_fs_num > 0 ? total_fs_num : 0;

    if (total_fs_num > 0 && (td_u32)total_fs_num >= ctx->dpb.size + 1) {
        ctx->dpb.size++;
        dprint(PRN_REF, "%s increase dpb size to %d\n", __func__, ctx->dpb.size);
        return HEVC_DEC_NORMAL;
    } else {
        dprint(PRN_WARN, "%s no enough fs(total_fs_num=%d) to increase dpb size.\n", __func__, total_fs_num);
        return HEVC_DEC_ERR;
    }
}

STATIC td_s32 hevc_output_unref_pic_with_min_poc(hevc_ctx *ctx)
{
    td_s32 ret;
    td_s32 pos;
    td_u32 i;
    td_s32 poc;
    td_u32 view_id;

    ret = hevc_get_unref_pic_with_min_poc(ctx, &pos);
    if (ret != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "%s call hevc_get_unref_pic_with_min_poc failed\n", __func__);
        return HEVC_DEC_ERR;
    }

    view_id = (td_u32)ctx->dpb.fs[pos]->view_id;
    if (ctx->dpb.used_size >= ctx->dpb.size) {
        poc = ctx->dpb.fs[pos]->poc;
        for (i = 0; i < ctx->dpb.used_size; i++) {
            if (poc <= ctx->dpb.fs[i]->poc || ctx->dpb.fs[i]->is_displayed != 0) {
                continue;
            }

            ctx->dpb.fs[i]->is_displayed = 1;
            if (hevc_output_frm_to_vo(ctx, ctx->dpb.fs[i]) == HEVC_DEC_NORMAL) {
                ctx->last_display_poc[view_id] = ctx->dpb.fs[i]->poc;
            }
        }
    }

    ret = hevc_output_one_frm_from_dpb(ctx, pos);
    if (ret != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "%s call hevc_output_one_frm_from_dpb failed\n", __func__);
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_recover_dpb_process(hevc_ctx *ctx)
{
    td_s32 ret;

    /* 检测DPB size是否能容纳当前帧，否则需要查看是否允许动态递增 */
    while (ctx->dpb.used_size >= ctx->dpb.size) {
        dprint(PRN_REF, "%s: dpb.used_size(%d) >= dpb.size(%d), need to increase dpb size.\n",
               __func__, ctx->dpb.used_size, ctx->dpb.size);
        ret = hevc_increase_dpb_size(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_WARN, "%s call hevc_increase_dpb_size failed\n", __func__);
            break;
        }
    }

    /* 容错: 如DPB仍满，输出非参考帧中最小POC值的一帧，直到DPB有空闲允许当前帧解码 */
    while (ctx->dpb.used_size >= ctx->dpb.size) {
        dprint(PRN_WARN, "%s: dpb.used_size(%d) >= dpb.size(%d), output minpoc frame.\n",
               __func__, ctx->dpb.used_size, ctx->dpb.size);

        ret = hevc_output_unref_pic_with_min_poc(ctx);
        if (ret != HEVC_DEC_NORMAL) {
#ifdef VFMW_INDUSTRY_FEATURES1_SUPPORT
            ret = hevc_discard_ref_from_dpb(ctx);
            if (ret != HEVC_DEC_NORMAL) {
                dprint(PRN_ERROR, "%s call hevc_discard_ref_from_dpb failed\n", __func__);
                hevc_clear_dpb(ctx);
                hevc_print_dpb_state(ctx);

                return HEVC_DEC_ERR;
            }

            return HEVC_DEC_NORMAL;
#else
            dprint(PRN_ERROR, "%s call HEVC_OutputOneFrmFromDPB failed\n", __func__);
            hevc_print_dpb_state(ctx);
            hevc_clear_dpb(ctx);
            return HEVC_DEC_ERR;
#endif
        }
    }

    return HEVC_DEC_NORMAL;
}

#ifdef VFMW_MV_HEVC_SUPPORT
STATIC td_void hevc_update_ref_list_if_multi_layers(hevc_ctx *ctx)
{
    if (ctx->is_multi_layers == 1) {
        hevc_update_inter_layer_ref_list(ctx);
    }
}
#endif

STATIC td_s32 hevc_ref_pic_process(hevc_ctx *ctx)
{
    td_s32 ret;
    td_s32 lost_pos;
    td_u32 create_cnt = 0;

    hevc_remove_dummy_frame(ctx);

#ifdef VFMW_MV_HEVC_SUPPORT
    hevc_mark_inter_layer_ref_list(ctx);
#endif

    /* 检测所有帧存参考信息 */
    hevc_apply_reference_picture_set(ctx);

    /* 补帧前先清除DPB不用的帧存 */
    ret = hevc_remove_unused_frame_store(ctx);
    if (ret != HEVC_TRUE) {
        /* need do nothing */
    }

    /* 检测是否丢失参考帧 */
    while (hevc_check_ref_pics(ctx, &lost_pos) == 0) {
        dprint(PRN_ERROR, "Ref frame(poc %d) lost.\n", lost_pos);
        if (create_cnt++ > HEVC_MAX_DPB_NUM) {
            dprint(PRN_ERROR, "drop poc %d as can't create lost ref poc %d\n",
                   ctx->curr_slice.poc, lost_pos);
            return HEVC_DEC_ERR;
        }

        /* 如丢失，补充参考帧 */
        if (ctx->extra_data_ptr->out_err_thr != 0) {
            ret = hevc_create_lost_picture(ctx, lost_pos);
            if (ret != HEVC_DEC_NORMAL) {
                hevc_remove_dummy_frame(ctx);
                return HEVC_DEC_ERR;
            }
            continue;
        }

        if (ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_CRA) {
            dprint(PRN_ERROR, "cra poc %d may dec ok, lost ref frame(pod %d) \n", ctx->curr_slice.poc, lost_pos);
            break;
        }

        dprint(PRN_ERROR, "out_err_thr = %d,drop poc %d because lost ref frame(pod %d)\n",
               ctx->extra_data_ptr->out_err_thr, ctx->curr_slice.poc, lost_pos);
        return HEVC_DEC_ERR;
    }

#ifdef VFMW_MV_HEVC_SUPPORT
    hevc_update_ref_list_if_multi_layers(ctx);
#endif
    /* 更新参考帧列表信息(长期参考帧，前向参考，后向参考) */
    hevc_update_ref_list(ctx);
    hevc_update_lt_ref_list(ctx);

    /* 检测DPB状态，进行错误纠正 */
    ret = hevc_recover_dpb_process(ctx);
    if (ret != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "hevc_recover_dpb_process execute return error\n");
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_u32 hevc_get_pic_type(const hevc_ctx *ctx)
{
    td_u32 pic_type = HEVC_I_FRAME;

    if (hevc_is_i_unit(ctx->curr_slice.nal_unit_type) == 1) {
        pic_type = HEVC_I_FRAME;
    }

    if ((hevc_is_i_unit(ctx->curr_pic.nal_unit_type)) == 0) {
        switch (ctx->curr_slice.slice_type) {
            case HEVC_B_SLICE:
                pic_type = HEVC_B_FRAME;
                break;

            case HEVC_P_SLICE:
                if (pic_type != HEVC_B_FRAME) {
                    pic_type = HEVC_P_FRAME;
                }
                break;

            case HEVC_I_SLICE:
                if (pic_type != HEVC_B_FRAME
                    && pic_type != HEVC_P_FRAME) {
                    pic_type = HEVC_I_FRAME;
                }
                break;

            default:
                break;
        }
    }

    return pic_type;
}

VFMW_ACC_SLICE_H265 STATIC td_void hevc_pic_type_statistic(hevc_ctx *ctx)
{
    /* 如是6种特定nal unit中的一种，则该帧直接指定为I Frame，
       同时将当前帧的nal_unit_type指定为该类型 */
    if (hevc_is_i_unit(ctx->curr_slice.nal_unit_type) == 1) {
        ctx->curr_pic.nal_unit_type = ctx->curr_slice.nal_unit_type;
        ctx->curr_pic.pic_type = HEVC_I_FRAME;
    }

    /* 判定pic_type: 如该帧有指定的五种nal_unit_type之一，
       强制设为I帧，不做判断。否则按以下优先级设定帧类型
       B slice > P slice > I slice */
    if ((hevc_is_i_unit(ctx->curr_pic.nal_unit_type)) == 0) {
        switch (ctx->curr_slice.slice_type) {
            case HEVC_P_SLICE:
                if (ctx->curr_pic.pic_type != HEVC_B_FRAME) {
                    ctx->curr_pic.pic_type = HEVC_P_FRAME;
                }
                break;
            case HEVC_B_SLICE:
                ctx->curr_pic.pic_type = HEVC_B_FRAME;
                break;
            case HEVC_I_SLICE:
                if (ctx->curr_pic.pic_type != HEVC_B_FRAME
                    && ctx->curr_pic.pic_type != HEVC_P_FRAME) {
                    ctx->curr_pic.pic_type = HEVC_I_FRAME;
                }
                break;
            default:
                break;
        }
    }

    return;
}

STATIC td_s32 hevc_cra_output_frame(hevc_ctx *ctx)
{
    td_u32 i;
    td_s32 ret;
    td_s32 notdisplay = 0;

    for (i = 0; i < ctx->dpb.used_size; i++) {
        if (ctx->dpb.fs[i]->is_displayed == 0) {
            notdisplay++;
        }
    }

    while (hevc_remove_unused_frame_store(ctx) == HEVC_TRUE) {
    }

    /* output frames in POC order */
    while (notdisplay > 0) {
        for (i = 0; i < ctx->dpb.used_size; i++) {
            if (ctx->dpb.fs[i] != TD_NULL &&
                ctx->dpb.fs[i]->is_displayed == 0) {
                break;
            }
        }

        if (i < ctx->dpb.used_size) {
            ret = hevc_output_one_frm_from_dpb(ctx, i);
            if (ret == HEVC_DEC_ERR) {
                return HEVC_DEC_ERR;
            }
        }

        notdisplay--;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_get_max_reorder(const hevc_ctx *ctx)
{
    td_u32 max_reorder_pics = 0;
    hevc_pic_param_set *pps = TD_NULL;
    hevc_seq_param_set *sps = TD_NULL;
    /* now curr_slice is next frame first slice */
    const hevc_slice_segment_header *next_frm_slc = &ctx->curr_slice;
    td_u8 flag;

    /* random stream test: PPSId maybe out of range */
    flag = ((ctx->curr_slice.pic_parameter_set_id < 0) ||
            (ctx->curr_slice.pic_parameter_set_id >= (ctx->max_pps_num)));
    vfmw_check_ret_print(!flag, 0, "%s: pic_parameter_set_id(%d) out of range(0 %d)!\n",
                         __func__, ctx->curr_slice.pic_parameter_set_id, ctx->max_pps_num);

    pps = ctx->pps[ctx->curr_slice.pic_parameter_set_id];
    if (pps == TD_NULL) {
        return max_reorder_pics;
    }

    /* random stream test: SPSId maybe out of range */
    flag = (pps->seq_parameter_set_id < 0 ||
            pps->seq_parameter_set_id >= (ctx->max_sps_num));
    vfmw_check_ret_print(!flag, 0, "%s: seq_parameter_set_id(%d) out of range(0 %d)!\n",
                         __func__, pps->seq_parameter_set_id, ctx->max_sps_num);

    sps = ctx->sps[pps->seq_parameter_set_id];
    if (sps == TD_NULL) {
        return max_reorder_pics;
    }

#ifdef VFMW_HEVC_SLC_LDY_SUPPORT
    if (ctx->no_next_slice == 1) {
        td_u32 i;
        for (i = 0; i < HEVC_MAX_TEMPLAYER; i++) {
            if (max_reorder_pics < sps->num_reorder_pics[i]) {
                max_reorder_pics = sps->num_reorder_pics[i];
            }
        }
    } else
#endif
    {
        if (next_frm_slc->nuh_temporal_id >= HEVC_MAX_TEMPLAYER) {
            dprint(PRN_ERROR, "%s tmpid %d err\n", __func__, next_frm_slc->nuh_temporal_id);
            return max_reorder_pics;
        }

        max_reorder_pics = sps->num_reorder_pics[next_frm_slc->nuh_temporal_id];
    }

    return max_reorder_pics;
}

STATIC td_void hevc_not_standard_cra_output_frame_process(hevc_ctx *ctx)
{
    td_s32 max_reorder_pics;

    max_reorder_pics = hevc_get_max_reorder(ctx);
    if ((ctx->last_display_poc[ctx->curr_nal->nal_layer_id] != -HEVC_MAX_INT)
        &&  (ctx->curr_slice.poc < (td_s32)(ctx->last_display_poc[ctx->curr_nal->nal_layer_id] + max_reorder_pics))
        &&  (ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_CRA)) {
        dprint(PRN_ERROR, "Meet CRA frame, force output remain frame, cur_poc %d, LastPoc %d\n",
               ctx->curr_slice.poc, ctx->last_display_poc[ctx->curr_nal->nal_layer_id]);
        hevc_cra_output_frame(ctx);
        ctx->last_display_poc[ctx->curr_nal->nal_layer_id] = -HEVC_MAX_INT;
    }

    return;
}

STATIC td_void hevc_update_mulit_layer_ref(hevc_ctx *ctx)
{
#ifdef VFMW_MV_HEVC_SUPPORT
    if (ctx->is_multi_layers == 1) {
        hevc_update_inter_layer_ref_list(ctx);
    }
#else
    vfmw_unused(ctx);
#endif

    return;
}

STATIC td_s32 hevc_non_mode_i_process(hevc_ctx *ctx) {
    /* CNcomment: 在非I帧解码模式下，先检测DPB状态，进行补参考帧/递增DPB SIZE等动作 */
    if (ctx->extra_data_ptr->dec_mode != VFMW_DEC_MODE_I) {
        if (ctx->extra_data_ptr->output_order == VFMW_DEC_ORDER_SIMPLE_DPB) {
            /* CNcomment: 更新参考帧列表信息(长期参考帧，前向参考，后向参考) */
            hevc_update_mulit_layer_ref(ctx);
            hevc_update_ref_list(ctx);
            hevc_update_lt_ref_list(ctx);

            if (ctx->dpb.size != 0) {
                hevc_simple_slide_dpb(ctx);
            }
        } else if (ctx->dpb.size != 0) {
            if (hevc_ref_pic_process(ctx) != HEVC_DEC_NORMAL) {
                return HEVC_DEC_ERR;
            }
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_dec_i_mode_change_clear_dpb(hevc_ctx *ctx, td_u32 pic_type)
{
    td_u32 ipb_dec_mode = (ctx->advanced_cfg & 0x70000) >> 16; /* 16 is bit num */

    if ((pic_type == HEVC_I_FRAME) &&
        (ctx->old_dec_mode != ipb_dec_mode) &&
        (ctx->old_dec_mode == VFMW_DEC_MODE_I ||
        ctx->old_dec_mode == VFMW_DEC_MODE_FIRST_I) &&
        (ipb_dec_mode != VFMW_DEC_MODE_I) &&
        (ipb_dec_mode == VFMW_DEC_MODE_IP ||
        ipb_dec_mode == VFMW_DEC_MODE_NORMAL ||
        ipb_dec_mode == VFMW_DEC_MODE_DROP_INVALID_B)) {
        ctx->old_dec_mode = ipb_dec_mode;
        if (ctx->dpb.used_size != 0) {
            hevc_clear_dpb(ctx);
        }
    }

    return;
}

VFMW_ACC_SLICE_H265 STATIC td_s32 hevc_dec_slice_new_pic_process(hevc_ctx *ctx)
{
    td_s32 ret;
    td_u32 pic_type;
    td_u32 is_pvr_flush = 0;

    pic_type = hevc_get_pic_type(ctx);
#ifdef HEVC_PVR_SUPPORT
    is_pvr_flush = hevc_is_pvr_flush_pic(ctx, pic_type);
#endif
    /* 检测是否需要输出DPB中所有帧存 */
    if ((hevc_is_flush_unit(ctx->curr_slice.nal_unit_type) == 1 && ctx->curr_slice.view_id == 0) ||
        is_pvr_flush == 1) {
        /* 指示要刷出当前DPB中所有帧存 */
        if (hevc_flush_output_frm_from_dpb(ctx) != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "%s call hevc_flush_output_frm_from_dpb failed\n", __func__);
            return HEVC_DEC_ERR;
        }
    }

    /* 检测是否需要清空DPB中所有帧存 */
    if (ctx->no_output_of_prior_pics_flag[ctx->curr_slice.view_id] == HEVC_TRUE && (ctx->curr_slice.view_id == 0) &&
        ctx->dpb.used_size && hevc_empty_frm_from_dpb(ctx) != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "%s call hevc_empty_frm_from_dpb failed\n", __func__);
        return HEVC_DEC_ERR;
    }

    /* below is not standard operation, not activated in CRC mode. when meet CRA, output frame. */
    if (ctx->extra_data_ptr->crc_mode == 0) {
        hevc_not_standard_cra_output_frame_process(ctx);
    }

    ret = hevc_non_mode_i_process(ctx);
    if (ret != HEVC_DEC_NORMAL) {
        return ret;
    }

    hevc_dec_i_mode_change_clear_dpb(ctx, pic_type);

    ret = hevc_init_pic(ctx);
    if (ret == SYN_PARSE_RESOURCE_INVALID) {
        return SYN_PARSE_RESOURCE_INVALID;
    } else if (ret != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "Init pic err, exit\n");
        return HEVC_DEC_ERR;
    }

    ctx->total_pic_num++;
    hevc_set_scaling_list(ctx);

    /* save xps param first */
    ctx->set_dps_xps = 1;
    ret = hevc_write_pic_msg(ctx);
    ctx->set_dps_xps = 0;
    vfmw_check_ret_print(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR, "hevc_write_pic_msg failed!\n");

    return HEVC_DEC_NORMAL;
}

VFMW_ACC_SLICE_H265 STATIC td_s32 hevc_dec_slice_non_skip_pic_process(hevc_ctx *ctx)
{
    td_s32 ret;

    /* not skip frame */
    /* independent_slice ,一般为第一个slice */
    if (!ctx->curr_slice.dependent_slice_segment_flag) {
        /* one slice one RefList */
        /* 在storePicInDpb中已经进行了参考帧管理 */
        /* 标记了DPB.fs_negative_ref  dpb.fs_positive_ref */
        ret = hevc_dec_list(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "Dec List error, ret=%d\n", ret);
            return HEVC_DEC_ERR;
        }

        ret = hevc_is_ref_list_wrong(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            hevc_print_dpb_state(ctx);
            if (ctx->curr_nal != TD_NULL) {
                dprint(PRN_ERROR, "Ref List is Wrong! stream_id %d \n", ctx->curr_nal->stream[0].stream_id);
            }
            return HEVC_DEC_ERR;
        }

        ctx->curr_pic.pic_output_flag &= ctx->curr_slice.pic_output_flag;
    }

    if (hevc_check_list_x(ctx) != HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

    ret = hevc_write_slice_msg(ctx);
    if (ret == SYN_PARSE_RESOURCE_INVALID) {
        return SYN_PARSE_RESOURCE_INVALID;
    } else if (ret != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "%s err.\n", __func__);
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

VFMW_ACC_SLICE_H265 STATIC td_s32 hevc_dec_slice(hevc_ctx *ctx)
{
    td_s32 ret;

    if (hevc_vps_sps_pps_check(ctx) != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "hevc_dec_slice Check Xps Error\n");
        return HEVC_DEC_ERR;
    }

    ctx->curr_slice.nal_unit_type = ctx->curr_nal->nal_unit_type;
    ctx->curr_slice.nuh_temporal_id = ctx->curr_nal->nuh_temporal_id;
    ctx->curr_slice.view_id = ctx->curr_nal->nal_layer_id;

    /* 清除bNewSequence标志 */
    ctx->new_sequence = HEVC_FALSE;

    if (ctx->curr_slice.new_pic_type == IS_NEW_PIC && ctx->curr_pic.state == HEVC_PIC_EMPTY) {
        ret = hevc_dec_slice_new_pic_process(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            return ret;
        }
    }

    /* 容错: 防止当前消息池槽位已满溢出。 */
    if (ctx->num_slice_para >= (td_u32)(ctx->max_slice_num)) {
        dprint(PRN_ERROR, "Too many slice %d >= %d\n", ctx->num_slice_para, ctx->max_slice_num);
        return HEVC_DEC_ERR;
    }

    /* 容错: 如果该帧未正常走InitPic分配资源，该帧所有slice都应该丢弃不解。 */
    if (ctx->curr_pic.state == HEVC_PIC_EMPTY) {
        /*******************************************************************************
          在vps不足的时候，一定概率下新的一帧码流的第一个slice会因为vps错误导致解码错误丢掉，
          第二个slice解码正确，这时候如果再碰上HEVC_AllocFrameStore失败，第二个slice就会要求重送，
          而且会更新prev_pic_parameter_set_id和pre_poc。因为第一个slice已经丢掉，vfmw判断当前slice
          是否是新的一帧的开始不能使用first_slice_segment_in_pic_flag,只能判断当前slice的pic_parameter_set_id
          或poc是否与前一个slice的pic_parameter_set_id或poc不一致。但因为第二个slice已经重送而且
          已经更新prev_pic_parameter_set_id和pre_poc，这样第二个slice就不会被认为是新的一帧的slice，
          不会进入HEVC_InitPic函数，这样第二个slice的重送标记s32SendStreamAgainFlag就无法清除，
          这样第二个slice就会不停重送，导致解码停止，SCD会不停强制释放码流。
          *******************************************************************************/
        ctx->have_nal_not_decoded = 0;
        dprint(PRN_ERROR, "Cur pic not init yet, discard this slice!\n");
        return HEVC_DEC_ERR;
    }

    /* 帧类型判断 */
    hevc_pic_type_statistic(ctx);

    /* 码流信息计算 */
    hevc_calc_stream_bits(ctx);

    /* 本来是写消息池的，现在先用全局数组代替 */
    if (ctx->curr_slice.new_pic_type != IS_SKIP_PIC) {
        ret = hevc_dec_slice_non_skip_pic_process(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            return ret;
        }
    } else {
        /* skip帧nal可以直接释放，否则 nal slot可能不足 */
        hevc_clear_curr_nal(ctx);
    }

    ctx->prev_slc_pps_id = ctx->curr_slice.pic_parameter_set_id;
    ctx->prev_poc = (td_u32)ctx->curr_slice.poc;
    ctx->allow_start_dec = 1;

    ctx->curr_pic.sei_field_flag = ctx->sei.pic_timming.pic_struct;
    ctx->curr_pic.sei_source_scan_type = ctx->sei.pic_timming.source_scan_type;

    return HEVC_DEC_NORMAL;
}

static inline td_u8 hevc_is_reference_nalu(const hevc_ctx *ctx)
{
    vfmw_check_ret(ctx != TD_NULL, 0);

    return (td_u8)(((ctx->curr_slice.nal_unit_type <= NAL_UNIT_RESERVED_VCL_R15) &&
                    (ctx->curr_slice.nal_unit_type % 2 != 0)) || /* 2 is num */
                   ((ctx->curr_slice.nal_unit_type >= NAL_UNIT_CODED_SLICE_BLA_W_LP) &&
                    (ctx->curr_slice.nal_unit_type <= NAL_UNIT_RESERVED_IRAP_VCL23)));
}

/*************************************************************************
* 函数原型 : hevc_slice_segment_header(hevc_ctx *ctx)
* 功能描述 : 解析slice信息，第二个参数决定是否对pHevcCtx->bNewSequence赋值
* 参数说明 : IsDecSlice
             0 InquireSliceProperty Call
             1 DecSlice Call
* 返回值   ：
*************************************************************************/
STATIC td_void hevc_dec_slice_segment_flag(hevc_ctx *ctx, hevc_pic_param_set *pps)
{
    if ((pps->dependent_slice_segments_enabled_flag) && (!ctx->curr_slice.first_slice_segment_in_pic_flag)) {
        ctx->curr_slice.dependent_slice_segment_flag = hevc_u_v(ctx->bs_ptr, 1, "dependent_slice_segment_flag");
    } else {
        ctx->curr_slice.dependent_slice_segment_flag = HEVC_FALSE;
    }
}

STATIC td_s32 hevc_dec_slice_segment_address(hevc_ctx *ctx, td_s32 req_bits_inner,
    td_s32 req_bits_outer, td_u32 *icu_address, td_u32 *inner_address)
{
    td_u32 lcu_addr = 0, inner_addr = 0;
    hevc_pic_param_set *pps;
    hevc_seq_param_set *sps;
    td_u8 flag = 0;

    pps = ctx->pps[ctx->curr_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];

    if (!ctx->curr_slice.first_slice_segment_in_pic_flag) {
        ctx->curr_slice.slice_segment_address =
            (td_s32)hevc_u_v(ctx->bs_ptr, req_bits_inner + req_bits_outer, "slice_segment_address");

        vfmw_check_ret(sps->ctb_num_width < HEVC_MAX_PIX_WIDTH, HEVC_DEC_ERR);
        vfmw_check_ret(sps->ctb_num_height < HEVC_MAX_PIX_HEIGHT, HEVC_DEC_ERR);

        flag = (ctx->curr_slice.slice_segment_address < 0 ||
                (td_u32)(ctx->curr_slice.slice_segment_address) >= (sps->ctb_num_width) * (sps->ctb_num_height));
        vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "slice_segment_address(%d) out of range(0,%d).\n",
                             ctx->curr_slice.slice_segment_address, (sps->ctb_num_width) * (sps->ctb_num_height));

        lcu_addr = (td_u32)ctx->curr_slice.slice_segment_address >> (td_u32)req_bits_inner;
        inner_addr = (td_u32)(ctx->curr_slice.slice_segment_address - (lcu_addr << (td_u32)req_bits_inner));

        *icu_address = lcu_addr;
        *inner_address = inner_addr;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_short_term_rps(hevc_ctx *ctx, const hevc_seq_param_set *sps)
{
    td_u32 num_bits;
    td_s32 ret;
    td_u8 flag;

    num_bits = 0;
    ctx->curr_slice.short_term_ref_pic_set_sps_flag = hevc_u_v(ctx->bs_ptr, 1, "short_term_ref_pic_set_sps_flag");

    if (!ctx->curr_slice.short_term_ref_pic_set_sps_flag) {
        ret = hevc_dec_short_term_ref_pic_set(ctx, sps,
            &(ctx->curr_slice.short_term_ref_pic_set), sps->num_short_term_ref_pic_sets);
        vfmw_check_ret_print(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR, "SH hevc_dec_short_term_ref_pic_set error.\n");
    } else {
        while ((1u << num_bits) < sps->num_short_term_ref_pic_sets) {
            num_bits++;
        }

        if (num_bits > 0) {
            ctx->curr_slice.short_term_ref_pic_set_idx =
                (td_s32)hevc_u_v(ctx->bs_ptr, num_bits, "short_term_ref_pic_set_idx");
        } else {
            ctx->curr_slice.short_term_ref_pic_set_idx = 0;
        }

        flag = (ctx->curr_slice.short_term_ref_pic_set_idx < 0 ||
                (td_u32)(ctx->curr_slice.short_term_ref_pic_set_idx) > sps->num_short_term_ref_pic_sets - 1);
        vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "short_term_ref_pic_set_idx(%d) out of range[0,%d].\n",
                             ctx->curr_slice.short_term_ref_pic_set_idx, sps->num_short_term_ref_pic_sets - 1);

        ctx->curr_slice.short_term_ref_pic_set =
            sps->short_term_ref_pic_set[ctx->curr_slice.short_term_ref_pic_set_idx];
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_get_long_term_param(hevc_ctx *ctx, const hevc_seq_param_set *sps,
                                                hevc_short_term_rpset *rps, td_u32 offset, td_u32 bfltp_in_sps)
{
    td_u8 flag = 0;
    td_u32 i = 0, j = 0;
    td_s32 poc_lsd_lt = 0, lt_idx_sps = 0;
    td_s32 prev_delta_msb = 0, tmp_delta_poc_msb_cycle_lt = 0;

    for (i = 0, j = offset + rps->num_of_longterm_pics - 1;
         i < rps->num_of_longterm_pics && i < HEVC_MAX_NUM_REF_PICS; j--, i++) {
        if (i < ctx->curr_slice.num_long_term_sps) {
            lt_idx_sps = 0;

            if (bfltp_in_sps) {
                ctx->curr_slice.lt_idx_sps[i] = (td_s32)hevc_u_v(ctx->bs_ptr, bfltp_in_sps, "lt_idx_sps");
                lt_idx_sps = ctx->curr_slice.lt_idx_sps[i];

                flag = (lt_idx_sps < 0 || (td_u32)lt_idx_sps > sps->num_long_term_ref_pic_sps - 1);
                vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "lt_idx_sps[%d] = %d, but out of range[0,%d].\n",
                                     i, lt_idx_sps, sps->num_long_term_ref_pic_sps - 1);
            }

            poc_lsd_lt = sps->lt_ref_pic_poc_lsb_sps[lt_idx_sps];
            rps->used_flag[j] = sps->used_by_curr_pic_lt_sps_flag[lt_idx_sps];
        } else {
            ctx->curr_slice.poc_lsb_lt[i] = (td_s32)hevc_u_v(ctx->bs_ptr, sps->bits_for_poc, "poc_lsb_lt");
            poc_lsd_lt = ctx->curr_slice.poc_lsb_lt[i];
            ctx->curr_slice.used_by_curr_pic_lt_flag[i] = hevc_u_v(ctx->bs_ptr, 1, "used_by_curr_pic_lt_flag");
            rps->used_flag[j] = ctx->curr_slice.used_by_curr_pic_lt_flag[i];
        }

        ctx->curr_slice.poclsb_lt[i] = poc_lsd_lt;
        ctx->curr_slice.delta_poc_msb_present_flag[i] = hevc_u_v(ctx->bs_ptr, 1, "delta_poc_msb_present_flag");

        if (ctx->curr_slice.delta_poc_msb_present_flag[i]) {
            ctx->curr_slice.delta_poc_msb_cycle_lt[i] = (td_s32)hevc_ue_v(ctx->bs_ptr, "delta_poc_msb_cycle_lt");

            flag = (i == 0 || i == ctx->curr_slice.num_long_term_sps);
            tmp_delta_poc_msb_cycle_lt = flag ? ctx->curr_slice.delta_poc_msb_cycle_lt[i] :
                (ctx->curr_slice.delta_poc_msb_cycle_lt[i] + prev_delta_msb);

            ctx->curr_slice.delta_poc_msbc_ycle_lt[j] = tmp_delta_poc_msb_cycle_lt;

            ctx->curr_slice.check_lt_msb[j] = 1;
        } else {
            tmp_delta_poc_msb_cycle_lt = (i == 0 || i == ctx->curr_slice.num_long_term_sps) ? 0 : prev_delta_msb;
            ctx->curr_slice.check_lt_msb[j] = 0;
        }

        prev_delta_msb = tmp_delta_poc_msb_cycle_lt;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_calc_long_term_rps(const hevc_ctx *ctx, hevc_short_term_rpset *rps,
                                    td_u32 offset, td_u32 max_poc_lsb, td_u32 poc_lsb)
{
    td_u32 i = 0, j = 0;
    td_s32 poc_lt_curr = 0;

    for (i = 0, j = offset + rps->num_of_longterm_pics - 1;
         i < rps->num_of_longterm_pics && i < HEVC_MAX_NUM_REF_PICS; j--, i++) {
        if (ctx->curr_slice.delta_poc_msb_present_flag[i]) {
            vfmw_check(ctx->curr_slice.delta_poc_msbc_ycle_lt[j] < VFMW_MAX_SINT32);
            vfmw_check(max_poc_lsb < VFMW_MAX_UINT32);

            poc_lt_curr = (td_s32)((td_u32)ctx->curr_slice.poc - (td_u32)ctx->curr_slice.delta_poc_msbc_ycle_lt[j] *
                            max_poc_lsb - poc_lsb + (td_u32)ctx->curr_slice.poclsb_lt[i]);
            rps->poc[j] = poc_lt_curr;

            rps->delta_poc[j] = poc_lt_curr - ctx->curr_slice.poc;
        } else {
            rps->poc[j] = ctx->curr_slice.poclsb_lt[i];
            rps->delta_poc[j] = ctx->curr_slice.poclsb_lt[i] - ctx->curr_slice.poc;
        }
    }

    return;
}

STATIC td_s32 hevc_dec_long_term_rps(hevc_ctx *ctx, const hevc_seq_param_set *sps,
                                     hevc_short_term_rpset *rps, td_u32 max_poc_lsb, td_u32 poc_lsb)
{
    td_s32 offset;
    td_u32 bits_for_long_term_pic_in_sps = 0;
    td_u8 flag = 0;
    td_s32 ret;

    if (sps->long_term_ref_pics_present_flag) {
        offset = (td_s32)(rps->num_negative_pics + rps->num_positive_pics);

        if (sps->num_long_term_ref_pic_sps > 0) {
            ctx->curr_slice.num_long_term_sps = hevc_ue_v(ctx->bs_ptr, "num_long_term_sps");
            flag = (ctx->curr_slice.num_long_term_sps <= HEVC_MAX_NUM_REF_PICS);
            vfmw_check_ret_print(flag, HEVC_DEC_ERR, "num_long_term_sps(%d) out of range[0,%d].\n",
                                 ctx->curr_slice.num_long_term_sps, HEVC_MAX_NUM_REF_PICS);

            flag = (ctx->curr_slice.num_long_term_sps > sps->num_long_term_ref_pic_sps);
            vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "num_long_term_sps(%d) out of range[0,%d].\n",
                                 ctx->curr_slice.num_long_term_sps, sps->num_long_term_ref_pic_sps);
        }

        bits_for_long_term_pic_in_sps = 0;
        while (sps->num_long_term_ref_pic_sps > ((td_u32)(1 << bits_for_long_term_pic_in_sps))) {
            bits_for_long_term_pic_in_sps++;
        }

        ctx->curr_slice.num_long_term_pics = hevc_ue_v(ctx->bs_ptr, "num_long_term_pics");
        flag = (ctx->curr_slice.num_long_term_pics <= HEVC_MAX_NUM_REF_PICS);
        vfmw_check_ret_print(flag, HEVC_DEC_ERR, "num_long_term_pics(%d) out of range[0,%d].\n",
                             ctx->curr_slice.num_long_term_pics, HEVC_MAX_NUM_REF_PICS);

        rps->num_of_longterm_pics = ctx->curr_slice.num_long_term_sps + ctx->curr_slice.num_long_term_pics;

        flag = (((td_u32)offset + rps->num_of_longterm_pics) > HEVC_MAX_NUM_REF_PICS);
        vfmw_check_ret_print(!flag, HEVC_DEC_ERR,
            "SH ERROR: (num_negative_pics(%d) + num_positive_pics(%d) + num_of_longterm_pics(%d)) > %d\n",
            rps->num_negative_pics, rps->num_positive_pics, rps->num_of_longterm_pics, HEVC_MAX_NUM_REF_PICS);

        ret = hevc_dec_get_long_term_param(ctx, sps, rps, offset, bits_for_long_term_pic_in_sps);
        vfmw_check_ret(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR);

        hevc_calc_long_term_rps(ctx, rps, offset, max_poc_lsb, poc_lsb);

        rps->num_of_pics = (td_u32)offset + rps->num_of_longterm_pics;

        flag = (rps->num_of_pics > HEVC_MAX_NUM_REF_PICS);
        vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "ERROR: rps->num_of_pics(%d) > %d.\n",
                             rps->num_of_pics, HEVC_MAX_NUM_REF_PICS);
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_num_ref_idx_of_list(hevc_ctx *ctx, const hevc_pic_param_set *pps)
{
    td_u8 flag;
    td_u32 tmp;

    if (ctx->curr_slice.slice_type != HEVC_I_SLICE) {
        ctx->curr_slice.num_ref_idx_active_override_flag = hevc_u_v(ctx->bs_ptr, 1, "num_ref_idx_active_override_flag");

        if (ctx->curr_slice.num_ref_idx_active_override_flag) {
            tmp = hevc_ue_v(ctx->bs_ptr, "num_ref_idx_l0_active_minus1");
            if (tmp > HEVC_MAX_UINT - 1) {
                dprint(PRN_ERROR, "num_ref_idx_l0_active_minus1 overflow!\n");
                return HEVC_DEC_ERR;
            }

            ctx->curr_slice.num_ref_idx_l0_active =  tmp + 1;

            flag = (ctx->curr_slice.num_ref_idx_l0_active > HEVC_MAX_NUM_REF_PICS);
            vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "num_ref_idx_l0_active(%d) out of range(0,%d).\n",
                                 ctx->curr_slice.num_ref_idx_l0_active, HEVC_MAX_NUM_REF_PICS);

            ctx->curr_slice.num_ref_idx[0] = ctx->curr_slice.num_ref_idx_l0_active;

            if (ctx->curr_slice.slice_type == HEVC_B_SLICE) {
                ctx->curr_slice.num_ref_idx_l1_active = hevc_ue_v(ctx->bs_ptr, "num_ref_idx_l1_active_minus1") + 1;

                flag = (ctx->curr_slice.num_ref_idx_l1_active > HEVC_MAX_NUM_REF_PICS);
                vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "num_ref_idx_l1_active(%d) out of range(0,%d).\n",
                                     ctx->curr_slice.num_ref_idx_l1_active, HEVC_MAX_NUM_REF_PICS);

                ctx->curr_slice.num_ref_idx[1] = ctx->curr_slice.num_ref_idx_l1_active;
            } else {
                ctx->curr_slice.num_ref_idx_l1_active = 0;
                ctx->curr_slice.num_ref_idx[1] = 0;
            }
        } else {
            ctx->curr_slice.num_ref_idx[0] = pps->num_ref_idx_l0_default_active;

            flag = (td_u8)(ctx->curr_slice.slice_type == HEVC_B_SLICE);
            ctx->curr_slice.num_ref_idx[1] = flag ? pps->num_ref_idx_l1_default_active : 0;
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_collocated_ref_idx(hevc_ctx *ctx)
{
    td_u32 max_collocated_ref_idx;
    td_u8 flag;

    if (ctx->curr_slice.slice_temporal_mvp_enable_flag) {
        if (ctx->curr_slice.slice_type == HEVC_B_SLICE) {
            ctx->curr_slice.collocated_from_l0_flag = hevc_u_v(ctx->bs_ptr, 1, "collocated_from_l0_flag");
        } else {
            ctx->curr_slice.collocated_from_l0_flag = 1;
        }

        flag = ((ctx->curr_slice.slice_type != HEVC_I_SLICE) &&
                ((ctx->curr_slice.collocated_from_l0_flag && (ctx->curr_slice.num_ref_idx[0] > 1)) ||
                 (!ctx->curr_slice.collocated_from_l0_flag && (ctx->curr_slice.num_ref_idx[1] > 1))));

        if (flag) {
            ctx->curr_slice.collocated_ref_idx = (td_s32)hevc_ue_v(ctx->bs_ptr, "collocated_ref_idx");

            if (ctx->curr_slice.collocated_from_l0_flag) {
                max_collocated_ref_idx = ctx->curr_slice.num_ref_idx_l0_active - 1;
            } else {
                max_collocated_ref_idx = ctx->curr_slice.num_ref_idx_l1_active - 1;
            }

            flag = (ctx->curr_slice.collocated_ref_idx < 0 ||
                    (td_u32)(ctx->curr_slice.collocated_ref_idx) > max_collocated_ref_idx ||
                    ctx->curr_slice.collocated_ref_idx >= HEVC_MAX_NUM_REF_PICS);
            vfmw_check_ret_print(!flag, HEVC_DEC_ERR,
                "collocated_from_l0_flag=%d, collocated_ref_idx(%d) out of range(0,%d).\n",
                ctx->curr_slice.collocated_from_l0_flag, ctx->curr_slice.collocated_ref_idx, max_collocated_ref_idx);
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_slice_chroma_qp_offset(hevc_ctx *ctx, const hevc_pic_param_set *pps)
{
    td_u8 flag;

    if (pps->pic_slice_chroma_qp_offsets_present_flag) {
        ctx->curr_slice.slice_cb_qp_offset = hevc_se_v(ctx->bs_ptr, "slice_cb_qp_offset");

        flag = (ctx->curr_slice.slice_cb_qp_offset < -12 || /* -12 is vfmw_min val */
                ctx->curr_slice.slice_cb_qp_offset > 12); /* 12 is vfmw_max val */
        vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "SH slice_cb_qp_offset(%d) out of range[-12,12].\n",
                             ctx->curr_slice.slice_cb_qp_offset);

        flag = ((ctx->curr_slice.slice_cb_qp_offset + pps->pic_cb_qp_offset) < -12 || /* -12 is vfmw_min val */
                (ctx->curr_slice.slice_cb_qp_offset + pps->pic_cb_qp_offset) > 12); /* 12 is vfmw_max val */
        vfmw_check_ret_print(!flag, HEVC_DEC_ERR,
                             "SH slice_cb_qp_offset(%d)+pic_cb_qp_offset(%d) out of range[-12,12].\n",
                             ctx->curr_slice.slice_cb_qp_offset, pps->pic_cb_qp_offset);

        ctx->curr_slice.slice_cr_qp_offset = hevc_se_v(ctx->bs_ptr, "slice_cr_qp_offset");

        flag = (ctx->curr_slice.slice_cr_qp_offset < -12 || /* -12 is vfmw_min val */
                ctx->curr_slice.slice_cr_qp_offset > 12); /* 12 is vfmw_max val */
        vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "SH slice_cr_qp_offset(%d) out of range[-12,12].\n",
                             ctx->curr_slice.slice_cr_qp_offset);

        flag = ((ctx->curr_slice.slice_cr_qp_offset + pps->pic_cr_qp_offset) < -12 || /* -12 is vfmw_min val */
                (ctx->curr_slice.slice_cr_qp_offset + pps->pic_cr_qp_offset) > 12); /* 12 is vfmw_max val */
        vfmw_check_ret_print(!flag, HEVC_DEC_ERR,
                             "SH slice_cr_qp_offset+pps->pic_cr_qp_offset(%d) out of range[-12,12].\n",
                             (ctx->curr_slice.slice_cr_qp_offset + pps->pic_cr_qp_offset));
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_deblock_filter(hevc_ctx *ctx, const hevc_pic_param_set *pps)
{
    if (pps->deblocking_filter_control_present_flag == 0) {
        return;
    }

    if (pps->deblocking_filter_override_enabled_flag) {
        ctx->curr_slice.deblocking_filter_override_flag = hevc_u_v(ctx->bs_ptr, 1, "deblocking_filter_override_flag");
    } else {
        ctx->curr_slice.deblocking_filter_override_flag = 0;
    }

    if (ctx->curr_slice.deblocking_filter_override_flag) {
        ctx->curr_slice.slice_disable_deblocking_filter_flag =
            hevc_u_v(ctx->bs_ptr, 1, "slice_disable_deblocking_filter_flag");

        if (!ctx->curr_slice.slice_disable_deblocking_filter_flag) {
            ctx->curr_slice.slice_beta_offset_div2 = hevc_se_v(ctx->bs_ptr, "slice_beta_offset_div2");

            if (ctx->curr_slice.slice_beta_offset_div2 < -6 || /* -6 is vfmw_min val */
                ctx->curr_slice.slice_beta_offset_div2 > 6) { /* 6 is vfmw_max val */
                dprint(PRN_ERROR, "SH slice_beta_offset_div2(%d) out of range(-6,6).\n",
                       ctx->curr_slice.slice_beta_offset_div2);
                ctx->curr_slice.slice_beta_offset_div2 = pps->pps_beta_offset_div2; /* 赋值强解 */
                /* 考虑钳位继续解 */
            }

            ctx->curr_slice.slice_tc_offset_div2 = hevc_se_v(ctx->bs_ptr, "slice_tc_offset_div2");

            if (ctx->curr_slice.slice_tc_offset_div2 < -6 || /* -6 is vfmw_min val */
                ctx->curr_slice.slice_tc_offset_div2 > 6) { /* 6 is vfmw_max val */
                dprint(PRN_ERROR, "SH slice_tc_offset_div2(%d) out of range(-6,6).\n",
                       ctx->curr_slice.slice_tc_offset_div2);
                ctx->curr_slice.slice_tc_offset_div2 = pps->pps_tc_offset_div2; /* 赋值强解 */
                /* 考虑钳位继续解 */
            }
        }
    } else {
        ctx->curr_slice.slice_disable_deblocking_filter_flag = pps->pic_disable_deblocking_filter_flag;
        ctx->curr_slice.slice_beta_offset_div2 = pps->pps_beta_offset_div2;
        ctx->curr_slice.slice_tc_offset_div2 = pps->pps_tc_offset_div2;
    }

    return;
}

STATIC td_s32 hevc_entry_point_offset(hevc_ctx *ctx, const hevc_pic_param_set *pps, const hevc_seq_param_set *sps)
{
    td_u32 max_num_entry_point_offsets;
    td_u8 flag;
    td_u32 i;

    if (pps->tiles_enabled_flag || pps->entropy_coding_sync_enabled_flag) {
        ctx->curr_slice.num_entry_point_offsets = hevc_ue_v(ctx->bs_ptr, "num_entry_point_offsets");

        vfmw_check_ret(pps->num_tile_rows > 0, HEVC_DEC_ERR);
        vfmw_check_ret(pps->num_tile_rows <= HEVC_MAX_TILE_ROWS, HEVC_DEC_ERR);
        vfmw_check_ret(pps->num_tile_columns > 0, HEVC_DEC_ERR);
        vfmw_check_ret(pps->num_tile_columns <= HEVC_MAX_TILE_COLUMNS, HEVC_DEC_ERR);
        vfmw_check_ret(sps->ctb_num_height > 0, HEVC_DEC_ERR);
        vfmw_check_ret(sps->ctb_num_height <= (HEVC_MAX_PIX_HEIGHT >> 0x4), HEVC_DEC_ERR);

        if (pps->tiles_enabled_flag == 0 && pps->entropy_coding_sync_enabled_flag == 1) {
            max_num_entry_point_offsets = sps->ctb_num_height - 1;
        } else if (pps->tiles_enabled_flag == 1 && pps->entropy_coding_sync_enabled_flag == 0) {
            max_num_entry_point_offsets = pps->num_tile_columns * pps->num_tile_rows - 1;
        } else {
            max_num_entry_point_offsets = pps->num_tile_columns * sps->ctb_num_height - 1;
        }

        flag = (ctx->curr_slice.num_entry_point_offsets > max_num_entry_point_offsets);
        vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "curr_slice.num_entry_point_offsets(%d) out of range[0,%d].\n",
                             ctx->curr_slice.num_entry_point_offsets, max_num_entry_point_offsets);

        if (ctx->curr_slice.num_entry_point_offsets > 0) {
            ctx->curr_slice.offset_len = (td_s32)(1 + hevc_ue_v(ctx->bs_ptr, "offset_len_minus1"));

            flag = (ctx->curr_slice.offset_len < 1 || ctx->curr_slice.offset_len > 32); /* 32 is vfmw_max len */
            vfmw_check_ret_print(!flag, HEVC_DEC_ERR,
                                 "curr_slice.offset_len(%d) out of range[1,32].\n", ctx->curr_slice.offset_len);
        }

        for (i = 0; i < ctx->curr_slice.num_entry_point_offsets; i++) {
            ctx->curr_slice.entry_point_offset[0] =
                (td_s32)hevc_u_v(ctx->bs_ptr, ctx->curr_slice.offset_len, "entry_point_offset_minus1");
        }
    } else {
        ctx->curr_slice.num_entry_point_offsets = 0;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_slice_seg_header_extension(hevc_ctx *ctx, hevc_pic_param_set *pps, const hevc_seq_param_set *sps)
{
    td_u32 i;
    td_u32 bytes_left;
    td_u32 bytes_used = 0;
    td_u8 flag;
    hevc_slice_segment_header *pslc = &ctx->curr_slice;

    if (pps->slice_segment_header_extension_present_flag) {
        pslc->slice_segment_header_extension_length = hevc_ue_v(ctx->bs_ptr, "slice_segment_header_extension_length");

        flag = (pslc->slice_segment_header_extension_length > 256); /* 256 is max num */
        vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "slice_segment_header_extension_length(%d) out of range [0,256]\n",
                             pslc->slice_segment_header_extension_length);

        bytes_left = (td_u32)bs_residual_bits(ctx->bs_ptr) >> 3;

        flag = (bytes_left < pslc->slice_segment_header_extension_length);
        vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "bytes_left(%d) < slice_segment_header_extension_length(%d)\n",
                             bytes_left, pslc->slice_segment_header_extension_length);

        if (sps->air_parameters.air_version != 0 && pslc->slice_segment_header_extension_length >= 0x2) {
            pslc->air_slice_param.air_slice_id = hevc_u_v(ctx->bs_ptr, 8, "air_slice_id");
            pslc->air_slice_param.air_layer_id = hevc_u_v(ctx->bs_ptr, 8, "air_layer_id");
            bytes_used += 2;
        }

        for (i = bytes_used; i < pslc->slice_segment_header_extension_length; i++) {
            pslc->slice_segment_header_extension_data_byte =
                hevc_u_v(ctx->bs_ptr, 8, "slice_segment_header_extension_data_byte"); /* 8 is bit num */
        }
    }

    return HEVC_DEC_NORMAL;
}

VFMW_ACC_SLICE_H265 STATIC td_s32 hevc_slice_calc_num_rps_curr_temp_list(const hevc_ctx *ctx,
    const hevc_short_term_rpset *rps)
{
    td_s32 num_rps_curr_temp_list;
    td_u8 flag;
    td_u32 i;

    num_rps_curr_temp_list = 0;
    if (ctx->curr_slice.slice_type == HEVC_I_SLICE) {
        num_rps_curr_temp_list = 0;
    } else {
        vfmw_check_ret_print((rps != TD_NULL), HEVC_DEC_ERR, "pRPS is null.\n");

        flag = (rps->num_of_pics > HEVC_MAX_NUM_REF_PICS);
        vfmw_check_ret_print(!flag, HEVC_DEC_ERR,
            "pRPS->num_of_pics out(%d) of range(0,%d).\n", rps->num_of_pics, HEVC_MAX_NUM_REF_PICS);

        for (i = 0; i < rps->num_of_pics; i++) {
            if (rps->used_flag[i]) {
                num_rps_curr_temp_list++;
            }
        }

#ifdef VFMW_MV_HEVC_SUPPORT
        if (ctx->is_multi_layers) {
            num_rps_curr_temp_list = num_rps_curr_temp_list +
            ctx->curr_slice.slice_extension.num_active_ref_layer_pics;
        }
#endif
    }

    return num_rps_curr_temp_list;
}

STATIC td_s32 hevc_calc_slice_addr(hevc_ctx *ctx, const hevc_seq_param_set *sps, hevc_pic_param_set *pps)
{
    td_u8 flag;
    td_s32 ret, num_cus, max_parts;
    td_u32 req_bits_outer;
    td_s32 req_bits_inner;
    td_u32 icu_address;
    td_u32 inner_address;
    td_u32 slice_address;
    td_u32 width_cu, height_cu;

    vfmw_unused(pps);
    flag = (sps->max_cu_width == 0 || sps->max_cu_height == 0);
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR,
                         "max_cu_width(%d)/max_cu_height(%d) = 0\n", sps->max_cu_width, sps->max_cu_height);

    width_cu = (sps->pic_width_in_luma_samples + sps->max_cu_width - 1) / sps->max_cu_width;
    height_cu = (sps->pic_height_in_luma_samples + sps->max_cu_height - 1) / sps->max_cu_height;
    vfmw_check_ret(width_cu < VFMW_MAX_UINT32, HEVC_DEC_ERR);
    vfmw_check_ret(height_cu < VFMW_MAX_UINT32, HEVC_DEC_ERR);

    num_cus = (td_s32)(width_cu * height_cu);
    max_parts = (1 << (sps->max_cu_depth << 1));

    vfmw_check_ret(num_cus < VFMW_MAX_SINT32, HEVC_DEC_ERR);
    vfmw_check_ret(max_parts < VFMW_MAX_SINT32, HEVC_DEC_ERR);

    req_bits_outer = 0;
    req_bits_inner = 0;
    while (num_cus > (1 << req_bits_outer)) {
        req_bits_outer++;
    }

    icu_address = 0;
    inner_address = 0;
    ret = hevc_dec_slice_segment_address(ctx, req_bits_inner, req_bits_outer, &icu_address, &inner_address);
    vfmw_check_ret_print(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR, "%s, %d return error\n", __func__, __LINE__);

    vfmw_check_ret(icu_address < VFMW_MAX_UINT32, HEVC_DEC_ERR);
    slice_address = (td_u32)max_parts * icu_address + inner_address;
    ctx->curr_slice.dependent_slice_curstart_cuaddr = slice_address;
    vfmw_check_ret(num_cus < VFMW_MAX_SINT32, HEVC_DEC_ERR);
    vfmw_check_ret(max_parts < VFMW_MAX_SINT32, HEVC_DEC_ERR);
    ctx->curr_slice.dependent_slice_curend_cuaddr = (td_u32)(num_cus * max_parts);

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_dec_output_flag_present(hevc_ctx *ctx, const hevc_pic_param_set *pps)
{
    if (pps->output_flag_present_flag) {
        ctx->curr_slice.pic_output_flag = hevc_u_v(ctx->bs_ptr, 1, "pic_output_flag");
    } else {
        ctx->curr_slice.pic_output_flag = HEVC_TRUE;
    }

    return;
}

STATIC td_void hevc_dec_idr_slice_param(hevc_ctx *ctx)
{
    ctx->curr_slice.poc = 0;
    ctx->last_slice_poc = ctx->curr_slice.poc;

    if (ctx->curr_slice.nuh_temporal_id == 0) {
        ctx->curr_slice.prev_poc = 0;
        ctx->prev_slice_poc = 0;
    }

    ctx->curr_slice.slice_temporal_mvp_enable_flag = HEVC_FALSE;

    return;
}

STATIC td_void hevc_dec_adaptive_enable(hevc_ctx *ctx, const hevc_seq_param_set *sps)
{
    if (sps->sample_adaptive_offset_enabled_flag) {
        ctx->curr_slice.slice_sao_luma_flag = hevc_u_v(ctx->bs_ptr, 1, "slice_sao_luma_flag");
        ctx->curr_slice.slice_sao_chroma_flag = hevc_u_v(ctx->bs_ptr, 1, "slice_sao_chroma_flag");
    }

    return;
}

STATIC td_s32 hevc_set_ref_pic_list(hevc_ctx *ctx, const hevc_pic_param_set *pps,
    const hevc_short_term_rpset *rps)
{
    td_s32 ret;
    td_s32 num_rps_curr_temp_list;

    num_rps_curr_temp_list = hevc_slice_calc_num_rps_curr_temp_list(ctx, rps);

    if (ctx->curr_slice.slice_type != HEVC_I_SLICE) {
        if (pps->lists_modification_present_flag && num_rps_curr_temp_list > 1) {
            ret = hevc_dec_ref_pic_lists_modification(ctx, &ctx->curr_slice,
                &(ctx->curr_slice.ref_pic_lists_modification), num_rps_curr_temp_list);
            vfmw_check_ret_print((ret == HEVC_DEC_NORMAL), HEVC_DEC_ERR,
                                 "SH hevc_dec_ref_pic_lists_modification error.\n");
        }
    }

    if (ctx->curr_slice.slice_type == HEVC_B_SLICE) {
        ctx->curr_slice.mvd_l1_zero_flag = hevc_u_v(ctx->bs_ptr, 1, "mvd_l1_zero_flag");
    }

    if ((ctx->curr_slice.slice_type != HEVC_I_SLICE) && (pps->cabac_init_present_flag)) {
        ctx->curr_slice.cabac_init_flag = hevc_u_v(ctx->bs_ptr, 1, "cabac_init_flag");
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_check_special_element(hevc_ctx *ctx, const hevc_pic_param_set *pps)
{
    td_u8 flag;
    td_s32 ret;
    td_u32 five_minus_max_num_merge_cand;

    flag = ((pps->weighted_pred_flag && (ctx->curr_slice.slice_type == HEVC_P_SLICE)) ||
            (pps->weighted_bipred_flag && (ctx->curr_slice.slice_type == HEVC_B_SLICE)));

    if (flag) {
        ret = hevc_dec_pred_weight_table(ctx);
        vfmw_check_ret_print(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR, "SH hevc_dec_pred_weight_table error.\n");
    }

    if (ctx->curr_slice.slice_type != HEVC_I_SLICE) {
        five_minus_max_num_merge_cand = hevc_ue_v(ctx->bs_ptr, "five_minus_max_num_merge_cand");
        vfmw_check_ret_print((five_minus_max_num_merge_cand < 5), HEVC_DEC_ERR, /* 5 is a num */
                             "five_minus_max_num_merge_cand(%d) out of range(0,4).\n", five_minus_max_num_merge_cand);
        ctx->curr_slice.max_num_merge_cand = (td_s32)(MRG_MAX_NUM_CANDS - five_minus_max_num_merge_cand);
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_slice_qp(hevc_ctx *ctx, const hevc_seq_param_set *sps, const hevc_pic_param_set *pps)
{
    td_u8 flag;

    ctx->curr_slice.slice_qp_delta = hevc_se_v(ctx->bs_ptr, "slice_qp_delta");
    ctx->curr_slice.slice_qp = ctx->curr_slice.slice_qp_delta + pps->pic_init_qp;

    flag = (ctx->curr_slice.slice_qp < (td_s32)(-sps->qp_bdoffset_y) ||
            ctx->curr_slice.slice_qp > 51); /* 51 is vfmw_max num */
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "SH slice_qp(%d) out of range [-%d,51].\n",
                         ctx->curr_slice.slice_qp, sps->qp_bdoffset_y);

    if (!ctx->curr_slice.first_slice_segment_in_pic_flag) {
        ctx->curr_pic.pic_qp += ctx->curr_slice.slice_qp;
        ctx->curr_pic.qp_slice_num++;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_calc_curr_slice_poc(hevc_ctx *ctx, const hevc_seq_param_set *sps)
{
    td_u8 flag;
    td_u32 poc_lsb, poc_msb, pre_poc_lsb, pre_poc_msb, max_poc_lsb;

    /* calc poc of current slice */
    poc_lsb = (td_u32)ctx->curr_slice.pic_order_cnt_lsb;
    max_poc_lsb = 1 << sps->max_pic_order_cnt_lsb;
    pre_poc_lsb = (td_u32)ctx->prev_slice_poc % max_poc_lsb;
    pre_poc_msb = (td_u32)ctx->prev_slice_poc - pre_poc_lsb;

    if ((poc_lsb < pre_poc_lsb) && ((pre_poc_lsb - poc_lsb) >= (max_poc_lsb / 2))) { /* 2 is num */
        poc_msb = pre_poc_msb + max_poc_lsb;
    } else if ((poc_lsb > pre_poc_lsb) && ((poc_lsb - pre_poc_lsb) > (max_poc_lsb / 2))) { /* 2 is num */
        poc_msb = pre_poc_msb - max_poc_lsb;
    } else {
        poc_msb = pre_poc_msb;
    }

    flag = (ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_LP ||
            ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_RADL ||
            ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP);

    if (flag) {
        poc_msb = 0;
    }

    ctx->curr_slice.poc = (td_s32)(poc_lsb + poc_msb);

    return;
}

STATIC td_s32 hevc_dec_no_idr_slice(hevc_ctx *ctx, const hevc_seq_param_set *sps,
    hevc_short_term_rpset *rps)
{
    td_s32 ret;
    td_u8 flag;
    td_u32 poc_lsb, pre_poc_lsb, pre_poc_msb, max_poc_lsb;

    if (ctx->is_multi_layers == 0) {
        ctx->curr_slice.pic_order_cnt_lsb = (td_s32)hevc_u_v(ctx->bs_ptr, sps->bits_for_poc, "pic_order_cnt_lsb");
    }
    /* calc poc of current slice */
    poc_lsb = (td_u32)ctx->curr_slice.pic_order_cnt_lsb;
    max_poc_lsb = 1 << sps->max_pic_order_cnt_lsb;
    pre_poc_lsb = (td_u32)ctx->prev_slice_poc % max_poc_lsb;
    pre_poc_msb = (td_u32)ctx->prev_slice_poc - pre_poc_lsb;
    vfmw_unused(pre_poc_msb);

    hevc_calc_curr_slice_poc(ctx, sps);

    if ((ctx->last_display_poc[ctx->curr_nal->nal_layer_id] >= ctx->curr_slice.poc) &&
        (ctx->curr_slice.first_slice_segment_in_pic_flag == 1)) {
        dprint(PRN_ERROR, "hevc_clear_dpb,last_display_poc:%d poc:%d \n",
               ctx->last_display_poc[ctx->curr_nal->nal_layer_id], ctx->curr_slice.poc);
    }

    ctx->last_slice_poc = ctx->curr_slice.poc;

    ret = hevc_dec_short_term_rps(ctx, sps);
    vfmw_check_ret_print(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR, "%s, %d return error\n", __func__, __LINE__);

    rps = &(ctx->curr_slice.short_term_ref_pic_set);

    ret = hevc_dec_long_term_rps(ctx, sps, rps, max_poc_lsb, poc_lsb);
    vfmw_check_ret_print(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR, "%s, %d return error\n", __func__, __LINE__);

    flag = (rps->num_negative_pics > HEVC_MAX_NUM_REF_PICS ||
            rps->num_positive_pics > HEVC_MAX_NUM_REF_PICS ||
            rps->num_of_longterm_pics > HEVC_MAX_NUM_REF_PICS ||
            (rps->num_negative_pics + rps->num_positive_pics + rps->num_of_longterm_pics) >
            HEVC_MAX_NUM_REF_PICS);

    vfmw_check_ret_print(!flag, HEVC_DEC_ERR,
        "num_negative_pics(%d) or num_positive_pics(%d) or num_of_longterm_pics(%d) > %d.\n",
        rps->num_negative_pics, rps->num_negative_pics, rps->num_of_longterm_pics, HEVC_MAX_NUM_REF_PICS);

    if (sps->sps_temporal_mvp_enable_flag) {
        ctx->curr_slice.slice_temporal_mvp_enable_flag =
            hevc_u_v(ctx->bs_ptr, 1, "slice_temporal_mvp_enable_flag");
    } else {
        ctx->curr_slice.slice_temporal_mvp_enable_flag = HEVC_FALSE;
    }

    return HEVC_DEC_NORMAL;
}

#ifdef VFMW_MV_HEVC_SUPPORT
STATIC td_void hevc_dec_independent_slice_multi_layers_info(hevc_ctx *ctx, const hevc_seq_param_set *sps,
    const hevc_video_param_set *vps, hevc_short_term_rpset **short_term_rpset, td_u32 idr_pic_flag)
{
    td_u32 i;

    if (ctx->is_multi_layers == 1) {
        if (ctx->curr_nal->nal_layer_id >= HEVC_MAX_NUM_LAYERS) {
            dprint(PRN_ERROR, "line %d func %s nal_layer_id = %d overrun error\n",
                __LINE__, __func__, ctx->curr_nal->nal_layer_id);
            return;
        }
        i = vps->vps_extension.layer_idx_in_vps[ctx->curr_nal->nal_layer_id];
        if ((ctx->curr_nal->nal_layer_id > 0 && !vps->vps_extension.poc_lsb_not_present_flag[i]) ||
            (idr_pic_flag == 0)) {
            ctx->curr_slice.pic_order_cnt_lsb = (td_s32)hevc_u_v(ctx->bs_ptr, sps->bits_for_poc,
                "slice_pic_order_cnt_lsb");
        } else {
            ctx->curr_slice.pic_order_cnt_lsb = 0;
        }
        *short_term_rpset = &(ctx->curr_slice.short_term_ref_pic_set);
    }
}
#endif

STATIC td_u32 hevc_parse_slice_ext(hevc_ctx *ctx, const hevc_video_param_set *vps)
{
#ifdef VFMW_MV_HEVC_SUPPORT
    td_s32 ret;

    if (ctx->is_multi_layers == 1) {
        ret = hevc_parse_slice_extension(ctx, vps);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "HEVC_ParseSliceExtension failed\n");
            return HEVC_DEC_ERR;
        }
    }
#else
    vfmw_unused(ctx);
    vfmw_unused(vps);
#endif
    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_dec_loop_filter_across_slices(hevc_ctx *ctx, const hevc_pic_param_set *pps,
    const hevc_seq_param_set *sps)
{
    td_u8 flag;
    td_u32 is_sao_enabled;
    td_u32 is_dbf_enabled;

    flag = (ctx->curr_slice.slice_sao_luma_flag || ctx->curr_slice.slice_sao_chroma_flag);
    is_sao_enabled = (!sps->sample_adaptive_offset_enabled_flag) ? (HEVC_FALSE) : flag;
    is_dbf_enabled = !(ctx->curr_slice.slice_disable_deblocking_filter_flag);

    if (pps->loop_filter_across_slices_enabled_flag && (is_sao_enabled || is_dbf_enabled)) {
        ctx->curr_slice.slice_loop_filter_across_slices_enabled_flag =
            hevc_u_v(ctx->bs_ptr, 1, "slice_loop_filter_across_slices_enabled_flag");
    } else {
        ctx->curr_slice.slice_loop_filter_across_slices_enabled_flag = pps->loop_filter_across_slices_enabled_flag;
    }
}

STATIC td_s32 hevc_dec_non_dependent_segment(hevc_ctx *ctx, const hevc_seq_param_set *sps,
    const hevc_video_param_set *vps, const hevc_pic_param_set *pps, td_u32 idr_pic_flag)
{
    td_u8 flag;
    td_s32 ret, num_rps_curr_temp_list;
    td_u32 i;
    hevc_short_term_rpset *rps = TD_NULL;

#ifdef VFMW_MV_HEVC_SUPPORT
    hevc_parse_slice_ext_flag(ctx, pps);
#endif

    for (i = 0; i < pps->num_extra_slice_header_bits; i++) {
        hevc_u_v(ctx->bs_ptr, 1, "slice_reserved_flag");
    }

    ctx->curr_slice.slice_type = (td_s32)hevc_ue_v(ctx->bs_ptr, "slice_type");

    flag = (ctx->curr_slice.slice_type < 0 || ctx->curr_slice.slice_type > 2); /* 2 is vfmw_max val */
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "slice_type(%d) out of range(0,2].\n", ctx->curr_slice.slice_type);
#ifndef HEVC_B_FRAME_SUPPORT
    vfmw_check_ret_print(ctx->curr_slice.slice_type != HEVC_B_SLICE, HEVC_DEC_ERR, "hevc unsupport B!\n");
#endif

    hevc_dec_output_flag_present(ctx, pps);
#ifdef VFMW_MV_HEVC_SUPPORT
    hevc_dec_independent_slice_multi_layers_info(ctx, sps, vps, &rps, idr_pic_flag);
#endif

    if (!idr_pic_flag) {
        rps = &(ctx->curr_slice.short_term_ref_pic_set);
        ret = hevc_dec_no_idr_slice(ctx, sps, rps);
        vfmw_check_ret(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR);
    } else {
        hevc_dec_idr_slice_param(ctx);
    }

    vfmw_check_ret(hevc_parse_slice_ext(ctx, vps) == HEVC_DEC_NORMAL, HEVC_DEC_ERR);

    hevc_dec_adaptive_enable(ctx, sps);

    ret = hevc_num_ref_idx_of_list(ctx, pps);
    vfmw_check_ret_print(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR, "%s, %d return error\n", __func__, __LINE__);

    num_rps_curr_temp_list = hevc_slice_calc_num_rps_curr_temp_list(ctx, rps);
    vfmw_unused(num_rps_curr_temp_list);

    ret = hevc_set_ref_pic_list(ctx, pps, rps);
    vfmw_check_ret(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR);

    ret = hevc_collocated_ref_idx(ctx);
    vfmw_check_ret_print(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR, "%s, %d return error\n", __func__, __LINE__);

    ret = hevc_check_special_element(ctx, pps);
    vfmw_check_ret(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR);

    ret = hevc_dec_slice_qp(ctx, sps, pps);
    vfmw_check_ret(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR);

    ret = hevc_slice_chroma_qp_offset(ctx, pps);
    vfmw_check_ret_print(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR, "%s, %d return error\n", __func__, __LINE__);

    hevc_deblock_filter(ctx, pps);

    hevc_dec_loop_filter_across_slices(ctx, pps, sps);

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_set_no_output_flag(hevc_ctx *ctx, td_u32 rap_pic_flag)
{
    td_u8 flag;

    if (ctx->curr_slice.dependent_slice_curstart_cuaddr == 0) {
        if (rap_pic_flag) {
            ctx->no_rasl_output_flag[ctx->curr_slice.view_id] = (ctx->new_sequence) ? HEVC_TRUE : HEVC_FALSE;
        }
    }

    flag = (ctx->no_rasl_output_flag[ctx->curr_slice.view_id] && ctx->curr_slice.poc != 0 && rap_pic_flag);
    if (flag) {
        flag = (ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_CRA);
        ctx->no_output_of_prior_pics_flag[ctx->curr_slice.view_id] = (flag == 1) ? HEVC_TRUE : HEVC_FALSE;
    } else {
        ctx->no_output_of_prior_pics_flag[ctx->curr_slice.view_id] = HEVC_FALSE;
    }

    return;
}

STATIC td_void hevc_dec_slice_update_poc(hevc_ctx *ctx, const hevc_seq_param_set *sps, td_u32 idr_pic_flag)
{
    td_u8 flag;

    if ((ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_CRA) &&
        ctx->no_rasl_output_flag[ctx->curr_slice.view_id]) {
        ctx->curr_slice.poc = (td_u32)ctx->curr_slice.poc & ((1 << sps->max_pic_order_cnt_lsb) - 1);
    }

    if (!idr_pic_flag) {
        flag = ((ctx->curr_slice.nuh_temporal_id == 0) &&
                (hevc_is_reference_nalu(ctx) == 1) &&
                (ctx->curr_slice.nal_unit_type != NAL_UNIT_CODED_SLICE_RASL_R) &&
                (ctx->curr_slice.nal_unit_type != NAL_UNIT_CODED_SLICE_RADL_R));
        if (flag) {
            ctx->curr_slice.prev_poc = ctx->curr_slice.poc;
            ctx->prev_slice_poc = ctx->curr_slice.poc;
        }
    }
}

STATIC td_s32 hevc_dec_slice_segment_header(hevc_ctx *ctx)
{
    td_s32 ret;
    td_u8 flag;
    td_u32 rap_pic_flag, idr_pic_flag;
    hevc_seq_param_set *sps = TD_NULL;
    hevc_pic_param_set *pps = TD_NULL;
    hevc_video_param_set *vps = TD_NULL;

    ctx->curr_slice.first_slice_segment_in_pic_flag = hevc_u_v(ctx->bs_ptr, 1, "first_slice_segment_in_pic_flag");

    rap_pic_flag = hevc_get_rap_pic_flag(ctx);
    idr_pic_flag = hevc_get_idr_pic_flag(ctx);

    if (rap_pic_flag) {
        ctx->curr_slice.no_output_of_prior_pics_flag = hevc_u_v(ctx->bs_ptr, 1, "no_output_of_prior_pics_flag");
    }

    ctx->curr_slice.pic_parameter_set_id = (td_s32)hevc_ue_v(ctx->bs_ptr, "slice_pic_parameter_set_id");

    flag = (ctx->curr_slice.pic_parameter_set_id < 0 || ctx->curr_slice.pic_parameter_set_id >= (ctx->max_pps_num));
    vfmw_check_ret_print(!flag, HEVC_DEC_ERR, "%s: pic_parameter_set_id(%d) out of range(0 %d)!\n",
                         __func__, ctx->curr_slice.pic_parameter_set_id, ctx->max_pps_num);

    if (hevc_vps_sps_pps_check(ctx) != HEVC_DEC_NORMAL) {
        ctx->curr_slice.poc = (ctx->num_slice_para > 0) ? ctx->last_slice_poc : 0;
        ctx->curr_slice.new_pic_type = hevc_is_new_pic(ctx);
        dprint(PRN_ERROR, "hevc_vps_sps_pps_check != HEVC_DEC_NORMAL\n");
        return HEVC_DEC_ERR;
    }

    pps = ctx->pps[ctx->curr_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];
    vps = ctx->vps[sps->video_parameter_set_id];

    hevc_dec_slice_segment_flag(ctx, pps);

    ret = hevc_calc_slice_addr(ctx, sps, pps);
    vfmw_check_ret(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR);

    ctx->curr_slice.poc = ctx->last_slice_poc;

    if (!ctx->curr_slice.dependent_slice_segment_flag) {
        ret = hevc_dec_non_dependent_segment(ctx, sps, vps, pps, idr_pic_flag);
        vfmw_check_ret(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR);
    }

    ret = hevc_entry_point_offset(ctx, pps, sps);
    vfmw_check_ret_print(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR, "%s, %d return error\n", __func__, __LINE__);

    ret = hevc_slice_seg_header_extension(ctx, pps, sps);
    vfmw_check_ret_print(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR, "%s, %d return error\n", __func__, __LINE__);

    hevc_set_no_output_flag(ctx, rap_pic_flag);

    hevc_dec_slice_update_poc(ctx, sps, idr_pic_flag);

    ctx->curr_slice.new_pic_type = hevc_is_new_pic(ctx);
    ret = hevc_read_byte_alignment(ctx);
    vfmw_check_ret(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR);

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_set_fs_air_state(hevc_ctx *ctx)
{
    td_s32 logic_fs_id;

    if (ctx->extra_data_ptr->air_version == AIR_DEC_FRAME) {
        logic_fs_id = ctx->curr_pic.frame_store->logic_fs_id;

        if (ctx->curr_pic.air_slice_id == 0 && ctx->curr_pic.air_layer_id == 0) {
            ctx->curr_pic.pic_output_flag = HEVC_TRUE;
            fsp_set_air_state(ctx->chan_id, logic_fs_id, FS_AIR_STATE_USE);
            ctx->air_logic_id = logic_fs_id;
        } else if (ctx->curr_pic.air_slice_id == ctx->curr_sps->air_parameters.air_slice_num - 1 &&
                   ctx->curr_pic.air_layer_id == ctx->curr_sps->air_parameters.air_layer_num - 1) {
            ctx->curr_pic.pic_output_flag = HEVC_FALSE;
            ctx->air_logic_id = -1;
            fsp_set_air_state(ctx->chan_id, logic_fs_id, FS_AIR_STATE_OVER);
        } else {
            ctx->curr_pic.pic_output_flag = HEVC_FALSE;
            fsp_set_air_state(ctx->chan_id, logic_fs_id, FS_AIR_STATE_USE);
        }
    }
}

STATIC td_void hevc_set_air_state(hevc_ctx *ctx)
{
    if (ctx->extra_data_ptr->air_version == AIR_DEC_FRAME) {
        if (ctx->curr_pic.air_slice_id == 0 && ctx->curr_pic.air_layer_id == 0) {
            ctx->air_sta = AIR_STATE_FIRST;
            if (ctx->curr_pic.air_slice_id == ctx->curr_sps->air_parameters.air_slice_num - 1 &&
                   ctx->curr_pic.air_layer_id == ctx->curr_sps->air_parameters.air_layer_num - 1) {
                ctx->air_sta = AIR_STATE_OVER;
            }
        } else if (ctx->curr_pic.air_slice_id == ctx->curr_sps->air_parameters.air_slice_num - 1 &&
                   ctx->curr_pic.air_layer_id == ctx->curr_sps->air_parameters.air_layer_num - 1) {
            ctx->air_sta = AIR_STATE_OVER;
        } else {
            ctx->air_sta = AIR_STATE_CONTINUE;
        }
    }
}

STATIC td_s32 hevc_write_pic_msg(hevc_ctx *ctx)
{
    td_s32 ret;

    if (ctx->curr_pic.frame_store == TD_NULL) {
        dprint(PRN_ERROR, "%s %d frame_store is null!!\n", __func__, __LINE__);
        return HEVC_DEC_ERR;
    }

    fsp_set_ref(ctx->chan_id, ctx->curr_pic.frame_store->logic_fs_id, 1);

    if (ctx->set_dps_xps == 0) {
        hevc_set_air_state(ctx);
    }

    ret = dps_set_pic_param(ctx->chan_id, ctx, ctx->curr_dps);
    if (ret == DPS_ERR) {
        hevc_clear_curr_pic(ctx);
        dprint(PRN_ERROR, "dps_set_pic_param ERR!\n");
        return HEVC_DEC_ERR;
    }

    if (ctx->set_dps_xps == 0) {
        hevc_set_fs_air_state(ctx);
    }

    return HEVC_DEC_NORMAL;
}

VFMW_ACC_SLICE_H265 STATIC td_s32 hevc_check_list_x(const hevc_ctx *ctx)
{
    td_u32 i;
    td_u32 list0size, list1size;
    const hevc_slice_segment_header *slice = (ctx->curr_slice.dependent_slice_segment_flag == 0) ?
        (&ctx->curr_slice) : &ctx->independent_slice;

    if (ctx->curr_slice.dependent_slice_segment_flag != 0 && ctx->have_independent_slice == 0) {
        dprint(PRN_ERROR, "%s ERROR: have_independent_slice = 0\n\n", __func__);
        return HEVC_DEC_ERR;
    }

    list0size = slice->list_x_size[0];
    list1size = slice->list_x_size[1];

    if (list0size >= HEVC_MAX_LIST_SIZE) {
        dprint(PRN_ERROR, "ListX ERROR: list0size %d >= %d\n", list0size, HEVC_MAX_LIST_SIZE);
        return HEVC_DEC_ERR;
    }

    if (list1size >= HEVC_MAX_LIST_SIZE) {
        dprint(PRN_ERROR, "ListX ERROR: list1size %d >= %d\n", list1size, HEVC_MAX_LIST_SIZE);
        return HEVC_DEC_ERR;
    }

    for (i = 0; i < list0size; i++) {
        if (ctx->list_x[0][i] == TD_NULL) {
            dprint(PRN_ERROR, "list_x[0][%d] = TD_NULL\n", i);
            return HEVC_DEC_ERR;
        }

        if (ctx->list_x[0][i]->frame_store == TD_NULL) {
            dprint(PRN_ERROR, "list_x[0][%d]->frame_store = TD_NULL\n", i);
            return HEVC_DEC_ERR;
        }
    }

    for (i = 0; i < list1size; i++) {
        if (ctx->list_x[1][i] == TD_NULL) {
            dprint(PRN_ERROR, "list_x[1][%d] = TD_NULL\n", i);
            return HEVC_DEC_ERR;
        }

        if (ctx->list_x[1][i]->frame_store == TD_NULL) {
            dprint(PRN_ERROR, "list_x[1][%d]->frame_store = TD_NULL\n", i);
            return HEVC_DEC_ERR;
        }
    }

    return HEVC_DEC_NORMAL;
}

VFMW_ACC_SLICE_H265 STATIC td_s32 hevc_write_slice_msg(hevc_ctx *ctx)
{
    td_s32 ret, is_new_pic;

    is_new_pic = (ctx->curr_slice.new_pic_type == IS_NEW_PIC) ? 1 : 0;

    if (ctx->num_slice_para == 0 && is_new_pic != 1) {
        dprint(PRN_ERROR, "Slice ERR!\n");
        return HEVC_DEC_ERR;
    }

    if (ctx->curr_slice.dependent_slice_segment_flag == 0) {
        vfmw_check_sec_func(memcpy_s(&ctx->independent_slice, sizeof(ctx->independent_slice),
                                     &ctx->curr_slice, sizeof(ctx->curr_slice)));
        ctx->have_independent_slice = 1;
    }

    if (is_new_pic) {
        dps_release_seg(ctx->chan_id, ctx->curr_dps);
        dps_reset_node(ctx->chan_id, ctx->curr_dps);
    }

    ret = dps_set_slice_param(ctx->chan_id, ctx, ctx->curr_dps);
    if (ret == DPS_ERR) {
        dprint(PRN_ERROR, "dps_set_pic_param ERR!\n");

        return SYN_PARSE_RESOURCE_INVALID;
    }

    ctx->curr_pic.total_valid_bit_len += ctx->curr_nal->stream[0].valid_bitlen;
    ctx->curr_pic.total_valid_bit_len += ctx->curr_nal->stream[1].valid_bitlen;

#if defined(VFMW_HEVC_SLC_LDY_SUPPORT)
    if (ctx->num_slice_para == 0) {
        ctx->first_slc_ctb = (td_u32)ctx->curr_slice.slice_segment_address;
    }
    if (ctx->half_nal_try == TRY_IDLE) {
        ctx->curr_nal = TD_NULL;
    }
#else
    ctx->curr_nal = TD_NULL;
#endif

    ctx->num_slice_para++;
    ctx->curr_msg_slot_num++;

    ctx->last_slice_pps_id = ctx->curr_slice.pic_parameter_set_id;

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_process_mode_first_i(hevc_ctx *ctx)
{
    if (ctx->first_i_flag == 0 &&
        ctx->curr_pic.pic_type == HEVC_I_FRAME) {
        ctx->first_i_flag = 1;
        return HEVC_DEC_NORMAL;
    }

    hevc_clear_curr_pic(ctx);
    hevc_clear_dpb(ctx);

    return HEVC_DEC_ERR; /* IDR_LOST */
}

STATIC td_s32 hevc_dec_process_mode_i(hevc_ctx *ctx)
{
    if (ctx->curr_pic.pic_type != HEVC_I_FRAME) {
        hevc_clear_curr_pic(ctx);
        return HEVC_DEC_ERR;
    }
    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_process_mode_ip(hevc_ctx *ctx)
{
    /* 不是参考帧的B帧，可以删除 */
    if ((ctx->curr_pic.pic_type == HEVC_B_FRAME) && (ctx->curr_pic.is_ref_idc == 0)) {
        hevc_clear_curr_pic(ctx);
        return HEVC_DEC_ERR;
    }
    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_mode_process(hevc_ctx *ctx, td_u32 ipb_dec_mode)
{
    td_s32 ret;

    if ((ctx->old_dec_mode != ipb_dec_mode) &&
        (ctx->old_dec_mode == VFMW_DEC_MODE_I ||
         ctx->old_dec_mode == VFMW_DEC_MODE_FIRST_I) &&
        (ipb_dec_mode != VFMW_DEC_MODE_I) &&
        (ipb_dec_mode == VFMW_DEC_MODE_IP ||
         ipb_dec_mode == VFMW_DEC_MODE_NORMAL ||
         ipb_dec_mode == VFMW_DEC_MODE_DROP_INVALID_B)) {
        dprint(PRN_ERROR, "ipb_dec_mode Changed (%d -> %d)\n", ctx->old_dec_mode, ipb_dec_mode);
        ctx->old_dec_mode = ipb_dec_mode;
        hevc_clear_curr_pic(ctx);
        hevc_flush_dpb(ctx);
        hevc_init_dpb(ctx, 1); /* dec mode change(I->other) need pmv fs */
        return HEVC_DEC_ERR; /* DPB_ERR */
    }

    ctx->old_dec_mode = ipb_dec_mode;
    switch (ctx->old_dec_mode) {
        case VFMW_DEC_MODE_FIRST_I:
            ret = hevc_dec_process_mode_first_i(ctx);
            return ret;

        case VFMW_DEC_MODE_I:
            ret = hevc_dec_process_mode_i(ctx);
            return ret;

        case VFMW_DEC_MODE_IP:
            ret = hevc_dec_process_mode_ip(ctx);
            return ret;

        case VFMW_DEC_MODE_NORMAL:
        case VFMW_DEC_MODE_DROP_INVALID_B:
        default:
            break;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_discard_frame(hevc_ctx *ctx)
{
    /* 因为HEVC很多码流仅有IB帧，所以仅在DISCARD_B_BF_P_MODE模式下使能丢I帧后B帧特性 */
    if ((ctx->old_dec_mode == VFMW_DEC_MODE_DROP_INVALID_B)
        &&  (ctx->curr_pic.pic_type == HEVC_I_FRAME)) {
        ctx->p_pic_found = 2; /* 2 is no p pic */
    }

    /* 丢帧前提: 1.当前帧为B帧，2.还未出现过P帧，3.解码模式为显示序，4.初始参数未使能B_before_P */
    if ((ctx->curr_pic.pic_type == HEVC_B_FRAME)
        &&  (ctx->p_pic_found == 2) /* 2 is no p pic */
        &&  (ctx->extra_data_ptr->output_order == VFMW_DISP_ORDER)
        &&  (ctx->extra_data_ptr->allow_b_before_p != 1)) {
        dprint(PRN_ERROR, "Discard this B(poc=%d) before P, is_ref_idc=%d.\n",
               ctx->curr_pic.thispoc, ctx->curr_pic.is_ref_idc);
        hevc_clear_curr_pic(ctx);

        return HEVC_DEC_ERR;
    } else if (ctx->curr_pic.pic_type == HEVC_P_FRAME) {
        ctx->p_pic_found = 1;
    }

    return HEVC_DEC_NORMAL;
}

/*************************************************************************
* 函数原型 : td_s32 hevc_dec_check(hevc_ctx *ctx)
* 功能描述 : 解码前检查
* 参数列表 : hevc_ctx *ctx - 上下文指针
* 返回值   ：
*************************************************************************/
STATIC td_s32 hevc_dec_check(hevc_ctx *ctx)
{
    td_s32 size;
    td_u32 ipb_dec_mode;

    if (ctx->curr_msg_slot_num < HEVC_SLICE_SLOT_START_NUM) {
        hevc_clear_curr_pic(ctx);
        report_err_frame((td_s8)ctx->chan_id, (td_s8)ctx->curr_pic.pic_type, 0);

        if (ctx->max_bytes_received > HEVC_MAX_BYTES_START) {
            dprint(PRN_ERROR,
                   "hevc_dec_check no slice and needn't start VDM, but max_bytes_received exceed threshold!\n");
            return HEVC_BS_OVERFLOW_ERR;
        } else {
            dprint(PRN_ERROR, "hevc_dec_check no slice and needn't start VDM\n");
            return HEVC_DEC_ERR;
        }
    }

    /* 统计帧数 */
    ctx->total_frame_num++;
    ctx->total_frame_num = vfmw_max(ctx->total_frame_num, ctx->output_frame_num);
    ctx->extra_data_ptr->err_frm_num = ctx->total_frame_num - ctx->output_frame_num;

    /* 计算当期帧参考标志 */
    ctx->curr_pic.is_ref_idc = 1;

    /* clean up information, clean here for statistic err mb when not start vdm or reset */
    ipb_dec_mode = (ctx->advanced_cfg & 0x70000) >> 16; /* 16 is bit num */

    if (hevc_dec_mode_process(ctx, ipb_dec_mode) != HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

    if (ctx->num_slice_para == 0) {
        report_err_frame((td_s8)ctx->chan_id, (td_s8)ctx->curr_pic.pic_type, 0);
        hevc_clear_curr_pic(ctx);
        return HEVC_DEC_ERR;
    }

    hevc_dec_discard_frame(ctx);

    if (ctx->num_slice_para == 0) {
        hevc_clear_curr_pic(ctx);
        dprint(PRN_ERROR, "%s no correct slice in the pic\n", __func__);
        return HEVC_DEC_ERR;
    }

    if (ctx->total_pic_num >= (HEVC_START_FRAME + 1)) {
        /* Start VDM */
        ctx->curr_pic.start_count++;

        /* 发现I帧, 上报事件 */
        if (ctx->curr_pic.pic_type == HEVC_I_FRAME) {
            size = hevc_get_pic_stream_size(ctx);
            report_find_iframe((td_u32)ctx->chan_id, (td_u32)size);
        }
    }

    ctx->curr_pic.frame_stream_size = ctx->curr_pic.current_nal_size;

    return HEVC_DEC_NORMAL;
}

STATIC td_u8 hevc_is_pic_vps_refresh(hevc_ctx *ctx, hevc_video_param_set *vps)
{
    if ((ctx->prev_vps_id != vps->video_parameter_set_id)
        ||  (vps->is_refresh)) {
        return 1;
    } else {
        return 0;
    }
}

STATIC td_void hevc_updata_pic_vps(hevc_ctx *ctx, hevc_video_param_set *vps)
{
    vps->is_refresh = 0;
    ctx->curr_vps = vps;
    ctx->prev_vps_id = vps->video_parameter_set_id;
}

STATIC td_u8 hevc_is_pic_sps_refresh(hevc_ctx *ctx, const hevc_seq_param_set *sps)
{
    if (((td_u32)(ctx->prev_sps_id) != sps->seq_parameter_set_id)
        ||  (sps->is_refresh)) {
        return 1;
    } else {
        return 0;
    }
}

STATIC td_void hevc_updata_pic_sps(hevc_ctx *ctx, hevc_seq_param_set *sps)
{
    ctx->curr_sps = sps;
    ctx->prev_sps_id = (td_s32)sps->seq_parameter_set_id;
    ctx->prev_dps_size = (td_s32)sps->dpb_size;
    ctx->prev_bit_depth_l = sps->bit_depth_luma;
    ctx->prev_bit_depth_c = sps->bit_depth_chroma;
    ctx->prev_pic_width = sps->pic_width_in_luma_samples;
    ctx->prev_pic_height = sps->pic_height_in_luma_samples;
    ctx->prev_l2_ctb_size_y = sps->log2_ctb_sizey;
}

STATIC td_u8 hevc_is_pic_pps_refresh(hevc_ctx *ctx, hevc_pic_param_set *pps)
{
    if ((ctx->prev_pps_id != pps->pic_parameter_set_id)
        ||  (pps->is_refresh)) {
        return 1;
    } else {
        return 0;
    }
}

STATIC td_void hevc_updata_pic_pps(hevc_ctx *ctx, const hevc_seq_param_set *sps, hevc_pic_param_set *pps)
{
    td_s32 i;

    pps->is_refresh = 0;
    ctx->curr_pps = pps;
    ctx->prev_pps_id = pps->pic_parameter_set_id;

    if (pps->tiles_enabled_flag) {
        if (pps->uniform_spacing_flag) {
            for (i = 0; i < pps->num_tile_columns; i++) {
                pps->column_bd[i] = (i + 1) * sps->ctb_num_width / pps->num_tile_columns
                                    -  (i * sps->ctb_num_width) / pps->num_tile_columns;
            }

            for (i = 0; i < pps->num_tile_rows; i++) {
                pps->row_bd[i] = (i + 1) * sps->ctb_num_height / pps->num_tile_rows
                                 -  (i * sps->ctb_num_height) / pps->num_tile_rows;
            }
        } else {

            vfmw_check((pps->num_tile_columns - 1) < HEVC_MAX_TILE_COLUMNS);

            pps->column_bd[pps->num_tile_columns - 1] = sps->ctb_num_width;

            for (i = 0; i < (pps->num_tile_columns - 1); i++) {
                pps->column_bd[i] = pps->column_width[i];
                pps->column_bd[pps->num_tile_columns - 1] -= pps->column_bd[i];
            }

            vfmw_check((pps->num_tile_rows - 1) < HEVC_MAX_TILE_ROWS);

            pps->row_bd[pps->num_tile_rows - 1] = sps->ctb_num_height;

            for (i = 0; i < (pps->num_tile_rows - 1); i++) {
                pps->row_bd[i] = pps->row_height[i];
                pps->row_bd[pps->num_tile_rows - 1] -= pps->row_bd[i];
            }
        }
    } else {
        pps->column_bd[0] = sps->ctb_num_width;
        pps->row_bd[0] = sps->ctb_num_height;
    }
}

#ifdef HEVC_PVR_SUPPORT
STATIC td_void hevc_init_fs_pvr_info(hevc_ctx *ctx)
{
    ctx->curr_pic.fs_image.user_tag = ctx->stm_info.user_tag;
    ctx->curr_pic.fs_image.disp_time = ctx->stm_info.disp_time;
    ctx->curr_pic.fs_image.disp_enable_flag = ctx->stm_info.disp_enable_flag;
    ctx->curr_pic.fs_image.disp_frame_distance = ctx->stm_info.disp_frame_distance;
    ctx->curr_pic.fs_image.distance_before_first_frame = ctx->stm_info.distance_before_1st_frame;
    ctx->curr_pic.fs_image.gop_num = ctx->stm_info.gop_num;
}
#endif

STATIC td_void hevc_init_fs_disp_info(hevc_ctx *ctx)
{
#ifdef HEVC_AIR_SUPPORT
    if ((ctx->curr_slice.slice_segment_header_extension_length == 0) &&
        ctx->extra_data_ptr->air_dec_attr.fixed_airdec == 1) {
        ctx->curr_slice.air_slice_param.air_slice_id =
            ctx->curr_air_slice_id % ctx->extra_data_ptr->air_dec_attr.slice_num;
        ctx->curr_slice.air_slice_param.air_layer_id =
            ctx->curr_air_layer_id % ctx->extra_data_ptr->air_dec_attr.layer_num;
        ctx->curr_air_slice_id = (ctx->curr_air_slice_id + 1) % ctx->extra_data_ptr->air_dec_attr.slice_num;
        ctx->curr_air_layer_id = (ctx->curr_air_layer_id + 1) % ctx->extra_data_ptr->air_dec_attr.layer_num;
    }
#endif

    if (ctx->extra_data_ptr->air_version == AIR_DEC_FRAME) {
        ctx->curr_pic.fs_image.disp_info.disp_width = ctx->curr_sps->air_parameters.air_big_width;
        ctx->curr_pic.fs_image.disp_info.disp_height = ctx->curr_sps->air_parameters.air_big_height;
        ctx->curr_pic.air_slice_id = ctx->curr_slice.air_slice_param.air_slice_id;
        ctx->curr_pic.air_layer_id = ctx->curr_slice.air_slice_param.air_layer_id;
    } else {
        ctx->curr_pic.fs_image.disp_info.disp_width = ctx->curr_sps->disp_width;
        ctx->curr_pic.fs_image.disp_info.disp_height = ctx->curr_sps->disp_height;
    }
}

STATIC td_void hevc_init_fs_set_dec_mem(hevc_ctx *ctx)
{
    td_u32 chroma_format;
    td_ulong lock_flag;

    /* Set dec memory parameter */
#ifdef VFMW_MV_HEVC_SUPPORT
    ctx->curr_pic.fs_image.image_width_ref = ctx->curr_sps->pic_width_in_luma_samples;
    ctx->curr_pic.fs_image.image_height_ref = ctx->curr_sps->pic_height_in_luma_samples;
    if (ctx->is_multi_layers == 1) {
        ctx->curr_pic.fs_image.image_width = ctx->max_width;
        ctx->curr_pic.fs_image.image_height = ctx->max_height;
    } else {
        ctx->curr_pic.fs_image.image_width = ctx->curr_sps->pic_width_in_luma_samples;
        ctx->curr_pic.fs_image.image_height = ctx->curr_sps->pic_height_in_luma_samples;
    }
#else
    ctx->curr_pic.fs_image.image_width = ctx->curr_sps->pic_width_in_luma_samples;
    ctx->curr_pic.fs_image.image_height = ctx->curr_sps->pic_height_in_luma_samples;
#endif

    chroma_format = (ctx->curr_sps->chroma_format_idc == 1) ? 0 : 1; /* 4:2:0, 0;  4:0:0, 1; */
    fsp_img_format_lock(&lock_flag);
    ctx->curr_pic.fs_image.format.video_format = ctx->curr_sps->vui_parameters.video_format;
    ctx->curr_pic.fs_image.format.sample_type = chroma_format;
    fsp_img_format_unlock(&lock_flag);
}

STATIC td_void hevc_init_fs_image_para(hevc_ctx *ctx)
{
    td_s32 dar;

    dprint(PRN_PTS, "Pic pts %lld\n", ctx->stm_info.pts);
    ctx->curr_pic.fs_image.pts = ctx->stm_info.pts;
    ctx->stm_info.pts = VFMW_INVALID_PTS;
    ctx->curr_pic.fs_image.poc = ctx->curr_pic.thispoc;

#ifdef HEVC_PVR_SUPPORT
    hevc_init_fs_pvr_info(ctx);
#endif
    ctx->curr_pic.fs_image.dv_dual_layer = ctx->stm_info.dv_dual_layers;

    hevc_init_fs_set_dec_mem(ctx);

    /* set display AspectWidth and AspectHeight */
    dar = ctx->curr_sps->vui_parameters.aspect_ratio & 0x7;
    syn_set_aspect_ratio(&ctx->curr_pic.fs_image, (vfmw_vdec_dar)dar);

    ctx->curr_pic.fs_image.is_fld_flag = 0;
    ctx->curr_pic.fs_image.left_offset = ctx->curr_sps->conf_win_left_offset;
    ctx->curr_pic.fs_image.right_offset = ctx->curr_sps->conf_win_right_offset;
    ctx->curr_pic.fs_image.top_offset = ctx->curr_sps->conf_win_top_offset;
    ctx->curr_pic.fs_image.bottom_offset = ctx->curr_sps->conf_win_bottom_offset;

#ifdef VFMW_MV_HEVC_SUPPORT
    if (ctx->is_multi_layers == 1) {
        ctx->curr_sps->disp_width = ctx->max_width;
        ctx->curr_sps->disp_height = ctx->max_height;
    } else {
#endif
        ctx->curr_sps->disp_width = ctx->curr_sps->pic_width_in_luma_samples -
            ctx->curr_pic.fs_image.left_offset - ctx->curr_pic.fs_image.right_offset;
        ctx->curr_sps->disp_height = ctx->curr_sps->pic_height_in_luma_samples -
            ctx->curr_pic.fs_image.top_offset - ctx->curr_pic.fs_image.bottom_offset;
#ifdef VFMW_MV_HEVC_SUPPORT
    }
#endif

    ctx->curr_pic.fs_image.bit_depth = ctx->curr_sps->bit_depth_luma;
    hevc_init_fs_disp_info(ctx);

    if (ctx->curr_slice.slice_type == HEVC_I_SLICE) {
        ctx->afd_info_data.active_format_flag = ctx->curr_afd_info.active_format_flag;
        ctx->afd_info_data.active_format = ctx->curr_afd_info.active_format;
    }

    ctx->curr_pic.fs_image.afd.active_format_flag = ctx->afd_info_data.active_format_flag;
    ctx->curr_pic.fs_image.afd.active_format = ctx->afd_info_data.active_format;
    ctx->curr_afd_info.active_format_flag = TD_FALSE;
    ctx->curr_afd_info.active_format = 0;
    ctx->curr_pic.fs_image.splice_info.type = ctx->splice_info.type;
    ctx->curr_pic.fs_image.splice_info.ori_width = ctx->splice_info.ori_width;
    ctx->curr_pic.fs_image.splice_info.ori_height = ctx->splice_info.ori_height;

    return;
}

STATIC td_void hevc_init_curr_pic_param(hevc_ctx *ctx)
{
    vfmw_check(ctx->curr_sps->ctb_num_height <= HEVC_MAX_PIX_HEIGHT);
    vfmw_check(ctx->curr_sps->ctb_num_width <= HEVC_MAX_PIX_WIDTH);

    ctx->curr_pic.is_in_dpb = 0;
    ctx->curr_pic.state = HEVC_PIC_DECODING;
    ctx->curr_pic.nal_unit_type = ctx->curr_slice.nal_unit_type;
    ctx->curr_pic.thispoc = ctx->curr_slice.poc;
    ctx->curr_pic.pic_qp = ctx->curr_slice.slice_qp;
    ctx->curr_pic.qp_slice_num = 1;
#ifdef VFMW_MV_HEVC_SUPPORT
    ctx->curr_pic.fs_image.view_id = (td_s32)ctx->curr_slice.view_id;
    ctx->curr_pic.view_id = ctx->curr_slice.view_id;
#endif

    ctx->curr_pic.stream_base_addr = MAX_VAL_UADDR;
    ctx->curr_pic.pic_width = ctx->curr_sps->pic_width_in_luma_samples;
    ctx->curr_pic.pic_height = ctx->curr_sps->pic_height_in_luma_samples;
    ctx->curr_pic.pic_ctbs = ctx->curr_sps->ctb_num_height * ctx->curr_sps->ctb_num_width;
    ctx->curr_pic.start_count = 0;
    ctx->curr_pic.total_valid_bit_len = 0;
    ctx->curr_pic.pic_output_flag = HEVC_TRUE;
    ctx->curr_pic.frame_store->poc = ctx->curr_slice.poc;
    ctx->curr_pic.pic_type = HEVC_I_FRAME; /* 默认I Frame，在每个slice解析时更新 */
    ctx->err_nal_cnt = 0;

    return;
}

STATIC td_s32 hevc_resize_process(hevc_ctx *ctx, hevc_seq_param_set *sps, td_u8 dpb_size_changed,
                                  td_s32 pre_w, td_s32 pre_h, td_s32 pre_ctb, td_u32 reorder_num, td_s32 pre_sps_id)
{
    td_s32 ret;
    td_s32 rerange_mem_flag;

    /* 检查解码分辨率是否超出用户设置的最大支持分辨率 */
    ret = (sps->pic_width_in_luma_samples > (td_u32)(ctx->extra_data_ptr->max_width)) ||
        (sps->pic_height_in_luma_samples > (td_u32)(ctx->extra_data_ptr->max_height));
    if (ret == 1 && ctx->extra_data_ptr->is_adaptive == 1) {
        dprint(PRN_ERROR, "actual bitstream resolution is larger than setted resolution[%d-%d].\n",
            ctx->extra_data_ptr->max_width, ctx->extra_data_ptr->max_height);
        ctx->extra_data_ptr->max_width = sps->pic_width_in_luma_samples;
        ctx->extra_data_ptr->max_height = sps->pic_height_in_luma_samples;
    } else if (ret == 1){
        dprint(PRN_ERROR, "resolution error, the CAP_LEVEL_USER_DEFINE_WITH_OPTION channel with "
                          "re_range_en == 0 can't support such bitstream.\n");
        return HEVC_DEC_ERR;
    }

    if (((td_u32)pre_sps_id != sps->seq_parameter_set_id) || dpb_size_changed ||
        (ctx->extra_data_ptr->bit_depth_changed) ||
        (!ctx->allow_start_dec) || ((td_u32)pre_w != sps->pic_width_in_luma_samples) ||
        ((td_u32)pre_h != sps->pic_height_in_luma_samples) || (ctx->extra_data_ptr->wait_fs_flag == 1)) {
        rerange_mem_flag =
            hevc_get_rerange_flag(ctx, pre_w, pre_h, pre_ctb, dpb_size_changed);
        if ((rerange_mem_flag) || (ctx->extra_data_ptr->wait_fs_flag == 1) || sps->reorder_num != reorder_num) {
            if (ctx->need_clear_que == 1) {
                hevc_get_back_pic_from_vo_queue(ctx);
            }

            ret = hevc_init_dpb(ctx, 1);
            if (ret == SYN_PARSE_RESOURCE_INVALID) {
                ctx->extra_data_ptr->wait_fs_flag = 1;
                return SYN_PARSE_RESOURCE_INVALID;
            } else if (ret != HEVC_DEC_NORMAL) {
                dprint(PRN_ERROR, "%s error, return %d\n", __func__, ret);
                hevc_clear_all(ctx, 0);
                return HEVC_DEC_ERR;
            }
        }

        ctx->extra_data_ptr->wait_fs_flag = 0;
        ctx->extra_data_ptr->bit_depth_changed = 0;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_fresh_xps_process(hevc_ctx *ctx, hevc_video_param_set *vps,
    hevc_seq_param_set *sps, hevc_pic_param_set *pps, td_u8 *dpb_size_changed)
{
    td_u32 pre_reorder_num;
    td_s32 pre_sps_id;
    td_u32 pre_pic_width, pre_l2_ctb_size_y, pre_pic_height;
    td_u8 first_start, pic_resolution_changed, ctb_size_changed, pic_info_changed;

    pre_sps_id = ctx->prev_sps_id;
    pre_pic_width = ctx->prev_pic_width;
    pre_pic_height = ctx->prev_pic_height;
    pre_l2_ctb_size_y = ctx->prev_l2_ctb_size_y;
    pre_reorder_num = ctx->prev_reorder_num;

    first_start = (ctx->allow_start_dec == 0);
    ctb_size_changed = (pre_l2_ctb_size_y != sps->log2_ctb_sizey);
    pic_resolution_changed = (pre_pic_width != sps->pic_width_in_luma_samples ||
                              pre_pic_height != sps->pic_height_in_luma_samples);

    pic_info_changed = (first_start || pic_resolution_changed || ctb_size_changed);

    if (hevc_is_pic_vps_refresh(ctx, vps) == 1 || first_start) {
        hevc_updata_pic_vps(ctx, vps);
    }

    if (hevc_is_pic_sps_refresh(ctx, sps) == 1 || pic_info_changed) {
        if (sps->dpb_size != (td_u32)(ctx->prev_dps_size)) {
            *dpb_size_changed = 1;
        }

        hevc_updata_pic_sps(ctx, sps);
    }

    if (hevc_is_pic_pps_refresh(ctx, pps) == 1 || pic_info_changed) {
        hevc_updata_pic_pps(ctx, sps, pps);
    }
    vfmw_unused(pre_sps_id);
    vfmw_unused(pre_reorder_num);

    return;
}

#ifndef HEVC_RESOLUTION_CHANGE_SUPPORT
STATIC td_s32 hevc_check_resolution_change(hevc_ctx *ctx, td_u8 dpb_size_changed, td_s32 pre_w,
    td_s32 pre_h)
{
    td_s32 rerange_flag;
    hevc_seq_param_set *sps = TD_NULL;
    hevc_pic_param_set *pps = TD_NULL;
    td_u32 neww, newh;
    td_u32 max_dec_w_align_up, max_dec_h_align_up;
    td_u32 align_value = 64;

    rerange_flag = 0;
    pps = ctx->pps[ctx->curr_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];
    neww = sps->pic_width_in_luma_samples;
    newh = sps->pic_height_in_luma_samples;
    max_dec_w_align_up = vfmw_align_up(ctx->extra_data.max_dec_width, align_value);
    max_dec_h_align_up = vfmw_align_up(ctx->extra_data.max_dec_height, align_value);

    if (ctx->extra_data.is_max_buf_size_en == TD_TRUE) {
        if (neww * newh <= max_dec_w_align_up * max_dec_h_align_up) {
            return HEVC_DEC_NORMAL;
        } else {
            dprint(PRN_ERROR, "%s, %d max_buf_size_en is true but new[%d, %d] > max[%d %d]\n", __func__, __LINE__,
                neww, newh, max_dec_w_align_up, max_dec_h_align_up);
            report_unsupport(ctx->chan_id);
            return HEVC_DEC_ERR;
        }
    }

    if (dpb_size_changed ||
        (neww != (td_u32)pre_w) ||
        (newh != (td_u32)pre_h) ||
        (ctx->extra_data_ptr->bit_depth_changed)) {
#ifdef VFMW_MV_HEVC_SUPPORT
        if (hevc_is_multi_layer_valid(ctx) == 1) {
            if (ctx->is_mem_arrange_done == 0) {
                ctx->is_mem_arrange_done = 1;
                rerange_flag = 1;
            }
        } else
#endif
        {
            rerange_flag = 1;
        }
    }

    if (rerange_flag == 1) {
        dprint(PRN_ERROR, "%s, %d resolution change is unsupport! [%d, %d] ==> [%d %d]\n", __func__, __LINE__,
            pre_w, pre_h, neww, newh);
        report_unsupport(ctx->chan_id);
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}
#endif

STATIC td_s32 hevc_init_pic(hevc_ctx *ctx)
{
    td_u32 pre_reorder_num;
    td_s32 ret, pre_sps_id, pre_pic_width, pre_pic_height, pre_l2_ctb_size_y;
    td_u8 dpb_size_changed = 0;
    hevc_video_param_set *vps = TD_NULL;
    hevc_seq_param_set *sps = TD_NULL;
    hevc_pic_param_set *pps = TD_NULL;

    pre_sps_id = ctx->prev_sps_id;
    pre_pic_width = (td_s32)ctx->prev_pic_width;
    pre_pic_height = (td_s32)ctx->prev_pic_height;
    pre_l2_ctb_size_y = (td_s32)ctx->prev_l2_ctb_size_y;
    pre_reorder_num = ctx->prev_reorder_num;

    if (ctx->curr_slice.pic_parameter_set_id < 0 || ctx->curr_slice.pic_parameter_set_id >= (ctx->max_pps_num)) {
        return HEVC_DEC_NORMAL;
    }

    pps = ctx->pps[ctx->curr_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];
    vps = ctx->vps[sps->video_parameter_set_id];
#ifdef VFMW_MV_HEVC_SUPPORT
    ctx->curr_pps_id[ctx->curr_slice.view_id] = ctx->curr_slice.pic_parameter_set_id;
    ctx->curr_sps_id[ctx->curr_slice.view_id] = pps->seq_parameter_set_id;
    ctx->curr_vps_id[ctx->curr_slice.view_id] = (td_s32)sps->video_parameter_set_id;
#endif

    hevc_fresh_xps_process(ctx, vps, sps, pps, &dpb_size_changed);

    ctx->extra_data_ptr->bit_depth =
        (td_s32)(vfmw_max(ctx->curr_sps->bit_depth_luma, ctx->curr_sps->bit_depth_chroma));

#ifdef VFMW_MV_HEVC_SUPPORT
    hevc_check_layer_info(ctx);
#endif

    fsp_refresh(ctx->chan_id);

#ifndef HEVC_RESOLUTION_CHANGE_SUPPORT
    if (ctx->allow_start_dec == 1) {
        ret = hevc_check_resolution_change(ctx, dpb_size_changed, pre_pic_width, pre_pic_height);
        vfmw_check_ret(ret == HEVC_DEC_NORMAL, HEVC_DEC_ERR);
    }
#endif

    ret = hevc_resize_process(ctx, sps, dpb_size_changed, pre_pic_width, pre_pic_height, pre_l2_ctb_size_y,
                              pre_reorder_num, pre_sps_id);
    if (ret != HEVC_DEC_NORMAL) {
        return ret;
    }

    /* 分配帧存和PMV空间 */
    ret = hevc_alloc_frame_store(ctx);
    if (ret != HEVC_DEC_NORMAL) {
        return ret;
    }

    /* 初始化当前解码图像的参数 */
    /* pmv_address_idc,structure, *frame_store均在帧存分配函数中产生 */
    hevc_init_curr_pic_param(ctx);

    hevc_init_fs_image_para(ctx);

    fsp_update_image_param(ctx->chan_id, ctx->curr_pic.frame_store->logic_fs_id, &ctx->curr_pic.fs_image);

    return HEVC_DEC_NORMAL;
}

VFMW_ACC_SLICE_H265 STATIC td_s32 hevc_repair_list_process(hevc_ctx *ctx, td_u32 i_list)
{
    td_s8 found = -1;
    td_u32 i;

    hevc_storable_pic *repair_pic = (hevc_storable_pic *)TD_NULL;

    for (i = 0; i < ctx->curr_slice.list_x_size[i_list]; i++) {
        if (ctx->list_x[i_list][i] != (hevc_storable_pic *)TD_NULL) {
            found = (td_s8)i;
            repair_pic = ctx->list_x[i_list][i];
            break;
        }
    }

    if ((found == -1) || (repair_pic == TD_NULL)) {
        if (ctx->extra_data_ptr->output_order == VFMW_DEC_ORDER_SIMPLE_DPB) {
            /* 如果运行于DEC_ORDER_SIMPLE_DPB, 则用当前pic填充所有list节点 */
            repair_pic = &ctx->curr_pic.frame_store->frame;
        } else {
            dprint(PRN_ERROR, "%s list_x[%d] no available pic to repair.\n", __func__, i_list);
            return HEVC_DEC_ERR;
        }
    }

    for (i = 0; i < ctx->curr_slice.list_x_size[i_list]; i++) {
        if (ctx->list_x[i_list][i] == (hevc_storable_pic *)TD_NULL) {
            dprint(PRN_ERROR, "%s: list_x[%d][%d] = TD_NULL, repair it with pos(%d).\n",
                   __func__, i_list, i, found);
            ctx->list_x[i_list][i] = repair_pic;
        }
    }

    return HEVC_DEC_NORMAL;
}

/*************************************************************************
* 函数原型 : td_s32 RepairList(hevc_ctx *ctx)
* 功能描述 : 修补list,只考虑去掉list中的NULL
* 参数说明 :
* 返回值   ：
*************************************************************************/
VFMW_ACC_SLICE_H265 STATIC td_s32 hevc_repair_list(hevc_ctx *ctx)
{
    td_u32 i_list, i_list_cnt;

    switch (ctx->curr_slice.slice_type) {
        case HEVC_P_SLICE:
            i_list_cnt = 1;
            break;

        case HEVC_B_SLICE:
            i_list_cnt = 2; /* 2 is list num */
            break;

        case HEVC_I_SLICE:
            i_list_cnt = 0;
            break;

        default:
            dprint(PRN_ERROR, "%s unkown slice type(%d).\n", __func__, ctx->curr_slice.slice_type);
            return HEVC_DEC_ERR;
    }

    for (i_list = 0; i_list < i_list_cnt; i_list++) {
        if (ctx->curr_slice.list_x_size[i_list] > 0) {
            if (hevc_repair_list_process(ctx, i_list) != HEVC_DEC_NORMAL) {
                return HEVC_DEC_ERR;
            }
        } else {
            if (ctx->extra_data_ptr->output_order == VFMW_DEC_ORDER_SIMPLE_DPB) {
                /* 如果运行于DEC_ORDER_SIMPLE_DPB, 则用当前pic填充所有list节点 */
                ctx->curr_slice.list_x_size[i_list] = 1;
                ctx->list_x[i_list][0] = &ctx->curr_pic.frame_store->frame;
            }
        }
    }

    return HEVC_DEC_NORMAL;
}

/***********************************************************************************
* Function:    hevc_dec_list()
* Description: 生成当前slice的参考列表
* Input:       ctx->curr_slice   当前Slice参数
*              ctx->list_x[2][HEVC_MAX_LIST_SIZE]   List列表
* Output:      ctx->list_x[2][HEVC_MAX_LIST_SIZE]   List列表
* Return:      HEVC_DEC_NORMAL   可以正确生成List列表
*              HEVC_DEC_ERR      不可以正确生成List列表
* Others:      无
***********************************************************************************/
VFMW_ACC_SLICE_H265 STATIC td_s32 hevc_dec_list(hevc_ctx *ctx)
{
    td_s32 ret;

    if (ctx->curr_slice.slice_type == HEVC_I_SLICE) {
        ctx->curr_slice.list_x_size[0] = 0;
        ctx->curr_slice.list_x_size[1] = 0;
        return HEVC_DEC_NORMAL;
    }

    ret = hevc_init_list_x(ctx);
    if (ret != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "%s error.\n", __func__);
        return HEVC_DEC_ERR;
    }

    ret = hevc_repair_list(ctx);
    if (ret != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "%s error.\n", __func__);
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_set_unused_list(hevc_ctx *ctx)
{
    td_u32 i;

    for (i = ctx->curr_slice.list_x_size[0]; i < (HEVC_MAX_LIST_SIZE); i++) {
        ctx->list_x[0][i] = ctx->list_x[0][0];
    }

    for (i = ctx->curr_slice.list_x_size[1]; i < (HEVC_MAX_LIST_SIZE); i++) {
        ctx->list_x[1][i] = ctx->list_x[1][0];
    }

    return;
}

STATIC td_void hevc_set_list_x(hevc_ctx *ctx, hevc_storable_pic *temp_list_x[2][HEVC_MAX_LIST_SIZE],
                                td_u32 ref_frame_num)
{
    td_u32 i, idx;
    hevc_ref_pic_lists_moddification *rpl_modify = TD_NULL;

    vfmw_check(ref_frame_num != 0);
    /* set vfmw_max size */
    ctx->curr_slice.list_x_size[0] = ctx->curr_slice.num_ref_idx[0];
    ctx->curr_slice.list_x_size[1] = ctx->curr_slice.num_ref_idx[1];

    rpl_modify = &(ctx->curr_slice.ref_pic_lists_modification);

    for (i = 0; i < ctx->curr_slice.list_x_size[0]; i++) {
        idx = rpl_modify->ref_pic_list_modification_flag_l0 ? rpl_modify->list_entry_l0[i] : i % ref_frame_num;
        if (idx >= HEVC_MAX_LIST_SIZE) {
            return;
        }
        ctx->list_x[0][i] = temp_list_x[0][idx];
    }

    if (ctx->curr_slice.slice_type == HEVC_B_SLICE) {
        for (i = 0; i < ctx->curr_slice.list_x_size[1]; i++) {
            idx = rpl_modify->ref_pic_list_modification_flag_l1 ? rpl_modify->list_entry_l1[i] : i % ref_frame_num;
            if (idx >= HEVC_MAX_LIST_SIZE) {
                return;
            }
            ctx->list_x[1][i] = temp_list_x[1][idx];
        }

        /* for generalized b slice */
        if (ctx->curr_slice.list_x_size[1] == 0) {
            vfmw_check_sec_func(memcpy_s(ctx->list_x[1], sizeof(ctx->list_x[1]),
                                         ctx->list_x[0], sizeof(ctx->list_x[0])));

            ctx->curr_slice.list_x_size[1] = ctx->curr_slice.list_x_size[0];
        }
    }

    return;
}

STATIC td_void hevc_no_ref_frame_process(hevc_ctx *ctx)
{
    ctx->curr_slice.list_x_size[0] = 0;
    ctx->curr_slice.list_x_size[1] = 0;

    vfmw_check_sec_func(memset_s(ctx->list_x, sizeof(ctx->list_x), 0, sizeof(ctx->list_x)));

    /* Simple DPB模式下，用当前帧作为参考图强制解码 */
    if (ctx->extra_data_ptr->output_order == VFMW_DEC_ORDER_SIMPLE_DPB) {
        ctx->curr_slice.list_x_size[0] = 1;
        ctx->list_x[0][0] = &ctx->curr_pic.frame_store->frame;
    }

    return;
}

STATIC td_u32 hevc_get_nega_ref_num(const hevc_ctx *ctx, hevc_storable_pic *temp_list_x[2][HEVC_MAX_LIST_SIZE],
                                        td_u32 num)
{
    td_u32 i;
    td_u32 idx = num;

    for (i = 0; i < ctx->dpb.negative_ref_frames_in_buffer; i++) {
        if ((ctx->dpb.fs_negative_ref[i]->is_reference == 1) &&
            (ctx->dpb.fs_negative_ref[i]->frame.is_long_term == 0) &&
            (ctx->dpb.fs_negative_ref[i]->frame.is_short_term == 1) &&
            (idx < HEVC_MAX_LIST_SIZE)) {
            temp_list_x[0][idx++] = &ctx->dpb.fs_negative_ref[i]->frame;
        }
    }

    return idx;
}

STATIC td_u32 hevc_get_pos_ref_num(const hevc_ctx *ctx, hevc_storable_pic *temp_list_x[2][HEVC_MAX_LIST_SIZE],
                                        td_u32 num)
{
    td_u32 i;
    td_u32 idx = num;

    for (i = 0; i < ctx->dpb.positive_ref_frames_in_buffer; i++) {
        if ((ctx->dpb.fs_positive_ref[i]->is_reference == 1) &&
            (ctx->dpb.fs_positive_ref[i]->frame.is_long_term == 0) &&
            (ctx->dpb.fs_positive_ref[i]->frame.is_short_term == 1)) {
            if (idx < HEVC_MAX_LIST_SIZE) {
                temp_list_x[0][idx++] = &ctx->dpb.fs_positive_ref[i]->frame;
            }
        }
    }

    return idx;
}

STATIC td_void hevc_init_long_term_ref_list(const hevc_ctx *ctx,
    hevc_storable_pic *temp_list_x[2][HEVC_MAX_LIST_SIZE],  /* 2: 2 direction reference */
    td_u32 *idx)
{
    td_u32 i;

    for (i = 0; i < ctx->dpb.ltref_frames_in_buffer; i++) {
        if ((ctx->dpb.fs_ltref[i]->is_reference == 1) && (ctx->dpb.fs_ltref[i]->frame.is_long_term == 1) &&
            (ctx->dpb.fs_ltref[i]->frame.is_short_term == 0) && (*idx < HEVC_MAX_LIST_SIZE)) {
            temp_list_x[0][(*idx)++] = &ctx->dpb.fs_ltref[i]->frame;
        }
    }
}

STATIC td_void hevc_init_temp_list_0(const hevc_ctx *ctx,
    hevc_storable_pic *temp_list_x[2][HEVC_MAX_LIST_SIZE],  /* 2: 2 direction reference */
    td_u32 *ref_frame_num, td_u32 *neg_ref_frame_num, td_u32 *pos_ref_frame_num,
    td_u32 *neg_inlayer_ref_num, td_u32 *pos_inlayer_ref_num, td_u32 *lt_ref_frame_num)
{
#ifdef VFMW_MV_HEVC_SUPPORT
    td_u32 i;
#endif
    td_u32 idx;

    idx = hevc_get_nega_ref_num(ctx, temp_list_x, 0);
    *neg_ref_frame_num = idx;
    *neg_inlayer_ref_num = 0;
#ifdef VFMW_MV_HEVC_SUPPORT
    if (ctx->is_multi_layers) {
        for (i = 0; i < ctx->dpb.negative_inter_layer_ref_frames; i++) {
            if (ctx->dpb.fs_negative_inter_layer_ref[i]->is_reference == 1 &&
                ctx->dpb.fs_negative_inter_layer_ref[i]->frame.is_inter_layer == 1) {
                temp_list_x[0][idx++] = &ctx->dpb.fs_negative_inter_layer_ref[i]->frame;
            }
        }
        *neg_inlayer_ref_num = idx - *neg_ref_frame_num;
    }
#endif

    idx = hevc_get_pos_ref_num(ctx, temp_list_x, idx);
    if (ctx->is_multi_layers) {
        *pos_ref_frame_num = idx - *neg_ref_frame_num - *neg_inlayer_ref_num;
    } else {
        *pos_ref_frame_num = idx - *neg_ref_frame_num;
    }

    /* long term handling */
    hevc_init_long_term_ref_list(ctx, temp_list_x, &idx);

#ifdef VFMW_MV_HEVC_SUPPORT
    if (ctx->is_multi_layers) {
        *lt_ref_frame_num = idx - *neg_ref_frame_num - *pos_ref_frame_num - *neg_inlayer_ref_num;
        for (i = 0; i < ctx->dpb.positive_inter_layer_ref_frames; i++) {
            if (ctx->dpb.fs_positive_inter_layer_ref[i]->is_reference == 1) {
                temp_list_x[0][idx++] = &ctx->dpb.fs_positive_inter_layer_ref[i]->frame;
            }
        }
        *pos_inlayer_ref_num = idx - *neg_ref_frame_num - *pos_ref_frame_num - *neg_inlayer_ref_num
            - *lt_ref_frame_num;
    } else {
#else
    vfmw_unused(pos_inlayer_ref_num);
#endif
        *lt_ref_frame_num = idx - *neg_ref_frame_num - *pos_ref_frame_num;
#ifdef VFMW_MV_HEVC_SUPPORT
    }
#endif

    *ref_frame_num = idx;
}

STATIC td_void hevc_init_negref_temp_list(
    hevc_storable_pic *temp_list_x[2][HEVC_MAX_LIST_SIZE], /* 2: direct reference */
    td_u32 neg_ref_frame_num, td_u32 *idx)
{
    td_u32 i;

    for (i = 0; i < neg_ref_frame_num && *idx < HEVC_MAX_LIST_SIZE; i++) {
        temp_list_x[1][(*idx)++] = temp_list_x[0][i];
    }
}

STATIC td_void hevc_init_lref_temp_list(
    hevc_storable_pic *temp_list_x[2][HEVC_MAX_LIST_SIZE], /* 2: direct reference */
    td_u32 lt_ref_frame_num, td_u32 neg_ref_frame_num, td_u32 pos_ref_frame_num, td_u32 *idx)
{
    td_u32 i;

    for (i = 0; i < lt_ref_frame_num && *idx < HEVC_MAX_LIST_SIZE; i++) {
        if (((neg_ref_frame_num + pos_ref_frame_num + i) < HEVC_MAX_LIST_SIZE) &&
            (neg_ref_frame_num < HEVC_MAX_LIST_SIZE) &&
            (pos_ref_frame_num < HEVC_MAX_LIST_SIZE)) {
            temp_list_x[1][(*idx)++] = temp_list_x[0][neg_ref_frame_num + pos_ref_frame_num + i];
        }
    }
}

#ifdef VFMW_MV_HEVC_SUPPORT
STATIC td_void hevc_init_multi_layer_temp_list(const hevc_ctx *ctx,
    hevc_storable_pic *temp_list_x[2][HEVC_MAX_LIST_SIZE], td_u32 neg_inlayer_ref_num,
    td_u32 lt_ref_frame_num, td_u32 neg_ref_frame_num, td_u32 pos_ref_frame_num, td_u32 *idx)
{
    td_u32 i;
    td_u32 total_list_num;

    total_list_num = 0;
    if (ctx->is_multi_layers) {
        for (i = 0; i < lt_ref_frame_num && (*idx) < HEVC_MAX_LIST_SIZE; i++) {
            total_list_num = neg_ref_frame_num + neg_inlayer_ref_num + pos_ref_frame_num + i;
            if ((total_list_num) >= HEVC_MAX_LIST_SIZE) {
                dprint(PRN_ERROR, "line %d func %s temp_list_x(%d) overrun error\n", __LINE__, __func__,
                    total_list_num);
                break;
            }
            temp_list_x[1][(*idx)++] = temp_list_x[0][total_list_num];
        }

        for (i = 0; i < neg_inlayer_ref_num && (*idx) < HEVC_MAX_LIST_SIZE; i++) {
            total_list_num = neg_ref_frame_num + i;
            if ((total_list_num) >= HEVC_MAX_LIST_SIZE) {
                dprint(PRN_ERROR, "line %d func %s idx = %d overrun error\n", __LINE__, __func__,
                    total_list_num);
                break;
            }
            temp_list_x[1][(*idx)++] = temp_list_x[0][total_list_num];
        }
    }
}
#endif

STATIC td_void hevc_init_temp_list_1(const hevc_ctx *ctx,
    hevc_storable_pic *temp_list_x[2][HEVC_MAX_LIST_SIZE],  /* 2: 2 direction reference */
    td_u32 neg_ref_frame_num, td_u32 pos_ref_frame_num, td_u32 lt_ref_frame_num,
    td_u32 neg_inlayer_ref_num, td_u32 pos_inlayer_ref_num)
{
    td_u32 i, idx;

    if (ctx->curr_slice.slice_type == HEVC_B_SLICE) {
        idx = 0;

#ifdef VFMW_MV_HEVC_SUPPORT
        if (ctx->is_multi_layers) {
            for (i = 0; i < pos_ref_frame_num && idx < HEVC_MAX_LIST_SIZE; i++) {
                temp_list_x[1][idx++] = temp_list_x[0][neg_ref_frame_num + neg_inlayer_ref_num + i];
            }

            for (i = 0; i < pos_inlayer_ref_num && idx < HEVC_MAX_LIST_SIZE; i++) {
                temp_list_x[1][idx++] = temp_list_x[0][neg_ref_frame_num + neg_inlayer_ref_num +
                pos_ref_frame_num + lt_ref_frame_num+ i];
            }
        } else
#endif
        {
            for (i = 0; i < pos_ref_frame_num &&
                idx < HEVC_MAX_LIST_SIZE && (neg_ref_frame_num + i) < HEVC_MAX_LIST_SIZE; i++) {
                temp_list_x[1][idx++] = temp_list_x[0][neg_ref_frame_num + i];
            }
        }

        hevc_init_negref_temp_list(temp_list_x, neg_ref_frame_num, &idx);

#ifdef VFMW_MV_HEVC_SUPPORT
        hevc_init_multi_layer_temp_list(ctx, temp_list_x, neg_inlayer_ref_num, lt_ref_frame_num,
            neg_ref_frame_num, pos_ref_frame_num, &idx);
#else
        vfmw_unused(neg_inlayer_ref_num);
        vfmw_unused(pos_inlayer_ref_num);
#endif

        hevc_init_lref_temp_list(temp_list_x, lt_ref_frame_num, neg_ref_frame_num, pos_ref_frame_num, &idx);
    }
}

/*************************************************************************
* 函数原型 : td_s32  hevc_init_list_x()
* 功能描述 : 参考列表初始化
* 参数说明 :
* 返回值   ：
*************************************************************************/
VFMW_ACC_SLICE_H265 STATIC td_s32 hevc_init_list_x(hevc_ctx *ctx)
{
    hevc_storable_pic *temp_list_x[2][HEVC_MAX_LIST_SIZE]; /* 2 is array size */
    td_u32 ref_frame_num;
    td_u32 neg_ref_frame_num, pos_ref_frame_num, lt_ref_frame_num;
    td_u32 neg_inter_layer_ref_frame_num = 0;
    td_u32 pos_inter_layer_ref_frame_num = 0;

    vfmw_check_sec_func(memset_s(temp_list_x, sizeof(temp_list_x), 0, sizeof(temp_list_x)));

    hevc_init_temp_list_0(ctx, temp_list_x, &ref_frame_num, &neg_ref_frame_num, &pos_ref_frame_num,
        &neg_inter_layer_ref_frame_num, &pos_inter_layer_ref_frame_num, &lt_ref_frame_num);

    if (ref_frame_num == 0) {
        hevc_no_ref_frame_process(ctx);
        return HEVC_DEC_NORMAL;
    }

    hevc_init_temp_list_1(ctx, temp_list_x, neg_ref_frame_num, pos_ref_frame_num, lt_ref_frame_num,
        neg_inter_layer_ref_frame_num, pos_inter_layer_ref_frame_num);

    hevc_set_list_x(ctx, temp_list_x, ref_frame_num);

    /* set the unused list entries to a valid one */
    hevc_set_unused_list(ctx);

    /* InitlistX over */
    return HEVC_DEC_NORMAL;
}

/*************************************************************************
* 函数原型 : td_s32 compare_pic_by_poc_asc(const td_void *arg1, const td_void *arg2)
* 功能描述 :  按升序比较hevc_frame_store结构体中的元素poc的大小，
* 参数说明 :
* 返回值   ：
*************************************************************************/
STATIC td_s32 hevc_compare_pic_by_poc_asc(const td_void *arg1, const td_void *arg2)
{
    if ((*(hevc_frame_store **)arg1)->poc < (*(hevc_frame_store **)arg2)->poc) {
        return -1;
    }

    if ((*(hevc_frame_store **)arg1)->poc > (*(hevc_frame_store **)arg2)->poc) {
        return 1;
    } else {
        return 0;
    }
}

STATIC td_void hevc_clear_logic_fs(const hevc_ctx *ctx)
{
    td_u32 i;

    for (i = 0; i < HEVC_MAX_DPB_NUM; i++) {
        if (ctx->dpb.fs[i] != TD_NULL) {
            fsp_clear_logic(ctx->chan_id, ctx->dpb.fs[i]->logic_fs_id, 1);
        }

        if (ctx->dpb.fs_negative_ref[i] != TD_NULL) {
            fsp_clear_logic(ctx->chan_id, ctx->dpb.fs_negative_ref[i]->logic_fs_id, 1);
        }

        if (ctx->dpb.fs_positive_ref[i] != TD_NULL) {
            fsp_clear_logic(ctx->chan_id, ctx->dpb.fs_positive_ref[i]->logic_fs_id, 1);
        }

        if (ctx->dpb.fs_ltref[i] != TD_NULL) {
            fsp_clear_logic(ctx->chan_id, ctx->dpb.fs_ltref[i]->logic_fs_id, 1);
        }
#ifdef VFMW_MV_HEVC_SUPPORT
        if (ctx->dpb.fs_negative_inter_layer_ref[i] != TD_NULL) {
            fsp_clear_logic(ctx->chan_id, ctx->dpb.fs_negative_inter_layer_ref[i]->logic_fs_id, 1);
        }

        if (ctx->dpb.fs_positive_inter_layer_ref[i] != TD_NULL) {
            fsp_clear_logic(ctx->chan_id, ctx->dpb.fs_positive_inter_layer_ref[i]->logic_fs_id, 1);
        }
#endif
    }

    return;
}

STATIC td_void hevc_init_dpb_size(hevc_ctx *ctx)
{
#ifdef VFMW_MV_HEVC_SUPPORT
    if (hevc_is_multi_layer_valid(ctx) == 1 && (ctx->layer_num > 0)) {
        vfmw_check(ctx->curr_sps->dpb_size * ctx->layer_num < VFMW_MAX_UINT32);
        ctx->dpb.size = ctx->curr_sps->dpb_size * ctx->layer_num + 1;
    } else
#endif
    {
        ctx->dpb.size = ctx->curr_sps->dpb_size; /* + 1 */
    }

    if (ctx->dpb.size > HEVC_MAX_DPB_NUM) {
        ctx->dpb.size = HEVC_MAX_DPB_NUM;
    }
}

STATIC td_s32 hevc_wait_flush_mem(hevc_ctx *ctx, td_u32 size_change)
{
    if (size_change == 1 && fsp_get_frame_buf_mode(ctx->chan_id) == VFMW_FRM_ALLOC_OUTER) {
        td_u32 readnum = 0;
        td_u32 newnum = 0;
        voq_get_queue_img_num(ctx->chan_id, &readnum, &newnum);
        if (newnum > 0) {
            return HEVC_DEC_ERR;
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_init_list(hevc_ctx *ctx)
{
    td_u32 i, j;
    for (i = 0; i < 2; i++) { /* 2 is vfmw_max num */
        for (j = 0; j < HEVC_MAX_LIST_SIZE; j++) {
            ctx->list_x[i][j] = TD_NULL;
        }

        ctx->curr_slice.list_x_size[i] = 0;
    }

    return;
}

STATIC td_void hevc_init_disp_res(const hevc_ctx *ctx, td_u32 *image_width_disp, td_u32 *image_height_disp)
{
    if (ctx->extra_data_ptr->air_version == AIR_DEC_FRAME) {
        *image_width_disp = ctx->curr_sps->air_parameters.air_big_width;
        *image_height_disp = ctx->curr_sps->air_parameters.air_big_height;
    } else {
        *image_width_disp = ctx->curr_sps->pic_width_in_luma_samples;
        *image_height_disp = ctx->curr_sps->pic_height_in_luma_samples;
    }
}

/*************************************************************************
*      函数原型 :  td_s32 hevc_init_dpb(hevc_ctx *ctx,td_u32 rerange_flag)
*      功能描述 :  初始化dpb
*      参数说明 :
*      返回值   ：
*************************************************************************/
STATIC td_s32 hevc_init_dpb(hevc_ctx *ctx, td_u32 rerange_flag)
{
    td_s32 ret;
    td_u32 i, image_width, image_height, bit_depth, is_arrange_mem;
    td_u32 image_width_disp, image_height_disp, max_dec_w_align_up, max_dec_h_align_up;

    hevc_clear_all_slice(ctx);

    /* 清除逻辑帧存 dpb refresh */
    hevc_clear_logic_fs(ctx);

    vfmw_check_sec_func(memset_s(&ctx->dpb, sizeof(hevc_dpb), 0, sizeof(hevc_dpb)));

    hevc_init_dpb_size(ctx);
    hevc_init_list(ctx);

    if (rerange_flag || (ctx->extra_data_ptr->wait_fs_flag == 1)) {
        vfmw_check_ret(hevc_wait_flush_mem(ctx, rerange_flag) == HEVC_DEC_NORMAL, SYN_PARSE_RESOURCE_INVALID);

        bit_depth = vfmw_max(ctx->curr_sps->bit_depth_luma, ctx->curr_sps->bit_depth_chroma);

        ctx->total_fs_num = HEVC_MAX_FRAME_STORE;
        ctx->total_pmv_num = HEVC_MAX_PMV_STORE;

        vfmw_check_ret(ctx->curr_sps->pic_width_in_luma_samples <= HEVC_MAX_PIX_WIDTH, HEVC_DEC_ERR);
        vfmw_check_ret(ctx->curr_sps->pic_height_in_luma_samples <= HEVC_MAX_PIX_HEIGHT, HEVC_DEC_ERR);
        image_width = ctx->curr_sps->pic_width_in_luma_samples;
        image_height = ctx->curr_sps->pic_height_in_luma_samples;

        hevc_init_disp_res(ctx, &image_width_disp, &image_height_disp);

        max_dec_w_align_up = vfmw_align_up(ctx->extra_data.max_dec_width, 64);
        max_dec_h_align_up = vfmw_align_up(ctx->extra_data.max_dec_height, 64);
        if ((ctx->extra_data.is_max_buf_size_en == TD_TRUE) && (ctx->allow_start_dec != 0) &&
            (image_width * image_height <= max_dec_w_align_up * max_dec_h_align_up)) {
            is_arrange_mem = TD_FALSE;
        } else {
            is_arrange_mem = TD_TRUE;
        }

        if (is_arrange_mem == TD_TRUE) {
#ifdef VFMW_MV_HEVC_SUPPORT
            if (hevc_is_multi_layer_valid(ctx) == 1) {
                ret = hevc_arrange_shvc_mem(ctx, bit_depth);
            } else {
#endif
                ret = hevc_arrange_vahb_mem(ctx, image_width, image_height, image_width_disp,
                                            image_height_disp, ctx->total_fs_num, ctx->total_pmv_num, bit_depth);
#ifdef VFMW_MV_HEVC_SUPPORT
            }
#endif
            if (ret == SYN_PARSE_RESOURCE_INVALID) {
                return SYN_PARSE_RESOURCE_INVALID;
            }
        }

        ctx->total_fs_num = (td_u32)(vfmw_min(fsp_get_total_num(ctx->chan_id), HEVC_MAX_FRAME_STORE));
        ctx->prev_reorder_num = ctx->curr_sps->reorder_num;
    }

    for (i = 0; i < ctx->total_pmv_num; i++) {
        ctx->pmv_store_used_flag[i] = 0;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_init_dec_buffers(hevc_ctx *ctx)
{
    td_u32 i, j;

    ctx->dpb.size = 0;

    if (ctx->curr_sps != TD_NULL) {
#ifdef VFMW_MV_HEVC_SUPPORT
        if (hevc_is_multi_layer_valid(ctx) == 1) {
            vfmw_check(ctx->curr_sps->dpb_size * ctx->layer_num  < VFMW_MAX_UINT32);
            ctx->dpb.size = ctx->curr_sps->dpb_size * ctx->layer_num + 1;
        } else
#endif
        {
            ctx->dpb.size = ctx->curr_sps->dpb_size;
        }
    }

    if (ctx->dpb.size > HEVC_MAX_DPB_NUM) {
        ctx->dpb.size = HEVC_MAX_DPB_NUM;
    }

    ctx->dpb.used_size = 0;
    ctx->dpb.negative_ref_frames_in_buffer = 0;
    ctx->dpb.positive_ref_frames_in_buffer = 0;
    ctx->dpb.ltref_frames_in_buffer = 0;
    ctx->dpb.max_long_term_pic_idx = 0;
#ifdef VFMW_MV_HEVC_SUPPORT
    ctx->dpb.negative_inter_layer_ref_frames = 0;
    ctx->dpb.positive_inter_layer_ref_frames = 0;
#endif

    for (i = 0; i < HEVC_MAX_DPB_NUM; i++) {
        ctx->dpb.fs_ltref[i] = TD_NULL;
        ctx->dpb.fs_negative_ref[i] = TD_NULL;
        ctx->dpb.fs_positive_ref[i] = TD_NULL;
        ctx->dpb.fs[i] = TD_NULL;
#ifdef VFMW_MV_HEVC_SUPPORT
        ctx->dpb.fs_negative_inter_layer_ref[i] = TD_NULL;
        ctx->dpb.fs_positive_inter_layer_ref[i] = TD_NULL;
#endif
    }

    hevc_init_frame_store_para(ctx);

    /* Init ListX */
    for (i = 0; i < 2; i++) { /* 2 is vfmw_max num */
        for (j = 0; j < HEVC_MAX_LIST_SIZE; j++) {
            ctx->list_x[i][j] = TD_NULL;
        }

        ctx->curr_slice.list_x_size[i] = 0;
    }

    /* Init PMVAddr used flag */
    for (i = 0; i < ctx->total_pmv_num; i++) {
        ctx->pmv_store_used_flag[i] = 0;
    }

    return;
}

/*************************************************************************
*      函数原型 :  td_s32 hevc_direct_output()
*      功能描述 :  直接输出当前帧
*      参数说明 :  无
*      返回值   ： HEVC_DEC_NORMAL: 成功;
*                  其他值
*************************************************************************/
STATIC td_s32 hevc_direct_output(hevc_ctx *ctx)
{
    td_s32 ret;
    hevc_frame_store *direct_out_buf = TD_NULL;

    /* 把当前图像存入DirectOutBuf，再输出 */
    direct_out_buf = ctx->curr_pic.frame_store;

    direct_out_buf->frame_store_state = FS_DISPLAY_ONLY;
    direct_out_buf->is_reference = 0;
    direct_out_buf->is_displayed = 1;
    direct_out_buf->pic_type = ctx->curr_pic.pic_type;
    direct_out_buf->err_level = ctx->curr_pic.err_level;
    direct_out_buf->frame.structure = FRAME;
    direct_out_buf->frame.err_level = ctx->curr_pic.err_level;

    fsp_set_ref(ctx->chan_id, direct_out_buf->logic_fs_id, 0);
    if (ctx->curr_pic.pic_output_flag == HEVC_TRUE) {
        ret = hevc_output_frm_to_vo(ctx, direct_out_buf);
    } else {
        ret = HEVC_DEC_NORMAL;
        direct_out_buf->is_displayed = 1;
        fsp_set_disp(ctx->chan_id, direct_out_buf->logic_fs_id, 0);
    }

    direct_out_buf->frame_store_state = FS_NOT_USED;

    vfmw_check_sec_func(memset_s(&ctx->curr_pic, sizeof(ctx->curr_pic), 0, sizeof(ctx->curr_pic)));

    return ret;
}

/*****************************************************************************
    简化的滑窗dpb管理策略.
    当DecOrderOutput == DEC_ORDER_SIMPLE_DPB时使用此策略.
    在视频电话等领域中途不发I帧. 这样如果发生了误码，解码器不能清除参考图.
    但是作为通用解码器这样非常危险，所以在这种应用中先把DPB管理机制进行简化，
    而通用码流的处理则沿用标准策略。
 *****************************************************************************/
STATIC td_void hevc_simple_slide_dpb(hevc_ctx *ctx)
{
    if (ctx->dpb.used_size >= ctx->dpb.size) {
        /* 如果已达上限，或尝试递增DPB Size失败，清空fs[0]
           因为当前帧要插入DPB中，需要获取APC，而APC最大值等于16，所以
           在这里DPB上限不能超过APC个数(16)，而不是DPB数组上限(17) */
        /* to avoid inc dpb(pmv), use real ref frm slide dpb as h264 */
#if 1
        if (1) {
#else
        if (ctx->dpb.size >= HEVC_APC_SIZE || hevc_increase_dpb_size(ctx) != HEVC_DEC_NORMAL) {
#endif
            /* 把第0个帧存标识为不参考，并从DPB中删除这个帧 */
            hevc_unmark_frame_store_ref(ctx, ctx->dpb.fs[0]);
            hevc_remove_frame_store_out_dpb(ctx, 0);
        }
    }

    return;
}

/*************************************************************************
*   函数原型 :  td_void HEVC_RemoveApcStore()
*   功能描述 :  将补充的参考帧移出DPB，包括APC与帧存
*   参数说明 :
*   返回值   ：
*************************************************************************/
STATIC td_void hevc_remove_dummy_frame(hevc_ctx *ctx)
{
    td_u32 i = 0;

    while (i < ctx->dpb.used_size) {
        if (i < HEVC_MAX_DPB_NUM && ctx->dpb.fs[i]->non_existing) {
            hevc_remove_frame_store_out_dpb(ctx, i);
        } else {
            i++;
        }
    }

    return;
}

STATIC td_void hevc_set_ref_state(td_s32 chan_id, hevc_frame_store *fs_store)
{
    fs_store->is_reference = 1;
#ifdef VFMW_MV_HEVC_SUPPORT
    fs_store->frame.is_inter_layer = 0;
#endif
    fs_store->frame_store_state = FS_IN_DPB;
    fsp_set_ref(chan_id, fs_store->logic_fs_id, 1);
}

STATIC td_s32 hevc_long_term_reference_picture_set(hevc_ctx *ctx,
    const hevc_short_term_rpset *temp_rps, td_s32 i)
{
    td_u32 j;
    td_s32 cur_poc, ref_poc;
    td_u32 poc_cycle;
    td_s32 is_reference = 0;
    hevc_pic_param_set *pps = TD_NULL;
    hevc_seq_param_set *sps = TD_NULL;

    pps = ctx->pps[ctx->curr_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];
    poc_cycle = 1 << sps->max_pic_order_cnt_lsb;

    for (j = temp_rps->num_negative_pics + temp_rps->num_positive_pics; j < temp_rps->num_of_pics; j++) {
        if (ctx->curr_slice.check_lt_msb[j]) {
            if ((ctx->dpb.fs[i]->frame.is_long_term) && (ctx->dpb.fs[i]->poc == temp_rps->poc[j])) {
                is_reference = 1;
                hevc_set_ref_state(ctx->chan_id, ctx->dpb.fs[i]);
            }
        } else if ((ctx->dpb.fs[i]->frame.is_long_term) &&
                   (((td_u32)ctx->dpb.fs[i]->poc & (poc_cycle - 1)) ==
                   ((td_u32)temp_rps->poc[j] & (poc_cycle - 1)))) {
            is_reference = 1;
            hevc_set_ref_state(ctx->chan_id, ctx->dpb.fs[i]);
        }

        if (is_reference == 0) {
            cur_poc = ctx->dpb.fs[i]->poc;
            ref_poc = temp_rps->poc[j];

            if (!ctx->curr_slice.check_lt_msb[j]) {
                cur_poc = (td_s32)((td_u32)cur_poc & (poc_cycle - 1));
                ref_poc = (td_s32)((td_u32)ref_poc & (poc_cycle - 1));
            }

            if (ctx->dpb.fs[i]->is_reference && cur_poc == ref_poc) {
                is_reference = 1;
                ctx->dpb.fs[i]->frame.is_long_term = 1;
                hevc_set_ref_state(ctx->chan_id, ctx->dpb.fs[i]);
            }
        }
    }

    return is_reference;
}

STATIC td_s32 hevc_short_term_reference_picture_set(hevc_ctx *ctx,
    const hevc_short_term_rpset *temp_rps, td_s32 i)
{
    td_s32 is_reference = 0;
    td_u32 j;

    for (j = 0; j < (temp_rps->num_negative_pics + temp_rps->num_positive_pics); j++) {
        if ((!ctx->dpb.fs[i]->frame.is_long_term) &&
            (ctx->dpb.fs[i]->poc == (ctx->curr_slice.poc + temp_rps->delta_poc[j]))) {
            is_reference = 1;
            ctx->dpb.fs[i]->is_reference = 1;
#ifdef VFMW_MV_HEVC_SUPPORT
            ctx->dpb.fs[i]->frame.is_inter_layer = 0;
#endif
            ctx->dpb.fs[i]->frame_store_state = FS_IN_DPB;
            fsp_set_ref(ctx->chan_id, ctx->dpb.fs[i]->logic_fs_id, 1);
        }
    }

    return is_reference;
}
/*************************************************************************
*   函数原型 :  td_void hevc_apply_reference_picture_set(hevc_ctx *ctx)
*   功能描述 :  标记DPB中参考帧信息
*   参数说明 :
*   返回值   ：
*************************************************************************/
STATIC td_void hevc_apply_reference_picture_set(hevc_ctx *ctx)
{
    td_u32 i;
    td_s32 is_reference, is_long_reference, is_short_reference;

    hevc_short_term_rpset *temp_rps = &(ctx->curr_slice.short_term_ref_pic_set);

    for (i = 0; i < ctx->dpb.used_size; i++) {
#ifdef VFMW_MV_HEVC_SUPPORT
        if ((td_u32)ctx->dpb.fs[i]->view_id != ctx->curr_slice.view_id) {
            continue;
        }
#endif

        /* long term reference */
        is_long_reference = hevc_long_term_reference_picture_set(ctx, temp_rps, i);

        /* short term reference */
        is_short_reference = hevc_short_term_reference_picture_set(ctx, temp_rps, i);

        is_reference = (td_s32)(is_long_reference || is_short_reference);
        /* mark the picture as "unused for reference" if it is not in */
        /* the Reference Picture Set */
        if ((ctx->dpb.fs[i]->poc != ctx->curr_slice.poc) && (is_reference == 0)) {
            ctx->dpb.fs[i]->is_reference = 0;
            ctx->dpb.fs[i]->frame.is_long_term = 0;
#ifdef VFMW_MV_HEVC_SUPPORT
            ctx->dpb.fs[i]->frame.is_inter_layer = 0;
#endif
            /* 如果是快速输出帧，处于等待正常输出状态，先不清逻辑帧存ref状态，以免逻辑帧存被释放重用 */
            if (ctx->dpb.fs[i]->first_frame_state != FIRST_FRAME_WAIT_OUT) {
                fsp_set_ref(ctx->chan_id, ctx->dpb.fs[i]->logic_fs_id, 0);
            }
        }
    }

    return;
}

STATIC td_void  hevc_check_long_term_pics(const hevc_ctx *ctx, const hevc_short_term_rpset *temp_rps,
    td_s32 *at_least_one_lost, td_s32 *poc_lost)
{
    td_u32 i, j;
    td_s32 is_available;
    td_u32 poc_cycle;
    hevc_pic_param_set *pps = TD_NULL;
    hevc_seq_param_set *sps = TD_NULL;

    pps = ctx->pps[ctx->curr_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];
    poc_cycle = 1 << sps->max_pic_order_cnt_lsb;

    for (i = temp_rps->num_negative_pics + temp_rps->num_positive_pics; i < temp_rps->num_of_pics; i++) {
        is_available = 0;

        /* loop through all pictures in the reference picture buffer */
        for (j = 0; j < ctx->dpb.used_size; j++) {
            if (((ctx->curr_slice.check_lt_msb[i] != 0) &&
                (ctx->dpb.fs[j]->frame.is_long_term != 0) &&
                (ctx->dpb.fs[j]->poc == temp_rps->poc[i] && ctx->dpb.fs[j]->is_reference)) ||
                ((ctx->curr_slice.check_lt_msb[i] == 0) &&
                (ctx->dpb.fs[j]->frame.is_long_term != 0) &&
                (ctx->dpb.fs[j]->is_reference != 0) &&
                (((td_u32)ctx->dpb.fs[j]->poc & (poc_cycle - 1)) == ((td_u32)temp_rps->poc[i] & (poc_cycle - 1))))) {
                    is_available = 1;
            }
        }

        /* report that a picture is lost if it is in the Reference Picture Set */
        /* but not available as reference picture */
        if (is_available == 0) {
            if ((ctx->curr_slice.poc + temp_rps->delta_poc[i] >= ctx->poc_random_access[ctx->curr_slice.view_id]) &&
                (temp_rps->used_flag[i] != 0)) {
               *at_least_one_lost = 1;
               *poc_lost = ctx->curr_slice.poc + temp_rps->delta_poc[i];
            }
        }
    }

    return;
}

STATIC td_void hevc_check_short_term_pics(const hevc_ctx *ctx, const hevc_short_term_rpset *temp_rps,
    td_s32 *at_least_one_lost, td_s32 *poc_lost)
{
    td_u32 i, j, is_available;

    for (i = 0; i < temp_rps->num_negative_pics + temp_rps->num_positive_pics; i++) {
        is_available = 0;

        /* loop through all pictures in the reference picture buffer */
        for (j = 0; j < ctx->dpb.used_size; j++) {
            if (!ctx->dpb.fs[j]->frame.is_long_term &&
                ctx->dpb.fs[j]->poc == ctx->curr_slice.poc + temp_rps->delta_poc[i] &&
                ctx->dpb.fs[j]->is_reference) {
                is_available = 1;
            }
        }

        /* report that a picture is lost if it is in the Reference Picture Set */
        /* but not available as reference picture */
        if ((is_available == 0) &&
            (ctx->curr_slice.poc + temp_rps->delta_poc[i] >= ctx->poc_random_access[ctx->curr_slice.view_id])) {
            if (!temp_rps->used_flag[i]) {
            } else {
                *at_least_one_lost = 1;
                *poc_lost = ctx->curr_slice.poc + temp_rps->delta_poc[i];
            }
        }
    }

    return;
}

STATIC td_s32 hevc_check_ref_pics(const hevc_ctx *ctx, td_s32 *lost_poc)
{
    td_s32 at_least_one_lost, poc_lost;
    const hevc_short_term_rpset *temp_rps = &(ctx->curr_slice.short_term_ref_pic_set);

#ifdef VFMW_MV_HEVC_SUPPORT
    if ((ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_CRA) ||
        (ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP) ||
        (ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL)) {
        return 1;
    }
#endif
    at_least_one_lost = 0;
    poc_lost = 0;
    /* loop through all long-term pictures in the Reference Picture Set */
    /* to see if the picture should be kept as reference picture */
    hevc_check_long_term_pics(ctx, temp_rps, &at_least_one_lost, &poc_lost);

    /* loop through all short-term pictures in the Reference Picture Set */
    /* to see if the picture should be kept as reference picture */
    hevc_check_short_term_pics(ctx, temp_rps, &at_least_one_lost, &poc_lost);

    if (at_least_one_lost) {
        *lost_poc = poc_lost;
        return 0;
    }

    return 1;
}

STATIC td_s32 hevc_find_unused_fs(hevc_ctx *ctx, td_s32 lost_pos, hevc_curr_pic *lost_pic)
{
    td_u32 i;

    /* 找一个没有用到的帧存 */
    for (i = 0; i < HEVC_MAX_FRAME_STORE; i++) {
        if (ctx->frame_store[i].frame_store_state == FS_NOT_USED &&
            (hevc_is_out_dpb(ctx, &ctx->frame_store[i]) == 1)) {
            /* error_level必须标记为有错，否则在解gap，或者其它原因其实并未解码
               但发生了StorePicInDPB()的操作，则这个本当不该输出的图有可能被输出 */
            lost_pic->err_level = 101; /* 101 is init err */
            lost_pic->state = HEVC_PIC_DECODING;
            lost_pic->thispoc = lost_pos;
            lost_pic->is_ref_idc = 1;
            lost_pic->frame_store = &ctx->frame_store[i];
            lost_pic->frame_store->is_displayed = 1; /* 1 防止该帧误输出 */
            lost_pic->frame_store->first_frame_state = FIRST_FRAME_OUT_INIT;
            lost_pic->frame_store->non_existing = 1; /* 1 补参考帧的标识 */
            break;
        }
    }

    if (i >= HEVC_MAX_FRAME_STORE) {
        dprint(PRN_ERROR, "No frame store for lost pic.\n");
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_create_lost_picture(hevc_ctx *ctx, td_s32 lost_pos)
{
    td_u32 i;
    td_s32 ret;
    td_s32 closest_poc = HEVC_MAX_INT;
    td_s8 is_find_poc = 0;
    hevc_curr_pic lost_pic;
    fsp_logic *ref_logic_fs = TD_NULL;

    ret = hevc_find_unused_fs(ctx, lost_pos, &lost_pic);
    if (ret != HEVC_DEC_NORMAL) {
        return ret;
    }

    for (i = 0; i < ctx->dpb.used_size; i++) {
        if (vfmw_abs(ctx->dpb.fs[i]->poc - lost_pos) < closest_poc && ctx->dpb.fs[i]->is_reference) {
            closest_poc = vfmw_abs(ctx->dpb.fs[i]->poc - lost_pos);
            is_find_poc = 1;
        }
    }

    /* 如果DPB没有参考帧,不能补参考帧 */
    if (is_find_poc == 0) {
        lost_pic.frame_store->non_existing = 0;
        dprint(PRN_ERROR, "dpb no suited fs for lost pic.\n");
        return HEVC_DEC_ERR;
    }

    for (i = 0; i < ctx->dpb.used_size; i++) {
        if (vfmw_abs(ctx->dpb.fs[i]->poc - lost_pos) == closest_poc && ctx->dpb.fs[i]->is_reference) {
            ref_logic_fs = fsp_get_logic(ctx->chan_id, ctx->dpb.fs[i]->logic_fs_id);
            dprint(PRN_ERROR, "Take poc(%d) to creat lost poc(%d).\n", ctx->dpb.fs[i]->poc, lost_pos);
            break;
        }
    }

    if (ref_logic_fs == TD_NULL) {
        dprint(PRN_ERROR, "dpb ERROR: Can not get ref_fs!\n");
        return HEVC_FS_ALLOC_ERR;
    }

    lost_pic.frame_store->logic_fs_id = ref_logic_fs->logic_id;
    lost_pic.create_lost_pic = HEVC_TRUE;

    ret = hevc_insert_frm_in_dpb(ctx, ctx->dpb.used_size, &lost_pic);
    lost_pic.state = HEVC_PIC_EMPTY;
    if (ret != HEVC_DEC_NORMAL) {
        lost_pic.frame_store->non_existing = 0;
        dprint(PRN_ERROR, "Lost pic insert in dpb Failed!\n");
        return HEVC_DEC_ERR;
    }

    if (ctx->poc_random_access[ctx->curr_slice.view_id] == HEVC_MAX_INT) {
        ctx->poc_random_access[ctx->curr_slice.view_id] = lost_pos;
    }

    return HEVC_DEC_NORMAL;
}

/*************************************************************************
*      函数原型 :  td_s32 hevc_i_mode_process()
*      功能描述 :  I 帧模式处理函数
*      参数说明 :  无
*      返回值   ： HEVC_DEC_NORMAL: 成功返回;
*                  other: 异常错误
*************************************************************************/
STATIC td_s32 hevc_i_mode_process(hevc_ctx *ctx)
{
    td_s32 ret;

    ret = hevc_direct_output(ctx);

    /* pmv槽位设置成缺省值 */
    if (ctx->curr_pic.pmv_address_idc != ctx->total_pmv_num) {
        ctx->pmv_store_used_flag[ctx->curr_pic.pmv_address_idc] = 0;
        ctx->curr_pic.pmv_address_idc = ctx->total_pmv_num;
    }

    return ret;
}

/*************************************************************************
*      函数原型 :  td_void hevc_set_frm_repeat_count(hevc_ctx *ctx, image *p_image)
*      功能描述 :  PVR要不要重复输出帧判断函数
*      参数说明 :  无
*      返回值   ：
*************************************************************************/
STATIC td_void hevc_set_frm_repeat_count(hevc_ctx *ctx, vfmw_image *p_image)
{
    vfmw_unused(p_image);
    if (!((ctx->extra_data_ptr->fast_forward_backward_speed > -2048) && /* 2048 is speed */
          (ctx->extra_data_ptr->fast_forward_backward_speed <= 2048))) { /* 2048 is speed */
        if (ctx->new_pic_counter >= (ctx->pic_out_counter + 1 + ctx->dpb.size)) {
            if ((ctx->new_pic_counter - (ctx->pic_out_counter + 1 + ctx->dpb.size)) >= 2) { /* 2 is frm cnt */
                ctx->pic_out_counter += 2; /* 2 is frm cnt */
            } else {
                ctx->pic_out_counter += 1;
            }
        }
    }

    return;
}

/*************************************************************************
*      函数原型 :  td_s32 hevc_output_curr_pic()
*      功能描述 :  直接输出当前帧，不经过排序处理
*      参数说明 :  无
*      返回值   ： HEVC_DEC_NORMAL: 成功返回;
*                  other: 异常错误
*************************************************************************/
STATIC td_s32 hevc_output_curr_pic(hevc_ctx *ctx)
{
    vfmw_image *img = TD_NULL;
    hevc_curr_pic *out_pic = TD_NULL;
    hevc_frame_store *out_frame_store = TD_NULL;

    out_pic = &ctx->curr_pic;
    out_frame_store = ctx->curr_pic.frame_store;

    if (out_frame_store->frame_store_state != FS_IN_DPB) {
        out_frame_store->frame_store_state = FS_DISPLAY_ONLY;
    }

    out_frame_store->err_level = out_pic->err_level;

    hevc_get_image_para(ctx, out_frame_store);

    img = fsp_get_image(ctx->chan_id, out_frame_store->logic_fs_id);
    if (img == TD_NULL) {
        dprint(PRN_ERROR, "%s: img is TD_NULL!\n", __func__);
        return HEVC_DEC_ERR;
    }

    img->error_level = out_frame_store->err_level;

    if (img->error_level > 0 && out_pic->pic_type == HEVC_I_FRAME) { /* 如果I帧有错误，上报一个事件通知应用层 */
        dprint(PRN_ERROR, "%s, REPORT_IFRAME_ERR\n", __func__);
    }

    if (img->error_level == 0 || img->error_level <= (td_u32)(ctx->extra_data_ptr->out_err_thr)) {
        if (ctx->curr_pic.is_ref_idc != 0) {
            fsp_set_ref(ctx->chan_id, out_frame_store->logic_fs_id, 1);
        }

        /* check when shvc support */
        if (hevc_shvc_frame_output(ctx, out_frame_store, img) == 1) {
            fsp_set_disp(ctx->chan_id, out_frame_store->logic_fs_id, 1);

            if (voq_insert(ctx->chan_id, img) != SYN_OK) {
                dprint(PRN_ERROR, "%s: InsertImgToVoQueue failed!\n", __func__);
                fsp_set_disp(ctx->chan_id, out_frame_store->logic_fs_id, 0);
                return HEVC_DEC_ERR;
            }
        }
        hevc_set_frm_repeat_count(ctx, img);
        ctx->output_frame_num++;
        ctx->pic_out_counter++;
    } else {
        fsp_set_disp(ctx->chan_id, out_frame_store->logic_fs_id, 0);
        dprint(PRN_ERROR, "%s: ImgErrorLevel = %d, OutErrThr:%d, discarded!\n",
               __func__, img->error_level, ctx->extra_data_ptr->out_err_thr);
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

/*************************************************************************
*      函数原型 :  td_s32 hevc_first_frame_fast_out()
*      功能描述 :  第一帧快速输出
*      参数说明 :  无
*      返回值   ： HEVC_DEC_NORMAL: 成功返回;
*                  other: 异常错误
*************************************************************************/
STATIC td_s32 hevc_first_frame_fast_out(hevc_ctx *ctx)
{
    td_s32 ret;

    /* 先直接输出 */
    ret = hevc_output_curr_pic(ctx);
    if (ret != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "%s output current picture failed\n", __func__);
    } else {
        /* 标记是快速输出帧，这种帧虽被输出，但在经历正常输出序后才能从DPB中删除，比较特殊 */
        vfmw_check_ret(ctx->curr_nal != TD_NULL, HEVC_DEC_ERR);
        ctx->curr_pic.frame_store->first_frame_state = FIRST_FRAME_ALREADY_OUT;
        ctx->first_frame_state = FIRST_FRAME_PROCESSED;
        ctx->last_display_poc[ctx->curr_nal->nal_layer_id] = ctx->curr_pic.thispoc;
        ctx->curr_pic.frame_store->is_displayed = 1;
    }

    return ret;
}

/*************************************************************************
*      函数原型 :  td_s32 hevc_simple_dpb_process()
*      功能描述 :  Simple DPB处理函数
*      参数说明 :  无
*      返回值   ： HEVC_DEC_NORMAL: 成功返回;
*                  other: 异常错误

  为可视对讲定制的简化DPB处理，目的是做到误码时不发I帧也能解码，旁路原DPB
  管理机制，仅用滑窗方式管理参考帧.原来处理没有区分是否参考全做slide dpb,
  实际上非参考帧不能参与滑窗，否则挤掉了真正的参考帧. 云游戏SVC-T就有这种情形.
  解决方法是加上是否被参考的判断。

*************************************************************************/
STATIC td_s32 hevc_simple_dpb_process(hevc_ctx *ctx)
{
    td_s32 ret;

    if (ctx->curr_pic.is_ref_idc != 0) {
        /* 把当前图像存入DPB */
        ret = hevc_insert_frm_in_dpb(ctx, ctx->dpb.used_size, &ctx->curr_pic);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "%s %d: InsertFrmInDPB failed, return %d\n", __func__, __LINE__, ret);
            return HEVC_APC_ERR;
        }
        ctx->curr_pic.is_in_dpb = 1;
    } else {
        ctx->curr_pic.frame_store->frame_store_state = FS_DISPLAY_ONLY;
        if (ctx->curr_pic.pic_output_flag != HEVC_TRUE) {
            fsp_set_disp(ctx->chan_id, ctx->curr_pic.frame_store->logic_fs_id, 0);
        }
    }

    /* 输出当前图像 */
    ctx->curr_pic.frame_store->is_displayed = 1;
    ret = hevc_check_frame_store(ctx, ctx->curr_pic.frame_store);
    if (ret == HEVC_DEC_ERR) {
        dprint(PRN_ERROR, "%s call hevc_check_frame_store failed!\n", __func__);
        return HEVC_DEC_ERR;
    } else if (ret == HEVC_ERR_FS_OUT_AGAIN) {
        return HEVC_DEC_NORMAL;
    } else {
        ret = hevc_output_curr_pic(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "%s output current picture failed\n", __func__);
            return HEVC_DEC_ERR;
        }
    }

    return ret;
}

/*************************************************************************
*      函数原型 :  td_s32 hevc_dec_order_process()
*      功能描述 :  解码序输出处理函数
*      参数说明 :  无
*      返回值   ： HEVC_DEC_NORMAL: 成功返回;
*                  other: 异常错误
*************************************************************************/
STATIC td_s32 hevc_dec_order_process(hevc_ctx *ctx)
{
    td_s32 ret;

    /* 清空已经输出且不做参考的图像，可以不必等到DPB满的时候清除 */
    if (ctx->dpb.used_size >= ctx->dpb.size) {
        ret = hevc_remove_unused_frame_store(ctx);
        if (ret != HEVC_TRUE) {
            dprint(PRN_ERROR, "%s call hevc_remove_unused_frame_store Failed.\n", __func__);
            if (hevc_discard_ref_from_dpb(ctx) != HEVC_DEC_NORMAL) {
                hevc_clear_dpb(ctx);
                dprint(PRN_ERROR, "%s dpb full and can not discard,clear all\n", __func__);
            }
        }
    }

    /* 把当前图像存入DPB */
    ret = hevc_insert_frm_in_dpb(ctx, ctx->dpb.used_size, &ctx->curr_pic);
    if (ret != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "%s call hevc_insert_frm_in_dpb Failed.\n", __func__);
        return HEVC_DEC_ERR;
    }
    ctx->curr_pic.is_in_dpb = 1;

    /* 输出当前图像 */
    ret = hevc_check_frame_store(ctx, ctx->curr_pic.frame_store);
    if (ret == HEVC_DEC_ERR) {
        dprint(PRN_ERROR, "%s call hevc_check_frame_store failed!\n", __func__);
        return HEVC_DEC_ERR;
    } else if (ret == HEVC_ERR_FS_OUT_AGAIN) {
        return HEVC_DEC_NORMAL;
    } else {
        ret = hevc_output_curr_pic(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "%s output current picture failed\n", __func__);
            return HEVC_DEC_ERR;
        }
    }

    return ret;
}

/*************************************************************************
*      函数原型 :  td_s32 hevc_simple_dpb_process()
*      功能描述 :  Simple DPB处理函数
*      参数说明 :  无
*      返回值   ： HEVC_DEC_NORMAL: 成功返回;
*                  other: 异常错误
*************************************************************************/
STATIC td_s32 hevc_disp_order_process(hevc_ctx *ctx)
{
    td_s32 ret;

    /* HEVC很多码流解码序第一帧不是显示序第一帧，使能后导致个别码流闪屏现象 */
    /* 第一帧快速输出处理: 回归采用的Allegro码流，解码序第一帧未必是显示序第一帧，所以不使能 */
    if (ctx->first_frame_state != FIRST_FRAME_PROCESSED &&
        ctx->curr_pic.pic_output_flag == HEVC_TRUE) {
        ret = hevc_first_frame_fast_out(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "%s call hevc_first_frame_fast_out Failed\n", __func__);
            return HEVC_DEC_ERR;
        }
    }

    /* 清空已经输出且不做参考的图像，可以不必等到DPB满的时候清除 */
    if (ctx->dpb.used_size >= ctx->dpb.size) {
        ret = hevc_remove_unused_frame_store(ctx);
        if (ret != HEVC_TRUE) {
            if (hevc_discard_ref_from_dpb(ctx) != HEVC_DEC_NORMAL) {
                hevc_clear_dpb(ctx);
                dprint(PRN_ERROR, "%s dpb full and can not discard,clear all\n", __func__);
            }
        }
    }

    /* 把当前图像存入DPB */
    ret = hevc_insert_frm_in_dpb(ctx, ctx->dpb.used_size, &ctx->curr_pic);
    if (ret != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "hevc_insert_frm_in_dpb Failed, ret = %d\n", ret);
        return HEVC_DEC_ERR;
    }
    ctx->curr_pic.is_in_dpb = 1;

    /* 输出DPB中该显示的图像 */
    if (ctx->dpb.used_size) {
        ret = hevc_output_frm_from_dpb(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "%s call hevc_output_frm_from_dpb failed\n", __func__);
            return HEVC_DEC_ERR;
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_dob_hdr_process(hevc_ctx *ctx)
{
    vfmw_unused(ctx);
    return;
}

STATIC td_s32 hevc_first_process(hevc_ctx *ctx)
{
    hevc_dob_hdr_process(ctx);

    hevc_remove_dummy_frame(ctx);

    /* 容错: 空图像直接丢弃 */
    if (ctx->curr_pic.state == HEVC_PIC_EMPTY) {
        dprint(PRN_ERROR, "%s curr_pic.state = HEVC_PIC_EMPTY, return HEVC_DEC_NORMAL\n", __func__);
        return HEVC_DEC_NORMAL;
    }

    /* 图像错误率门限控制 */
    if (ctx->curr_pic.err_level > (td_u32)(ctx->extra_data_ptr->out_err_thr)) {
        hevc_clear_curr_pic(ctx);
        dprint(PRN_ERROR, "Discard cur pic, ErrLevel(%d) > OutErrThr(%d)\n",
               ctx->curr_pic.err_level, ctx->extra_data_ptr->out_err_thr);
        return HEVC_DEC_NORMAL;
    }

    return HEVC_DEC_ERR;
}
/*************************************************************************
*      函数原型 :  td_s32 hevc_store_pic_in_dpb()
*      功能描述 :  把当前图像存入DPB
*      参数说明 :  无
*      返回值   ： HEVC_DEC_NORMAL: 配成功;
*                  DPB_ERR: 在存入DPB时发现DPB出现重大错误，需要firmware重启
*                  HEVC_OUT_FLAG: 检测到适配层需要退出，返回该标志
*************************************************************************/
STATIC td_s32 hevc_store_pic_in_dpb(hevc_ctx *ctx, td_bool is_slc)
{
    td_s32 ret = HEVC_DEC_ERR;

    if (ctx->curr_pic.frame_store == TD_NULL) {
        dprint(PRN_ERROR, "ctx->curr_pic.frame_store is null pointer\n");
        goto store_pic_exit;
    }

    if (is_slc == TD_FALSE) {
        /**************** 处理流程0: 图像输出前处理 ****************************/
        ret = hevc_first_process(ctx);
        vfmw_check_ret(ret != HEVC_DEC_NORMAL, ret);
    }

    ctx->curr_pic.frame_store->sei_field_flag = ctx->curr_pic.sei_field_flag;
    ctx->curr_pic.frame_store->sei_source_scan_type = ctx->curr_pic.sei_source_scan_type;

    /******************** 处理流程1: I帧模式下直接输出 *************************/
    if (ctx->old_dec_mode == VFMW_DEC_MODE_I) {
        ret = hevc_i_mode_process(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "%s call hevc_i_mode_process Failed\n", __func__);
        }
        goto store_pic_exit;
    }

    if(ctx->ldy_mode == LDY_MULTI_SLC && ctx->curr_pic.is_in_dpb == 1 && is_slc == TD_FALSE) {
        ret = HEVC_DEC_NORMAL;
        goto store_pic_exit;
    }

    /******************** 处理流程2: SIMPLE_DPB的特殊处理 **********************/
    if (ctx->extra_data_ptr->output_order == VFMW_DEC_ORDER_SIMPLE_DPB) {
        ret = hevc_simple_dpb_process(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "%s call hevc_simple_dpb_process Failed\n", __func__);
        }
        goto store_pic_exit;
    }

    /******************** 处理流程3: 解码序输出的特殊处理 **********************/
    if (ctx->extra_data_ptr->output_order == VFMW_DEC_ORDER) {
        ret = hevc_dec_order_process(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "%s call hevc_dec_order_process Failed\n", __func__);
        }
        /* 正常返回，继续往下走 */
    }

    /******************** 处理流程4: 显示序标准处理流程 ************************/
    if (ctx->extra_data_ptr->output_order == VFMW_DISP_ORDER) {
        ret = hevc_disp_order_process(ctx);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "%s call hevc_disp_order_process Failed\n", __func__);
        }
        /* 正常返回，继续往下走 */
    }

store_pic_exit:

    if (ret == HEVC_DEC_ERR) {
        /* 既然Store失败，该帧的帧存应该释放掉，免得引起虚拟帧存泄漏 */
        hevc_clear_curr_pic(ctx);
        dprint(PRN_ERROR, "%s return HEVC_DEC_ERR\n", __func__);
    }

    /* 当前图像置空 */
    ctx->curr_pic.state = is_slc == TD_FALSE ? HEVC_PIC_EMPTY : ctx->curr_pic.state;

    return ret;
}

STATIC td_void hevc_update_negative_ref(hevc_ctx *ctx, const hevc_short_term_rpset *rps, td_u32 *index)
{
    td_u32 i, j, k;
    td_s32 poc;

    k = 0;

    for (i = 0; i < rps->num_negative_pics; i++) {
        if (!rps->used_flag[i]) {
            continue;
        }

        poc = ctx->curr_slice.poc + rps->delta_poc[i];

        for (j = 0; j < ctx->dpb.used_size; j++) {
            if ((poc != ctx->dpb.fs[j]->poc) || (ctx->curr_slice.view_id != (td_u32)(ctx->dpb.fs[j]->view_id))) {
                continue;
            }

            ctx->dpb.fs[j]->is_reference = 1;

            /* 逻辑帧标记为待参考 */
            if (ctx->dpb.fs[j]->is_reference != 0) {
                fsp_set_ref(ctx->chan_id, ctx->dpb.fs[j]->logic_fs_id, 1);
            }

            ctx->dpb.fs[j]->frame.is_short_term = 1;
            ctx->dpb.fs[j]->frame.is_long_term = 0;
            ctx->dpb.fs_negative_ref[k++] = ctx->dpb.fs[j];
            break;
        }
    }

    ctx->dpb.negative_ref_frames_in_buffer = k;

    while (k < ctx->dpb.size) {
        ctx->dpb.fs_negative_ref[k++] = (hevc_frame_store *)TD_NULL;
    }

    *index = i;

    return;
}

/*************************************************************************
*      函数原型 :  td_void hevc_update_ref_list()
*      功能描述 :  更新DPB中短期参考列表
*      参数说明 :  无
*      返回值   ： 无
*************************************************************************/
STATIC td_void hevc_update_ref_list(hevc_ctx *ctx)
{
    td_u32 i, j, k, val;
    td_s32 poc;
    hevc_short_term_rpset *rps = TD_NULL;

    rps = &(ctx->curr_slice.short_term_ref_pic_set);

    hevc_update_negative_ref(ctx, rps, &i);

    k = 0;
    val = rps->num_negative_pics + rps->num_positive_pics;

    for (; i < val; i++) {
        if (!rps->used_flag[i]) {
            continue;
        }

        poc = ctx->curr_slice.poc + rps->delta_poc[i];

        for (j = 0; j < ctx->dpb.used_size; j++) {
            if ((poc != ctx->dpb.fs[j]->poc) || (ctx->curr_slice.view_id != (td_u32)(ctx->dpb.fs[j]->view_id))) {
                continue;
            }

            ctx->dpb.fs[j]->is_reference = 1;

            /* 逻辑帧标记为待参考 */
            if (ctx->dpb.fs[j]->is_reference != 0) {
                fsp_set_ref(ctx->chan_id, ctx->dpb.fs[j]->logic_fs_id, 1);
            }

            ctx->dpb.fs[j]->frame.is_short_term = 1;
            ctx->dpb.fs[j]->frame.is_long_term = 0;
            ctx->dpb.fs_positive_ref[k++] = ctx->dpb.fs[j];
            break;
        }
    }

    ctx->dpb.positive_ref_frames_in_buffer = k;

    while (k < ctx->dpb.size) {
        ctx->dpb.fs_positive_ref[k++] = (hevc_frame_store *)TD_NULL;
    }

    return;
}

/*************************************************************************
*      函数原型 :  td_void hevc_update_lt_ref_list()
*      功能描述 :  更新DPB中长期参考列表
*      参数说明 :  无
*      返回值   ： 无
*************************************************************************/
STATIC td_void hevc_update_lt_ref_list(hevc_ctx *ctx)
{
    td_s32 i, j, k;
    td_s32 rps_poc;
    td_s32 cur_poc;
    td_u8 delta_poc_msb_cycle_lt_flag;
    td_u32 poc_cycle;
    hevc_short_term_rpset *rps = TD_NULL;
    hevc_pic_param_set *pps = TD_NULL;
    hevc_seq_param_set *sps = TD_NULL;

    k = 0;
    pps = ctx->pps[ctx->curr_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];
    poc_cycle = 1 << sps->max_pic_order_cnt_lsb;
    rps = &(ctx->curr_slice.short_term_ref_pic_set);

    for (i = (td_s32)rps->num_of_pics - 1;
         i > (td_s32)(rps->num_negative_pics + rps->num_positive_pics - 1); i--) {
        if (rps->used_flag[i] == 0) {
            continue;
        }

        rps_poc = rps->poc[i];
        delta_poc_msb_cycle_lt_flag = (td_u8)ctx->curr_slice.check_lt_msb[i];
        rps_poc = delta_poc_msb_cycle_lt_flag ? rps_poc : (td_s32)((td_u32)rps_poc & (poc_cycle - 1));

        for (j = 0; j < (td_s32)ctx->dpb.used_size; j++) {
            cur_poc = ctx->dpb.fs[j]->poc;
            cur_poc = delta_poc_msb_cycle_lt_flag ? cur_poc : (td_s32)((td_u32)cur_poc & (poc_cycle - 1));
            if ((cur_poc == rps_poc) && ((td_u32)(ctx->dpb.fs[j]->view_id) == ctx->curr_slice.view_id)) {
                ctx->dpb.fs[j]->is_reference = 1;

                ctx->dpb.fs[j]->frame.is_short_term = 0;
                ctx->dpb.fs[j]->frame.is_long_term = 1;
                ctx->dpb.fs_ltref[k++] = ctx->dpb.fs[j];
                break;
            }
        }
    }

    ctx->dpb.ltref_frames_in_buffer = (td_u32)k;

    while (k < (td_s32)ctx->dpb.size) {
        ctx->dpb.fs_ltref[k++] = (hevc_frame_store *)TD_NULL;
    }

    return;
}

STATIC td_void hevc_remove_same_poc_out_dpb(hevc_ctx *ctx, td_u32 *pos, const hevc_curr_pic *curr_pic)
{
    td_u32 i;

    for (i = 0; i < ctx->dpb.used_size; i++) {
        if (ctx->dpb.fs[i] == TD_NULL) {
            dprint(PRN_ERROR, "pos: %d, ctx->dpb.fs[i]: is null\n", i);
            continue;
        }

        if (ctx->dpb.fs[i]->poc == curr_pic->thispoc) {
            dprint(PRN_ERROR, "%s have same poc(%d) in dpb, Remove the old\n", __func__, curr_pic->thispoc);
            hevc_remove_frame_store_out_dpb(ctx, i);
            *pos = ctx->dpb.used_size;
            break;
        }
    }

    return;
}

/*************************************************************************
*   函数原型 :  td_s32 hevc_insert_frm_in_dpb(hevc_ctx *ctx,td_u32 pos, hevc_curr_pic *curr_pic)
*   功能描述 :  插入一幅图像到pHevcCtx->dpb
*   参数说明 :
*   返回值   ：
*************************************************************************/
STATIC td_s32 hevc_insert_frm_in_dpb(hevc_ctx *ctx, td_u32 pos, hevc_curr_pic *curr_pic)
{
    if (pos >= HEVC_MAX_DPB_NUM || curr_pic->frame_store == TD_NULL) {
        dprint(PRN_ERROR, "%s with invalid pos(%d)\n", __func__, pos);
        hevc_clear_dpb(ctx);
        return HEVC_DEC_ERR;
    }

    if (ctx->is_multi_layers == 0) {
        hevc_remove_same_poc_out_dpb(ctx, &pos, curr_pic);
    }

    ctx->dpb.fs[pos] = curr_pic->frame_store;

    ctx->dpb.fs[pos]->frame_store_state = FS_IN_DPB;

    /* 逻辑帧标记为待参考 */
    fsp_set_ref(ctx->chan_id, ctx->dpb.fs[pos]->logic_fs_id, 1);
    ctx->dpb.fs[pos]->is_reference = (curr_pic->is_ref_idc == 0) ? 0 : 1;

    ctx->dpb.fs[pos]->pmv_address_idc = curr_pic->pmv_address_idc;
    ctx->dpb.fs[pos]->err_level = ctx->dpb.fs[pos]->frame.err_level = curr_pic->err_level;

    ctx->dpb.fs[pos]->frame.frame_store = curr_pic->frame_store;
    ctx->dpb.fs[pos]->pic_type = ctx->dpb.fs[pos]->frame.pic_type = curr_pic->pic_type;

    ctx->dpb.fs[pos]->poc = curr_pic->thispoc;
    ctx->dpb.fs[pos]->frame.poc = curr_pic->thispoc;
#ifdef VFMW_MV_HEVC_SUPPORT
    ctx->dpb.fs[pos]->frame.is_inter_layer = 0;
    ctx->dpb.fs[pos]->view_id = (td_s32)curr_pic->view_id;
    ctx->dpb.fs[pos]->fs_img = curr_pic->fs_image;
#endif
    /* DPB按照poc递增排序 */
    syn_qsort((td_void *)ctx->dpb.fs, (pos + 1), sizeof(hevc_frame_store *), hevc_compare_pic_by_poc_asc);

    ctx->dpb.used_size++;

    if (curr_pic->pic_output_flag != HEVC_TRUE && curr_pic->create_lost_pic != HEVC_TRUE) {
        curr_pic->frame_store->is_displayed = 1;
        fsp_set_disp(ctx->chan_id, curr_pic->frame_store->logic_fs_id, 0);
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_get_no_display_num(const hevc_ctx *ctx, td_u32 *num)
{
    td_u32 i;
    td_u32 view_id;
    td_u32 no_displayed;

    no_displayed = 0;
    view_id = ctx->curr_pic.view_id;
    for (i = 0; i < ctx->dpb.used_size; i++) {
        if (ctx->dpb.fs[i] == TD_NULL) {
            dprint(PRN_ERROR, "%s chn %d fs[%d] null\n", __func__, ctx->chan_id, i);
            return HEVC_DEC_ERR;
        }

        if ((ctx->dpb.fs[i]->is_displayed == 0) && ((td_u32)(ctx->dpb.fs[i]->view_id) == view_id)) {
            if (ctx->dpb.fs[i]->poc >= ctx->last_display_poc[view_id]) {
                no_displayed++;
            } else {
                dprint(PRN_ERROR, "%s: BlockFsPoc:%d, LastPoc:%d!\n",
                       __func__, ctx->dpb.fs[i]->poc, ctx->last_display_poc[view_id]);
                fsp_set_disp(ctx->chan_id, ctx->dpb.fs[i]->logic_fs_id, 0);
            }
        }
    }

    *num = no_displayed;

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_output_frame_from_dpb_buffer(hevc_ctx *ctx, td_u32 i)
{
    td_s32 ret;
    td_u32 view_id;

    view_id = ctx->curr_pic.view_id;
    ctx->dpb.fs[i]->is_displayed = 1;
    ret = hevc_output_frm_to_vo(ctx, ctx->dpb.fs[i]);
    if (ret == HEVC_DEC_NORMAL) {
        ctx->last_display_poc[view_id] = ctx->dpb.fs[i]->poc;
    }

    return ret;
}

STATIC td_void hevc_output_frm_disp_clear(const hevc_ctx *ctx, td_u32 i)
{
    if (ctx->dpb.fs[i]->is_displayed == 0) {
        fsp_set_disp(ctx->chan_id, ctx->dpb.fs[i]->logic_fs_id, 0);
    }
}

STATIC td_s32 hevc_output_frame_to_vo(hevc_ctx *ctx, td_u32 i)
{
    td_s32 ret;
    hevc_slice_segment_header *next_frm_slc = &ctx->curr_slice;
    td_u32 view_id;

    view_id = ctx->curr_pic.view_id;
    ret = hevc_output_frm_to_vo(ctx, ctx->dpb.fs[i]);
    if (next_frm_slc->poc < ctx->last_display_poc[view_id]) {
        ctx->last_display_poc[view_id] = next_frm_slc->poc - 1;
    }

    return ret;
}

/*************************************************************************
*   函数原型 :  td_s32 hevc_output_frm_from_dpb()
*   功能描述 :  从pHevcCtx->DPB中输出一帧
*   参数说明 :
*   返回值   ：
*************************************************************************/
STATIC td_s32 hevc_output_frm_from_dpb(hevc_ctx *ctx)
{
    td_s32 ret;
    td_u32 i;
    td_u32 view_id, output_flag;
    td_u32 no_displayed;
    td_u32 max_reorder_pics;
    /* now curr_slice is next frame first slice */
    hevc_slice_segment_header *next_frm_slc = &ctx->curr_slice;
    view_id = ctx->curr_pic.view_id;
    ret = hevc_get_no_display_num(ctx, &no_displayed);
    vfmw_check_ret_no_print(ret == HEVC_DEC_NORMAL, ret);

    max_reorder_pics = (td_u32)hevc_get_max_reorder(ctx);

    /* num_reorder_pics 某一层,最多有多少个reorder pic */
    /* no_displayed > ctx->CurrSPS.num_reorder_pics[ctx->curr_slice.nuh_temporal_id] */
    /* 保证后续帧不可能比送显帧的poc小,因为我们知道最多有多少个前序的帧了, dpb中有多余 */
    /* 这个值的帧送显,则多余的帧必定可以输出 */
    for (i = 0; i < ctx->dpb.used_size; i++) {
        output_flag = (td_u32)(ctx->dpb.fs[i]->is_displayed == 0) &
            (td_u32)(ctx->dpb.fs[i]->poc >= ctx->last_display_poc[view_id]) &
            (td_u32)(next_frm_slc->nuh_temporal_id < HEVC_MAX_TEMPLAYER) &
            (td_u32)(no_displayed > max_reorder_pics) &
            (td_u32)(ctx->dpb.fs[i]->poc < next_frm_slc->poc || ctx->no_output_of_prior_pics_flag[view_id]) &
            (td_u32)((td_u32)(ctx->dpb.fs[i]->view_id) == view_id);
        if (output_flag == 1) {
            no_displayed--;
            ret = hevc_output_frame_from_dpb_buffer(ctx, i);
        } else if ((ctx->last_display_poc[view_id] == 0 || ctx->need_drop_b_frame == 1) &&
                   (ctx->dpb.fs[i]->poc <= ctx->last_display_poc[view_id])) {
            hevc_output_frm_disp_clear(ctx, i);
        } else if ((ctx->dpb.fs[i]->is_displayed == 0) &&
                   (ctx->dpb.fs[i]->poc > next_frm_slc->poc + HEVC_GOP_LEN) &&
                   (ctx->poc_random_access[view_id] != -HEVC_MAX_INT) &&
                   (ctx->need_drop_b_frame == 1)) {
            no_displayed--;
            ctx->dpb.fs[i]->is_displayed = 1;
            ret = hevc_output_frame_to_vo(ctx, i);
        }
    }

    return ret;
}

STATIC td_s32 hevc_output_one_frm_from_dpb(hevc_ctx *ctx, td_u32 pos)
{
    td_s32 ret;
    td_u32 view_id;

    ctx->dpb.fs[pos]->is_displayed = 1;
    view_id = (td_u32)ctx->dpb.fs[pos]->view_id;
    ret = hevc_output_frm_to_vo(ctx, ctx->dpb.fs[pos]);
    if (ret != HEVC_DEC_NORMAL) {
        return ret;
    }
    ctx->last_display_poc[view_id]  = ctx->dpb.fs[pos]->poc;

    /* free frame store and move empty store to end of buffer */
    if (!hevc_frame_store_ref_flg(ctx->dpb.fs[pos])) {
        hevc_remove_frame_store_out_dpb(ctx, pos);
    }

    return ret;
}

/*************************************************************************
*   函数原型 :  td_s32 hevc_flush_output_frm_from_dpb()
*   功能描述 :  从pHevcCtx->DPB中输出所有帧
*   参数说明 :
*   返回值   ：
*************************************************************************/
VFMW_ACC_SLICE_H265 STATIC td_s32 hevc_flush_output_frm_from_dpb(hevc_ctx *ctx)
{
    td_u32 i;

    for (i = 0; i < ctx->dpb.used_size; i++) {
        fsp_set_ref(ctx->chan_id, ctx->dpb.fs[i]->logic_fs_id, 0);
        if (ctx->dpb.fs[i]->is_displayed == 0) {
            ctx->dpb.fs[i]->is_displayed = 1;
            if (hevc_output_frm_to_vo(ctx, ctx->dpb.fs[i]) == HEVC_DEC_ERR) {
                dprint(PRN_ERROR, "%s: hevc_output_frm_to_vo err!\n", __func__);
                return HEVC_DEC_ERR;
            }
        } else {
            if (fsp_get_disp(ctx->chan_id, ctx->dpb.fs[i]->logic_fs_id) != FSP_DISP_STATE_WAIT) {
                fsp_set_disp(ctx->chan_id, ctx->dpb.fs[i]->logic_fs_id, 0);
            }
        }
    }

    for (i = 0; i < HEVC_MAX_NUM_LAYERS; i++) {
        ctx->last_display_poc[i] = -HEVC_MAX_INT;
    }
    hevc_init_dec_buffers(ctx);

    return HEVC_DEC_NORMAL;
}

/*************************************************************************
*   函数原型 :  td_s32 hevc_empty_frm_from_dpb()
*   功能描述 :  从pHevcCtx->DPB中清空所有帧，不管有没有显示帧
*   参数说明 :
*   返回值   ：
*************************************************************************/
STATIC td_s32 hevc_empty_frm_from_dpb(hevc_ctx *ctx)
{
    td_u32 i;

    for (i = 0; i < ctx->dpb.used_size; i++) {
        if (ctx->dpb.fs[i]->is_displayed == 0) {
            ctx->dpb.fs[i]->is_displayed = 1;
        }
    }

    for (i = 0; i < HEVC_MAX_NUM_LAYERS; i++) {
        ctx->last_display_poc[i] = -HEVC_MAX_INT;
    }

    /* dpb refresh 清除逻辑帧存 */
    for (i = 0; i < HEVC_MAX_DPB_NUM; i++) {
        if (ctx->dpb.fs[i] != TD_NULL) {
            fsp_clear_logic(ctx->chan_id, ctx->dpb.fs[i]->logic_fs_id, 1);
        }

        if (ctx->dpb.fs_negative_ref[i] != TD_NULL) {
            fsp_clear_logic(ctx->chan_id, ctx->dpb.fs_negative_ref[i]->logic_fs_id, 1);
        }

        if (ctx->dpb.fs_positive_ref[i] != TD_NULL) {
            fsp_clear_logic(ctx->chan_id, ctx->dpb.fs_positive_ref[i]->logic_fs_id, 1);
        }

        if (ctx->dpb.fs_ltref[i] != TD_NULL) {
            fsp_clear_logic(ctx->chan_id, ctx->dpb.fs_ltref[i]->logic_fs_id, 1);
        }
    }

    hevc_init_dec_buffers(ctx);

    return HEVC_DEC_NORMAL;
}

/*************************************************************************
*   函数原型 :  td_s32 hevc_output_frm_to_vo(hevc_ctx *ctx,hevc_frame_store *frame_store)
*   功能描述 :  输出一帧图像
*   参数说明 :
*   返回值   ：
*************************************************************************/
STATIC td_s32 hevc_output_frm_to_vo(hevc_ctx *ctx, hevc_frame_store *frame_store)
{
    td_s32 ret;
    vfmw_image *logic_fs_image = TD_NULL;

    ret = hevc_check_frame_store(ctx, frame_store);
    if (ret == HEVC_DEC_NORMAL) {
        hevc_get_image_para(ctx, frame_store);

        fsp_set_disp(ctx->chan_id, frame_store->logic_fs_id, 1);
        logic_fs_image = fsp_get_image(ctx->chan_id, frame_store->logic_fs_id);
        if (logic_fs_image == TD_NULL) {
            dprint(PRN_ERROR, "%s: logic_fs_image is TD_NULL!\n", __func__);
            return HEVC_DEC_ERR;
        }

        if (hevc_shvc_frame_output(ctx, frame_store, logic_fs_image) == 1) {
            ret = voq_insert(ctx->chan_id, logic_fs_image);
            if (ret != SYN_OK) {
                /* 插入队列超时且需要退出 */
                dprint(PRN_ERROR, "HEVC InsertVO Failed return %d, ClearAll\n", ret);
                fsp_set_disp(ctx->chan_id, frame_store->logic_fs_id, 0);
                hevc_clear_curr_nal(ctx);
                hevc_clear_all(ctx, 1);
                return HEVC_DEC_ERR;
            }
        }

        hevc_set_frm_repeat_count(ctx, logic_fs_image);
        ctx->output_frame_num++;
        ctx->pic_out_counter++;
    } else if (ret == HEVC_ERR_FS_OUT_AGAIN) {
        /* pFrameStore可能已经输出，若如此这里必须及时中止输出帧的操作.
           已输出的帧又走到这里的原因是，某些帧虽已输出但仍作参考，
           所以还在DPB里，有可能再次被挑中尝试输出. */
        return HEVC_DEC_NORMAL;
    } else {
        dprint(PRN_WARN, "HEVC.c line %d: frame para err(ret=%d), recycle image self\n", __LINE__, ret);
        hevc_dec_recycle_image(ctx, frame_store->logic_fs_id);
        return HEVC_DEC_ERR; /* OUT_FLAG */
    }

    return HEVC_DEC_NORMAL;
}

/*************************************************************************
*   函数原型 :  td_s32 Check_FrameStore(hevc_ctx *ctx, hevc_frame_store * frame_store)
*   功能描述 :  检查该图像是否能够输出到VO对列
*   参数说明 :
*   返回值   ： 0:可以输出;-1不能输出
*************************************************************************/
STATIC td_s32 hevc_check_frame_store(const hevc_ctx *ctx, hevc_frame_store *frame_store)
{
    td_s8 already_output_flag;
    fsp_logic *logic_fs = TD_NULL;

    already_output_flag = 0;
    logic_fs = fsp_get_logic(ctx->chan_id, frame_store->logic_fs_id);
    if (logic_fs != TD_NULL) {
        if ((logic_fs->disp_state == FSP_DISP_STATE_WAIT)
            ||  (logic_fs->disp_state == FSP_DISP_STATE_IDLE)) {
            dprint(PRN_REF, "%s frame logic_fs_id=%d, state=%d\n",
                   __func__, frame_store->logic_fs_id, logic_fs->disp_state);
            already_output_flag = 1;
        }
    } else {
        dprint(PRN_ERROR, "LogicFs(poc=%d, first_frame_state=%d) is null, logic_fs_id = %d\n",
               frame_store->poc, frame_store->first_frame_state, frame_store->logic_fs_id);
        already_output_flag = 1;
    }

    /* 如果该帧是快速输出帧则标记它已经经历了正常输出序，后续不再强求它依然呆在DPB里. */
    if (frame_store->first_frame_state == FIRST_FRAME_WAIT_OUT) {
        frame_store->is_displayed = 1;
        frame_store->first_frame_state = FIRST_FRAME_ALREADY_OUT;
        return HEVC_ERR_FS_OUT_AGAIN;
    }

    if (already_output_flag == 1) {
        return HEVC_ERR_FS_OUT_AGAIN;
    }

    /* the control condition would added */
    if ((frame_store->frame_store_state == FS_NOT_USED) || (frame_store->non_existing == 1)) {
        dprint(PRN_ERROR, "%s invalid, state:%d, non_existing:%d\n",
               __func__, frame_store->frame_store_state, frame_store->non_existing);
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_exchange_pts(hevc_ctx *ctx, vfmw_image *image)
{
    td_s64 min_pts;
    td_u32 i;
    td_s32 min_pts_poc;
    fsp_logic *logic_fs = TD_NULL;

    min_pts_poc = -1;
    min_pts = VFMW_MAX_PTS;
    if (image->pts == VFMW_INVALID_PTS) {
        return;
    }

    for (i = 0; i < ctx->dpb.used_size; i++) {
        if (ctx->dpb.fs[i] != TD_NULL) {
            logic_fs = fsp_get_logic(ctx->chan_id, ctx->dpb.fs[i]->logic_fs_id);
            if ((logic_fs  == TD_NULL) ||
                (logic_fs->image.pts == VFMW_INVALID_PTS) ||
                (logic_fs->disp_state == FSP_DISP_STATE_WAIT) ||
                (logic_fs->disp_state == FSP_DISP_STATE_IDLE)) {
                continue;
            }

            if (logic_fs->image.pts < min_pts) {
                min_pts_poc = (td_s32)i;
                min_pts = logic_fs->image.pts;
            }
        }
    }

    if (min_pts_poc != -1) {
        if (image->pts > min_pts) {
            if (syn_is_pts_rewind(ctx->chan_id, image->pts, min_pts)) {
                return;
            }

            dprint(PRN_PTS, "Exchange pts: %lld <-> %lld\n", image->pts, min_pts);
            logic_fs = fsp_get_logic(ctx->chan_id, ctx->dpb.fs[min_pts_poc]->logic_fs_id);
            vfmw_check(logic_fs != TD_NULL);
            logic_fs->image.pts = image->pts;
            image->pts = min_pts;
        }
    }

    return;
}

STATIC td_void hevc_set_image_format(hevc_ctx *ctx, td_u32 fld_flag, td_u32 scan_type, td_u32 pic_type)
{
    td_ulong lock_flag;

    fsp_img_format_lock(&lock_flag);
    ctx->curr_pic.fs_image.format.top_field_first = 0;
    ctx->curr_pic.fs_image.format.field_valid_flag = 0;
    ctx->curr_pic.fs_image.format.source_format = 0;
    ctx->curr_pic.fs_image.format.frame_type = 0;
    if (judge_framerate_valid(ctx->frame_rate) == SYN_OK) {
        ctx->curr_pic.fs_image.format.frame_rate_valid = 1;
    } else {
        ctx->curr_pic.fs_image.format.frame_rate_valid = 0;
    }

    ctx->curr_pic.fs_image.format.top_field_first = 2; /* 2 is unk */
    ctx->curr_pic.fs_image.format.field_valid_flag = fld_flag;
    ctx->curr_pic.fs_image.format.source_format = scan_type;
    ctx->curr_pic.fs_image.format.frame_type = pic_type;
    fsp_img_format_unlock(&lock_flag);

    return;
}

STATIC td_u32 hevc_get_source_scan_type(hevc_ctx *ctx)
{
    td_u32 source_scan_type = 1;

    if (ctx->curr_vps->profile_tier_level.general_progressive_source_flag == 1 &&
        ctx->curr_vps->profile_tier_level.general_interlaced_source_flag == 0) {
        source_scan_type = 0;
    } else if (ctx->curr_vps->profile_tier_level.general_progressive_source_flag == 0 &&
               ctx->curr_vps->profile_tier_level.general_interlaced_source_flag == 1) {
        source_scan_type = 1;
    } else if (ctx->curr_vps->profile_tier_level.general_progressive_source_flag == 1 &&
               ctx->curr_vps->profile_tier_level.general_interlaced_source_flag == 1 &&
               ctx->sei.pic_timming.source_scan_type == 0) {
        source_scan_type = 1;
    } else if (ctx->curr_vps->profile_tier_level.general_progressive_source_flag == 1 &&
               ctx->curr_vps->profile_tier_level.general_interlaced_source_flag == 1 &&
               ctx->sei.pic_timming.source_scan_type == 1) {
        source_scan_type = 0;
    } else if (ctx->curr_vps->profile_tier_level.general_progressive_source_flag == 0 &&
               ctx->curr_vps->profile_tier_level.general_interlaced_source_flag == 0) {
        source_scan_type = 0;
    }

    return source_scan_type;
}

STATIC td_void hevc_get_image_para(hevc_ctx *ctx, const hevc_frame_store *frame_store)
{
    td_u32 pic_type = frame_store->pic_type;
    td_u32 source_scan_type;
    td_u32 field_flag = 3;
    vfmw_image *logic_fs_image = TD_NULL;

    logic_fs_image = fsp_get_image(ctx->chan_id, frame_store->logic_fs_id);
    if (logic_fs_image == TD_NULL) {
        dprint(PRN_ERROR, "%s: logic_fs_image is TD_NULL!\n", __func__);
        return;
    }

    hevc_exchange_pts(ctx, logic_fs_image);

    ctx->curr_pic.fs_image.frame_rate = ctx->frame_rate;
    source_scan_type = hevc_get_source_scan_type(ctx);

    if (ctx->curr_sps->vui_parameters.frame_field_info_present_flag) {
        if ((frame_store->sei_field_flag == 1) || (frame_store->sei_field_flag == 11)) { /* 11 top fld */
            field_flag = 1;
        } else if ((frame_store->sei_field_flag == 2) || (frame_store->sei_field_flag == 10)) { /* 2 10 btm fld */
            field_flag = 2; /* 2 is btm fld */
        } else if (frame_store->sei_field_flag == 0) {
            field_flag = 3; /* 3 is two fld */
        }

        if (frame_store->sei_source_scan_type == 1) {
            source_scan_type = 0;
        } else if (frame_store->sei_source_scan_type == 0) {
            source_scan_type = 1;
        } else {
            source_scan_type = 0;
        }
    }

    hevc_set_image_format(ctx, field_flag, source_scan_type, pic_type);

    if (frame_store->frame_store_state != FS_NOT_USED) {
        ctx->curr_pic.fs_image.error_level = frame_store->err_level;
    }

    fsp_update_image_ext(ctx->chan_id, frame_store->logic_fs_id, &ctx->curr_pic.fs_image);

    return;
}

/*************************************************************************
*   函数原型 :  td_void hevc_dec_recycle_image(td_u32 pic_id)
*   功能描述 :  hevc回收VO已经显示的图像槽位
*   参数说明 :
*   返回值   ：
*************************************************************************/
STATIC td_s32 hevc_dec_recycle_image(const hevc_ctx *ctx, td_u32 logic_id)
{
    fsp_logic *logic_fs = TD_NULL;

    /* 找到PicID所对应的逻辑帧存，从而找到image结构体，这样才能释放image所附带的userdata */
    logic_fs = fsp_get_logic(ctx->chan_id, logic_id);
    if (logic_fs != TD_NULL) {
        /* 检查这个逻辑帧存是否正在等待显示释放 */
        if (logic_fs->disp_state == FSP_DISP_STATE_IDLE) {
            dprint(PRN_ERROR, "hevc_dec_recycle_image ERROR, DispState = FS_DISP_STATE_NOT_USED\n");
            return HEVC_DEC_ERR;
        }

        /* 将图像'pic_id'标记为"显示结束" */
        fsp_set_disp(ctx->chan_id, logic_id, 0);
        return HEVC_DEC_NORMAL;
    }

    return HEVC_DEC_ERR;
}

STATIC td_bool hevc_check_width_and_height_valid(td_u32 chan_id, td_u32 check_width, td_u32 check_height)
{
    if (check_width < HEVC_MIN_PIX_WIDTH || check_width > HEVC_MAX_PIX_WIDTH) {
        dprint(PRN_ERROR, "report unsupport hevc image width:%d.\n", check_width);
        report_unsupport_spec(chan_id, VFMW_SPEC_RESOLUTION, check_width);

        return TD_FALSE;
    }

    if (check_height < HEVC_MIN_PIX_HEIGHT || check_height > HEVC_MAX_PIX_HEIGHT) {
        dprint(PRN_ERROR, "report unsupport hevc image height:%d.\n", check_height);
        report_unsupport_spec(chan_id, VFMW_SPEC_RESOLUTION, check_height);

        return TD_FALSE;
    }

    return TD_TRUE;
}

STATIC td_s32 hevc_arrange_vahb_mem(hevc_ctx *ctx, td_u32 img_width, td_u32 img_height,
    td_u32 img_width_in_disp, td_u32 img_height_in_disp, td_u32 img_num, td_u32 pmv_blk_num, td_u32 bit_depth)
{
    td_u32 dec_num, pmv_num;
    fsp_cfg fsp_inst_cfg;

    vfmw_unused(img_num);
    vfmw_unused(pmv_blk_num);
    if (hevc_check_width_and_height_valid(ctx->chan_id, img_width, img_height) == TD_FALSE) {
        dprint(PRN_ERROR, "image size abnormal(%dx%d)\n", img_width, img_height);
        return SYN_ERR;
    }
    /* if width < 3840 and dpb size < 15, add extra fs num, in order to be compatible for error stream */
    if (img_width < 3840 && ctx->dpb.size < 15) {
        ctx->dpb.size += 0x2;
    }

    dec_num = ctx->dpb.size;
    pmv_num = ctx->dpb.size;

    vfmw_check_sec_func(memset_s(&fsp_inst_cfg, sizeof(fsp_cfg), 0, sizeof(fsp_cfg)));

    fsp_inst_cfg.vid_std = VFMW_HEVC;
    fsp_inst_cfg.bit_depth = bit_depth;
    fsp_inst_cfg.dec_width = ctx->extra_data_ptr->is_adaptive == 1 ? ctx->extra_data_ptr->max_width : img_width;
    fsp_inst_cfg.dec_height = ctx->extra_data_ptr->is_adaptive == 1 ? ctx->extra_data_ptr->max_height : img_height;
    fsp_inst_cfg.disp_width =
        ctx->extra_data_ptr->is_adaptive == 1 ? ctx->extra_data_ptr->max_width : img_width_in_disp;
    fsp_inst_cfg.disp_height =
        ctx->extra_data_ptr->is_adaptive == 1 ? ctx->extra_data_ptr->max_height : img_height_in_disp;
    fsp_inst_cfg.disp_wofs = ctx->curr_sps->conf_win_left_offset + ctx->curr_sps->conf_win_right_offset;
    fsp_inst_cfg.disp_hofs = ctx->curr_sps->conf_win_top_offset + ctx->curr_sps->conf_win_bottom_offset;
    fsp_inst_cfg.need_dec_num = (ctx->extra_data_ptr->dec_mode == VFMW_DEC_MODE_FIRST_I ||
        ctx->extra_data_ptr->dec_mode == VFMW_DEC_MODE_I) ? 1 : dec_num;
    fsp_inst_cfg.need_disp_num = ctx->curr_sps->reorder_num;
    fsp_inst_cfg.need_pmv_num = (ctx->extra_data_ptr->dec_mode == VFMW_DEC_MODE_FIRST_I ||
        ctx->extra_data_ptr->dec_mode == VFMW_DEC_MODE_I) ? 0 : pmv_num;
    fsp_inst_cfg.frame_mbs_only = 1;
    fsp_inst_cfg.splice_info.ori_width = ctx->splice_info.ori_width;
    fsp_inst_cfg.splice_info.ori_height = ctx->splice_info.ori_height;
    fsp_inst_cfg.splice_info.type = ctx->splice_info.type;

    /* 配置FSP实例，并分割帧存 */
    if (fsp_config(ctx->chan_id, &fsp_inst_cfg) != FSP_OK) {
        return SYN_PARSE_RESOURCE_INVALID;
    }

    return SYN_OK;
}

#ifdef HEVC_CUVA_SUPPORT
STATIC td_void hevc_get_cuva_hdr_info(const hevc_ctx *ctx, const hevc_sei_param_set *sei)
{
    dps_info *dps;
    td_u8 *meta_data = TD_NULL;

    dps = ctx->curr_dps;
    if (dps->mtdt_addr == TD_NULL) {
        meta_data = dps_alloc_mem(ctx->chan_id, sizeof(sei->cuva_info.cuva_metadata));
        if (meta_data == TD_NULL) {
            dprint(PRN_FATAL, "%s, alloc metadata buffer failed\n", __func__);
            return;
        }
        dps->mtdt_addr = meta_data;
    }

    meta_data = dps->mtdt_addr;
    vfmw_check_sec_func(memset_s(meta_data, sizeof(sei->cuva_info.cuva_metadata),
        0, sizeof(sei->cuva_info.cuva_metadata)));
    if (dps->mtdt_size != 0) {
        dprint(PRN_WARN, "meta data info will be covered");
    }

    vfmw_check_sec_func(memcpy_s(meta_data, BASE_PROFILE_MAX_BITSTREAM_SIZE,
                                 &(sei->cuva_info.cuva_metadata), sizeof(sei->cuva_info.cuva_metadata)));
    dps->mtdt_size = sizeof(sei->cuva_info.cuva_metadata);

    return;
}
#endif

STATIC td_void hevc_convert_hdr_info(fsp_logic *logic_fs, const hevc_vui_param_set *vui_parameters,
    hevc_ctx *ctx)
{
    td_u32 i;
    hevc_sei_param_set *sei = &ctx->sei;
    vfmw_hdr_info *hdr_input = &(logic_fs->image.hdr_input);

    hdr_input->colour_info.full_rangeflag = vui_parameters->video_full_range_flag;
    hdr_input->colour_info.is_available = vui_parameters->colour_description_present_flag;
    hdr_input->colour_info.colour_primaries = vui_parameters->colour_primaries;
    hdr_input->colour_info.transfer_characteristics = vui_parameters->transfer_characteristics;
    hdr_input->colour_info.matrix_coeffs = vui_parameters->matrix_coeffs;

    hdr_input->mastering_available = sei->mastering_display_colour_volume.is_avalible;

    for (i = 0; i < 3; i++) { /* 3 is vfmw_max num */
        hdr_input->mastering_display_colour_volume.display_primaries_x[i] =
            sei->mastering_display_colour_volume.display_primaries_x[i];
        hdr_input->mastering_display_colour_volume.display_primaries_y[i] =
            sei->mastering_display_colour_volume.display_primaries_y[i];
    }

    hdr_input->mastering_display_colour_volume.white_point_x = sei->mastering_display_colour_volume.white_point_x;
    hdr_input->mastering_display_colour_volume.white_point_y = sei->mastering_display_colour_volume.white_point_y;
    hdr_input->mastering_display_colour_volume.max_disp_mastering_luminance =
        sei->mastering_display_colour_volume.max_display_mastering_luminance;
    hdr_input->mastering_display_colour_volume.min_disp_mastering_luminance =
        sei->mastering_display_colour_volume.min_display_mastering_luminance;

    hdr_input->content_available = sei->content_light_level.is_avalible;
    hdr_input->content_light_level.max_content_light_level = sei->content_light_level.max_content_light_level;
    hdr_input->content_light_level.max_pic_average_light_level = sei->content_light_level.max_pic_average_light_level;

    hdr_input->comapitibility_info.hdr_transfer_characteristic_idc =
        sei->hdr_compatible_info.hdr_transfer_characteristic_idc;

#ifdef HEVC_CUVA_SUPPORT
    hdr_input->cuva_dynamic_metadata_valid = sei->cuva_info.is_cuva;

    if (sei->cuva_info.is_cuva == TD_TRUE) {
        hevc_get_cuva_hdr_info(ctx, sei);
        sei->cuva_info.is_cuva = TD_FALSE;
    }
#endif

    return;
}

/*************************************************************************
*      函数原型 :  td_s32 hevc_alloc_frame_store()
*      功能描述 :  给当前图像分配一个帧存空间,pmv槽位
*      参数说明 :  无
*      返回值   ： DEC_NORMAL:配成功;
*                  FS_ALLOC_ERR:能分配到PMV槽位,解码错误得不可恢复，
*                               需要重新启动解码（从某种程度上说该
*                               类错误出现，必定是出现了bug需要定位
*                               解决）
*                  OUT_FLAG: 检测到适配层需要退出，返回该标志
*************************************************************************/
STATIC td_s32 hevc_alloc_frame_store(hevc_ctx *ctx)
{
    td_u32 i;
    td_s32 logic_fs_id;
    fsp_logic *logic_fs = TD_NULL;

    hevc_seq_param_set *sps = TD_NULL;
    hevc_pic_param_set *pps = TD_NULL;

    pps = ctx->pps[ctx->curr_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];

    /* Allocate frame store */
    ctx->curr_pic.state = HEVC_PIC_EMPTY;

    /* 找一个没有用到的帧存给当前帧 */
    for (i = 0; i < HEVC_MAX_FRAME_STORE; i++) {
        if (ctx->frame_store[i].frame_store_state == FS_NOT_USED &&
            (hevc_is_out_dpb(ctx, &ctx->frame_store[i]) == 1)) {
            /* error_level必须标记为有错，否则在解gap，或者其它原因其实并未解码
               但发生了StorePicInDPB()的操作，则这个本当不该输出的图有可能被输出 */
            ctx->curr_pic.err_level = 101; /* 101 is init err */
            ctx->curr_pic.state = HEVC_PIC_DECODING;
            ctx->curr_pic.frame_store = &ctx->frame_store[i];
            ctx->curr_pic.frame_store->err_level = 0;
            ctx->curr_pic.frame_store->is_displayed = 0;
            ctx->curr_pic.frame_store->non_existing = 0;
            ctx->curr_pic.frame_store->frame.is_long_term = 0;
            ctx->curr_pic.frame_store->frame.is_short_term = 0;
            ctx->curr_pic.frame_store->first_frame_state = FIRST_FRAME_OUT_INIT;
            break;
        }
    }

    if (ctx->curr_pic.state != HEVC_PIC_DECODING) {
        dprint(PRN_ERROR, "FS_ALLOC_ERR, ClearAll\n");
        return SYN_PARSE_RESOURCE_INVALID;
    }

    /* FrameStore申请到之后, 还要新建逻辑帧存，这样才能得到真正的地址 */
    /* 获取逻辑帧存 */
    logic_fs_id = fsp_new_logic(ctx->chan_id, FSP_TYPE_EXCLU);
    logic_fs = fsp_get_logic(ctx->chan_id, logic_fs_id);
    if (logic_fs_id >= 0 && logic_fs != TD_NULL) {
        hevc_convert_hdr_info(logic_fs, &(sps->vui_parameters), ctx);
        ctx->curr_pic.frame_store->logic_fs_id = logic_fs_id;
    } else {
        ctx->curr_pic.state = HEVC_PIC_EMPTY;
        return SYN_PARSE_RESOURCE_INVALID;
    }

    if (dcheck(PRN_TIME)) {
        td_u32 cid = (td_u32)ctx->chan_id;
        td_u32 fid = logic_fs->tick;
        dbg_add_time(cid, fid, TPOS_QUE_RAW, ctx->stm_info.que_raw_time);
        dbg_add_time(cid, fid, TPOS_DEQ_RAW, ctx->stm_info.deq_raw_time);
        dbg_add_time(cid, fid, TPOS_QUE_SEG, ctx->stm_info.que_seg_time);
        dbg_add_time(cid, fid, TPOS_DEQ_SEG, ctx->stm_info.deq_seg_time);
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_get_back_pic_from_vo_queue(hevc_ctx *ctx)
{
    td_u32 i;

    /* clean all frame store */
    for (i = 0; i < HEVC_MAX_FRAME_STORE; i++) {
        ctx->frame_store[i].is_reference = 0;
        ctx->frame_store[i].frame_store_state = FS_NOT_USED;
    }

    /* clean all pmv store */
    for (i = 0; i < ctx->total_pmv_num; i++) {
        ctx->pmv_store_used_flag[i] = 0;
    }

    return;
}

/*************************************************************************
*   函数原型 :  td_s32 hevc_remove_unused_frame_store()
*   功能描述 :  清除已经从pHevcCtx->DPB输出的非参考帧，针对已经vo显示与否两种情况对frame store 参数赋值
*   参数说明 :
*   返回值   ：
*************************************************************************/
STATIC td_s32 hevc_remove_unused_frame_store(hevc_ctx *ctx)
{
    td_u32 i, j, record_used_size;
    td_u32 is_removed;
    fsp_logic *logic_fs = TD_NULL;

    /* check for frames that were already output and no longer used for reference */
    /* dpb.used_size不能直接作为如下循环的控制变量，这是因为Remove操作本身会使used_size减1，
       若用它作循环控制变量，会使得DPB中的某些帧实际上并未遍历到，从而导致本该从DPB删除的帧依然留在DPB里.
       这样，DPB和FSP的信息是冲突的，可以引发很多问题.
       同州卡顿码流即由此引起，原因是FSP和DPB冲突造成GetAPC()失败，进而清除了DPB，导致丢帧. */
    record_used_size = ctx->dpb.used_size;
    is_removed = HEVC_FALSE;

    for (i = 0, j = 0; i < record_used_size; i++) {
        if (ctx->dpb.fs[j] == TD_NULL) {
            continue;
        }

        logic_fs = fsp_get_logic(ctx->chan_id, ctx->dpb.fs[j]->logic_fs_id);
        if (logic_fs != TD_NULL) {
            /* 如果fsp帧存已经空闲，要确保该帧立即从DPB中删除 */
            if ((logic_fs->is_ref == 0)
                &&  (logic_fs->disp_state == FSP_DISP_STATE_IDLE || logic_fs->disp_state == FSP_DISP_STATE_WAIT)
                &&  (ctx->dpb.fs[j]->first_frame_state != FIRST_FRAME_WAIT_OUT)) {
                ctx->dpb.fs[j]->logic_fs_id = -1;
                ctx->dpb.fs[j]->is_reference = 0;
                ctx->dpb.fs[j]->is_displayed = 1;
            }
        } else {
            if (ctx->dpb.fs[j]->first_frame_state != FIRST_FRAME_WAIT_OUT) {
                ctx->dpb.fs[j]->logic_fs_id = -1;
                ctx->dpb.fs[j]->is_reference = 0;
                ctx->dpb.fs[j]->is_displayed = 1;
            }
        }

        if ((ctx->dpb.fs[j]->is_reference == 0)
            &&  (ctx->dpb.fs[j]->is_displayed == 1)) {
            hevc_remove_frame_store_out_dpb(ctx, j);
            is_removed = HEVC_TRUE;
        } else {
            j++;
        }
    }

    return is_removed;
}

/*************************************************************************
*   函数原型 :  td_void hevc_remove_frame_store_out_dpb(td_u32 pos)
*   功能描述 :  将framestore指针移出dpb
*   参数说明 :
*   返回值   ：
*************************************************************************/
STATIC td_void hevc_remove_frame_store_out_dpb(hevc_ctx *ctx, td_u32 pos)
{
    td_u32 i;
    fsp_logic *logic = TD_NULL;

    if (ctx->dpb.fs[pos] == TD_NULL || ctx->dpb.used_size == 0) {
        dprint(PRN_ERROR, "%s: pos:%d used size:%d error\n", __func__, pos, ctx->dpb.used_size);
        return;
    }

    ctx->dpb.fs[pos]->frame_store_state = FS_NOT_USED;

    if (ctx->dpb.fs[pos]->non_existing) {
        ctx->dpb.fs[pos]->non_existing = 0;
    } else {
        logic = fsp_get_logic(ctx->chan_id, ctx->dpb.fs[pos]->logic_fs_id);
        if (logic != TD_NULL) {
            if (logic->disp_state != FSP_DISP_STATE_WAIT) {
                fsp_set_disp(ctx->chan_id, ctx->dpb.fs[pos]->logic_fs_id, 0);
            }

            if (ctx->dpb.fs[pos]->is_reference == 0) {
                fsp_set_ref(ctx->chan_id, ctx->dpb.fs[pos]->logic_fs_id, 0);
            }
            logic->is_syn_drop = 1;
        }
    }

    /* frame sort */
    for (i = pos; i < (ctx->dpb.used_size - 1); i++) {
        ctx->dpb.fs[i] = ctx->dpb.fs[i + 1];
    }

    ctx->dpb.fs[ctx->dpb.used_size - 1] = (hevc_frame_store *)TD_NULL;
    ctx->dpb.used_size--;

    return;
}

/*************************************************************************
*   函数原型 :  td_s32 hevc_get_min_poc(td_s32 *poc, td_s32 *pos)
*   功能描述 :  找出最小poc 和最小poc图像位置
*   参数说明 :
*   返回值   ：
*************************************************************************/
STATIC td_void hevc_get_min_poc(hevc_ctx *ctx, td_s32 *poc, td_u32 *pos)
{
    td_u32 i;

    *pos = 0;
    *poc = 0x7fffffff;

    if (ctx->dpb.used_size > ctx->dpb.size) {
        dprint(PRN_ERROR, "%s: ctx->dpb.used_size(%d) > dpb.size(%d), error resilence.\n",
               __func__, ctx->dpb.used_size, ctx->dpb.size);
        ctx->dpb.used_size = ctx->dpb.size; /* for error resilence */
    }

    for (i = 0; i < ctx->dpb.used_size; i++) {
        if (*poc > ctx->dpb.fs[i]->poc) {
            *poc = ctx->dpb.fs[i]->poc;
            *pos = i;
        }
    }

    return;
}

/*************************************************************************
*      函数原型 :      td_s32 hevc_get_rerange_flag()
*      功能描述 :  判断帧存空间是否要重新分配
*      参数说明 :
*      返回值   ：
*************************************************************************/
STATIC td_s32 hevc_get_rerange_flag(hevc_ctx *ctx, td_s32 old_width, td_s32 old_height,
                             td_s32 old_log2_ctb_size_y, td_s32 dpb_size_changed)
{
    td_s32 rerange_flag;
    hevc_seq_param_set *sps = TD_NULL;
    hevc_pic_param_set *pps = TD_NULL;
    td_u32 neww, newh;

    vfmw_unused(old_log2_ctb_size_y);
    rerange_flag = 0;
    pps = ctx->pps[ctx->curr_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];
    neww = sps->pic_width_in_luma_samples;
    newh = sps->pic_height_in_luma_samples;

    if (dpb_size_changed || (neww != (td_u32)old_width) || (newh != (td_u32)old_height) ||
        (!ctx->allow_start_dec) || (ctx->extra_data_ptr->bit_depth_changed)) {
#ifdef VFMW_MV_HEVC_SUPPORT
        if (hevc_is_multi_layer_valid(ctx) == 1) {
            if (ctx->is_mem_arrange_done == 0) {
                ctx->is_mem_arrange_done = 1;
                rerange_flag = 1;
            }
        } else
#endif
        {
            rerange_flag = 1;
        }
    }

    return rerange_flag;
}

VFMW_ACC_SLICE_H265 STATIC td_void hevc_calc_stream_bits(hevc_ctx *ctx)
{
    td_u32 code_len_byte;
    td_u32 i;

    /* calculate bytes used before curr segment */
    code_len_byte = 0;

    for (i = 0; i < (td_u32)ctx->curr_nal->nal_used_segment; i++) {
        /* length of nal in byte,2 nals起作用，when 1 nal,code_len_byte = 0 */
        code_len_byte += ctx->curr_nal->stream[i].bitstream_length;
    }
    ctx->curr_nal->stream[ctx->curr_nal->nal_used_segment].bits_offset =
        (td_u32)bs_pos(ctx->bs_ptr) & 7; /* 7 is align */
    ctx->curr_nal->stream[ctx->curr_nal->nal_used_segment].bytes_pos =
        ctx->curr_nal->stream[ctx->curr_nal->nal_used_segment].stream_phy_addr +
        ((td_u32)(bs_pos(ctx->bs_ptr) / 8) - code_len_byte); /* 8 is bit num */

    vfmw_check(ctx->curr_nal->stream[ctx->curr_nal->nal_used_segment].bitstream_length < VFMW_MAX_UINT32);
    ctx->curr_nal->stream[ctx->curr_nal->nal_used_segment].valid_bitlen =
        (ctx->curr_nal->stream[ctx->curr_nal->nal_used_segment].bitstream_length * 8) - /* 8 is bit num */
        ((td_u32)bs_pos(ctx->bs_ptr) - (code_len_byte * 8)); /* 8 is bit num */
    /* 16byte 对齐 */
    ctx->curr_nal->stream[ctx->curr_nal->nal_used_segment].bits_offset +=
        (td_u32)(8 * (ctx->curr_nal->stream[ctx->curr_nal->nal_used_segment].bytes_pos & 3)); /* 8 3 is num */
    ctx->curr_nal->stream[ctx->curr_nal->nal_used_segment].bytes_pos =
        ((ctx->curr_nal->stream[ctx->curr_nal->nal_used_segment].bytes_pos) & (~3)); /* 3 is num */
    /* last sement sub trailing zeros, when cavlc sun stop one */
    if (ctx->curr_nal->nal_segment != 1 && ctx->curr_nal->nal_segment != 2) { /* 2 is nal num */
        dprint(PRN_ERROR, "nal_segment = %d is not expected value\n", ctx->curr_nal->nal_segment);
        return;
    }

    /* other segment bits_offset and so on */
    for (i = ctx->curr_nal->nal_used_segment + 1; (i < ctx->curr_nal->nal_segment) &&
         (i < 2); i++) { /* 2 is nal num */
        ctx->curr_nal->stream[i].bits_offset = 0;
        ctx->curr_nal->stream[i].bytes_pos = ctx->curr_nal->stream[i].stream_phy_addr;
        vfmw_check(ctx->curr_nal->stream[i].bitstream_length < VFMW_MAX_UINT32);
        ctx->curr_nal->stream[i].valid_bitlen = ctx->curr_nal->stream[i].bitstream_length * 8; /* 8 is bit num */

        ctx->curr_nal->stream[i].bits_offset +=
            (td_u32)(8 * (ctx->curr_nal->stream[i].bytes_pos & HEVC_STREAM_BASE_ALIGN)); /* 8 is bit num */
        ctx->curr_nal->stream[i].bytes_pos = ctx->curr_nal->stream[i].bytes_pos & (~HEVC_STREAM_BASE_ALIGN);
    }

    /* last segment sub trailing zeros, when cavlc sun stop one */
    ctx->curr_nal->stream[ctx->curr_nal->nal_segment - 1].valid_bitlen -= ctx->curr_nal->nal_trail_zero_bit_num;

    /* find stream_base_addr */
    for (i = ctx->curr_nal->nal_used_segment; i < ctx->curr_nal->nal_segment; i++) {
        if (ctx->curr_nal->stream[i].bytes_pos < ctx->curr_pic.stream_base_addr) {
            ctx->curr_pic.stream_base_addr = ctx->curr_nal->stream[i].bytes_pos;
            // 多slice低延时模式下赋值 seg_buf 的起始地址，避免多slice情况下会出现stream_base_addr大于seg_buf地址的情况
            if (ctx->ldy_mode == LDY_MULTI_SLC) {
                ctx->curr_pic.stream_base_addr = ctx->seg_base_phy_addr;
            }
        }
    }

    return;
}

STATIC td_s32 hevc_pps_check(const hevc_ctx *ctx, const hevc_pic_param_set *pps)
{
    vfmw_unused(ctx);
    vfmw_unused(pps);
    if (pps == TD_NULL) {
        dprint(PRN_ERROR, "pps is null with this pic_parameter_set_id = %d havn't be decoded\n",
               ctx->curr_slice.pic_parameter_set_id);
        return HEVC_DEC_ERR;
    }

    if (!pps->valid) {
        dprint(PRN_ERROR, "pps with this pic_parameter_set_id = %d havn't be decoded\n",
               ctx->curr_slice.pic_parameter_set_id);
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sps_check(const hevc_pic_param_set *pps, const hevc_seq_param_set *sps)
{
    vfmw_unused(sps);
    vfmw_unused(pps);

    if (sps == TD_NULL) {
        dprint(PRN_ERROR, "sps is null with this seq_parameter_set_id = %d havn't be decoded\n",
               pps->seq_parameter_set_id);
        return HEVC_DEC_ERR;
    }

    if (!sps->valid) {
        dprint(PRN_ERROR, "sps with this seq_parameter_set_id = %d havn't be decoded\n", pps->seq_parameter_set_id);
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_vps_check(const hevc_seq_param_set *sps, const hevc_video_param_set *vps)
{
    vfmw_unused(sps);
    vfmw_unused(vps);

    if (vps == TD_NULL) {
        dprint(PRN_ERROR, "vps is null with this video_parameter_set_id = %d havn't be decoded\n",
               sps->video_parameter_set_id);
        return HEVC_DEC_ERR;
    }

    if (!vps->valid) {
        dprint(PRN_ERROR, "vps with this video_parameter_set_id = %d havn't be decoded\n", sps->video_parameter_set_id);
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_vps_sps_pps_check(const hevc_ctx *ctx)
{
    td_s32 ret;
    hevc_video_param_set *vps = TD_NULL;
    hevc_seq_param_set *sps = TD_NULL;
    hevc_pic_param_set *pps = TD_NULL;

    /* check pps is get or not */
    if (ctx->curr_slice.pic_parameter_set_id < 0 || ctx->curr_slice.pic_parameter_set_id >= (ctx->max_pps_num)) {
        dprint(PRN_ERROR, "hevc_vps_sps_pps_check pic_parameter_set_id(%d) out of range\n",
               ctx->curr_slice.pic_parameter_set_id);
        return HEVC_DEC_ERR;
    }

    pps = ctx->pps[ctx->curr_slice.pic_parameter_set_id];

    ret = hevc_pps_check(ctx, pps);
    if (ret != HEVC_DEC_NORMAL) {
        return ret;
    }

    /* check sps is get or not */
    if (pps->seq_parameter_set_id < 0 || pps->seq_parameter_set_id >= (ctx->max_sps_num)) {
        dprint(PRN_ERROR, "hevc_vps_sps_pps_check seq_parameter_set_id(%d) out of range\n", pps->seq_parameter_set_id);
        return HEVC_DEC_ERR;
    }

    sps = ctx->sps[pps->seq_parameter_set_id];

    ret = hevc_sps_check(pps, sps);
    if (ret != HEVC_DEC_NORMAL) {
        return ret;
    }

    /* check vps is get or not */
    if (sps->video_parameter_set_id >= (td_u32)(ctx->max_vps_num)) {
        dprint(PRN_ERROR, "hevc_vps_sps_pps_check video_parameter_set_id out of range\n");
        return HEVC_DEC_ERR;
    }

    vps = ctx->vps[sps->video_parameter_set_id];

    ret = hevc_vps_check(sps, vps);
    if (ret != HEVC_DEC_NORMAL) {
        return ret;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_skip_pic_poc_random_access(hevc_ctx *ctx)
{
    if (ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_CRA ||
        ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_LP ||
        ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP ||
        ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_RADL ||
        (((ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_TRAIL_N) ||
          (ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_TRAIL_R)) &&
         (ctx->curr_slice.slice_type == HEVC_I_SLICE))) {
        if ((ctx->poc_random_access[ctx->curr_slice.view_id] == HEVC_MAX_INT) ||
            (ctx->last_display_poc[ctx->curr_nal->nal_layer_id] >= ctx->curr_slice.poc)) {
            ctx->poc_random_access[ctx->curr_slice.view_id] = ctx->curr_slice.poc;

            if ((ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_TRAIL_N) ||
                (ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_TRAIL_R)) {
                ctx->need_drop_b_frame = 1;
            }
        }
    } else if ((ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL) ||
               (ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP)) {
        ctx->poc_random_access[ctx->curr_slice.view_id] = -HEVC_MAX_INT;
    }
}

STATIC hevc_pic_type hevc_is_skip_pic(hevc_ctx *ctx)
{
    hevc_skip_pic_poc_random_access(ctx);

    if ((ctx->curr_slice.poc < ctx->poc_random_access[ctx->curr_slice.view_id]) &&
        (ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_RASL_R ||
         ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_RASL_N)) {
        dprint(PRN_ERROR, "%s, %d, curr_slice.poc = %d, poc_random_access = %d, nal_unit_type = %d, "
               "should skip this frame\n",
               __func__, __LINE__, ctx->curr_slice.poc,
               ctx->poc_random_access[ctx->curr_slice.view_id], ctx->curr_nal->nal_unit_type);
        return IS_SKIP_PIC;
    } else if ((ctx->no_rasl_output_flag[ctx->curr_slice.view_id]) &&
               (ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_RASL_R ||
                ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_RASL_N)) {
        dprint(PRN_ERROR, "%s, %d, curr_slice.poc = %d, no_rasl_output_flag = %d, nal_unit_type = %d, "
               "should skip this frame\n",
               __func__, __LINE__, ctx->curr_slice.poc,
               ctx->no_rasl_output_flag[ctx->curr_slice.view_id], ctx->curr_nal->nal_unit_type);
        return IS_SKIP_PIC;
    }

    /* check if picture should be skipped because of association with a previous BLA picture */
    if (ctx->prev_rap_is_bla && ctx->curr_slice.poc < ctx->poc_cra &&
        (ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_RASL_R ||
         ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_RASL_N)) {
        dprint(PRN_ERROR, "%s, %d, prev_rap_is_bla = %d, curr_slice.poc = %d, poc_cra = %d, "
               "nal_unit_type = %d, should skip this frame\n",
               __func__, __LINE__,
               ctx->prev_rap_is_bla, ctx->curr_slice.poc, ctx->poc_cra, ctx->curr_nal->nal_unit_type);
        return IS_SKIP_PIC;
    }

    return NOT_NEW_PIC;
}

STATIC hevc_pic_type hevc_is_new_pic(hevc_ctx *ctx)
{
    td_u32 ret, flag;

    /* as 'poc_random_access == HEVC_MAX_INT' is mean the first frame want to decode,
      it's nal type should be as fellow these type! */
    if (ctx->poc_random_access[ctx->curr_slice.view_id] == HEVC_MAX_INT) {
        flag = 0;
        flag |= (td_u32)(ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_CRA);
        flag |= (td_u32)(ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_LP);
        flag |= (td_u32)(ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP);
        flag |= (td_u32)(ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_RADL);
        flag |= (td_u32)(ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL);
        flag |= (td_u32)(ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP);
        flag |= (td_u32)(((ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_TRAIL_N) ||
                          (ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_TRAIL_R)) &&
                         (ctx->curr_slice.slice_type == HEVC_I_SLICE));
        if (flag == 0) {
            return IS_SKIP_PIC;
        }
    }

    if (hevc_is_skip_pic(ctx) == IS_SKIP_PIC) {
        return IS_SKIP_PIC;
    }

    if (!ctx->curr_slice.dependent_slice_segment_flag) {
        if (ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL ||
            ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP) {
            ctx->poc_cra = ctx->curr_slice.poc;
            ctx->prev_rap_is_bla = HEVC_FALSE;
        } else if (ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_CRA) {
            ctx->poc_cra = ctx->curr_slice.poc;
            ctx->prev_rap_is_bla = HEVC_FALSE;
        } else if (ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_LP ||
                   ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_RADL ||
                   ctx->curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP) {
            ctx->poc_cra = ctx->curr_slice.poc;
            ctx->prev_rap_is_bla = HEVC_TRUE;
        }
    }

    ret = (td_u32)(ctx->curr_slice.first_slice_segment_in_pic_flag);

    if (ctx->num_slice_para >= 1) {
        if (ctx->curr_slice.pic_parameter_set_id != ctx->last_slice_pps_id) {
            ret = IS_NEW_PIC;
        }
    }

    if (ctx->prev_slc_pps_id != ctx->curr_slice.pic_parameter_set_id ||
        ctx->prev_poc != (td_u32)(ctx->curr_slice.poc)) {
        ret = IS_NEW_PIC;
    }

    return ret;
}

STATIC td_s32 hevc_read_byte_alignment(const hevc_ctx *ctx)
{
    td_s32 code, skiplen;

    code = (td_s32)hevc_u_v(ctx->bs_ptr, 1, "");
    if (code == 0) {
        dprint(PRN_ERROR, "hevc_read_byte_alignment: code should not be zero.\n");
        return HEVC_DEC_ERR;
    }

    skiplen = bs_next_byte(ctx->bs_ptr);
    if (skiplen == 0) {
        return HEVC_DEC_NORMAL;
    } else {
        hevc_u_v(ctx->bs_ptr, skiplen, "");
    }

    return HEVC_DEC_NORMAL;
}

STATIC __inline td_u8 hevc_get_rap_pic_flag(hevc_ctx *ctx)
{
    vfmw_check_ret(ctx != TD_NULL, 0);
    return (td_u8)(ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL
                   || ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP
                   || ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP
                   || ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_RADL
                   || ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_LP
                   || ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_CRA);
}

STATIC __inline td_u8 hevc_get_idr_pic_flag(hevc_ctx *ctx)
{
    vfmw_check_ret(ctx != TD_NULL, 0);
    return (td_u8)(ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL
                   || ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP);
}

STATIC td_void hevc_get_profile_and_level(hevc_ctx *ctx, hevc_profile_tier_level *ptl,
    td_s32 profile_present_flag)
{
    td_s32 i;

    if (profile_present_flag) {
        ptl->general_profile_space = (td_s32)hevc_u_v(ctx->bs_ptr, 2, "general_profile_space[]"); /* 2 is bit num */
        ptl->general_tier_flag = hevc_u_v(ctx->bs_ptr, 1, "general_tier_flag[]");
        ptl->general_profile_idc = (td_s32)hevc_u_v(ctx->bs_ptr, 5, "general_profile_idc[]"); /* 5 is bit num */

        for (i = 0; i < 32; i++) { /* 32 is vfmw_max num */
            /* delete snprintf for cpu load */
            ptl->general_profile_compatibility_flag[i] = hevc_u_v(ctx->bs_ptr, 1, "general_profile_compatibility_flag");
        }

        ptl->general_progressive_source_flag = hevc_u_v(ctx->bs_ptr, 1, "general_progressive_source_flag");
        ptl->general_interlaced_source_flag = hevc_u_v(ctx->bs_ptr, 1, "general_interlaced_source_flag");
        ptl->general_non_packed_constraint_flag = hevc_u_v(ctx->bs_ptr, 1, "general_non_packed_constraint_flag");
        ptl->general_frame_only_constraint_flag = hevc_u_v(ctx->bs_ptr, 1, "general_frame_only_constraint_flag");

        hevc_u_v(ctx->bs_ptr, 16, "general_reserved_zero_44bits[0..15]"); /* 16 is bit num */
        hevc_u_v(ctx->bs_ptr, 16, "general_reserved_zero_44bits[16..31]"); /* 16 is bit num */
        hevc_u_v(ctx->bs_ptr, 12, "general_reserved_zero_44bits[32..43]"); /* 12 is bit num */

        ctx->general_profile_idc = (td_u32)ptl->general_profile_idc;
        ctx->general_profile_compatibility_flag_1 = ptl->general_profile_compatibility_flag[1];
        ctx->general_profile_compatibility_flag_2 = ptl->general_profile_compatibility_flag[2]; /* 2 is idx */
    }

    ptl->general_level_idc = (td_s32)hevc_u_v(ctx->bs_ptr, 8, "general_level_idc"); /* 8 is bit num */
    return;
}

STATIC td_void hevc_get_sub_layer_info(const hevc_ctx *ctx, hevc_profile_tier_level *ptl,
    td_s32 profile_present_flag, td_s32 max_num_sub_layers_minus1)
{
    td_s32 i, j;

    for (i = 0; i < max_num_sub_layers_minus1; i++) {
        if (i >= HEVC_MAX_SUB_LAYER_NUM) {
            break;
        }

        if (profile_present_flag && ptl->sub_layer_profile_present_flag[i]) {
            ptl->sub_layer_profile_space[i] =
                (td_s32)hevc_u_v(ctx->bs_ptr, 2, "sub_layer_profile_space"); /* 2 is bit num */
            ptl->sub_layer_tier_flag[i] = hevc_u_v(ctx->bs_ptr, 1, "sub_layer_tier_flag");
            ptl->sub_layer_profile_idc[i] =
                (td_s32)hevc_u_v(ctx->bs_ptr, 5, "sub_layer_profile_idc"); /* 5 is bit num */

            for (j = 0; j < 32; j++) { /* 32 is vfmw_max num */
                ptl->sub_layer_profile_compatibility_flag[i][j] =
                    hevc_u_v(ctx->bs_ptr, 1, "sub_layer_profile_compatibility_flag");
            }

            ptl->sub_layer_progressive_source_flag[i] = hevc_u_v(ctx->bs_ptr, 1, "general_progressive_source_flag");
            ptl->sub_layer_interlaced_source_flag[i] = hevc_u_v(ctx->bs_ptr, 1, "general_interlaced_source_flag");
            ptl->sub_layer_non_packed_constraint_flag[i] =
                hevc_u_v(ctx->bs_ptr, 1, "general_non_packed_constraint_flag");
            ptl->sub_layer_frame_only_constraint_flag[i] =
                hevc_u_v(ctx->bs_ptr, 1, "general_frame_only_constraint_flag");

            hevc_u_v(ctx->bs_ptr, 16, "general_reserved_zero_44bits[0..15]"); /* 16 is bit num */
            hevc_u_v(ctx->bs_ptr, 16, "general_reserved_zero_44bits[16..31]"); /* 16 is bit num */
            hevc_u_v(ctx->bs_ptr, 12, "general_reserved_zero_44bits[32..43]"); /* 12 is bit num */
        }

        if (ptl->sub_layer_level_present_flag[i]) {
            ptl->sub_layer_level_idc[i] = (td_s32)hevc_u_v(ctx->bs_ptr, 8, "sub_layer_level_idc"); /* 8 is bit num */
        }
    }

    return;
}

STATIC td_s32 hevc_dec_ptl(hevc_ctx *ctx, hevc_profile_tier_level *ptl,
                    td_s32 profile_present_flag, td_s32 max_num_sub_layers_minus1)
{
    td_s32 i;
    td_u32 code;

    hevc_get_profile_and_level(ctx, ptl, profile_present_flag);

    for (i = 0; i < max_num_sub_layers_minus1; i++) {
        if (i >= HEVC_MAX_SUB_LAYER_NUM) {
            break;
        }

        ptl->sub_layer_profile_present_flag[i] = hevc_u_v(ctx->bs_ptr, 1, "sub_layer_profile_present_flag");
        ptl->sub_layer_level_present_flag[i] = hevc_u_v(ctx->bs_ptr, 1, "sub_layer_level_present_flag");
    }

    if (max_num_sub_layers_minus1 > 0) {
        for (i = max_num_sub_layers_minus1; i < 8; i++) { /* 8 is vfmw_max num */
            code = hevc_u_v(ctx->bs_ptr, 2, "reserved_zero_2bits"); /* 2 is bit num */
            if (code != 0) {
                dprint(PRN_ERROR, "reserved_zero_2bits not equal 0.\n");
                return HEVC_DEC_ERR;
            }
        }
    }

    hevc_get_sub_layer_info(ctx, ptl, profile_present_flag, max_num_sub_layers_minus1);

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_scaling_list_of_copy_mode(const hevc_ctx *ctx, hevc_scaling_list *scaling_list,
    td_u32 matrix_id, td_u32 size_id)
{
    td_u32 code, ref_list_id;
    td_s32 coefNum;
    td_u8 *matrix_address = TD_NULL;
    td_u8 *dst_scaling_list = TD_NULL;

    vfmw_check_ret((matrix_id < HEVC_SCALING_LIST_NUM), HEVC_DEC_ERR);
    vfmw_check_ret((size_id < SCALING_LIST_SIZE_NUM), HEVC_DEC_ERR);

    coefNum = vfmw_min(HEVC_MAX_MATRIX_COEF_NUM, (td_s32)ctx->scaling_list_size[size_id]);
    dst_scaling_list = scaling_list->scaling_list_coef[size_id][matrix_id];

    code = hevc_ue_v(ctx->bs_ptr, "scaling_list_pred_matrix_id_delta");
    if (code > matrix_id) {
        dprint(PRN_ERROR, "scaling_list_pred_matrix_id_delta out of range(0,matrix_id).\n");
        return HEVC_DEC_ERR;
    }

    scaling_list->scaling_list_pred_matrix_id_delta[size_id][matrix_id] = (td_u8)code;
    scaling_list->ref_matrix_id[size_id][matrix_id] = (td_u8)(matrix_id - code);

    if (size_id > SCALING_LIST_8X8) {
        ref_list_id = scaling_list->ref_matrix_id[size_id][matrix_id];
        code = (td_u32)((matrix_id == ref_list_id) ? 16 : /* 16 is num */
               scaling_list->scaling_list_dc[size_id][ref_list_id]);
        scaling_list->scaling_list_dc[size_id][matrix_id] = (td_s32)code;
    }

    ref_list_id = scaling_list->ref_matrix_id[size_id][matrix_id];
    matrix_address = (matrix_id == ref_list_id) ?
        hevc_get_scaling_list_default_address(size_id, ref_list_id) :
        scaling_list->scaling_list_coef[size_id][ref_list_id];
    if (matrix_address == TD_NULL) {
        dprint(PRN_ERROR, "hevc_dec_scaling_list_data matrix_address TD_NULL.\n");
        return HEVC_DEC_ERR;
    }

    vfmw_check_sec_func(memcpy_s(dst_scaling_list, HEVC_MAX_MATRIX_COEF_NUM,
                                 matrix_address, sizeof(td_u8) * (td_u32)coefNum));
    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_scaling_list_of_dpcm_mode(const hevc_ctx *ctx, hevc_scaling_list *scaling_list,
    td_u32 matrix_id, td_u32 size_id)
{
    td_s32 coefNum, i;
    td_s32 pos;
    td_s32 next_coef;
    td_u8 *dst_scaling_list = TD_NULL;

    vfmw_check_ret(size_id < SCALING_LIST_SIZE_NUM, HEVC_DEC_ERR);
    vfmw_check_ret(matrix_id < HEVC_SCALING_LIST_NUM, HEVC_DEC_ERR);

    next_coef = HEVC_SCALING_LIST_START_VALUE;
    dst_scaling_list = scaling_list->scaling_list_coef[size_id][matrix_id];
    coefNum = vfmw_min(HEVC_MAX_MATRIX_COEF_NUM, (td_s32)ctx->scaling_list_size[size_id]);

    if (size_id > SCALING_LIST_8X8) {
      scaling_list->scaling_list_dc_coef_minus8 = hevc_se_v(ctx->bs_ptr, "scaling_list_dc_coef_minus8");

      if (scaling_list->scaling_list_dc_coef_minus8 < -7 || /* -7 is vfmw_min num */
          scaling_list->scaling_list_dc_coef_minus8 > 247) { /* 247 is vfmw_max num */
          dprint(PRN_ERROR, "scaling_list_dc_coef_minus8 out of range(-7,247).\n");
          return HEVC_DEC_ERR;
      }

      scaling_list->scaling_list_dc[size_id][matrix_id] =
          scaling_list->scaling_list_dc_coef_minus8 + 8; /* 8 is num */
      next_coef = scaling_list->scaling_list_dc[size_id][matrix_id];
    }

    for (i = 0; i < coefNum; i++) {
      scaling_list->scaling_list_delta_coef = hevc_se_v(ctx->bs_ptr, "scaling_list_delta_coef");

      if (scaling_list->scaling_list_delta_coef < -128 || /* -128 is vfmw_min num */
          scaling_list->scaling_list_delta_coef > 127) { /* 127 is vfmw_max num */
          dprint(PRN_ERROR, "scaling_list_dc_coef_minus8 out of range(-128,127).\n");
          return HEVC_DEC_ERR;
      }

      next_coef = (next_coef + scaling_list->scaling_list_delta_coef + 256) % 256; /* 256 is vfmw_max num */

      if (size_id == 0) {
          pos = g_hevc_diag_scan4x4_y[i];
          pos = 4 * pos + g_hevc_diag_scan4x4_x[i]; /* 4 is num */
      } else {
          pos = g_hevc_diag_scan8x8_y[i];
          pos = 8 * pos + g_hevc_diag_scan8x8_x[i]; /* 8 is num */
      }

      dst_scaling_list[pos] = (td_u8)next_coef;
    }
    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_scaling_list_data(const hevc_ctx *ctx, hevc_scaling_list *scaling_list)
{
    td_u8 scaling_list_pred_mode_flag;
    td_u32 size_id, matrix_id;
    td_s32 ret;

    ret = HEVC_DEC_NORMAL;

    /* init quantization matrix array */
    vfmw_check_sec_func(memcpy_s(scaling_list->scaling_list_coef[SCALING_LIST_32X32][3], /* 3 is idx */
                                 sizeof(scaling_list->scaling_list_coef[SCALING_LIST_32X32][3]),
                                 scaling_list->scaling_list_coef[SCALING_LIST_32X32][1], HEVC_MAX_MATRIX_COEF_NUM));

    /* for each size */
    for (size_id = 0; size_id < SCALING_LIST_SIZE_NUM; size_id++) {
        for (matrix_id = 0; (matrix_id < HEVC_SCALING_LIST_NUM) &&
             (matrix_id < ctx->scaling_list_num[size_id]); matrix_id++) {
            scaling_list_pred_mode_flag = hevc_u_v(ctx->bs_ptr, 1, "scaling_list_pred_mode_flag");
            scaling_list->scaling_list_pred_mode_flag[size_id][matrix_id] = scaling_list_pred_mode_flag;

            if (!scaling_list_pred_mode_flag) { /* Copy Mode */
                ret = hevc_scaling_list_of_copy_mode(ctx, scaling_list, matrix_id, size_id);
                vfmw_check_ret(ret == HEVC_DEC_NORMAL, ret);
            } else { /* DPCM Mode */
                ret = hevc_scaling_list_of_dpcm_mode(ctx, scaling_list, matrix_id, size_id);
                vfmw_check_ret(ret == HEVC_DEC_NORMAL, ret);
            }
        }
    }

    return ret;
}

STATIC hevc_scaling_list *hevc_get_scaling_list(const hevc_ctx *ctx)
{
    hevc_scaling_list *scaling_list = NULL;

    if (ctx->curr_pps->pic_scaling_list_data_present_flag) {
        scaling_list = &(ctx->curr_pps->scaling_list);
    } else if (ctx->curr_sps->sps_scaling_list_data_present_flag) {
        scaling_list = &(ctx->curr_sps->scaling_list);
    }

    return scaling_list;
}

STATIC hevc_scaling_list *hevc_get_default_scaling_list(const hevc_ctx *ctx)
{
    hevc_scaling_list *scaling_list = NULL;

#ifdef VFMW_MV_HEVC_SUPPORT
    if (ctx->is_multi_layers == 1) {
        scaling_list = hevc_get_infer_scaling_list(ctx);
    } else {
#endif
        scaling_list = hevc_get_scaling_list(ctx);
#ifdef VFMW_MV_HEVC_SUPPORT
    }
#endif
    return scaling_list;
}

/*************************************************************************
* 函数原型 :  td_s32 hevc_set_scaling_list()
* 功能描述 :  设置scaling_list
* 参数说明 :
* 返回值   ：
*************************************************************************/
STATIC td_s32 hevc_set_scaling_list(hevc_ctx *ctx)
{
    td_u32 i, j;
    td_u8 *temp_scaling_list = NULL;
    hevc_scaling_list *scaling_list = NULL;

    /* 先给相应的元素的全局变量赋值，后面将写入消息池 */
    /* set scaling_list */
    if (!ctx->curr_sps->scaling_list_enabled_flag) {
        return HEVC_DEC_NORMAL;
    }

    scaling_list = hevc_get_default_scaling_list(ctx);
    if (scaling_list == NULL) {
        hevc_set_default_scaling_list(ctx);
        return HEVC_DEC_NORMAL;
    }

    vfmw_check_sec_func(memset_s(ctx->qmatrix, sizeof(ctx->qmatrix), 0, sizeof(ctx->qmatrix)));

    for (i = 0; i < SCALING_LIST_SIZE_NUM; i++) {
        for (j = 0; j < ctx->scaling_list_num[i]; j++) {
            temp_scaling_list = scaling_list->scaling_list_coef[i][j];
            hevc_cal_scaling_list(ctx, temp_scaling_list, i, j);
        }
    }

    vfmw_check_sec_func(memcpy_s(ctx->scaling_list_dc, sizeof(ctx->scaling_list_dc),
                                &scaling_list->scaling_list_dc, sizeof(scaling_list->scaling_list_dc)));

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_set_default_scaling_list(hevc_ctx *ctx)
{
    td_u32 i, j;

    for (i = 0; i < SCALING_LIST_SIZE_NUM; i++) {
        for (j = 0; j < ctx->scaling_list_num[i]; j++) {
            if (i == 0) {
                vfmw_check_sec_func(memcpy_s(ctx->qmatrix[i][j], sizeof(ctx->qmatrix[i][j]),
                    g_quant_ts_default, 4 * 4)); /* 4 * 4 is array size */
            } else if (i == 3) { /* 3 is idx */
                vfmw_check_sec_func(memcpy_s(ctx->qmatrix[i][j], sizeof(ctx->qmatrix[i][j]),
                    (j < 1) ? g_quant8_intra_default : g_quant8_inter_default, 4 * 16)); /* 4 * 16 is array size */
            } else {
                vfmw_check_sec_func(memcpy_s(ctx->qmatrix[i][j], sizeof(ctx->qmatrix[i][j]),
                    (j < 3) ? g_quant8_intra_default : g_quant8_inter_default, 4 * 16)); /* 3 idx 4 * 16 array size */
            }

            ctx->scaling_list_dc[i][j] = HEVC_SCALING_LIST_DC;
        }
    }

    return;
}

STATIC td_void hevc_cal_scaling_list(hevc_ctx *ctx, const td_u8 *scaling_list_coef, td_u32 list_size, td_u32 matrix_id)
{
    td_u32 i, k;
    td_u32 coef_num;

    if (matrix_id >= 6 || list_size >= 4) { /* 6 is matrix index 4 a num */
        return;
    }

    coef_num = vfmw_min(HEVC_MAX_MATRIX_COEF_NUM, ctx->scaling_list_size[list_size]);
    k = 0;

    while (k < (coef_num / 4)) { /* 4 is num */
        for (i = 0; i < coef_num; i++) {
            /* 4个bytes拼在1个words中 */
            ctx->qmatrix[list_size][matrix_id][k] =
                ctx->qmatrix[list_size][matrix_id][k] | ((td_u32)scaling_list_coef[i] << ((i % 4) * 8));
            /* 4 8 is num */

            if (3 == i % 4) { /* 3 4 is num */
                k++;
            }
        }
    }

    return;
}

/** get default address of quantization matrix
 * \param size_id size index
 * \param listId list index
 * \returns pointer of quantization matrix
 */
STATIC td_u8 *hevc_get_scaling_list_default_address(td_u32 size_id, td_u32 matrix_id)
{
    td_u8 *src = TD_NULL;

    switch (size_id) {
        case SCALING_LIST_4X4:
            src = g_quant_ts_default4x4;
            break;

        case SCALING_LIST_8X8:
            src = (matrix_id < 3) ? g_quant_intra_default8x8 : g_quant_inter_default8x8; /* 3 is idx */
            break;

        case SCALING_LIST_16X16:
            src = (matrix_id < 3) ? g_quant_intra_default8x8 : g_quant_inter_default8x8; /* 3 is idx */
            break;

        case SCALING_LIST_32X32:
            src = (matrix_id < 1) ? g_quant_intra_default8x8 : g_quant_inter_default8x8;
            break;

        default:
            dprint(PRN_ERROR, "hevc_get_scaling_list_default_address TD_NULL.\n");
            src = TD_NULL;
            break;
    }

    return src;
}

STATIC td_void hevc_calc_short_term_pics_num(const hevc_ctx *ctx, hevc_short_term_rpset *short_term_rpset,
   const hevc_short_term_rpset *temp_rpset, td_s32 delta_rps)
{
    td_u32 ref_idc, i, k, k1, k2;
    td_u32 used_by_curr_pic_flag, use_delta_flag;
    td_s32 delta_poc;

    k = 0;
    k1 = 0;
    k2 = 0;

    for (i = 0; i <= temp_rpset->num_of_pics; i++) {
        /* first bit is "1" if Idc is 1 */
        used_by_curr_pic_flag = hevc_u_v(ctx->bs_ptr, 1, "used_by_curr_pic_flag");
        ref_idc = used_by_curr_pic_flag;

        if (ref_idc == 0) {
            /* second bit is "1" if Idc is 2, "0" otherwise. */
            use_delta_flag = hevc_u_v(ctx->bs_ptr, 1, "use_delta_flag");
            ref_idc = use_delta_flag << 1;
        }

        if (ref_idc == 1 || ref_idc == 2) { /* 2 is idc */
            delta_poc = delta_rps + ((i < temp_rpset->num_of_pics) ? temp_rpset->delta_poc[i] : 0);
            short_term_rpset->delta_poc[k] = delta_poc;
            short_term_rpset->used_flag[k] = (ref_idc == 1);

            if (delta_poc < 0) {
                k1++;
            } else {
                k2++;
            }
            k++;
        }

        short_term_rpset->ref_idc[i] = ref_idc;
    }

    short_term_rpset->num_ref_idc = temp_rpset->num_of_pics + 1;
    short_term_rpset->num_of_pics = k;
    short_term_rpset->num_negative_pics = k1;
    short_term_rpset->num_positive_pics = k2;

    return;
}

STATIC td_s32 hevc_get_short_term_pics_param(const hevc_ctx *ctx, const hevc_seq_param_set *sps,
    hevc_short_term_rpset *short_term_rpset, td_u32 idx)
{
    td_u32 ridx, code;
    td_s32 delta_rps;
    const hevc_short_term_rpset *temp_rpset = TD_NULL;

    if (idx == sps->num_short_term_ref_pic_sets) {
        code = hevc_ue_v(ctx->bs_ptr, "delta_idx_minus1");
        if (code > HEVC_MAX_UINT - 1) {
            dprint(PRN_ERROR, "delta_idx_minus1 overflow!\n");
            return HEVC_DEC_ERR;
        }
        short_term_rpset->delta_idx = code + 1;
    } else {
        code = 0;
        short_term_rpset->delta_idx = 0;
    }

    if (short_term_rpset->delta_idx > idx) {
        dprint(PRN_ERROR, "delta_idx(%d) > idx(%d).\n", short_term_rpset->delta_idx, idx);
        return HEVC_DEC_ERR;
    }

    ridx = idx - 1 - code;

    if (ridx > (idx - 1)) {
        dprint(PRN_ERROR, "rIdx(%d) > (idx-1)(%d) or < 0).\n", ridx, (idx - 1));
        return HEVC_DEC_ERR;
    }

    temp_rpset = &(sps->short_term_ref_pic_set[ridx]);

    short_term_rpset->delta_rps_sign = hevc_u_v(ctx->bs_ptr, 1, "delta_rps_sign");
    short_term_rpset->abs_delta_rps = hevc_ue_v(ctx->bs_ptr, "abs_delta_rps_minus1") + 1;
    delta_rps = (td_s32)((short_term_rpset->delta_rps_sign == 1) ? (-short_term_rpset->abs_delta_rps) :
        (short_term_rpset->abs_delta_rps));

    if (temp_rpset->num_of_pics > HEVC_MAX_NUM_REF_PICS) {
        dprint(PRN_ERROR, "temp_rpset->num_of_pics(%d) out of range(0,%d).\n", temp_rpset->num_of_pics,
            HEVC_MAX_NUM_REF_PICS);
        return HEVC_DEC_ERR;
    }

    hevc_calc_short_term_pics_num(ctx, short_term_rpset, temp_rpset, delta_rps);
    if (short_term_rpset->num_of_pics > HEVC_MAX_NUM_REF_PICS ||
        short_term_rpset->num_negative_pics > HEVC_MAX_NUM_REF_PICS ||
        short_term_rpset->num_positive_pics > HEVC_MAX_NUM_REF_PICS) {
        dprint(PRN_ERROR, "num_of_pics out of range(0,%d).\n", HEVC_MAX_NUM_REF_PICS);
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_sort_delta_poc(hevc_short_term_rpset *short_term_rpset)
{
    td_u8 used;
    td_s32 j, delta_poc, temp;
    td_u32 i, num_neg_pics;

    /* sortDeltaPOC: sort in increasing order (smallest first) */
    for (i = 1; i < short_term_rpset->num_of_pics; i++) {
        delta_poc = short_term_rpset->delta_poc[i];
        used = short_term_rpset->used_flag[i];

        for (j = (td_s32)(i - 1); j >= 0; j--) {
            temp = short_term_rpset->delta_poc[j];

            if (delta_poc < temp) {
                short_term_rpset->delta_poc[j + 1] = temp;
                short_term_rpset->used_flag[j + 1] = short_term_rpset->used_flag[j];
                short_term_rpset->delta_poc[j] = delta_poc;
                short_term_rpset->used_flag[j] = used;
            }
        }
    }

    /* flip the negative values to largest first */
    num_neg_pics = short_term_rpset->num_negative_pics;

    for (i = 0, j = (td_s32)(num_neg_pics - 1); i < (num_neg_pics >> 1); i++, j--) {
        if (i > HEVC_MAX_NUM_REF_PICS) {
            break;
        }

        delta_poc = short_term_rpset->delta_poc[i];
        used = short_term_rpset->used_flag[i];
        short_term_rpset->delta_poc[i] = short_term_rpset->delta_poc[j];
        short_term_rpset->used_flag[i] = short_term_rpset->used_flag[j];
        short_term_rpset->delta_poc[j] = delta_poc;
        short_term_rpset->used_flag[j] = used;
    }

    return;
}

STATIC td_s32 hevc_sort_delta_poc_s(const hevc_ctx *ctx, hevc_short_term_rpset *short_term_rpset)
{
    td_s32 delta_poc;
    td_u32 i, prev;
    td_u32 delta_poc_s0_minus1, used_by_curr_pic_s0_flag, delta_poc_s1_minus1, used_by_curr_pic_s1_flag;

    prev = 0;

    for (i = 0; i < short_term_rpset->num_negative_pics; i++) {
        delta_poc_s0_minus1 = hevc_ue_v(ctx->bs_ptr, "delta_poc_s0_minus1");
        if (delta_poc_s0_minus1 > 32767) { /* 32767 is vfmw_max num */
            dprint(PRN_ERROR, "delta_poc_s0_minus1(%d) out of range.\n", delta_poc_s0_minus1);
            return HEVC_DEC_ERR;
        }

        delta_poc = (td_s32)(prev - delta_poc_s0_minus1 - 1);
        prev = (td_u32)delta_poc;
        short_term_rpset->delta_poc[i] = delta_poc;
        used_by_curr_pic_s0_flag = hevc_u_v(ctx->bs_ptr, 1, "used_by_curr_pic_s0_flag");
        short_term_rpset->used_flag[i] = used_by_curr_pic_s0_flag;
    }

    prev = 0;

    for (i = short_term_rpset->num_negative_pics; i < short_term_rpset->num_of_pics; i++) {
        delta_poc_s1_minus1 = hevc_ue_v(ctx->bs_ptr, "delta_poc_s1_minus1");
        if (delta_poc_s1_minus1 > 32767) { /* 32767 is vfmw_max num */
            dprint(PRN_ERROR, "delta_poc_s1_minus1(%d) out of range.\n", delta_poc_s1_minus1);
            return HEVC_DEC_ERR;
        }

        delta_poc = (td_s32)(prev + delta_poc_s1_minus1 + 1);
        prev = (td_u32)delta_poc;
        short_term_rpset->delta_poc[i] = delta_poc;
        used_by_curr_pic_s1_flag = hevc_u_v(ctx->bs_ptr, 1, "used_by_curr_pic_s1_flag");
        short_term_rpset->used_flag[i] = used_by_curr_pic_s1_flag;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_parse_short_term_ref(const hevc_ctx *ctx, hevc_short_term_rpset *short_term_rpset)
{
    td_s32 ret;

    short_term_rpset->num_negative_pics = hevc_ue_v(ctx->bs_ptr, "num_negative_pics");

    if (short_term_rpset->num_negative_pics > HEVC_MAX_NUM_REF_PICS) {
        dprint(PRN_ERROR, "temp_rpset->num_negative_pics(%d) out of range(0,15).\n",
               short_term_rpset->num_negative_pics);
        return HEVC_DEC_ERR;
    }

    short_term_rpset->num_positive_pics = hevc_ue_v(ctx->bs_ptr, "num_positive_pics");

    if (short_term_rpset->num_positive_pics > HEVC_MAX_NUM_REF_PICS) {
        dprint(PRN_ERROR, "temp_rpset->num_positive_pics(%d) out of range(0,15).\n",
               short_term_rpset->num_negative_pics);
        return HEVC_DEC_ERR;
    }

    short_term_rpset->num_of_pics = short_term_rpset->num_negative_pics + short_term_rpset->num_positive_pics;

    if (short_term_rpset->num_of_pics > HEVC_MAX_NUM_REF_PICS) {
        dprint(PRN_ERROR, "short_term_rpset->num_of_pics(%d) out of range[0,%d].\n",
               short_term_rpset->num_of_pics, HEVC_MAX_NUM_REF_PICS);
        return HEVC_DEC_ERR;
    }

    ret = hevc_sort_delta_poc_s(ctx, short_term_rpset);

    return ret;
}

STATIC td_s32 hevc_dec_short_term_ref_pic_set(const hevc_ctx *ctx, const hevc_seq_param_set *sps,
                                       hevc_short_term_rpset *short_term_rpset, td_u32 idx)
{
    td_s32 ret;

    if (idx > 0) {
        short_term_rpset->inter_ref_pic_set_prediction_flag =
            hevc_u_v(ctx->bs_ptr, 1, "inter_ref_pic_set_prediction_flag");
    } else {
        short_term_rpset->inter_ref_pic_set_prediction_flag = HEVC_FALSE;
    }

    if (short_term_rpset->inter_ref_pic_set_prediction_flag) {
        ret = hevc_get_short_term_pics_param(ctx, sps, short_term_rpset, idx);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "%s, hevc_get_short_term_pics_param is err.\n", __func__);
            return HEVC_DEC_ERR;
        }

        hevc_sort_delta_poc(short_term_rpset);
    } else {
        ret = hevc_parse_short_term_ref(ctx, short_term_rpset);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "%s, hevc_parse_short_term_ref is err.\n", __func__);
            return HEVC_DEC_ERR;
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_ref_pic_lists_modification(const hevc_ctx *ctx, const hevc_slice_segment_header *slice,
    hevc_ref_pic_lists_moddification *rp_list_modification, td_s32 num_rps_curr_temp_list)
{
    td_u32 i, length, temp, num;

    if (ctx == TD_NULL || slice == TD_NULL || rp_list_modification == TD_NULL || ctx->bs_ptr == TD_NULL) {
        return HEVC_DEC_ERR;
    }

    temp = (td_u32)num_rps_curr_temp_list;
    length = 1;
    num_rps_curr_temp_list--;
    num =  num_rps_curr_temp_list > 0 ? (td_u32)num_rps_curr_temp_list : 0;
    while (num >>= 1) {
        length++;
    }

    rp_list_modification->ref_pic_list_modification_flag_l0 =
        hevc_u_v(ctx->bs_ptr, 1, "ref_pic_list_modification_flag_l0");

    if (rp_list_modification->ref_pic_list_modification_flag_l0 && (temp > 1)) {
        for (i = 0; i < slice->num_ref_idx[0]; i++) {
            rp_list_modification->list_entry_l0[i] = hevc_u_v(ctx->bs_ptr, length, "list_entry_l0");
        }
    }

    if (slice->slice_type == HEVC_B_SLICE) {
        rp_list_modification->ref_pic_list_modification_flag_l1 =
            hevc_u_v(ctx->bs_ptr, 1, "ref_pic_list_modification_flag_l1");

        if (rp_list_modification->ref_pic_list_modification_flag_l1 && (temp > 1)) {
            for (i = 0; i < slice->num_ref_idx[1]; i++) {
                rp_list_modification->list_entry_l1[i] = hevc_u_v(ctx->bs_ptr, length, "list_entry_l1");
            }
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_parse_chroma_weight_l0_flag(td_u32 i, const hevc_ctx *ctx, hevc_pred_weight_table *pwt)
{
    td_u32 j;
    td_s32 shift, pred;

    if (pwt->chroma_weight_l0_flag[i]) {
        for (j = 0; j < 2; j++) { /* 2 is vfmw_max num */
            pwt->delta_chroma_weight_l0[i][j] = hevc_se_v(ctx->bs_ptr, "delta_chroma_weight_l0");

            vfmw_check_ret(pwt->delta_chroma_offset_l0[i][j] <= 127, HEVC_DEC_ERR); /* 127 is vfmw_max num */
            vfmw_check_ret(pwt->delta_chroma_offset_l0[i][j] >= -128, HEVC_DEC_ERR); /* -128 is vfmw_min num */

            pwt->delta_chroma_offset_l0[i][j] = hevc_se_v(ctx->bs_ptr, "delta_chroma_offset_l0");

            vfmw_check_ret(pwt->delta_chroma_offset_l0[i][j] <= 511, HEVC_DEC_ERR); /* 511 is vfmw_max num */
            vfmw_check_ret(pwt->delta_chroma_offset_l0[i][j] >= -512, HEVC_DEC_ERR); /* -512 is vfmw_min num */

            pwt->chroma_weight_l0[i][j] = (1 << pwt->chroma_log2_weight_denom)
                                          + pwt->delta_chroma_weight_l0[i][j];
            /* HEVC 10Bit */
            shift = 128; /* 128 is bit num */
            pred = (shift - ((shift * pwt->chroma_weight_l0[i][j]) / (1 << pwt->chroma_log2_weight_denom)));
            pwt->chroma_offset_l0[i][j] =
                vfmw_clip_3(HEVC_SINT8_MIN_VAL, HEVC_SINT8_MAX_VAL, (pred + pwt->delta_chroma_offset_l0[i][j]));
        }
    } else {
        for (j = 0; j < 2; j++) { /* 2 is vfmw_max num */
            pwt->chroma_weight_l0[i][j] = (1 << pwt->chroma_log2_weight_denom);
            pwt->chroma_offset_l0[i][j] = 0;
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_parse_pred_weight_l0(const hevc_ctx *ctx, hevc_seq_param_set *sps,
    hevc_pred_weight_table *pwt)
{
    td_u32 i;
    td_s32 ret;

    ret = HEVC_DEC_NORMAL;

    for (i = 0; i < ctx->curr_slice.num_ref_idx[0]; i++) {
        pwt->luma_weight_l0_flag[i] = (td_s32)hevc_u_v(ctx->bs_ptr, 1, "luma_weight_l0_flag");
    }

    if (sps->chroma_format_idc != 0) {
        for (i = 0; i < ctx->curr_slice.num_ref_idx[0]; i++) {
            pwt->chroma_weight_l0_flag[i] = (td_s32)hevc_u_v(ctx->bs_ptr, 1, "chroma_weight_l0_flag");
        }
    }

    for (i = 0; i < ctx->curr_slice.num_ref_idx[0]; i++) {
        if (pwt->luma_weight_l0_flag[i]) {
            pwt->delta_luma_weight_l0[i] = hevc_se_v(ctx->bs_ptr, "delta_luma_weight_l0");

            if (pwt->delta_luma_weight_l0[i] > 127 || /* 127 is vfmw_max num */
                pwt->delta_luma_weight_l0[i] < -128) { /* -128 is vfmw_min num */
                dprint(PRN_ERROR, "delta_luma_weight_l0(%d) out of range(-128,127).\n", pwt->delta_luma_weight_l0[i]);
                return HEVC_DEC_ERR;
            }

            pwt->luma_weight_l0[i] = pwt->delta_luma_weight_l0[i] + (1 << pwt->luma_log2_weight_denom);
            pwt->luma_offset_l0[i] = hevc_se_v(ctx->bs_ptr, "luma_offset_l0");

            if (pwt->luma_offset_l0[i] > 127 || /* 127 is vfmw_max num */
                pwt->luma_offset_l0[i] < -128) { /* -128 is vfmw_min num */
                dprint(PRN_ERROR, "luma_offset_l0(%d) out of range(-128,127).\n", pwt->luma_offset_l0[i]);
                return HEVC_DEC_ERR;
            }
        } else {
            pwt->luma_weight_l0[i] = 1 << pwt->luma_log2_weight_denom;
            pwt->luma_offset_l0[i] = 0;
        }

        if (sps->chroma_format_idc == 0) {
            continue;
        }

        ret = hevc_parse_chroma_weight_l0_flag(i, ctx, pwt);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "%s: hevc_parse_chroma_weight_l0_flag is err.\n", __func__);
            return HEVC_DEC_ERR;
        }
    }

    return ret;
}

STATIC td_s32 hevc_parse_chroma_weight_l1_flag(td_u32 i, const hevc_ctx *ctx, hevc_pred_weight_table *pwt)
{
    td_u32 j;
    td_s32 shift, pred;

    if (pwt->chroma_weight_l1_flag[i]) {
        for (j = 0; j < 2; j++) { /* 2 is vfmw_max num */
            pwt->delta_chroma_weight_l1[i][j] = hevc_se_v(ctx->bs_ptr, "delta_chroma_weight_l1");

            vfmw_check_ret(pwt->delta_chroma_weight_l1[i][j] <= 127, HEVC_DEC_ERR); /* 127 is vfmw_max num */
            vfmw_check_ret(pwt->delta_chroma_weight_l1[i][j] >= -128, HEVC_DEC_ERR); /* 128 is vfmw_min num */

            pwt->delta_chroma_offset_l1[i][j] = hevc_se_v(ctx->bs_ptr, "delta_chroma_offset_l1");

            vfmw_check_ret(pwt->delta_chroma_offset_l1[i][j] <= 511, HEVC_DEC_ERR); /* 511 is vfmw_max num */
            vfmw_check_ret(pwt->delta_chroma_offset_l1[i][j] >= -512, HEVC_DEC_ERR); /* -512 is vfmw_min num */

            pwt->chroma_weight_l1[i][j] = (1 << pwt->chroma_log2_weight_denom)
                                          + pwt->delta_chroma_weight_l1[i][j];
            /* HEVC 10Bit */
            shift = 128; /* 128 is bit num */
            pred = shift - ((shift * pwt->chroma_weight_l1[i][j]) / (1 << (pwt->chroma_log2_weight_denom)));
            pwt->chroma_offset_l1[i][j] =
                vfmw_clip_3(HEVC_SINT8_MIN_VAL, HEVC_SINT8_MAX_VAL, (pred + pwt->delta_chroma_offset_l1[i][j]));
        }
    } else {
        for (j = 0; j < 2; j++) { /* 2 is vfmw_max num */
            pwt->chroma_weight_l1[i][j] = (1 << pwt->chroma_log2_weight_denom);
            pwt->chroma_offset_l1[i][j] = 0;
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_parse_pred_weight_l1(const hevc_ctx *ctx, const hevc_seq_param_set *sps,
    hevc_pred_weight_table *pwt)
{
    td_u32 i;
    td_s32 ret;

    ret = HEVC_DEC_NORMAL;

    for (i = 0; i < ctx->curr_slice.num_ref_idx[1]; i++) {
        pwt->luma_weight_l1_flag[i] = (td_s32)hevc_u_v(ctx->bs_ptr, 1, "luma_weight_l1_flag");
    }

    if (sps->chroma_format_idc != 0) {
        for (i = 0; i < ctx->curr_slice.num_ref_idx[1]; i++) {
            pwt->chroma_weight_l1_flag[i] = (td_s32)hevc_u_v(ctx->bs_ptr, 1, "chroma_weight_l1_flag");
        }
    }

    for (i = 0; i < ctx->curr_slice.num_ref_idx[1]; i++) {
        if (pwt->luma_weight_l1_flag[i]) {
            pwt->delta_luma_weight_l1[i] = hevc_se_v(ctx->bs_ptr, "delta_luma_weight_l1");

            if (pwt->delta_luma_weight_l1[i] > 127 || /* 127 is vfmw_max num */
                pwt->delta_luma_weight_l1[i] < -128) { /* -128 is vfmw_min num */
                dprint(PRN_ERROR, "delta_luma_weight_l1 out of range(-128,127).\n");
                return HEVC_DEC_ERR;
            }

            pwt->luma_weight_l1[i] = pwt->delta_luma_weight_l1[i] + (1 << pwt->luma_log2_weight_denom);
            pwt->luma_offset_l1[i] = hevc_se_v(ctx->bs_ptr, "luma_offset_l1");

            if (pwt->luma_offset_l1[i] > 127 || /* 127 is vfmw_max num */
                pwt->luma_offset_l1[i] < -128) { /* -128 is vfmw_min num */
                dprint(PRN_ERROR, "luma_offset_l1 out of range(-128,127).\n");
                return HEVC_DEC_ERR;
            }
        } else {
            pwt->luma_weight_l1[i] = 1 << pwt->luma_log2_weight_denom;
            pwt->luma_offset_l1[i] = 0;
        }

        if (sps->chroma_format_idc == 0) {
            continue;
        }

        ret = hevc_parse_chroma_weight_l1_flag(i, ctx, pwt);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "%s: hevc_parse_chroma_weight_l1_flag is err.\n", __func__);
            return HEVC_DEC_ERR;
        }
    }

    return ret;
}

STATIC td_s32 hevc_dec_pred_weight_table(hevc_ctx *ctx)
{
    td_s32 ret;
    hevc_seq_param_set *sps = TD_NULL;
    hevc_pic_param_set *pps = TD_NULL;
    hevc_pred_weight_table *pwt = TD_NULL;

    pwt = &(ctx->curr_slice.pred_weight_table);
    pps = ctx->pps[ctx->curr_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];

    if (((ctx->curr_slice.slice_type == HEVC_P_SLICE) && (pps->weighted_pred_flag)) ||
        ((ctx->curr_slice.slice_type == HEVC_B_SLICE) && (pps->weighted_bipred_flag))) {
        pwt->luma_log2_weight_denom = hevc_ue_v(ctx->bs_ptr, "luma_log2_weight_denom");
        if (pwt->luma_log2_weight_denom > 7) { /* 7 is vfmw_max num */
            dprint(PRN_ERROR, "luma_log2_weight_denom out of range(0,7).\n");
            return HEVC_DEC_ERR;
        }

        if (sps->chroma_format_idc != 0) {
            pwt->delta_chroma_log2_weight_denom = hevc_se_v(ctx->bs_ptr, "delta_chroma_log2_weight_denom");

            if (pwt->delta_chroma_log2_weight_denom > 7 || /* 7 is vfmw_max num */
                pwt->delta_chroma_log2_weight_denom < -7) { /* -7 is vfmw_min num */
                dprint(PRN_ERROR, "delta_chroma_log2_weight_denom out of range(-7,7).\n");
                return HEVC_DEC_ERR;
            }
        }

        pwt->chroma_log2_weight_denom = pwt->luma_log2_weight_denom + (td_u32)pwt->delta_chroma_log2_weight_denom;

        if (pwt->chroma_log2_weight_denom > 7) { /* 7 is vfmw_max num */
            dprint(PRN_ERROR, "chroma_log2_weight_denom out of range(0,7).\n");
            return HEVC_DEC_ERR;
        }

        ret = hevc_parse_pred_weight_l0(ctx, sps, pwt);
        if (ret != HEVC_DEC_NORMAL) {
            return ret;
        }

        if (ctx->curr_slice.slice_type == HEVC_B_SLICE) {
            ret = hevc_parse_pred_weight_l1(ctx, sps, pwt);
            if (ret != HEVC_DEC_NORMAL) {
                return ret;
            }
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_dec_vui_param_part1(const hevc_ctx *ctx, hevc_vui_param_set *vui)
{
    vui->aspect_ratio_info_present_flag = hevc_u_v(ctx->bs_ptr, 1, "aspect_ratio_info_present_flag");

    if (vui->aspect_ratio_info_present_flag) {
        vui->aspect_ratio_idc = hevc_u_v(ctx->bs_ptr, 8, "aspect_ratio_idc"); /* 8 is bit num */

        if (vui->aspect_ratio_idc > 255) { /* 255 is vfmw_max num */
            dprint(PRN_ERROR, "VUI aspect_ratio_idc out of range[0, 255]\n");
            vui->aspect_ratio_idc = 0;
        }

        if (vui->aspect_ratio_idc == 255) { /* 255 is vfmw_max num */
            vui->sar_width = hevc_u_v(ctx->bs_ptr, 16, "sar_width"); /* 16 is bit num */
            vui->sar_height = hevc_u_v(ctx->bs_ptr, 16, "sar_height"); /* 16 is bit num */
        }
    }

    vui->overscan_info_present_flag = hevc_u_v(ctx->bs_ptr, 1, "overscan_info_present_flag");

    if (vui->overscan_info_present_flag) {
        vui->overscan_appropriate_flag = hevc_u_v(ctx->bs_ptr, 1, "overscan_appropriate_flag");
    }

    vui->video_signal_type_present_flag = hevc_u_v(ctx->bs_ptr, 1, "video_signal_type_present_flag");

    if (vui->video_signal_type_present_flag) {
        vui->video_format = hevc_u_v(ctx->bs_ptr, 3, "video_format"); /* 3 is bit num */
        vui->video_full_range_flag = hevc_u_v(ctx->bs_ptr, 1, "video_full_range_flag");
        vui->colour_description_present_flag = hevc_u_v(ctx->bs_ptr, 1, "colour_description_present_flag");

        if (vui->colour_description_present_flag) {
            vui->colour_primaries = hevc_u_v(ctx->bs_ptr, 8, "colour_primaries"); /* 8 is bit num */
            vui->transfer_characteristics = hevc_u_v(ctx->bs_ptr, 8, "transfer_characteristics"); /* 8 is bit num */
            vui->matrix_coeffs = hevc_u_v(ctx->bs_ptr, 8, "matrix_coeffs"); /* 8 is bit num */

            report_hdr_info(ctx->chan_id, vui->video_full_range_flag, vui->colour_primaries,
                            vui->matrix_coeffs, vui->transfer_characteristics);
        }
    }

    vui->chroma_loc_info_present_flag = hevc_u_v(ctx->bs_ptr, 1, "chroma_loc_info_present_flag");

    if (vui->chroma_loc_info_present_flag) {
        vui->chroma_sample_loc_type_top_field = hevc_ue_v(ctx->bs_ptr, "chroma_sample_loc_type_top_field");
        vui->chroma_sample_loc_type_bottom_field = hevc_ue_v(ctx->bs_ptr, "chroma_sample_loc_type_bottom_field");
    }

    vui->neutral_chroma_indication_flag = hevc_u_v(ctx->bs_ptr, 1, "neutral_chroma_indication_flag");

    vui->field_seq_flag = hevc_u_v(ctx->bs_ptr, 1, "field_seq_flag");

    vui->frame_field_info_present_flag = hevc_u_v(ctx->bs_ptr, 1, "frame_field_info_present_flag");

    vui->default_display_window_flag = hevc_u_v(ctx->bs_ptr, 1, "default_display_window_flag");

    return;
}

STATIC td_void hevc_dec_vui_param_frame_rate(hevc_ctx *ctx, const hevc_vui_param_set *vui)
{
    td_u64 result;

    if (vui->vui_num_units_in_tick == 0) {
        return;
    }

    result = vui->vui_time_scale;
    result *= 1000 * 10; /* 1000 10 is times */

    ctx->frame_rate = (td_u32)os_do_div(result, vui->vui_num_units_in_tick);
    ctx->frame_rate = (ctx->frame_rate + 5) / 10; /* 5 10 is num */
}

STATIC td_s32 hevc_dec_vui_param_def_disp_proc(hevc_ctx *ctx, hevc_vui_param_set *vui, const hevc_seq_param_set *sps)
{
    const td_u32 m_win_unit_x[] = { 1, 2, 2, 1 };
    const td_u32 m_win_unit_y[] = { 1, 2, 1, 1 };

    if (vui->default_display_window_flag) {
        vui->def_disp_win_left_offset = hevc_ue_v(ctx->bs_ptr, "def_disp_win_left_offset");

        if (sps->chroma_format_idc > 3) { /* 3 is vfmw_max num */
            dprint(PRN_ERROR, "VUI sps->chroma_format_idc out of range(0,3).\n");
            return HEVC_DEC_ERR;
        }

        vfmw_check_ret(vui->def_disp_win_left_offset <= HEVC_MAX_PIX_WIDTH, HEVC_DEC_ERR);
        vui->def_disp_win_left_offset *= m_win_unit_x[sps->chroma_format_idc];
        vui->def_disp_win_right_offset = hevc_ue_v(ctx->bs_ptr, "def_disp_win_right_offset");
        vfmw_check_ret(vui->def_disp_win_right_offset <= HEVC_MAX_PIX_WIDTH, HEVC_DEC_ERR);
        vui->def_disp_win_right_offset *= m_win_unit_x[sps->chroma_format_idc];
        vui->def_disp_win_top_offset = hevc_ue_v(ctx->bs_ptr, "def_disp_win_top_offset");
        vfmw_check_ret(vui->def_disp_win_top_offset <= HEVC_MAX_PIX_HEIGHT, HEVC_DEC_ERR);
        vui->def_disp_win_top_offset *= m_win_unit_y[sps->chroma_format_idc];
        vui->def_disp_win_bottom_offset = hevc_ue_v(ctx->bs_ptr, "def_disp_win_bottom_offset");
        vfmw_check_ret(vui->def_disp_win_bottom_offset <= HEVC_MAX_PIX_HEIGHT, HEVC_DEC_ERR);
        vui->def_disp_win_bottom_offset *= m_win_unit_y[sps->chroma_format_idc];
    }
    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_dec_vui_param_part2(hevc_ctx *ctx, hevc_vui_param_set *vui, const hevc_seq_param_set *sps)
{
    td_s32 ret;

    if (hevc_dec_vui_param_def_disp_proc(ctx, vui, sps) != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "VUI hevc_dec_vui_param_def_disp_proc error.\n");
        return HEVC_DEC_ERR;
    }
    vui->vui_timing_info_present_flag = hevc_u_v(ctx->bs_ptr, 1, "vui_timing_info_present_flag");
    ctx->frame_rate = 0xffffffff;
    if (vui->vui_timing_info_present_flag) {
        vui->vui_num_units_in_tick = hevc_u_v(ctx->bs_ptr, 32, "vui_num_units_in_tick"); /* 32 is bit num */
        vui->vui_time_scale = hevc_u_v(ctx->bs_ptr, 32, "vui_time_scale"); /* 32 is bit num */
        vui->vui_poc_proportional_to_timing_flag = hevc_u_v(ctx->bs_ptr, 1, "vui_poc_proportional_to_timing_flag");

        hevc_dec_vui_param_frame_rate(ctx, vui);

        if (vui->vui_poc_proportional_to_timing_flag) {
            vui->vui_num_ticks_poc_diff_one_minus1 = hevc_ue_v(ctx->bs_ptr, "vui_num_ticks_poc_diff_one_minus1");
        }
        vui->vui_hrd_parameters_present_flag = hevc_u_v(ctx->bs_ptr, 1, "vui_hrd_parameters_present_flag");
        if (vui->vui_hrd_parameters_present_flag) {
            ret = hevc_dec_hrd_param(ctx, &(vui->hrd_parameters), 1, sps->sps_max_sub_layers_minus1);
            if (ret != HEVC_DEC_NORMAL) {
                dprint(PRN_ERROR, "VUI hevc_dec_hrd_param error.\n");
                return HEVC_DEC_ERR;
            }
        }
    } else {
        hevc_video_param_set *vps = TD_NULL;
        vps = ctx->vps[sps->video_parameter_set_id];
        if (vps != TD_NULL && vps->vps_timing_info_present_flag) {
            vui->vui_num_units_in_tick = vps->vps_num_units_in_tick;
            vui->vui_time_scale = vps->vps_time_scale;
            hevc_dec_vui_param_frame_rate(ctx, vui);
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_dec_vui_param_part3(const hevc_ctx *ctx, hevc_vui_param_set *vui)
{
    vui->bitstream_restriction_flag = hevc_u_v(ctx->bs_ptr, 1, "bitstream_restriction_flag");

    if (vui->bitstream_restriction_flag) {
        vui->tiles_fixed_structure_flag = hevc_u_v(ctx->bs_ptr, 1, "tiles_fixed_structure_flag");
        vui->motion_vectors_over_pic_boundaries_flag =
            hevc_u_v(ctx->bs_ptr, 1, "motion_vectors_over_pic_boundaries_flag");
        vui->restricted_ref_pic_lists_flag = hevc_u_v(ctx->bs_ptr, 1, "restricted_ref_pic_lists_flag");
        vui->min_spatial_segmentation_idc = hevc_ue_v(ctx->bs_ptr, "min_spatial_segmentation_idc");

        if (vui->min_spatial_segmentation_idc >= 4096) { /* 4096 is vfmw_max num */
            dprint(PRN_ERROR, "VUI min_spatial_segmentation_idc out of range(0,4095).\n");
        }

        vui->max_bytes_per_pic_denom = hevc_ue_v(ctx->bs_ptr, "max_bytes_per_pic_denom");
        vui->max_bits_per_min_cu_denom = hevc_ue_v(ctx->bs_ptr, "max_bits_per_min_cu_denom");
        vui->log2_max_mv_length_horizontal = hevc_ue_v(ctx->bs_ptr, "log2_max_mv_length_horizontal");
        vui->log2_max_mv_length_vertical = hevc_ue_v(ctx->bs_ptr, "log2_max_mv_length_vertical");
    }
    return;
}

STATIC td_s32 hevc_dec_vui_param(hevc_ctx *ctx, hevc_vui_param_set *vui, const hevc_seq_param_set *sps)
{
    td_s32 ret;

    hevc_dec_vui_param_part1(ctx, vui);

    ret = hevc_dec_vui_param_part2(ctx, vui, sps);
    if (ret != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "VUI hevc_dec_vui_param_part2 error.\n");
        return HEVC_DEC_ERR;
    }

    hevc_dec_vui_param_part3(ctx, vui);

    return ret;
}

STATIC td_void hevc_dec_nal_hrd_param(const hevc_ctx *ctx, hevc_hrd_param_set *hrd, td_u32 i, td_u32 j)
{
    td_u32 k;

    if (((j == 0) && (hrd->nal_hrd_parameters_present_flag))
        ||  ((j == 1) && (hrd->vcl_hrd_parameters_present_flag))) {
        for (k = 0; k <= hrd->hrd_sublayer[i].cpb_cnt_minus1; k++) {
            hrd->hrd_sublayer[i].bit_rate_value_minus1[k][j] = hevc_ue_v(ctx->bs_ptr, "bit_rate_value_minus1");
            hrd->hrd_sublayer[i].cpb_size_value_minus1[k][j] = hevc_ue_v(ctx->bs_ptr, "cpb_size_value_minus1");

            if (hrd->sub_pic_hrd_params_present_flag) {
                hrd->hrd_sublayer[i].cpb_size_du_value_minus1[k][j] =
                    hevc_ue_v(ctx->bs_ptr, "cpb_size_du_value_minus1");
                hrd->hrd_sublayer[i].bit_rate_du_value_minus1[k][j] =
                    hevc_ue_v(ctx->bs_ptr, "bit_rate_du_value_minus1");
            }

            hrd->hrd_sublayer[i].cbr_flag[k][j] = hevc_u_v(ctx->bs_ptr, 1, "cbr_flag");
        }
    }
}

STATIC td_void hevc_dec_hrd_sublayer_param(const hevc_ctx *ctx, hevc_hrd_param_set *hrd,
    td_u32 max_num_sub_layers_minus1)
{
    td_u32 i, j;

    for (i = 0; i <= max_num_sub_layers_minus1; i++) {
        hrd->hrd_sublayer[i].fixed_pic_rate_general_flag = hevc_u_v(ctx->bs_ptr, 1, "fixed_pic_rate_general_flag");

        if (!(hrd->hrd_sublayer[i].fixed_pic_rate_general_flag)) {
            hrd->hrd_sublayer[i].fixed_pic_rate_within_cvs_flag =
                hevc_u_v(ctx->bs_ptr, 1, "fixed_pic_rate_within_cvs_flag");
        } else {
            hrd->hrd_sublayer[i].fixed_pic_rate_within_cvs_flag = HEVC_TRUE;
        }

        hrd->hrd_sublayer[i].low_delay_hrd_flag = 0;
        hrd->hrd_sublayer[i].cpb_cnt_minus1 = 0;

        if (hrd->hrd_sublayer[i].fixed_pic_rate_within_cvs_flag) {
            hrd->hrd_sublayer[i].elemental_duration_in_tc_minus1 =
                hevc_ue_v(ctx->bs_ptr, "elemental_duration_in_tc_minus1");
        } else {
            hrd->hrd_sublayer[i].low_delay_hrd_flag = hevc_u_v(ctx->bs_ptr, 1, "low_delay_hrd_flag");
        }

        if (!(hrd->hrd_sublayer[i].low_delay_hrd_flag)) {
            hrd->hrd_sublayer[i].cpb_cnt_minus1 = hevc_ue_v(ctx->bs_ptr, "cpb_cnt_minus1");
        }

        if (hrd->hrd_sublayer[i].cpb_cnt_minus1 > 31) { /* 31 is vfmw_max num */
            dprint(PRN_ERROR, "cpb_cnt_minus1(%d) out of range(0,31).\n", hrd->hrd_sublayer[i].cpb_cnt_minus1);
            hrd->hrd_sublayer[i].cpb_cnt_minus1 = 0; /* 赋值强解 */
        }

        for (j = 0; j < 2; j++) { /* 2 is vfmw_max num */
            hevc_dec_nal_hrd_param(ctx, hrd, i, j);
        }
    }

    return;
}

STATIC td_s32 hevc_dec_hrd_param(const hevc_ctx *ctx, hevc_hrd_param_set *hrd,
                          td_u8 common_inf_present_flag, td_u32 max_num_sub_layers_minus1)
{
    if (ctx == TD_NULL || hrd == TD_NULL || ctx->bs_ptr == TD_NULL) {
        return HEVC_DEC_ERR;
    }

    if (common_inf_present_flag) {
        hrd->nal_hrd_parameters_present_flag = hevc_u_v(ctx->bs_ptr, 1, "nal_hrd_parameters_present_flag");
        hrd->vcl_hrd_parameters_present_flag = hevc_u_v(ctx->bs_ptr, 1, "vcl_hrd_parameters_present_flag");

        if (hrd->nal_hrd_parameters_present_flag || hrd->vcl_hrd_parameters_present_flag) {
            hrd->sub_pic_hrd_params_present_flag = hevc_u_v(ctx->bs_ptr, 1, "sub_pic_cpb_params_present_flag");

            if (hrd->sub_pic_hrd_params_present_flag) {
                hrd->tick_divisor_minus2 = hevc_u_v(ctx->bs_ptr, 8, "tick_divisor_minus2"); /* 8 is bit num */
                hrd->du_cpb_removal_delay_increment_length_minus1 =
                    hevc_u_v(ctx->bs_ptr, 5, "du_cpb_removal_delay_increment_length_minus1"); /* 5 is bit num */
                hrd->sub_pic_cpb_params_in_pic_timing_sei_flag =
                    hevc_u_v(ctx->bs_ptr, 1, "sub_pic_cpb_params_in_pic_timing_sei_flag");
                hrd->dpb_output_delay_du_length_minus1 =
                    hevc_u_v(ctx->bs_ptr, 5, "dpb_output_delay_du_length_minus1"); /* 5 is bit num */
            }

            hrd->bit_rate_scale = hevc_u_v(ctx->bs_ptr, 4, "bit_rate_scale"); /* 4 is bit num */
            hrd->cpb_size_scale = hevc_u_v(ctx->bs_ptr, 4, "cpb_size_scale"); /* 4 is bit num */

            if (hrd->sub_pic_hrd_params_present_flag) {
                hrd->cpb_size_du_scale = hevc_u_v(ctx->bs_ptr, 4, "cpb_size_du_scale"); /* 4 is bit num */
            }

            hrd->initial_cpb_removal_delay_length_minus1 =
                hevc_u_v(ctx->bs_ptr, 5, "initial_cpb_removal_delay_length_minus1"); /* 5 is bit num */
            hrd->au_cpb_removal_delay_length_minus1 =
                hevc_u_v(ctx->bs_ptr, 5, "au_cpb_removal_delay_length_minus1"); /* 5 is bit num */
            hrd->dpb_output_delay_length_minus1 =
                hevc_u_v(ctx->bs_ptr, 5, "dpb_output_delay_length_minus1"); /* 5 is bit num */
        }
    }

    hevc_dec_hrd_sublayer_param(ctx, hrd, max_num_sub_layers_minus1);

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_more_rbsp_data(const hevc_ctx *ctx)
{
    td_s32 ret;
    td_s32 bits_left;

    bits_left = bs_residual_bits(ctx->bs_ptr);
    if (bits_left > 0 && (td_u32)bits_left > (ctx->curr_nal->nal_trail_zero_bit_num + 3)) { /* 3 is bit num */
        ret = HEVC_TRUE;
    } else {
        ret = HEVC_FALSE;
    }

    return ret;
}

STATIC td_s32 hevc_sei_more_rbsp_data(const hevc_ctx *ctx)
{
    if (ctx->bs_ptr->bs_len * 8 <= bs_pos(ctx->bs_ptr)) { /* 8 is bit num */
        dprint(PRN_ERROR, "Error, bs_len: 0x%x, total_pos: 0x%x\n",
               ctx->bs_ptr->bs_len * 8, bs_pos(ctx->bs_ptr)); /* 8 is bit num */
        return HEVC_FALSE;
    } else {
        if (0x80 == bs_show(ctx->bs_ptr, 8)) { /* 8 is bit num */
            return HEVC_FALSE;
        } else {
            return HEVC_TRUE;
        }
    }
}

/*************************************************************************
*   函数原型 :  td_void hevc_release_nal(td_s32 chan_id, hevc_nalu *p_nall)
*   功能描述 :  释放一个nal
*   参数说明 :
*   返回值   ：
*************************************************************************/
STATIC td_void hevc_release_nal(td_s32 chan_id, hevc_nalu *p_nal)
{
    td_u32 i;

    vfmw_check(p_nal != TD_NULL);

    p_nal->is_valid = 0;
    p_nal->nal_integ = 0;

    /* replace i < 2 as i < nal_segment to avoid */
    /* release twice while first slice is two segment */
    for (i = 0; i < p_nal->nal_segment; i++) {
        if (p_nal->stream[i].stream_buffer != TD_NULL) {
            stm_release_seg(chan_id, p_nal->stream[i].stream_id);
            p_nal->stream[i].stream_buffer = TD_NULL;
            p_nal->stream[i].bits_offset = 0;
            p_nal->stream[i].bitstream_length = 0;
        }
    }

    vfmw_check_sec_func(memset_s(p_nal, sizeof(hevc_nalu), 0, sizeof(hevc_nalu)));

    return;
}

/*************************************************************************
*      函数原型 :  td_void hevc_combine_packet()
*      功能描述 :  拼包功能函数
*      参数说明 :  无
*      返回值   ： td_void
*************************************************************************/
STATIC td_void hevc_combine_packet(hevc_ctx *ctx)
{
    td_s32 stream_id;

    if (ctx->curr_nal == TD_NULL) {
        dprint(PRN_ERROR, "hevc_combine_packet FATAL: curr_nal=TD_NULL!\n");
        return;
    }

    /* 表示肯定分包了，也就是说肯定Buf触底了 */
    if (ctx->curr_nal->stream[0].bitstream_length < HEVC_GET_ONE_NALU_SIZE) { /* 需要copy本包到buf的头部 */
        /* copy到最前面 */
        vfmw_check_sec_func(memcpy_s(ctx->curr_nal->stream[1].stream_buffer - ctx->curr_nal->stream[0].bitstream_length,
            HEVC_GET_ONE_NALU_SIZE, ctx->curr_nal->stream[0].stream_buffer, ctx->curr_nal->stream[0].bitstream_length));

        stream_id = ctx->curr_nal->stream[0].stream_id;
        ctx->curr_nal->stream[0].stream_id = ctx->curr_nal->stream[1].stream_id;
        ctx->curr_nal->stream[0].bits_offset = 0;
        ctx->curr_nal->stream[0].stream_buffer =
            ctx->curr_nal->stream[1].stream_buffer - ctx->curr_nal->stream[0].bitstream_length;
        ctx->curr_nal->stream[0].stream_phy_addr =
            ctx->curr_nal->stream[1].stream_phy_addr - ctx->curr_nal->stream[0].bitstream_length;
        /* include nal header */
        ctx->curr_nal->stream[0].bitstream_length =
            ctx->curr_nal->stream[0].bitstream_length + ctx->curr_nal->stream[1].bitstream_length;
        ctx->curr_nal->nal_segment = 1;

        /* 完成拼包 , 将第一个包拷贝到第2个包的位置,同时  释放原来第一个包,原来这里没有释放,造成码流泄露 */
        stm_release_seg(ctx->chan_id, stream_id);
        ctx->curr_nal->stream[1].stream_buffer = TD_NULL;
        ctx->curr_nal->stream[1].bits_offset = 0;
        ctx->curr_nal->stream[1].bitstream_length = 0;
    }

    return;
}

/*************************************************************************
*      函数原型 :  td_s32 hevc_receive_packet(hevc_stream_packet *packet)
*      功能描述 :  获取一片码流，并判断该片码流是否是完整的nal
*      参数说明 :  无
*      返回值   ： 1:一个完整的Nalu;0不是一个完整的Nalu;DEC_ERR:解码错误
*************************************************************************/
/* 错误处理可以提出来做,但有两种错误处理需要处理,所以暂时先放在这里 */
STATIC td_void hevc_calc_curr_pic_stream_len(hevc_ctx *ctx, const hevc_stream_packet *packet)
{
    if (ctx->extra_data_ptr->stop_record != 1) {
        ctx->curr_pic.frame_stream_size += packet->stream_len;
        ctx->curr_pic.current_nal_size += packet->stream_len;
    }

    return;
}

STATIC td_s32 hevc_receive_first_packet(hevc_ctx *ctx, const hevc_stream_packet *packet)
{
    if ((packet->stream_len > 0) && (packet->p_stream != TD_NULL) && (packet->strm_phy_addr != 0)) {
        ctx->curr_nal = &ctx->nal_array[0];
        ctx->curr_nal->stream[0].stream_buffer = packet->p_stream;
        ctx->curr_nal->stream[0].bitstream_length = packet->stream_len; /* include nal header */
        ctx->curr_nal->stream[0].stream_id = packet->stream_id;
        ctx->curr_nal->stream[0].stream_phy_addr = packet->strm_phy_addr;
        ctx->curr_nal->stream[0].bits_offset = 0;
        ctx->curr_nal->nal_segment = 1;
        ctx->curr_nal->nal_used_segment = 0;
        ctx->curr_nal->nal_integ = packet->last_pack_in_nal;
        ctx->curr_nal->raw_stream_len = packet->raw_stream_len;
        ctx->seg_cut_state = packet->seg_cut_state;

        hevc_calc_curr_pic_stream_len(ctx, packet);
    } else {
        dprint(PRN_ERROR, "receive a zero packet\n");
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_receive_second_packet(hevc_ctx *ctx, const hevc_stream_packet *packet)
{
    ctx->curr_nal->nal_integ = packet->last_pack_in_nal;

    if ((ctx->extra_data_ptr->dynamic_fs_en != 1) ||
        ((ctx->extra_data_ptr->dynamic_fs_en == 1) &&
         (ctx->extra_data_ptr->wait_fs_flag == 0))) {
        if ((packet->last_pack_in_nal != 1) ||
            ((ctx->curr_nal->nal_segment >= 2) && /* 2 is nal num */
             (ctx->curr_nal->nal_integ == 1))) {
            dprint(PRN_ERROR, "nal_release_err\n");
            return HEVC_DEC_ERR;
        }

        if ((packet->stream_len > 0) && (packet->p_stream != TD_NULL) && (packet->strm_phy_addr != 0)) {
            ctx->curr_nal->stream[ctx->curr_nal->nal_segment].stream_buffer = packet->p_stream;
            ctx->curr_nal->stream[ctx->curr_nal->nal_segment].bitstream_length = packet->stream_len;
            ctx->curr_nal->stream[ctx->curr_nal->nal_segment].stream_phy_addr = packet->strm_phy_addr;
            ctx->curr_nal->stream[ctx->curr_nal->nal_segment].stream_id = packet->stream_id;
            ctx->curr_nal->stream[ctx->curr_nal->nal_segment].bits_offset = 0;
            ctx->curr_nal->nal_segment++;

            hevc_calc_curr_pic_stream_len(ctx, packet);

            /* 拼包功能函数 */
            hevc_combine_packet(ctx);
            ctx->scd_wrap_around = 1;
        } else {
            /* 第二包长度为0或码流地址为空,那么就只解第一包数据 */
            ctx->curr_nal->nal_integ = 1;
            stm_release_seg(ctx->chan_id, packet->stream_id);
        }
    }
    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_receive_packet(hevc_ctx *ctx, const hevc_stream_packet *packet)
{
    td_s32 ret;

    ctx->scd_wrap_around = 0;

    /* 第一次收到包 */
    if (ctx->curr_nal == TD_NULL) {
        /* 收到非零的包 */
        ret = hevc_receive_first_packet(ctx, packet);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "hevc_receive_first_packet is err\n");
            return HEVC_DEC_ERR;
        }
    } else {
        /* 收到第二包数据 */
        ret = hevc_receive_second_packet(ctx, packet);
        if (ret != HEVC_DEC_NORMAL) {
            dprint(PRN_ERROR, "hevc_receive_second_packet is err\n");
            return HEVC_DEC_ERR;
        }
    }

    ret = ctx->curr_nal->nal_integ ? HEVC_DEC_NORMAL : HEVC_DEC_ERR;

    return ret;
}

STATIC td_void hevc_updata_img_qp(hevc_ctx *ctx)
{
    vfmw_image *image = TD_NULL;

    image = fsp_get_image(ctx->chan_id, ctx->curr_pic.frame_store->logic_fs_id);
    vfmw_check(image != TD_NULL);
    vfmw_check(ctx->curr_pic.qp_slice_num != 0);
    image->pic_qp = ctx->curr_pic.pic_qp / ctx->curr_pic.qp_slice_num;
#if defined(VFMW_HEVC_SLC_LDY_SUPPORT)
    if (ctx->extra_data_ptr->slc_ldy_en) {
        image->pic_qp = 0;
    }
#endif

    return;
}

/***********************************************************************************
* Function:    td_u32 hevc_dec_post_proc( hevc_ctx *ctx, td_s32 ErrRatio, td_s32 CalledByISR, LUMA_INFO_S *pLumaInfo )
* Description: 解码中断后处理函数
* Input:       无
* Output:      无
* Return:      DEC_NORMAL   可以解码此Nalu数据
*              DEC_ERR      不可以解码
* Others:      无
***********************************************************************************/
STATIC td_s32 hevc_dec_post_proc(hevc_ctx *ctx, td_bool is_slc)
{
    td_s32 ret, logic_id;

    logic_id = ctx->curr_pic.frame_store->logic_fs_id; /* VFMW_DEC_MODE_I will memset curr_pic */

#if defined(VFMW_HEVC_SLC_LDY_SUPPORT)
    if (ctx->extra_data_ptr->crc_mode == 0) {
        if ((ctx->post_pic_count < 2) || /* 2 is pic cnt */
            (ctx->curr_pic.pic_width <= 3840) || /* 3840 is width */
            (ctx->extra_data_ptr->dec_mode == VFMW_DEC_MODE_I)) {
            ctx->half_nal_en = ctx->extra_data_ptr->sed_only_en;
            ctx->half_nal_en |= (td_u32)ctx->extra_data_ptr->slc_ldy_en;
        } else {
            ctx->half_nal_en = 0;
        }

        if (ctx->post_pic_count < 2) { /* 2 is pic cnt */
            ctx->post_pic_count++;
        }
    }
#endif

    ctx->curr_pic.err_level = 0;


    if (is_slc == TD_FALSE) {
        hevc_updata_img_qp(ctx);
        hevc_clear_all_slice(ctx);
    }

    ret = hevc_store_pic_in_dpb(ctx, is_slc);
    if (ret != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "hevc_store_pic_in_dpb failed, ret = %d\n", ret);
        voq_delete(ctx->chan_id, &ctx->curr_pic.fs_image);
        fsp_set_disp(ctx->chan_id, logic_id, 0);
        hevc_clear_curr_pic(ctx);
        dps_release_seg(ctx->chan_id, ctx->curr_dps);
        hevc_clear_dpb(ctx);
        return HEVC_DEC_ERR;
    }

    ctx->allow_start_dec = 1;

    ctx->general_profile_idc = (td_u32)ctx->curr_sps->profile_tier_level.general_profile_idc;
    ctx->general_level_idc = (td_u32)ctx->curr_sps->profile_tier_level.general_level_idc;

#ifdef VFMW_HEVC_SLC_LDY_SUPPORT
    if (ctx->curr_dps->sed_state == SED_SUCC) {
        return HEVC_DEC_NORMAL;
    }
#endif

    fsp_set_wait(ctx->chan_id, logic_id, 0);

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_is_out_dpb(const hevc_ctx *ctx, const hevc_frame_store *frame_store)
{
    td_u32 i;

    if (frame_store == TD_NULL) {
        return 1;
    }

    for (i = 0; i < ctx->dpb.used_size; i++) {
        if (ctx->dpb.fs[i] == frame_store) {
            return 0;
        }
    }

    return 1;
}

STATIC td_s32 hevc_clear_all(hevc_ctx *ctx, td_s32 flag)
{
    hevc_clear_curr_pic(ctx);

    if (hevc_init_dpb(ctx, 0) != HEVC_DEC_NORMAL) {
        return HEVC_DEC_ERR;
    }

    if (flag) {
        fsp_clear_idle(ctx->chan_id);
    } else {
        fsp_empty(ctx->chan_id);
    }

    hevc_init_dec_para(ctx);

    return HEVC_DEC_NORMAL;
}

#if defined(VFMW_HEVC_SLC_LDY_SUPPORT)
STATIC td_void hevc_init_ldy_info(hevc_ctx *ctx)
{
    vfmw_check(ctx != TD_NULL);
    vfmw_check(ctx->extra_data_ptr != TD_NULL);

    ctx->half_nal_try = 0;
    ctx->half_nal_succ = 0;
    ctx->post_pic_count = 0;
    ctx->ldy_cfg = LDY_IDLE;
    if (ctx->extra_data_ptr->slc_ldy_en) {
        ctx->ldy_cfg = LDY_SLC;
    } else if (ctx->extra_data_ptr->sed_only_en) {
        ctx->ldy_cfg = LDY_SED;
    } else if (ctx->extra_data_ptr->multi_slc_ldy_en == TD_TRUE) {
        ctx->ldy_cfg = LDY_MULTI_SLC;
    }

    ctx->ldy_mode = ctx->ldy_cfg;

    ctx->half_nal_en = (ctx->ldy_cfg != LDY_IDLE && ctx->ldy_cfg != LDY_MULTI_SLC);
}
#endif

td_s32 hevc_init_(td_s32 chan_id, td_void **ctx, const syn_cfg_info *extra_data_ptr)
{
    hevc_ctx *ctx_tmp = TD_NULL;
    mem_record mem = { 0 };
    td_s32 i;

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, SYN_ERR);
    vfmw_check_ret((ctx != TD_NULL && extra_data_ptr != TD_NULL), SYN_ERR);

    mem.mem_name = "h265_ctx";
    mem.length = vfmw_align_up(sizeof(hevc_ctx), VFMW_CTX_ALIGN);
    mem.is_cached = 1;
    mem.type = CACHE_MEM_TYPE;
    mem.unid = (td_u32)chan_id;
    os_malloc(&mem); /* "H265_CTX" */
    ctx_tmp = (hevc_ctx *)(mem.vir_addr);
    vfmw_check_ret(ctx_tmp != TD_NULL, HEVC_DEC_ERR);

    vfmw_check_sec_func(memset_s(ctx_tmp, sizeof(hevc_ctx), 0, sizeof(hevc_ctx)));
    ctx_tmp->hevc_ctx_size = sizeof(hevc_ctx);
    ctx_tmp->chan_id = chan_id;

    vfmw_check_sec_func(memcpy_s(&ctx_tmp->extra_data, sizeof(ctx_tmp->extra_data),
        extra_data_ptr, sizeof(syn_cfg_info)));
    ctx_tmp->extra_data_ptr = &ctx_tmp->extra_data;

    hevc_config_max_para(ctx_tmp->extra_data_ptr);
    ctx_tmp->max_vps_num = ctx_tmp->extra_data_ptr->max_vps_num;
    ctx_tmp->max_sps_num = ctx_tmp->extra_data_ptr->max_sps_num;
    ctx_tmp->max_pps_num = ctx_tmp->extra_data_ptr->max_pps_num;
    ctx_tmp->max_slice_num = ctx_tmp->extra_data_ptr->max_slice_num;
    ctx_tmp->max_slot_num = ctx_tmp->max_slice_num + 5; /* 5 is slot num */

    ctx_tmp->is_sec = ctx_tmp->extra_data_ptr->sec_flag;

    ctx_tmp->need_clear_que = 1;

    for (i = 0; i < HEVC_MAX_NUM_LAYERS; i++) {
        ctx_tmp->last_display_poc[i] = -HEVC_MAX_INT;
    }

    hevc_init_dec_para(ctx_tmp);

    ctx_tmp->first_frame_state = FIRST_FRAME_INIT;
    ctx_tmp->prev_poc = 9999; /* 9999 is init poc */
    ctx_tmp->prev_vps_id = HEVC_MAX_VPS_NUM;
    ctx_tmp->prev_sps_id = HEVC_MAX_SPS_NUM;
    ctx_tmp->prev_pps_id = HEVC_MAX_PPS_NUM;
    ctx_tmp->prev_slc_pps_id = HEVC_MAX_PPS_NUM;
    ctx_tmp->slc_cost.independent_min_cost = 0xFFFFFFFF;
    ctx_tmp->slc_cost.dependent_min_cost = 0xFFFFFFFF;
    ctx_tmp->air_logic_id = -1;
    ctx_tmp->sei.pic_timming.source_scan_type = 0xFFFFFFFF;
#if defined(VFMW_HEVC_SLC_LDY_SUPPORT)
    hevc_init_ldy_info(ctx_tmp);
#endif

    ctx_tmp->is_init = 1;
    *ctx = ctx_tmp;

    return HEVC_DEC_NORMAL;
}

/* 码字解析函数,区别h264 */
/*****************************************************************************
函 数 名  : u_v
功能描述  : 从码流中取无符号数
输入参数  : bs_info    *bs_ptr
            td_s32 v
            td_s8 *name
输出参数  : 无
返 回 值  : td_u32
调用函数  :
被调函数  :
*****************************************************************************/
VFMW_ACC_SLICE_H265 STATIC td_u32 hevc_u_v(bs_info *bs_ptr, td_s32 v, td_char *name)
{
    td_u32 code;

    vfmw_unused(name);
    code = (td_u32)bs_get(bs_ptr, v);

    return code;
}

/*****************************************************************************
函 数 名  : hevc_ue_v
功能描述  : 解0阶无符号哥伦布码
输入参数  : bs_info    *bs_ptr
            td_s8 *name
输出参数  : 无
返 回 值  : td_u32
调用函数  :
被调函数  :
*****************************************************************************/
VFMW_ACC_SLICE_H265 STATIC td_u32 hevc_ue_v(bs_info *bs_ptr, td_char *name)
{
    td_u32 tmp_bits;
    td_u32 info;
    td_u32 leading_zeros;

    vfmw_unused(name);
    tmp_bits = (td_u32)bs_show(bs_ptr, 32); /* 32 is bit num */
    leading_zeros = bs_zero_ms_32(tmp_bits);
    if (leading_zeros < 32) { /* 32 is bit num */
        /* 因为最大的数为2的31次方，所以zeros+1的最大个数为32 */
        bs_skip(bs_ptr, leading_zeros);
        info = (td_u32)(bs_show(bs_ptr, (leading_zeros + 1)) - 1);
        bs_skip(bs_ptr, (leading_zeros + 1));
    } else {
        /* 这时有32个零，说明有错误发生 */
        info = 0xffffeeee; /* 0xffffffff */
        dprint(PRN_ERROR, "hevc_ue_v ERROR: leading_zeros (%d) >= 32, return 0x%x\n", leading_zeros, info);
        return info;
    }

    return info;
}

/*****************************************************************************
函 数 名  : hevc_se_v
功能描述  : 解0 阶有符号哥伦布码
输入参数  : bs_info *bs_ptr
td_s8 *name
输出参数  : 无
返 回 值  : td_s32
调用函数  :
被调函数  :
*****************************************************************************/
VFMW_ACC_SLICE_H265 STATIC td_s32 hevc_se_v(bs_info *bs_ptr, td_char *name)
{
    td_u32 tmp_bits;
    td_s32 info;
    td_u32 leading_zeros;
    td_u32 info_last_bit;

    vfmw_unused(name);
    tmp_bits = (td_u32)bs_show(bs_ptr, 32); /* 32 is bit num */
    leading_zeros = bs_zero_ms_32(tmp_bits);
    /* 因为最大的数为2的31次方，所以zeros的最大个数为32，所以在这种情况下需要BS_Show两次 */
    if (leading_zeros < 32) { /* 32 is bit num */
        bs_skip(bs_ptr, leading_zeros);
        info = bs_show(bs_ptr, (leading_zeros + 1));
        vfmw_check_ret(info > VFMW_MIN_SINT32 && info < VFMW_MAX_SINT32, 0);
        info -= 1;
        info_last_bit = (td_u32)info & 1;
        info = (td_u32)info >> 1;
        info = ((info_last_bit & 1) ? (info + 1) : (-info));
        bs_skip(bs_ptr, (leading_zeros + 1));
    } else {
        /* 这时有32个零，说明有错误发生 */
        info = 0x7fffffff;
        return info;
    }

    vfmw_check_ret(info > VFMW_MIN_SINT32 && info < VFMW_MAX_SINT32, 0);

    return info;
}

STATIC td_s32 hevc_clear_curr_nal(hevc_ctx *ctx)
{
    if (ctx->curr_nal != TD_NULL) {
        hevc_release_nal(ctx->chan_id, ctx->curr_nal);
        ctx->curr_nal = TD_NULL;
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_clear_dpb(hevc_ctx *ctx)
{
    td_s32 ret;

    ret = hevc_flush_dpb(ctx);
    if (ret != HEVC_DEC_NORMAL) {
        dprint(PRN_ERROR, "%s: flush dpb failed, return %d\n", __func__, ret);
    }

    hevc_init_dpb(ctx, 0);

    return HEVC_DEC_NORMAL;
}

/*************************************************************************
*   函数原型 :  td_u32 FrameStoreRefFlg(const hevc_frame_store *fs)
*   功能描述 :  指示该framestore是否用于参考
*   参数说明 :
*   返回值   ：
*************************************************************************/
STATIC td_u32 hevc_frame_store_ref_flg(const hevc_frame_store *fs)
{
    return (fs->is_reference > 0);
}

STATIC td_s32 hevc_clear_curr_slice(hevc_ctx *ctx)
{
    hevc_clear_curr_nal(ctx);
    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_clear_all_slice(hevc_ctx *ctx)
{
    ctx->num_slice_para = 0;
    ctx->max_bytes_received = 0;
    ctx->curr_msg_slot_num = HEVC_SLICE_SLOT_START_NUM - 1;
    ctx->have_independent_slice = 0;
}

STATIC td_void hevc_clear_all_nal(hevc_ctx *ctx)
{
    ctx->have_nal_not_decoded = 0;
}

STATIC td_s32 hevc_clear_curr_pic(hevc_ctx *ctx)
{
    if (ctx->num_slice_para > 0) {
        dps_release_seg(ctx->chan_id, ctx->curr_dps);
    }

    hevc_clear_all_slice(ctx);

    if (ctx->curr_pic.frame_store != TD_NULL) {
        fsp_clear_logic(ctx->chan_id, ctx->curr_pic.frame_store->logic_fs_id, 1);
        ctx->curr_pic.frame_store->frame_store_state = FS_NOT_USED;
        ctx->curr_pic.frame_store = TD_NULL;
    }

    ctx->curr_pic.state = HEVC_PIC_EMPTY;

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_flush_dpb(hevc_ctx *ctx)
{
    td_u32 i;
    td_s32 ret;
    td_u32 pos;
    td_s32 min_poc;

    /* mark all frames unused */
    for (i = 0; i < ctx->dpb.used_size; i++) {
        if (ctx->dpb.fs[i]->is_reference != 0) {
            hevc_unmark_frame_store_ref(ctx, ctx->dpb.fs[i]);
        }
    }

    while (hevc_remove_unused_frame_store(ctx) == HEVC_TRUE) {
    }

    /* output frames in POC order */
    while (ctx->dpb.used_size) {
        hevc_get_min_poc(ctx, &min_poc, &pos);
        ret = hevc_output_one_frm_from_dpb(ctx, pos);
        if (ret == HEVC_DEC_ERR) {
            return HEVC_DEC_ERR;
        }
    }

    return HEVC_DEC_NORMAL;
}

/*************************************************************************
*      函数原型 :  td_void UnMarkFrameStoreRef(hevc_ctx *ctx, hevc_frame_store *frame_store)
*      功能描述 :  将pFrameStore对因帧存中所有图像标记为不参考，同时释放pmv槽位
*      参数说明 :  pFrameStore帧存指针
*      返回值   ： 无
*************************************************************************/
STATIC td_void hevc_unmark_frame_store_ref(const hevc_ctx *ctx, hevc_frame_store *frame_store)
{
    /* 经过以上步骤不一定就把is_reference赋成0，所有最后强制一下 */
    frame_store->is_reference = 0;
    /* 把逻辑帧存标记为不参考 */
    fsp_set_ref(ctx->chan_id, frame_store->logic_fs_id, 0);
    return;
}

STATIC td_void hevc_is_size_changed(hevc_ctx *ctx, const hevc_seq_param_set *sps, td_s32 *is_size_changed)
{
    td_u32 pre_max_bit_depth;

    ctx->extra_data_ptr->bit_depth = vfmw_max(sps->bit_depth_luma, sps->bit_depth_chroma);
    pre_max_bit_depth = vfmw_max(ctx->prev_bit_depth_l, ctx->prev_bit_depth_c);
    if (((ctx->extra_data_ptr->bit_depth == 8) && (pre_max_bit_depth > 8)) || /* 8 is bit depth */
        ((ctx->extra_data_ptr->bit_depth > 8) && (pre_max_bit_depth == 8))) { /* 8 is bit depth */
        ctx->extra_data_ptr->bit_depth_changed = 1;
        *is_size_changed = 1;
        ctx->is_resolution_changed = 1;
    }

    if (ctx->prev_pic_width != sps->pic_width_in_luma_samples ||
        ctx->prev_pic_height != sps->pic_height_in_luma_samples ||
        (td_u32)(ctx->prev_dps_size) != sps->dpb_size) {
        ctx->is_resolution_changed = 1;
#ifdef VFMW_MV_HEVC_SUPPORT
        if (hevc_is_multi_layer_valid(ctx) == 1) {
            *is_size_changed = (ctx->is_mem_arrange_done == 0) ? 1 : 0;
        } else
#endif
       {
          *is_size_changed = 1;
       }
    }

    return;
}

VFMW_ACC_SLICE_H265 STATIC td_s32 hevc_inquire_slice_property(hevc_ctx *ctx,
    td_s32 *is_new_pic, td_s32 *is_size_changed)
{
    td_s32 ret;
    hevc_seq_param_set *sps = TD_NULL;
    hevc_pic_param_set *pps = TD_NULL;
    td_s32 prev_slice_poc;
    td_s32 last_slice_poc;

    *is_new_pic = *is_size_changed = 0;
    ctx->is_resolution_changed = 0;

    vfmw_check_sec_func(memset_s(&ctx->curr_slice,
                                 sizeof(hevc_slice_segment_header), 0, sizeof(hevc_slice_segment_header)));

    ctx->curr_slice.nal_unit_type = ctx->curr_nal->nal_unit_type;
    ctx->curr_slice.nuh_temporal_id = ctx->curr_nal->nuh_temporal_id;
    ctx->curr_slice.view_id = ctx->curr_nal->nal_layer_id;
    prev_slice_poc = ctx->prev_slice_poc;
    last_slice_poc = ctx->last_slice_poc;

    ret = hevc_dec_slice_segment_header(ctx);
    if (ret != HEVC_DEC_NORMAL) {
        report_dec_err(ctx->chan_id, ctx->stm_info.pts, SYNTAX_ERR_SLICE_HDR);
        dprint(PRN_ERROR, "hevc_dec_slice_segment_header dec err\n");
        return HEVC_DEC_ERR;
    }

    pps = ctx->pps[ctx->curr_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];

#if defined(VFMW_HEVC_SLC_LDY_SUPPORT)
    if (ctx->half_nal_try == TRY_HALF) {
        td_s32 cur_bits_len = bs_residual_bits(ctx->bs_ptr);
        td_s32 min_bits_len = (td_s32)((sps->ctb_num_height * sps->ctb_num_width) / (ctx->num_slice_para + 1));

        if ((cur_bits_len < 0) || (cur_bits_len < min_bits_len)) {
            dprint(PRN_ERROR, "cur_bits_len = %d, min_bits_len = %d\n", cur_bits_len, min_bits_len);

            ctx->last_slice_poc = last_slice_poc;
            ctx->prev_slice_poc = prev_slice_poc;

            return HEVC_DEC_ERR;
        }

        return HEVC_DEC_NORMAL;
    }
#else
    vfmw_unused(prev_slice_poc);
    vfmw_unused(last_slice_poc);
#endif

    *is_new_pic = (ctx->curr_slice.new_pic_type == 1) ? 1 : 0;

    hevc_is_size_changed(ctx, sps, is_size_changed);

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_have_slice_to_dec(hevc_ctx *ctx)
{
    if (ctx->num_slice_para > 0) {
        return HEVC_DEC_NORMAL;
    } else {
#if defined(VFMW_HEVC_SLC_LDY_SUPPORT)
        /* first slice is half and call initpic */
        /* second slice is repeat can not clearpic */
        if (ctx->half_nal_en == 1 &&
            ctx->seg_cut_state != SEG_CUT_NORMAL) {
            return HEVC_DEC_ERR;
        }
#endif

        /* 容错，在开始解新帧前，先判断上帧是否申请了帧存，
          而且未成功解出一个slice，如是先将其申请的帧存释放 */
        if (ctx->curr_pic.state != HEVC_PIC_EMPTY) {
            dprint(PRN_ERROR, "%s last frame fs unrelease, fix it!\n", __func__);
            hevc_clear_curr_pic(ctx);
        }

        return HEVC_DEC_ERR;
    }
}

STATIC td_s32 hevc_get_pic_stream_size(const hevc_ctx *ctx)
{
    td_s32 total_size;

    total_size = (ctx->curr_pic.total_valid_bit_len + 7) / 8; /* 7 is align, 8 is bit num */

    return total_size;
}

STATIC td_s32 hevc_process_remain_img(hevc_ctx *ctx, const hevc_frame_store *frame_store)
{
    vfmw_image *logic_fs_image = TD_NULL;
    td_s32 ret;

    ret = LAST_OUTPUT_OK;
    hevc_get_image_para(ctx, frame_store);
    fsp_set_disp(ctx->chan_id, frame_store->logic_fs_id, 1);
    logic_fs_image = fsp_get_image(ctx->chan_id, frame_store->logic_fs_id);
    if (logic_fs_image == TD_NULL) {
        dprint(PRN_ERROR, "%s logic_fs_image is TD_NULL!\n", __func__);
        ret = LAST_OUTPUT_FAIL;
    } else {
        if (ctx->dpb.used_size == 1) {
            logic_fs_image->last_frame = 1;
        }

        if (hevc_shvc_frame_output(ctx, frame_store, logic_fs_image) == 1) {
            ret = voq_insert(ctx->chan_id, logic_fs_image);
            if (ret != SYN_OK) {
                dprint(PRN_ERROR, "%s InsertVO err:%d, ClearAll\n", __func__, ret);
                fsp_set_disp(ctx->chan_id, frame_store->logic_fs_id, 0);
                hevc_clear_all(ctx, 1);
                ret = LAST_OUTPUT_FAIL;
            }
        }
        hevc_set_frm_repeat_count(ctx, logic_fs_image);
        ctx->output_frame_num++;
        ctx->pic_out_counter++;

        if (ctx->dpb.used_size == 1) {
            ret = LAST_OUTPUT_OK;
        }
    }

    return ret;
}

STATIC td_s32 hevc_get_remain_img(hevc_ctx *ctx)
{
    td_s32 ret, min_poc;
    td_u32 pos;
    hevc_frame_store *frame_store = TD_NULL;

    ret = LAST_OUTPUT_FAIL;

    /* output frames in POC order */
    while (ctx->dpb.used_size) {
        hevc_get_min_poc(ctx, &min_poc, &pos);

        frame_store = ctx->dpb.fs[pos];

        if (frame_store != TD_NULL && frame_store->is_displayed == 0) {
            frame_store->is_displayed = 1;

            ret = hevc_check_frame_store(ctx, frame_store);
            if (ret == HEVC_DEC_NORMAL) {
                ret = hevc_process_remain_img(ctx, frame_store);
            } else {
                ret = LAST_OUTPUT_OK;
            }
        }

        /* free frame store and move empty store to end of buffer */
        hevc_remove_frame_store_out_dpb(ctx, pos);
    }

    return ret;
}

STATIC td_u8 hevc_is_eos_nal(const hevc_ctx *ctx)
{
    td_u32 code0, code1; /* add for eos nal */

    /* 真正的NAL_UNIT_RESERVED_VCL_N11类型nal,不进行特殊处理, */
    code0 = (td_u32)bs_get(ctx->bs_ptr, 32); /* 32 is bit num */
    code1 = (td_u32)bs_get(ctx->bs_ptr, 32); /* 32 is bit num */
    if (code0 == HEVC_STREAM_END_CODE1 && code1 == HEVC_STREAM_END_CODE2) {
        return 1;
    } else {
        return 0;
    }
}

STATIC td_s32 hevc_get_final_dar(td_s32 img_width, td_s32 img_height)
{
    td_s32 final_dar;

    if (((img_width == 7680) && (img_height == 4320)) /* 7680*4320 is sample */
        ||  ((img_width == 3840) && (img_height == 2160)) /* 3840*2160 is sample */
        ||  ((img_width == 1920) && (img_height == 1088)) /* 1920*1088 is sample */
        ||  ((img_width == 1920) && (img_height == 1080)) /* 1920*1080 is sample */
        ||  ((img_width == 1280) && (img_height == 720))) { /* 1280*720 is sample */
        final_dar = VFMW_DAR_16_9;
    } else if ((img_width == 640) && (img_height == 480)) { /* 640*480 is sample */
        final_dar = VFMW_DAR_4_3;
    } else {
        final_dar = VFMW_DAR_IMG_SIZE;
    }

    return final_dar;
}

#define CONST_DAR_235_100 (235 * 1024 / 100)
#define CONST_DAR_221_100 (221 * 1024 / 100)
#define CONST_DAR_16_9    (16 * 1024 / 9)
#define CONST_DAR_4_3     (4 * 1024 / 3)
STATIC td_s32 hevc_get_dar(td_s32 ar_idc, td_s32 sar_width, td_s32 sar_height, td_s32 img_width, td_s32 img_height)
{
    STATIC td_s32 SarTable[][2] = {
        { 1,  1 }, { 1, 1 }, { 12, 11 }, { 10, 11 }, { 16, 11 },
        { 40, 33 }, { 24, 11 }, { 20, 11 }, { 32, 11 }, { 80, 33 },
        { 18, 11 }, { 15, 11 }, { 64, 33 }, { 160, 99 }, { 4, 3 }, { 3, 2 }, { 2, 1 }
    };
    td_s32 dar, final_dar;

    if (ar_idc <= 0 || (ar_idc > 16 && ar_idc != 255) || /* 16 255 is idc */
        ((sar_width == 0 || sar_height == 0) && (ar_idc == 255))) { /* 255 is idc */
        final_dar = VFMW_DAR_UNKNOWN;
    } else if (ar_idc == 1) {
        /* CNcomment:, 2011-11-21.
        sample_aspect_ratio = 1:1本来意味着显示宽高比等于图象的宽度:高度，以下分支本来无错. 但目前VO把1:1理解为无效，
        做法是填充输出窗口，这使得显示效果不可控.
        鉴于各种条件VO不敢做改动，以免使现有效果被改变而引起客户投诉. 未来VO会对宽高比做一次彻底重构，所以暂时把下面
        ar_idc==1的分支去掉，以配合VO把眼下的问题解决掉. */
        final_dar = hevc_get_final_dar(img_width, img_height);
    } else {
        if (ar_idc != 255) { /* 255 is idc */
            sar_width = SarTable[ar_idc][0];
            sar_height = SarTable[ar_idc][1];
        }

        /* 20131012: l165842 上面的计算方法有可能导致数值(sar_width * img_width * 1024)
           溢出，最终将导致SINT32类型的da值为 -1 */
        dar = (((sar_width * 1024) / (sar_height)) * ((img_width * 1024) / (img_height))) / 1024; /* 1024 is num */

        if (vfmw_abs(dar - CONST_DAR_235_100) < vfmw_abs(dar - CONST_DAR_221_100)) {
            final_dar = VFMW_DAR_235_100;
        } else if (vfmw_abs(dar - CONST_DAR_221_100) < vfmw_abs(dar - CONST_DAR_16_9)) {
            final_dar = VFMW_DAR_221_100;
        } else if (vfmw_abs(dar - CONST_DAR_16_9) < vfmw_abs(dar - CONST_DAR_4_3)) {
            final_dar = VFMW_DAR_16_9;
        } else {
            final_dar = VFMW_DAR_4_3;
        }
    }

    return final_dar;
}

STATIC td_s32 hevc_sei_tsi_info(const hevc_ctx *ctx, hevc_sei_tsi *tsi)
{
    if (ctx == TD_NULL || tsi == TD_NULL || ctx->bs_ptr == TD_NULL) {
        return HEVC_DEC_ERR;
    }

    tsi->ts_103_433_spec_version = hevc_u_v(ctx->bs_ptr, 4, "ts_103_433_spec_version"); /* 4 is bit num */
    tsi->ts_103_433_payload_mode = hevc_u_v(ctx->bs_ptr, 4, "ts_103_433_payload_mode"); /* 4 is bit num */

    tsi->is_avalible = 1;

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sei_color_volume_reconstructuion_info(const hevc_ctx *ctx,
                                                         hevc_sei_color_volume_reconstruction_info *cvri)
{
    td_u32 i;

    if (ctx == TD_NULL || cvri == TD_NULL || ctx->bs_ptr == TD_NULL) {
        return HEVC_DEC_ERR;
    }

    cvri->cv_rec_id = hevc_ue_v(ctx->bs_ptr, "cv_rec_id");
    cvri->cv_rec_cancel_flag = hevc_u_v(ctx->bs_ptr, 1, "cv_rec_cancel_flag");

    if (cvri->cv_rec_cancel_flag == 0) {
        cvri->cv_rec_persistence_flag = hevc_u_v(ctx->bs_ptr, 1, "cv_rec_persistence_flag");
        cvri->cv_rec_target_info_flag = hevc_u_v(ctx->bs_ptr, 1, "cv_rec_target_info_flag");

        if (cvri->cv_rec_target_info_flag == 1) {
            cvri->cv_rec_primaries = hevc_u_v(ctx->bs_ptr, 8, "cv_rec_primaries"); /* 8 is bit num */
            cvri->cv_rec_matrix_coefficients =
                hevc_u_v(ctx->bs_ptr, 8, "cv_rec_matrix_coefficients"); /* 8 is bit num */
        }

        cvri->tone_mapping_input_signal_black_level_offset =
            hevc_u_v(ctx->bs_ptr, 8, "tone_mapping_input_signal_black_level_offset"); /* 8 is bit num */
        cvri->tone_mapping_input_signal_white_level_offset =
            hevc_u_v(ctx->bs_ptr, 8, "tone_mapping_input_signal_white_level_offset"); /* 8 is bit num */
        cvri->shadow_gain_control = hevc_u_v(ctx->bs_ptr, 8, "shadow_gain_control"); /* 8 is bit num */
        cvri->highlight_gain_control = hevc_u_v(ctx->bs_ptr, 8, "highlight_gain_control"); /* 8 is bit num */
        cvri->mid_tone_width_adjustment_factor =
            hevc_u_v(ctx->bs_ptr, 8, "mid_tone_width_adjustment_factor"); /* 8 is bit num */

        cvri->tone_mapping_output_fine_tuning_num_val =
            hevc_u_v(ctx->bs_ptr, 4, "tone_mapping_output_fine_tuning_num_val"); /* 4 is bit num */
        vfmw_check_ret(cvri->tone_mapping_output_fine_tuning_num_val <= 15, HEVC_DEC_ERR); /* 15: array size */

        for (i = 0; i < cvri->tone_mapping_output_fine_tuning_num_val; i++) {
            cvri->tone_mapping_output_fine_tuning_x[i] =
                hevc_u_v(ctx->bs_ptr, 8, "tone_mapping_output_fine_tuning_x"); /* 8 is bit num */
            cvri->tone_mapping_output_fine_tuning_y[i] =
                hevc_u_v(ctx->bs_ptr, 8, "tone_mapping_output_fine_tuning_y"); /* 8 is bit num */
        }

        cvri->saturation_gain_num_val = hevc_u_v(ctx->bs_ptr, 4, "saturation_gain_num_val"); /* 4 is bit num */
        vfmw_check_ret(cvri->saturation_gain_num_val <= 15, HEVC_DEC_ERR); /* 15: array size */

        for (i = 0; i < cvri->saturation_gain_num_val; i++) {
            cvri->saturation_gain_x[i] = hevc_u_v(ctx->bs_ptr, 8, "saturation_gain_x"); /* 8 is bit num */
            cvri->saturation_gain_y[i] = hevc_u_v(ctx->bs_ptr, 8, "saturation_gain_y"); /* 8 is bit num */
        }
    }

    cvri->is_avalible = 1;

    return HEVC_DEC_NORMAL;
}

#ifdef HEVC_CUVA_SUPPORT
STATIC td_void hevc_sei_cuva_hdr_p3_spline(const hevc_ctx *ctx,
                                           vfmw_cuva_metadata *cuva_metadata, td_u32 i)
{
    td_u16 j, p3_spline_num;

    cuva_metadata->p_3spline_enable_flag[i] = hevc_u_v(ctx->bs_ptr, P3SPLINE_FLAG_BIT, "P3Spline_flag_BIT");
    if (cuva_metadata->p_3spline_enable_flag[i]) {
        cuva_metadata->p_3spline_enable_num[i] = hevc_u_v(ctx->bs_ptr, P3SPLINE_NUM_BIT, "P3Spline_num_BIT");
        p3_spline_num = cuva_metadata->p_3spline_enable_num[i] + 1;
        vfmw_check(p3_spline_num <= CUVA_MAX_PARANUM_3SPLINE);
        for (j = 0; j < p3_spline_num; j++) {
            cuva_metadata->p_3spline_th_enable_mode[j][i] = hevc_u_v(ctx->bs_ptr, P3SPLINE_TH_MODE_BIT,
                "P3Spline_TH_mode_BIT");
            if (cuva_metadata->p_3spline_th_enable_mode[j][i] == 0 ||
                cuva_metadata->p_3spline_th_enable_mode[j][i] == 2) {
                cuva_metadata->p_3spline_th_enable_mb[j][i] =
                    hevc_u_v(ctx->bs_ptr, P3SPLINE_TH_MB_BIT, "P3Spline_TH_MB_BIT");
            }
            cuva_metadata->p_3spline_th_enable[j][i] = hevc_u_v(ctx->bs_ptr, P3SPLINE_TH1_BIT, "P3Spline_TH1_BIT");
            cuva_metadata->p_3spline_th_enable_delta1[j][i] = (td_s16)hevc_u_v(ctx->bs_ptr, P3SPLINE_TH2_BIT,
                "P3Spline_TH2_BIT");
            cuva_metadata->p_3spline_th_enable_delta2[j][i] = (td_s16)hevc_u_v(ctx->bs_ptr, P3SPLINE_TH3_BIT,
                "P3Spline_TH3_BIT");
            cuva_metadata->p_3spline_enable_strength[j][i] = (td_s16)hevc_u_v(ctx->bs_ptr, P3SPLINE_STRENGTH_BIT,
                "P3Spline_Strength_BIT");
        }
    }
}
#endif

#ifdef HEVC_CUVA_SUPPORT
STATIC td_void hevc_sei_cuva_hdr_base_param(const hevc_ctx *ctx,
                                            vfmw_cuva_metadata *cuva_metadata, td_u32 i)
{
    vfmw_check(i < CUVA_MAX_PARANUM_TM);
    cuva_metadata->base_param_m_p[i] = hevc_u_v(ctx->bs_ptr, BASE_PARAM_M_P_BIT, "Base_param_m_p_BIT");
    cuva_metadata->base_param_m_m[i] = hevc_u_v(ctx->bs_ptr, BASE_PARAM_M_M_BIT, "Base_param_m_m_BIT");
    cuva_metadata->base_param_m_a[i] = hevc_u_v(ctx->bs_ptr, BASE_PARAM_M_A_BIT, "Base_param_m_a_BIT");
    cuva_metadata->base_param_m_b[i] = hevc_u_v(ctx->bs_ptr, BASE_PARAM_M_B_BIT, "Base_param_m_b_BIT");
    cuva_metadata->base_param_m_n[i] = hevc_u_v(ctx->bs_ptr, BASE_PARAM_M_N_BIT, "Base_param_m_n_BIT");
    cuva_metadata->base_param_k1[i] = hevc_u_v(ctx->bs_ptr, BASE_PARAM_K1_BIT, "Base_param_K1_BIT");
    cuva_metadata->base_param_k2[i] = hevc_u_v(ctx->bs_ptr, BASE_PARAM_K2_BIT, "Base_param_K2_BIT");
    cuva_metadata->base_param_k3[i] = hevc_u_v(ctx->bs_ptr, BASE_PARAM_K3_BIT, "Base_param_K3_BIT");
    cuva_metadata->base_param_delta_enable_mode[i] = hevc_u_v(ctx->bs_ptr, BASE_PARAM_DELTA_MODE_BIT,
        "Base_param_Delta_mode_BIT");
    cuva_metadata->base_param_enable_delta[i] = hevc_u_v(ctx->bs_ptr, BASE_PARAM_DELTA_BIT,
        "Base_param_Delta_BIT");
}
#endif

#ifdef HEVC_CUVA_SUPPORT
STATIC td_s32 hevc_sei_cuva_hdr(hevc_ctx *ctx, vfmw_cuva_metadata *cuva_metadata)
{
    td_u16 i, tone_mapping_param_number;

    if (ctx == TD_NULL || cuva_metadata == TD_NULL || ctx->bs_ptr == TD_NULL) {
        return HEVC_DEC_ERR;
    }

    cuva_metadata->system_start_code = hevc_u_v(ctx->bs_ptr, SYSTEM_START_CODE_BIT, "system_start_code_BIT");
    if (cuva_metadata->system_start_code != 0x1) {
        dprint(PRN_ERROR, "%s, %d, system_start_code != 1 error\n", __func__, __LINE__);
        return HEVC_DEC_ERR;
    }
    cuva_metadata->minimum_maxrgb_pq    = hevc_u_v(ctx->bs_ptr, MINIMUM_MAXRGB_BIT, "minimum_maxrgb_BIT");
    cuva_metadata->average_maxrgb_pq    = hevc_u_v(ctx->bs_ptr, AVERAGE_MAXRGB_BIT, "average_maxrgb_BIT");
    cuva_metadata->variance_maxrgb_pq   = hevc_u_v(ctx->bs_ptr, VARIANCE_MAXRGB_BIT, "variance_maxrgb_BIT");
    cuva_metadata->maximum_maxrgb_pq    = hevc_u_v(ctx->bs_ptr, MAXIMUM_MAXRGB_BIT, "maximum_maxrgb_BIT");
    cuva_metadata->tone_mapping_enable_mode_flag =
        hevc_u_v(ctx->bs_ptr, TONE_MAPPING_MODE_BIT, "tone_mapping_mode_BIT");
    if (cuva_metadata->tone_mapping_enable_mode_flag) {
        cuva_metadata->tone_mapping_param_enable_num = hevc_u_v(ctx->bs_ptr, TONE_MAPPING_PARAM_NUM_BIT,
            "tone_mapping_param_num_BIT");
        tone_mapping_param_number = cuva_metadata->tone_mapping_param_enable_num + 1;
        vfmw_check_ret(tone_mapping_param_number <= CUVA_MAX_PARANUM_TM, HEVC_DEC_ERR);
        for (i = 0; i < tone_mapping_param_number; i++) {
            cuva_metadata->targeted_system_display_maximum_luminance_pq[i] = hevc_u_v(ctx->bs_ptr,
                TARGETED_SYSTEM_DISPLAY_BIT, "targeted_system_display_BIT");
            cuva_metadata->base_enable_flag[i] = hevc_u_v(ctx->bs_ptr, BASE_FLAG_BIT, "Base_flag_BIT");
            if (cuva_metadata->base_enable_flag[i]) {
                hevc_sei_cuva_hdr_base_param(ctx, cuva_metadata, i);
            }

            hevc_sei_cuva_hdr_p3_spline(ctx, cuva_metadata, i);
        }
    }

    cuva_metadata->color_saturation_mapping_enable_flag = hevc_u_v(ctx->bs_ptr, COLOR_SATURATION_BIT,
        "color_saturation_BIT");
    if (cuva_metadata->color_saturation_mapping_enable_flag) {
        cuva_metadata->color_saturation_enable_num = hevc_u_v(ctx->bs_ptr, COLOR_SATURATION_NUM_BIT,
            "color_saturation_num_BIT");
        vfmw_check_ret(cuva_metadata->color_saturation_enable_num <= CUVA_MAX_PARANUM_SM, HEVC_DEC_ERR);
        for (i = 0; i < cuva_metadata->color_saturation_enable_num; i++) {
            cuva_metadata->color_saturation_enable_gain[i] = hevc_u_v(ctx->bs_ptr, COLOR_SATURATION_GAIN_BIT,
                "color_saturation_gain_BIT");
        }
    }

    ctx->sei.cuva_info.is_cuva = TD_TRUE;

    return HEVC_DEC_NORMAL;
}
#endif

STATIC td_s32 hevc_sei_usr_dat_reg_itu_t_t35(hevc_ctx *ctx, hevc_sei_user_data_reg_itu_t_t35 *itu_t35,
    td_u32 payload_size)
{
    td_s32 ret = HEVC_DEC_ERR;
    td_u16 terminal_provider_oriented_code;
    td_u32 i;

    if (ctx == TD_NULL || itu_t35 == TD_NULL || ctx->bs_ptr == TD_NULL) {
        return HEVC_DEC_ERR;
    }

    if (payload_size < 4) { /* 4 is byte num */
        dprint(PRN_ERROR, "hevc_sei_usr_dat_reg_itu_t_t35 payload_size < 4!!\n");
        return HEVC_DEC_ERR;
    }

    itu_t35->itu_t_t35_country_code = hevc_u_v(ctx->bs_ptr, 8, "itu_t_t35_country_code"); /* 8 is bit num */
    itu_t35->terminal_provider_code = hevc_u_v(ctx->bs_ptr, 16, "terminal_provider_code"); /* 16 :a number */
    terminal_provider_oriented_code = hevc_u_v(ctx->bs_ptr, 8, "terminal Provider Oriented Code");

    if (itu_t35->itu_t_t35_country_code == 0x26 &&
        itu_t35->terminal_provider_code == 0x4 &&
        terminal_provider_oriented_code == 0x0) {
        terminal_provider_oriented_code = hevc_u_v(ctx->bs_ptr, 8, "terminal Provider Oriented Code");
        if (terminal_provider_oriented_code == 0x5) {
#ifdef HEVC_CUVA_SUPPORT
            ret = hevc_sei_cuva_hdr(ctx, &(ctx->sei.cuva_info.cuva_metadata));
#endif
        }
    } else if (itu_t35->terminal_provider_oriented_code_message_idc == 0x2) {
        ret = hevc_sei_tsi_info(ctx, &(ctx->sei.tsi));
    } else if (itu_t35->terminal_provider_oriented_code_message_idc == 0x4) {
        ret = hevc_sei_color_volume_reconstructuion_info(ctx, &(ctx->sei.color_volume_rec_info));
    } else {
        // SMPTE ST 2094-40 standard
        if (itu_t35->itu_t_t35_country_code == 0xb5 &&  // SMPTE ST 2094-40 itu_t_t35_country_code shall be 0xb5
            itu_t35->terminal_provider_code == 0x3c &&  // SMPTE ST 2094-40 terminal_provider_code shall be 0x3c
            terminal_provider_oriented_code == 0x0) {
            terminal_provider_oriented_code =
                hevc_u_v(ctx->bs_ptr, 8, "terminal Provider Oriented Code"); /* 8 is bit num */
            if (terminal_provider_oriented_code == 0x1) {
                dprint(PRN_ERROR, "%s %d report unsupport terminal_provider_oriented_code=1.\n", __func__, __LINE__);
                report_unsupport(ctx->chan_id);
                return HEVC_DEC_ERR;
            }
            payload_size = payload_size - 1; /* 1 is byte num */
        }
        payload_size = payload_size - 4; /* 4 is byte num */
        for (i = 0; i < payload_size; i++) {
            hevc_u_v(ctx->bs_ptr, 8, "user_data"); /* 8 is bit num */
        }
        ret = HEVC_DEC_NORMAL;
    }

    return ret;
}

STATIC td_s32 hevc_sei_lut_coded(const hevc_ctx *ctx, hevc_sei_color_remapping_info *remapping)
{
    td_u32 j;
    td_u8 i;

    for (j = 0; j < 3; j++) { /* 3 is vfmw_max num */
        remapping->pre_lut_num_val_minus1[j] =
            hevc_u_v(ctx->bs_ptr, 8, "pre_lut_num_val_minus1 "); /* 8 is bit num */
        vfmw_check_ret(remapping->pre_lut_num_val_minus1[j] < 33, HEVC_DEC_ERR); /* 33: array size */

        if (remapping->pre_lut_num_val_minus1[j] > 0) {
            for (i = 0; i < remapping->pre_lut_num_val_minus1[j] + 1; i++) {
                remapping->pre_lut_coded_value[j][i] =
                    hevc_u_v(ctx->bs_ptr, 16, "pre_lut_coded_value"); /* 16 is bit num */
                remapping->pre_lut_target_value[j][i] =
                    hevc_u_v(ctx->bs_ptr, 16, "pre_lut_target_value "); /* 16 is bit num */
            }
        }
    }

    remapping->colour_remap_matrix_present_flag = hevc_u_v(ctx->bs_ptr, 1, "colour_remap_matrix_present_flag");

    if (remapping->colour_remap_matrix_present_flag == 1) {
        remapping->log2_matrix_denom = hevc_u_v(ctx->bs_ptr, 4, "log2_matrix_denom"); /* 4 is bit num */

        for (j = 0; j < 3; j++) { /* 3 is vfmw_max num */
            for (i = 0; i < 3; i++) { /* 3 is vfmw_max num */
                remapping->colour_remap_coeffs[j][i] = hevc_se_v(ctx->bs_ptr, "colour_remap_coeffs");
            }
        }
    }

    for (j = 0; j < 3; j++) { /* 3 is vfmw_max num */
        remapping->post_lut_num_val_minus1[j] =
            hevc_u_v(ctx->bs_ptr, 8, "post_lut_num_val_minus1"); /* 8 is bit num */

        if (remapping->post_lut_num_val_minus1[j] > 32) { /* 32 is num */
            dprint(PRN_ERROR, "ERROR: post_lut_num_val_minus1 %d out of range[0, 32]\n",
                   remapping->post_lut_num_val_minus1[j]);
            return HEVC_DEC_ERR;
        }

        if (remapping->post_lut_num_val_minus1[j] > 0) {
            for (i = 0; i < remapping->post_lut_num_val_minus1[j] + 1; i++) {
                remapping->post_lut_coded_value[j][i] =
                    hevc_u_v(ctx->bs_ptr, 16, "post_lut_coded_value"); /* 16 is bit num */
                remapping->post_lut_target_value[j][i] =
                    hevc_u_v(ctx->bs_ptr, 16, " post_lut_target_value "); /* 16 is bit num */
            }
        }
    }

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sei_color_remapping(const hevc_ctx *ctx, hevc_sei_color_remapping_info *remapping)
{
    td_s32 ret;

    if (ctx == TD_NULL || remapping == TD_NULL || ctx->bs_ptr == TD_NULL) {
        return HEVC_DEC_ERR;
    }

    ret = HEVC_DEC_NORMAL;

    remapping->colour_remap_id = hevc_ue_v(ctx->bs_ptr, "colour_remap_id");
    remapping->colour_remap_cancel_flag = hevc_u_v(ctx->bs_ptr, 1, "colour_remap_cancel_flag");

    if (remapping->colour_remap_cancel_flag == 0) {
        remapping->colour_remap_persistence_flag = hevc_u_v(ctx->bs_ptr, 1, "colour_remap_persistence_flag");
        remapping->colour_remap_video_signal_info_present_flag =
            hevc_u_v(ctx->bs_ptr, 1, "colour_remap_video_signal_info_present_flag");

        if (remapping->colour_remap_video_signal_info_present_flag == 1) {
            remapping->colour_remap_full_range_flag = hevc_u_v(ctx->bs_ptr, 1, "colour_remap_full_range_flag ");
            remapping->colour_remap_primaries =
                hevc_u_v(ctx->bs_ptr, 8, "colour_remap_primaries "); /* 8 is bit num */
            remapping->colour_remap_transfer_function =
                hevc_u_v(ctx->bs_ptr, 8, "colour_remap_transfer_function"); /* 8 is bit num */
            remapping->colour_remap_matrix_coefficients =
                hevc_u_v(ctx->bs_ptr, 8, "colour_remap_matrix_coefficients"); /* 8 is bit num */
        }

        remapping->colour_remap_input_bit_depth =
            hevc_u_v(ctx->bs_ptr, 8, "colour_remap_input_bit_depth "); /* 8 is bit num */
        remapping->colour_remap_bit_depth = hevc_u_v(ctx->bs_ptr, 8, "colour_remap_bit_depth "); /* 8 is bit num */

        ret = hevc_sei_lut_coded(ctx, remapping);
        if (ret != HEVC_DEC_NORMAL) {
             dprint(PRN_ERROR, "%s: hevc_sei_lut_coded\n", __func__);
             return HEVC_DEC_ERR;
        }
    }

    remapping->is_avalible = 1;

    return ret;
}

STATIC td_s32 hevc_sei_content_light_level_information(const hevc_ctx *ctx,
    hevc_sei_content_light_level_info *reg)
{
    if (ctx == TD_NULL || reg == TD_NULL || ctx->bs_ptr == TD_NULL) {
        return HEVC_DEC_ERR;
    }

    reg->is_avalible = 1;

    reg->max_content_light_level = hevc_u_v(ctx->bs_ptr, 16, "max_content_light_level"); /* 16 is bit num */
    reg->max_pic_average_light_level = hevc_u_v(ctx->bs_ptr, 16, "max_pic_average_light_level"); /* 16 is bit num */

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sei_hlghdr_comapitibility_info(const hevc_ctx *ctx, hevc_sei_hlg_compatible_info *reg)
{
    if (ctx == TD_NULL || reg == TD_NULL || ctx->bs_ptr == TD_NULL) {
        return HEVC_DEC_ERR;
    }

    reg->hdr_transfer_characteristic_idc = hevc_u_v(ctx->bs_ptr, 8, "hdr_transfer_idc"); /* 8 is bit num */

    return HEVC_DEC_NORMAL;
}

STATIC td_s32 hevc_sei_scalable_nesting_info(hevc_ctx *ctx, hevc_sei_param_set *sei,
                                      hevc_seq_param_set *sps, td_u32 payloadsize);

STATIC td_void hevc_proc_err_nal(hevc_ctx *ctx, const stm_seg_info *seg, td_s32 dec_ret)
{
    /* for error stm/cfg cause seglist full */
    if (dec_ret == SYN_PARSE_PRE_FRMDONE ||
        dec_ret == SYN_PARSE_CURR_FRMDONE) {
        ctx->err_nal_cnt = 0;
    } else if (ctx->last_seg_id != (td_u32)(seg->stream_id)) {
        ctx->err_nal_cnt++;

        if (ctx->num_slice_para > 0 &&
            ctx->err_nal_cnt > 1024) { /* 1024 is err num */
            dprint(PRN_ERROR, "Hevc err_nal_cnt(%d) ClearCurPic\n", ctx->err_nal_cnt);
            ctx->err_nal_cnt = 0;
            hevc_clear_curr_pic(ctx);
        }
    }

    ctx->last_seg_id = seg->stream_id;
}

STATIC td_void hevc_config_max_para(syn_cfg_info *extra_data)
{
    if (extra_data->max_width < 0) {
        extra_data->max_width = HEVC_MIN_PIX_WIDTH;
    } else if ((extra_data->max_width == 0) || (extra_data->max_width > HEVC_MAX_PIX_WIDTH)) {
        extra_data->max_width = HEVC_MAX_PIX_WIDTH;
    }

    if (extra_data->max_height < 0) {
        extra_data->max_height = HEVC_MIN_PIX_HEIGHT;
    } else if ((extra_data->max_height == 0) || (extra_data->max_height > HEVC_MAX_PIX_HEIGHT)) {
        extra_data->max_height = HEVC_MAX_PIX_HEIGHT;
    }

    extra_data->max_vps_num = ((extra_data->max_vps_num <= 0) ||
        (extra_data->max_vps_num > HEVC_MAX_VPS_NUM)) ? HEVC_MAX_VPS_NUM : extra_data->max_vps_num;
    extra_data->max_sps_num = ((extra_data->max_sps_num <= 0) ||
        (extra_data->max_sps_num > HEVC_MAX_SPS_NUM)) ? HEVC_MAX_SPS_NUM : extra_data->max_sps_num;
    extra_data->max_pps_num = ((extra_data->max_pps_num <= 0) ||
        (extra_data->max_pps_num > HEVC_MAX_PPS_NUM)) ? HEVC_MAX_PPS_NUM : extra_data->max_pps_num;
    extra_data->max_slice_num = ((extra_data->max_slice_num <= 0) ||
        (extra_data->max_slice_num > HEVC_MAX_SLICE_NUM)) ? HEVC_MAX_SLICE_NUM : extra_data->max_slice_num;
    extra_data->max_ref_num = ((extra_data->max_ref_num <= 0) ||
        (extra_data->max_ref_num > HEVC_MAX_NUM_REF_PICS)) ? HEVC_MAX_NUM_REF_PICS : extra_data->max_ref_num;
}

STATIC td_void hevc_process_pts(hevc_ctx *ctx, const stm_seg_info *seg)
{
    vfmw_check(ctx->extra_data_ptr != TD_NULL);

    if (seg->pts != VFMW_INVALID_PTS) {
        ctx->stm_info.pts = seg->pts;
    }

    ctx->stm_info.user_tag = seg->user_tag;
    ctx->stm_info.disp_time = seg->disp_time;
    ctx->stm_info.disp_enable_flag = seg->disp_enable_flag;
    ctx->stm_info.disp_frame_distance = seg->disp_frame_distance;
    ctx->stm_info.distance_before_1st_frame = seg->distance_beforce_first_frame;
    ctx->stm_info.gop_num = seg->gop_num;
    ctx->stm_info.dv_dual_layers = seg->dv_dual_layer;

    if (dcheck(PRN_TIME)) {
        ctx->stm_info.que_raw_time = seg->que_raw_time;
        ctx->stm_info.deq_raw_time = seg->deq_raw_time;
        ctx->stm_info.que_seg_time = seg->que_seg_time;
        ctx->stm_info.deq_seg_time = seg->deq_seg_time;
    }
}

td_s32 hevc_get_max_size(td_void)
{
    td_s32 size;

    size = vfmw_align_up(sizeof(hevc_ctx), VFMW_CTX_ALIGN);

    /* + sps pps vps */
    size += HEVC_MAX_VPS_NUM * vfmw_align_up(sizeof(hevc_video_param_set), VFMW_CTX_ALIGN);
    size += HEVC_MAX_SPS_NUM * vfmw_align_up(sizeof(hevc_seq_param_set), VFMW_CTX_ALIGN);
    size += HEVC_MAX_PPS_NUM * vfmw_align_up(sizeof(hevc_pic_param_set), VFMW_CTX_ALIGN);

    /* + tmpvps tmpsps tmppps */
    size += vfmw_align_up(sizeof(hevc_video_param_set), VFMW_CTX_ALIGN);
    size += vfmw_align_up(sizeof(hevc_seq_param_set), VFMW_CTX_ALIGN);
    size += vfmw_align_up(sizeof(hevc_pic_param_set), VFMW_CTX_ALIGN);
    size = vfmw_align_up((td_u32)size, VFMW_CTX_ALIGN);

    return size;
}

td_u32 hevc_get_ctx_size(const td_void *ctx)
{
    const hevc_ctx *ctx_tmp = (hevc_ctx *)ctx;
    td_u32 ret;

    if (ctx_tmp == TD_NULL) {
        dprint(PRN_ERROR, "HEVC is not init\n");
        return 0;
    }

    ret = ctx_tmp->hevc_ctx_size;

    return ret;
}

td_s32 hevc_get_parse_info(const td_void *ctx_ext, td_void *args_ext)
{
    const hevc_ctx *ctx = (hevc_ctx *)ctx_ext;
    syn_syntax_info *info = (syn_syntax_info *)args_ext;

    vfmw_check_ret(info != TD_NULL, SYN_ERR);
    vfmw_check_ret(ctx != TD_NULL, SYN_ERR);

    info->profile = ctx->general_profile_idc;
    info->level = ctx->general_level_idc;
    info->spec_invalid = ctx->spec_invalid;
    info->spec_value = ctx->spec_value;

    return SYN_OK;
}

STATIC VFMW_ACC_SLICE_H265 td_void hevc_calc_slice_cost(hevc_ctx *ctx, td_u32 cost)
{
    if (ctx->curr_slice.dependent_slice_segment_flag == 1) {
        ctx->slc_cost.dependent_cost += cost;
        ctx->slc_cost.dependent_num++;
        if (ctx->slc_cost.dependent_max_cost < cost) {
            ctx->slc_cost.dependent_max_cost = cost;
        }
        if (ctx->slc_cost.dependent_min_cost > cost) {
            ctx->slc_cost.dependent_min_cost = cost;
        }
    } else {
        ctx->slc_cost.independent_cost += cost;
        ctx->slc_cost.independent_num++;
        if (ctx->slc_cost.independent_max_cost < cost) {
            ctx->slc_cost.independent_max_cost = cost;
        }
        if (ctx->slc_cost.independent_min_cost > cost) {
            ctx->slc_cost.independent_min_cost = cost;
        }
    }
}

VFMW_ACC_SLICE_H265 syn_parse_state hevc_decode(stm_seg_info *seg, dps_info *dps, td_void *ctx)
{
    td_s32 decode_result;
    syn_parse_state ret;
    hevc_ctx *ctx_tmp = (hevc_ctx *)ctx;
    hevc_stream_packet hevc_pack;
    td_u32 time_begin = 0;
    td_u32 slice_num = 0;
    td_u32 time_end;

    vfmw_check_ret(ctx_tmp != TD_NULL, SYN_PARSE_RESOURCE_INVALID);

    if (ctx_tmp->slc_cost.calc_slc_num > 0) {
        slice_num = ctx_tmp->num_slice_para;
        time_begin = os_get_time_us();
    }

    vfmw_check_ret(seg != TD_NULL, SYN_PARSE_RESOURCE_INVALID);
    vfmw_check_ret(dps != TD_NULL, SYN_PARSE_RESOURCE_INVALID);

    hevc_pack.p_stream = seg->vir_addr;
    hevc_pack.strm_phy_addr = seg->phy_addr;
    hevc_pack.stream_len = (td_s32)seg->length;
    hevc_pack.last_pack_in_nal = seg->is_last_seg;
    hevc_pack.stream_id = seg->stream_id;
    hevc_pack.raw_stream_len = (td_s32)seg->raw_len;
    hevc_pack.seg_cut_state = seg->cut_result;

    ctx_tmp->seg_type = STM_SEG_INVALID;
    ctx_tmp->seg_base_phy_addr = seg->seg_base_phy_addr;

    hevc_process_pts(ctx_tmp, seg);

    ctx_tmp->curr_dps = dps;

    decode_result = hevc_decode_packet(ctx_tmp, &hevc_pack);

    seg->seg_type = ctx_tmp->seg_type;

    if (decode_result == HEVC_DEC_ERR) {
        ret = SYN_PARSE_NOT_FRMDONE;
    } else {
        ret = decode_result;
    }

    if (ctx_tmp->is_stream_end_flag == 1) {
        hevc_get_remain_img(ctx_tmp);
        voq_last_frm(ctx_tmp->chan_id);
        ctx_tmp->is_stream_end_flag = 0;
    }

    hevc_proc_err_nal(ctx_tmp, seg, ret);

    if (ctx_tmp->slc_cost.calc_slc_num > 0) {
        if (ctx_tmp->num_slice_para != slice_num &&
            (ctx_tmp->slc_cost.dependent_num + ctx_tmp->slc_cost.independent_num) < ctx_tmp->slc_cost.calc_slc_num) {
            time_end = os_get_time_us();
            hevc_calc_slice_cost(ctx_tmp, time_end - time_begin);
        }
    }

    return ret;
}

td_s32 hevc_deinit(td_void *ctx, td_void *extra_data_ptr)
{
    hevc_ctx *ctx_tmp = (hevc_ctx *)ctx;
    td_s32 cnt;
    mem_record mem = { 0 };

    vfmw_check_ret(ctx != TD_NULL, HEVC_DEC_ERR);

    vfmw_unused(extra_data_ptr);
    ctx_tmp->is_init = 0;

    for (cnt = 0; cnt < HEVC_MEM_NUM; cnt++) {
        if (ctx_tmp->mem_list[cnt].mem_addr != TD_NULL) {
            mem.vir_addr = (td_u8 *)(ctx_tmp->mem_list[cnt].mem_addr);
            mem.is_cached = 1;
            os_free(&mem);
            ctx_tmp->mem_list[cnt].mem_addr = TD_NULL;
        }
    }

    mem.vir_addr = (td_u8 *)(ctx_tmp);
    mem.is_cached = 1;
    os_free(&mem);

    return HEVC_DEC_NORMAL;
}

td_s32 hevc_config(td_void *ctx, const syn_cfg_info *extra_data_ptr)
{
    hevc_ctx *ctx_tmp = (hevc_ctx *)ctx;
    td_s32 max_width, max_height;

    vfmw_check_ret((ctx != TD_NULL), HEVC_DEC_ERR);
    vfmw_check_ret((extra_data_ptr != TD_NULL), HEVC_DEC_ERR);

    max_width = extra_data_ptr->max_width;
    max_height = extra_data_ptr->max_height;
    if (extra_data_ptr->is_adaptive == 1) {
        max_width = ctx_tmp->extra_data.max_width > extra_data_ptr->max_width ?
            ctx_tmp->extra_data.max_width : extra_data_ptr->max_width;
        max_height = ctx_tmp->extra_data.max_height > extra_data_ptr->max_height ?
            ctx_tmp->extra_data.max_height : extra_data_ptr->max_height;
    }
    vfmw_check_sec_func(memcpy_s(&ctx_tmp->extra_data, sizeof(ctx_tmp->extra_data),
        extra_data_ptr, sizeof(syn_cfg_info)));
    ctx_tmp->extra_data.max_width = max_width;
    ctx_tmp->extra_data.max_height = max_height;
    hevc_config_max_para(&ctx_tmp->extra_data);
    ctx_tmp->max_vps_num = ctx_tmp->extra_data_ptr->max_vps_num;
    ctx_tmp->max_sps_num = ctx_tmp->extra_data_ptr->max_sps_num;
    ctx_tmp->max_pps_num = ctx_tmp->extra_data_ptr->max_pps_num;
    ctx_tmp->max_slice_num = ctx_tmp->extra_data_ptr->max_slice_num;
    ctx_tmp->max_slot_num = ctx_tmp->max_slice_num + 5; /* 5 is slot num */
    ctx_tmp->slc_cost.calc_slc_num = ctx_tmp->extra_data_ptr->calc_perf_slc_num;

    return HEVC_DEC_NORMAL;
}

STATIC td_void hevc_reset_report_info(hevc_ctx *ctx, vfmw_chan_reset_option *rst_opt)
{
    if (fsp_get_frame_buf_mode(ctx->chan_id) == VFMW_FRM_ALLOC_OUTER &&
        rst_opt != TD_NULL && rst_opt->clear_report_info == 1) {
        ctx->prev_pic_width = 0;
        ctx->prev_pic_height = 0;
        ctx->prev_dps_size = 0;
        ctx->prev_bit_depth_l = 0;
        ctx->prev_bit_depth_c = 0;
        ctx->prev_l2_ctb_size_y = 0;
    }
    return;
}

td_s32 hevc_reset(td_void *ctx, td_void *args)
{
    td_s32 cnt;
    hevc_ctx *ctx_tmp = (hevc_ctx *)ctx;
    vfmw_chan_reset_option *rst_opt = (vfmw_chan_reset_option *)args;

    vfmw_check_ret((ctx != TD_NULL), HEVC_DEC_ERR);

    if (rst_opt == TD_NULL || rst_opt->keep_sps_pps == 0) {
        for (cnt = 0; cnt < HEVC_MEM_NUM; cnt++) {
            if (ctx_tmp->mem_list[cnt].mem_addr != TD_NULL) {
                vfmw_check_sec_func(memset_s(ctx_tmp->mem_list[cnt].mem_addr,
                                             ctx_tmp->mem_list[cnt].mem_size, 0, ctx_tmp->mem_list[cnt].mem_size));
            }
        }
    }

    ctx_tmp->curr_pic.state = HEVC_PIC_EMPTY;
    ctx_tmp->curr_vps = TD_NULL;
    ctx_tmp->curr_sps = TD_NULL;
    ctx_tmp->curr_pps = TD_NULL;
    ctx_tmp->sei_sps = TD_NULL;

    ctx_tmp->first_i_flag = 0;
    ctx_tmp->need_clear_que = 1;

    for (cnt = 0; cnt < HEVC_MAX_NUM_LAYERS; cnt++) {
        ctx_tmp->last_display_poc[cnt] = -HEVC_MAX_INT;
    }

    hevc_init_dec_para(ctx_tmp);

    ctx_tmp->first_frame_state = FIRST_FRAME_INIT;
    ctx_tmp->prev_poc = 9999; /* 9999 is init poc */
    ctx_tmp->prev_vps_id = HEVC_MAX_VPS_NUM;
    ctx_tmp->prev_sps_id = HEVC_MAX_SPS_NUM;
    ctx_tmp->prev_pps_id = HEVC_MAX_PPS_NUM;
    ctx_tmp->prev_slc_pps_id = HEVC_MAX_PPS_NUM;
    ctx_tmp->slc_cost.independent_cost = 0;
    ctx_tmp->slc_cost.independent_num = 0;
    ctx_tmp->slc_cost.dependent_cost = 0;
    ctx_tmp->slc_cost.dependent_num = 0;
    ctx_tmp->slc_cost.independent_min_cost = 0xFFFFFFFF;
    ctx_tmp->slc_cost.dependent_min_cost = 0xFFFFFFFF;
    ctx_tmp->slc_cost.independent_max_cost = 0;
    ctx_tmp->slc_cost.dependent_max_cost = 0;
    hevc_reset_report_info(ctx_tmp, rst_opt);
#if defined(VFMW_HEVC_SLC_LDY_SUPPORT)
    hevc_init_ldy_info(ctx_tmp);
#endif

    return HEVC_DEC_NORMAL;
}

td_void hevc_read_log(const td_void *args, td_void *buf, td_s32 len, td_s32 *used)
{
    hevc_ctx *ctx = (hevc_ctx *)args;
    hevc_profile_tier_level *ptl = TD_NULL;

    if (ctx == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "syn context not exists.%-56s|\n", " ");
        return;
    }

    if (ctx->curr_sps == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "curr_sps is null.%-62s|\n", " ");
        return;
    }

    if (ctx->curr_pps == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "curr_pps is null.%-62s|\n", " ");
        return;
    }

    ptl = &ctx->curr_sps->profile_tier_level;

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "profile", ptl->general_profile_idc,
                 "level", ptl->general_level_idc);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "tier", ptl->general_tier_flag,
                 "bit_depth_luma", ctx->curr_sps->bit_depth_luma);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "dpb_size", ctx->curr_sps->dpb_size,
                 "bit_depth_chroma", ctx->curr_sps->bit_depth_chroma);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "left_offset", ctx->curr_sps->conf_win_left_offset,
                 "top_offset", ctx->curr_sps->conf_win_top_offset);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "right_offset", ctx->curr_sps->conf_win_right_offset,
                 "bottom_offset", ctx->curr_sps->conf_win_bottom_offset);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "tiles_enable", ctx->curr_pps->tiles_enabled_flag,
                 "tile_rows", ctx->curr_pps->num_tile_rows);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "pps_refresh", ctx->curr_pps->is_refresh,
                 "tile_cols", ctx->curr_pps->num_tile_columns);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "dpnt_max_cost", ctx->slc_cost.dependent_max_cost,
                 "dpnt_min_cost", ctx->slc_cost.dependent_min_cost);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "dpnt_slice_num", ctx->slc_cost.dependent_num,
                 "dpnt_avg_cost", (ctx->slc_cost.dependent_num == 0) ? (-1) :
                 ctx->slc_cost.dependent_cost / ctx->slc_cost.dependent_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "indpnt_max_cost", ctx->slc_cost.independent_max_cost,
                 "indpnt_min_cost", ctx->slc_cost.independent_min_cost);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "indpnt_slice_num", ctx->slc_cost.independent_num,
                 "indpnt_avg_cost", (ctx->slc_cost.independent_num == 0) ? (-1) :
                 ctx->slc_cost.independent_cost / ctx->slc_cost.independent_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "aspect_width", ctx->curr_pic.fs_image.aspect_width,
                 "aspect_height", ctx->curr_pic.fs_image.aspect_height);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "afd_format_flag", ctx->curr_pic.fs_image.afd.active_format_flag,
                 "afd_format", ctx->curr_pic.fs_image.afd.active_format);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "is_adaptive", ctx->extra_data.is_adaptive, "actual_max_width",
                 ctx->extra_data.max_width);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "actual_max_height", ctx->extra_data.max_height, " ");
}

STATIC td_s32 hevc_dec_cfg_fffb_speed(td_void *ctx, const td_void *args)
{
    td_s32 *fffb_speed = (td_s32 *)args;
    hevc_ctx *ctx_tmp = (hevc_ctx *)ctx;

    vfmw_check_ret_print(fffb_speed != TD_NULL, SYN_ERR, "%s fffb_speed is TD_NULL\n", __func__);

    ctx_tmp->extra_data.fast_forward_backward_speed = *fffb_speed;

    return SYN_OK;
}

#ifdef HEVC_PVR_SUPPORT
STATIC td_s32 hevc_dec_cfg_pvr_info(td_void *ctx, const td_void *args)
{
    hevc_ctx *ctx_tmp = (hevc_ctx *)ctx;
    vfmw_control_info *pvr_info = (vfmw_control_info *)args;

    vfmw_check_ret_print(pvr_info != TD_NULL, SYN_ERR, "%s pvr_info is TD_NULL\n", __func__);

    vfmw_check_sec_func(memcpy_s(&ctx_tmp->extra_data.pvr_info, sizeof(ctx_tmp->extra_data.pvr_info),
                                 pvr_info, sizeof(vfmw_control_info)));

    return SYN_OK;
}
#endif

td_s32 hevc_control(td_void *ctx, td_s32 cmd_id, const td_void *args)
{
    td_s32 ret;

    vfmw_check_ret((ctx != TD_NULL), HEVC_DEC_ERR);

    switch (cmd_id) {
        case SYNCTX_CMD_CFG_FFFB_SPEED:
            ret = hevc_dec_cfg_fffb_speed(ctx, args);
            break;

#ifdef HEVC_PVR_SUPPORT
        case SYNCTX_CMD_CFG_PVR_INFO:
            ret = hevc_dec_cfg_pvr_info(ctx, args);
            break;
#endif

        default:
            ret = SYN_ERR;
            dprint(PRN_ERROR, "Unsupport cmd:%d\n", cmd_id);
            break;
    }

    return ret;
}
