/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "vfmw_osal.h"
#include "stm_log.h"
#include "syn_log.h"
#include "dec_log.h"
#include "fsp_log.h"
#include "dps_log.h"
#include "voque_log.h"
#include "dbg.h"
#include "vctrl.h"
#include "vfmw_pdt.h"

STATIC td_void vct_log_set_print_level(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), (td_u32 *)&data); /* 1 means 2nd param */

    (td_void)dbg_set_print_word(data);
    dprint(PRN_ALWS, "set print level 0x%x\n", data);
}

STATIC td_void vct_log_set_dps_ser_mode(cmd_str_ptr buffer, td_u32 count)
{
    dps_log_write(buffer, (td_s32)count);
}

STATIC td_void vct_log_set_dps_num(cmd_str_ptr buffer, td_u32 count)
{
    dps_log_write(buffer, (td_s32)count);
}

STATIC td_void vct_log_set_dps_no_recv(cmd_str_ptr buffer, td_u32 count)
{
    dps_log_write(buffer, (td_s32)count);
}

STATIC td_void vct_log_set_err_threshold(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    vctrl_dflt *dflt = vctrl_get_dflt();

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data); /* 1 means 2nd param */

    vfmw_check_print(data >= 0 && data <= 100, "err_threshold %d out of range[0,100].\n", data); // 100, max threshold

    dflt->err_threshold = (td_s8)data;

    dprint(PRN_ALWS, "set err_threshold = %u.\n", data);
}

STATIC td_void vct_log_set_cmp_mode(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    vctrl_dflt *dflt = vctrl_get_dflt();

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data); /* 1 means 2nd param */

    vfmw_check_print(data < VFMW_CMP_MAX, "cmp_mode %d out of range[0,%d).\n", data, VFMW_CMP_MAX);

    dflt->cmp_mode = (td_s32)data;
    dprint(PRN_ALWS, "set cmp_mode = %u.\n", data);
}

STATIC td_void vct_log_set_frm_layout(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    vctrl_dflt *dflt = vctrl_get_dflt();

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data); /* 1 means 2nd param */

    vfmw_check_print(data < VFMW_FRM_LAYOUT_MAX, "frm_layout %d out of range[0,%d).\n", data, VFMW_FRM_LAYOUT_MAX);

    dflt->frm_layout = (td_s32)data;
    dprint(PRN_ALWS, "set frm_layout = %u.\n", data);
}

STATIC td_void vct_log_set_tunnel_line_num(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    vctrl_dflt *dflt = vctrl_get_dflt();

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data); /* 1 means 2nd param */

    dflt->tunnel_line_num = (td_s32)data;
    dprint(PRN_ALWS, "set tunnel_line_num = %u.\n", data);
}

STATIC td_void vct_log_set_module_lowdelay_en(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    vctrl_dflt *dflt = vctrl_get_dflt();

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data); /* 1 means 2nd param */

    vfmw_check_print(data == 0 || data == 1, "mod_lowdelay_en %d out of range[0,1].\n", data);

    dflt->mod_lowdelay_en = (td_s8)data;
    dprint(PRN_ALWS, "set mod_lowdelay_en = %u.\n", data);
}

STATIC td_void vct_log_set_output_order(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    vctrl_dflt *dflt = vctrl_get_dflt();

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data); /* 1 means 2nd param */

    vfmw_check_print(data < VFMW_OUTPUT_ORDER_MAX, "out_order %d out of range[0,%d).\n", data, VFMW_OUTPUT_ORDER_MAX);

    dflt->out_order = (td_s32)data;
    dprint(PRN_ALWS, "set out_order = %u.\n", data);
}

STATIC td_void vct_log_set_dec_mode(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    vctrl_dflt *dflt = vctrl_get_dflt();

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data); /* 1 means 2nd param */

    vfmw_check_print(data < VFMW_DEC_MODE_MAX, "dec_mode %d out of range[0,%d).\n", data, VFMW_DEC_MODE_MAX);

    dflt->dec_mode = (td_s32)data;
    dprint(PRN_ALWS, "set dec_mode = %u.\n", data);
}

STATIC td_void vct_log_set_linear_no_gap(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    vctrl_dflt *dflt = vctrl_get_dflt();

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data); /* 1 means 2nd param */

    vfmw_check_print(data == TD_FALSE || data == TD_TRUE, "linear_no_gap %d out of range[0,1].\n", data);

    dflt->linear_no_gap_en = (td_s32)data;
    dprint(PRN_ALWS, "set linear_no_gap_en = %u.\n", data);
}

STATIC td_void vct_log_set_clock(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data); /* 1 means 2nd param */

    pdt_set_attr(ATTR_SET_CLK, &data);
}

STATIC td_void vct_log_show_default(cmd_str_ptr buffer, td_u32 count)
{
    vctrl_dflt *dflt = vctrl_get_dflt();

    vfmw_unused(buffer);
    vfmw_unused(count);

    vfmw_check(dflt != TD_NULL);

#define show_dflt(val) dprint(PRN_ALWS, "  %s = %d\n", #val, val)

    show_dflt(dflt->dec_mode);
    show_dflt(dflt->err_threshold);
    show_dflt(dflt->out_order);
    show_dflt(dflt->fix_frame_num);
    show_dflt(dflt->fix_frame_width);
    show_dflt(dflt->fix_frame_height);
    show_dflt(dflt->sedldy_en);
    show_dflt(dflt->mod_lowdelay_en);
    show_dflt(dflt->tunnel_line_num);
    show_dflt(dflt->linear_no_gap_en);
}

STATIC const log_cmd com_cmd[] = {
    { LOG_CMD_PRINT,         vct_log_set_print_level },
    { LOG_CMD_DPS_SER,       vct_log_set_dps_ser_mode },
    { LOG_CMD_DPS_NUM,       vct_log_set_dps_num },
    { LOG_CMD_DPS_NO_RECV,   vct_log_set_dps_no_recv },
    { LOG_CMD_ERR_HOLD,      vct_log_set_err_threshold },
    { LOG_CMD_CMP_MODE,      vct_log_set_cmp_mode },
    { LOG_CMD_FRM_LAYOUT,    vct_log_set_frm_layout },
    { LOG_CMD_OUT_ORDER,     vct_log_set_output_order },
    { LOG_CMD_DEC_MODE,      vct_log_set_dec_mode },
    { LOG_CMD_MOD_LOW_DLY,   vct_log_set_module_lowdelay_en },
    { LOG_CMD_TUNL_LINE,     vct_log_set_tunnel_line_num },
    { LOG_CMD_SET_CLK,       vct_log_set_clock },
    { LOG_CMD_LINEAR_NO_GAP, vct_log_set_linear_no_gap},
    { LOG_CMD_SHOW_DFLT,     vct_log_show_default },
};

STATIC td_void vct_log_handle_cmd(cmd_str_ptr cmd_str, td_u32 cmd_len)
{
    td_s32 i;
    td_s32 count;
    cmd_handler handler = TD_NULL;
    td_char *cmd = (td_char *)((*cmd_str)[0]);

    count = (td_s32)(sizeof(com_cmd) / sizeof(log_cmd));
    vfmw_check((os_strncmp != TD_NULL) && (os_strlen != TD_NULL));

    for (i = 0; i < count; i++) {
        if (os_strncmp(cmd, com_cmd[i].cmd_name, os_strlen(cmd)) == 0) {
            handler = com_cmd[i].handler;
            break;
        }
    }

    if (handler == TD_NULL) {
        dprint(PRN_ALWS, "Unsupport command.\n");
        return;
    }

    handler(cmd_str, cmd_len);
}

STATIC td_s32 vct_log_write(cmd_str_ptr buffer, td_s32 count)
{
    if (buffer == TD_NULL || count <= 0) {
        return EXT_FAILURE;
    }

    vct_log_handle_cmd(buffer, (td_u32)count);

    return EXT_SUCCESS;
}

STATIC td_void vct_log_read_chan(td_void *buf, td_s32 chan_id, td_s32 *used, td_s32 len)
{
    vfmw_adpt_type type;
    vctrl_chan *chan = TD_NULL;
    td_char *adpt_str[ADPT_TYPE_MAX + 1] = { "VDEC", "NA" };
    td_char *frame_layout[VFMW_FRM_LAYOUT_MAX + 1] = {"ADPT", "TILE", "LINEAR", "MAX"};

    os_dump_proc(buf, len, used, 0, LOG_COM_FMT_HEAD);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "vfmw_version", VFMW_VERSION, " ");

    chan = vctrl_get_chan(chan_id);
    if (chan == TD_NULL || chan->handle == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "vctrl chan %d not exists.%-55s|\r\n", chan_id, " ");
        return;
    }

    type = (chan->opt.adpt_type < ADPT_TYPE_MAX) ? chan->opt.adpt_type : ADPT_TYPE_MAX;

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "chan_id", chan->id,
                 "vid_std", dbg_show_std(chan->cfg.vid_std, &chan->cfg.std_ext));
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_N, "unid", chan->unid, " ");
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_P, "ref_cnt", chan->ref_cnt, "handle", chan->handle);
    os_dump_proc(buf, len, used, 0, LOG_FMT_S_D, "adpt_type", adpt_str[type], "is_sec_mode",
                 chan->opt.is_sec_mode);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "err_ratio", chan->cfg.chan_err_thr, "crc_mode",
                 chan->cfg.crc_mode);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "dec_mode", chan->cfg.dec_mode, "order_output",
                 chan->cfg.dec_order_output);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "eop_en", chan->cfg.chan_lowly_enable, "low_delay",
                 chan->cfg.module_lowly_enable);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "cmp_mode", chan->cfg.v_cmp_mode, "priority",
                 chan->cfg.chan_priority);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "dec_ldy", chan->cfg.dec_lowly_enable, "bit_depth_cfg",
                 chan->cfg.en_bit_depth_cfg);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "is_hdr", 0, "compatible",
                 chan->cfg.dv_compatible);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "is_omx_path", 0, "is_tunneled",
                 chan->cfg.is_tunneled_path);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "is_an_test", chan->cfg.android_test, "uv_mos",
                 chan->cfg.uvmos_enable);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "source_type", chan->cfg.source, "support_all_p",
                 chan->cfg.support_all_p);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "is_miracast", chan->cfg.is_miracast_chan, "extra_fs_num",
        chan->cfg.extra_frame_store_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_S_N, "frm_layout", frame_layout[chan->cfg.frame_layout], " ");
}

STATIC td_s32 vct_log_read(td_void *buf, td_s32 chan_id, td_s32 len)
{
    td_s32 used = 0;

    if (chan_id < 0 || chan_id >= VFMW_CHAN_NUM) {
        return EXT_FAILURE;
    }

    (td_void)vct_log_read_chan(buf, chan_id, &used, len);
    (td_void)dps_log_read(buf, chan_id, &used, len);
    (td_void)voq_log_read(buf, chan_id, &used, len);

    return EXT_SUCCESS;
}

td_s32 vctrl_log_read(td_s32 module, td_s32 chan_id, td_void *buffer, td_s32 page_size)
{
    vfmw_check_ret(buffer != TD_NULL, EXT_FAILURE);

    switch (module) {
        case LOG_MODULE_COM:
            (td_void)vct_log_read(buffer, chan_id, page_size);
            break;
        case LOG_MODULE_FSP:
            (td_void)fsp_log_read(buffer, chan_id, page_size);
            break;
        case LOG_MODULE_STM:
            (td_void)stm_log_read(buffer, chan_id, page_size);
            break;
        case LOG_MODULE_SYN:
            (td_void)syn_log_read(buffer, chan_id, page_size);
            break;
        case LOG_MODULE_DEC:
            (td_void)dec_log_read(buffer, chan_id, page_size);
            break;
        default:
            break;
    }

    return EXT_SUCCESS;
}

td_s32 vctrl_log_write(td_s32 module, cmd_str_ptr buffer, td_s32 count)
{
    td_s32 ret;

    ret = 0;
    switch (module) {
        case LOG_MODULE_STM:
            ret = stm_log_write(buffer, (td_u32)count);
            break;
        case LOG_MODULE_SYN:
            ret = syn_log_write(buffer, (td_u32)count);
            break;
        case LOG_MODULE_DEC:
            ret = dec_log_write(buffer, (td_u32)count);
            break;
        case LOG_MODULE_FSP:
            ret = fsp_log_write(buffer, (td_u32)count);
            break;
        case LOG_MODULE_COM:
            ret = vct_log_write(buffer, count);
            break;
        default:
            break;
    }

    return ret;
}
