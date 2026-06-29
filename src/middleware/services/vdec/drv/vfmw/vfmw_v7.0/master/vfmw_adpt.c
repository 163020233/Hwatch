/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "vfmw_adpt.h"
#include "vfmw_intf.h"
#include "dbg.h"
#include "vfmw_pdt.h"
#include "vfmw_define.h"
#include "vfmw_adpt_proc.h"
#include "pts.h"
#include "soc_osal.h"
#include "liteos_mem.h"
#include "pm_veto.h"
#include "drv_gpu_intf.h"
#include "vdec_chr.h"
#ifdef VFMW_VMM_SUPPORT
#include "vmm_intf.h"
#endif
#ifdef VFMW_IPT_SUPPORT
#include "ip_test.h"
#endif
#include "vfmw_img.h"
#include "pts_ext.h"
#ifdef VFMW_LOCAL_SUPPORT
#include "local.h"
#endif
#ifdef VFMW_USER_SUPPORT
#include "userdec.h"
#endif
#ifdef VFMW_VPP_SUPPORT
#include "vpp.h"
#endif
#ifdef VFMW_STREAM_SUPPORT
#include "stream.h"
#endif

#define RESERVE_DATA 0x1234abcd

static vfmw_entry g_s_vfmw_entry = {
    .reserve1 = RESERVE_DATA,
    .state = VFMW_STATE_NULL,
    .reserve2 = RESERVE_DATA,
    .dflt.save_begin = 0,
    .dflt.save_end = 0xffffffff,
};

static td_bool g_s_vfmw_load = TD_FALSE;

vfmw_entry *vadpt_entry(td_void)
{
    return &g_s_vfmw_entry;
}

vfmw_dflt *vadpt_get_dflt(td_void)
{
    return &g_s_vfmw_entry.dflt;
}

vfmw_chan *vadpt_chan(td_s32 chan_id)
{
    if (chan_id < 0 || chan_id >= VFMW_CHAN_NUM) {
        return TD_NULL;
    }

    return g_s_vfmw_entry.chan + chan_id;
}

STATIC td_void vadpt_sema_down(td_void)
{
    os_sema_down(g_s_vfmw_entry.sema);
}

STATIC td_void vadpt_sema_up(td_void)
{
    os_sema_up(g_s_vfmw_entry.sema);
}

STATIC td_void vadpt_set_load(td_bool val)
{
    g_s_vfmw_load = val;
}

STATIC td_bool vadpt_get_load(td_void)
{
    return g_s_vfmw_load;
}

#ifdef VFMW_VMM_SUPPORT
STATIC td_s32 vadpt_vmm_cb_alloc(const td_void *param_out, td_s32 ret_val)
{
    td_s32 ret;
    td_s32 chan_id;
    const vmm_buffer *vmm_buf = TD_NULL;
    vfmw_mem_report mem_report;

    vfmw_check_ret(param_out != TD_NULL, VMM_FAILURE);

    vmm_buf = (vmm_buffer *)param_out;

    if (ret_val != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s name %s failed with ret_val %x\n", __func__, vmm_buf->buf_name, ret_val);
        return VMM_FAILURE;
    }

    vfmw_check_sec_func(memset_s(&mem_report, sizeof(vfmw_mem_report), 0, sizeof(vfmw_mem_report)));

    if (snprintf_s(mem_report.buf_name, VFMW_REPORT_MAX_NAME_LEN,
                   VFMW_REPORT_MAX_NAME_LEN - 1, "%s", vmm_buf->buf_name) < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    mem_report.buf_name[sizeof(mem_report.buf_name) - 1] = '\0';
    mem_report.dma_buf = vmm_buf->dma_buf;
    mem_report.is_map = vmm_buf->map;
    mem_report.is_cache = vmm_buf->cache;
    mem_report.size = vmm_buf->size;
    mem_report.unique_id = vmm_buf->priv_id;
    mem_report.phy_addr = vmm_buf->start_phy_addr;
    mem_report.vir_addr = vmm_buf->start_vir_addr;

    chan_id = (td_s32)get_chan_id_by_uid(mem_report.unique_id);

    ret = vfmw_control(chan_id, VFMW_CID_BIND_MEM, &mem_report, sizeof(vfmw_mem_report));

    return (ret == EXT_SUCCESS) ? VMM_SUCCESS : VMM_FAILURE;
}

STATIC td_s32 vadpt_vmm_cb_release(const td_void *param_out, td_s32 ret_val)
{
    const vmm_buffer *vmm_buf = TD_NULL;

    vfmw_check_ret(param_out != TD_NULL, VMM_FAILURE);

    vmm_buf = (vmm_buffer *)param_out;

    if (ret_val != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s name %s failed with ret_val %x\n", __func__, vmm_buf->buf_name, ret_val);
        return VMM_FAILURE;
    }

    return VMM_SUCCESS;
}

STATIC td_s32 vadpt_vmm_cb_release_burst(const td_void *param_out, td_s32 ret_val)
{
    td_s64 *ppriv_id = TD_NULL;

    vfmw_check_ret(param_out != TD_NULL, VMM_FAILURE);

    ppriv_id = (td_s64 *)param_out;

    if (ret_val != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s priv_id failed with ret_val %x\n", __func__, ret_val);
        return VMM_FAILURE;
    }

    return VMM_SUCCESS;
}

STATIC td_s32 vadpt_vmm_cb_cancle(const td_void *param_out, td_s32 ret_val)
{
    td_s64 *ppriv_id = TD_NULL;

    vfmw_check_ret(param_out != TD_NULL, VMM_FAILURE);

    ppriv_id = (td_s64 *)param_out;

    if (ret_val != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s priv_id failed with ret_val %x\n", __func__, ret_val);
        return VMM_FAILURE;
    }

    return VMM_SUCCESS;
}

td_s32 vadpt_vmm_call_back(vmm_cmd_type cmd_id, const td_void *param_out, td_s32 ret_val)
{
    td_s32 ret;

    ret = VMM_FAILURE;
    switch (cmd_id) {
        case VMM_CMD_ALLOC:
            ret = vadpt_vmm_cb_alloc(param_out, ret_val);
            break;

        case VMM_CMD_RELEASE:
            ret = vadpt_vmm_cb_release(param_out, ret_val);
            break;

        case VMM_CMD_RELEASE_BURST:
            ret = vadpt_vmm_cb_release_burst(param_out, ret_val);
            break;

        case VMM_CMD_CANCLE:
            ret = vadpt_vmm_cb_cancle(param_out, ret_val);
            break;

        default:
            dprint(PRN_ERROR, "%s unsupport cmd_id %d\n", __func__, cmd_id);
            break;
    }

    return ret;
}

STATIC td_s32 vadpt_vmm_set_image_out(const vfmw_image *image)
{
    td_s32 ret;
    vmm_attr tmp_location = { 0 };

    tmp_location.location = VMM_LOCATION_OUTSIDE;
    tmp_location.phy_addr = image->disp_info.luma_phy_addr;
    ret = vmm_send_command_block(VMM_CMD_BLK_SET_LOCATION, &tmp_location);
    if (ret != VMM_SUCCESS) {
        dprint(PRN_ERROR, "Set image phy 0x%x location %d failed.\n", tmp_location.phy_addr, tmp_location.location);
        goto error_exit_0;
    }

    if (image->image_id_1 != -1 && image->disp_info.luma_phy_addr_1 != 0) {
        tmp_location.location = VMM_LOCATION_OUTSIDE;
        tmp_location.phy_addr = image->disp_info.luma_phy_addr_1;
        ret = vmm_send_command_block(VMM_CMD_BLK_SET_LOCATION, &tmp_location);
        if (ret != VMM_SUCCESS) {
            dprint(PRN_ERROR, "Set image view 1 phy 0x%x location %d failed.\n",
                tmp_location.phy_addr, tmp_location.location);
            goto error_exit_2;
        }
    }

    return EXT_SUCCESS;

error_exit_2:

error_exit_0:

    return EXT_FAILURE;
}

STATIC td_s32 vadpt_vmm_set_image_in(const vfmw_image *image)
{
    td_s32 ret = EXT_FAILURE;
    vmm_attr tmp_location = { 0 };

    if (image->disp_info.luma_phy_addr != 0) {
        tmp_location.location = VMM_LOCATION_INSIDE;
        tmp_location.phy_addr = image->disp_info.luma_phy_addr;
        ret = vmm_send_command_block(VMM_CMD_BLK_SET_LOCATION, &tmp_location);
        if (ret != VMM_SUCCESS) {
            dprint(PRN_ERROR, "Set image phy 0x%x location %d failed.\n", tmp_location.phy_addr, tmp_location.location);
            return EXT_FAILURE;
        }
    }

    if (image->image_id_1 != -1 && image->disp_info.luma_phy_addr_1 != 0) {
        tmp_location.location = VMM_LOCATION_INSIDE;
        tmp_location.phy_addr = image->disp_info.luma_phy_addr_1;
        ret = vmm_send_command_block(VMM_CMD_BLK_SET_LOCATION, &tmp_location);
        if (ret != VMM_SUCCESS) {
            dprint(PRN_ERROR, "Set image view 1 phy 0x%x location %d failed.\n",
                tmp_location.phy_addr, tmp_location.location);
            return EXT_FAILURE;
        }
    }

    return (ret == VMM_SUCCESS) ? EXT_SUCCESS : EXT_FAILURE;
}
#endif

STATIC vfmw_type vadpt_get_vfmw_type(td_s32 chan_id, vfmw_cid cmd_id, const td_void *args)
{
    vfmw_type type;
    vfmw_chan *chan = TD_NULL;
    td_s32 sec_flag = 0;
    td_s32 user_flag = 0;

    type = VFMW_TYPE_LOCAL;

    switch (cmd_id) {
        case VFMW_CID_CREATE_CHAN:
        case VFMW_CID_GET_CHAN_MEMSIZE:
            if (args != TD_NULL) {
                const vfmw_chan_option *opt = (vfmw_chan_option *)args;

                sec_flag = opt->is_sec_mode;
                user_flag = (opt->vid_std == VFMW_MJPEG);
            }

            if (sec_flag != 0) {
                type = VFMW_TYPE_TEE;
            }

            if (user_flag != 0) {
                type = VFMW_TYPE_USER;
            }

            if (cmd_id == VFMW_CID_CREATE_CHAN) {
                chan = vadpt_chan(chan_id);
                if (chan != TD_NULL) {
                    chan->type = type;
                    chan->is_sec = (sec_flag == 0) ? TD_FALSE : TD_TRUE;
                }
            }
            break;

        case VFMW_CID_ALLOC_SEC_BUF:  /* use default type */
        case VFMW_CID_FREE_SEC_BUF:
        case VFMW_CID_GET_CAPABILITY:
            break;

        default:
            chan = vadpt_chan(chan_id);
            if (chan != TD_NULL) {
                type = chan->type;
            }
            break;
    }

    return type;
}

STATIC td_bool vadpt_is_chan_valid(td_s32 chan_id, vfmw_cid cmd_id)
{
    if (cmd_id == VFMW_CID_GET_CAPABILITY ||
        cmd_id == VFMW_CID_GET_CHAN_MEMSIZE ||
        cmd_id == VFMW_CID_CREATE_CHAN ||
        cmd_id == VFMW_CID_ALLOC_SEC_BUF ||
        cmd_id == VFMW_CID_FREE_SEC_BUF) {
        return TD_TRUE; /* not care chan id in these cmds */
    }

    if (vadpt_chan(chan_id) != TD_NULL) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

STATIC td_s32 vadpt_new_chan_id(td_void)
{
    td_s32 id;
    vfmw_chan *chan = TD_NULL;

    for (id = 0; id < VFMW_CHAN_NUM; id++) {
        chan = vadpt_chan(id);
        if (chan != TD_NULL && chan->state == VFMW_CHAN_STATE_NULL) {
            break;
        }
    }

    return (id >= VFMW_CHAN_NUM) ? -1 : id;
}

STATIC td_s32 vadpt_get_chan_id(vfmw_cid cmd, td_s32 *chan_id)
{
    vfmw_chan *chan = TD_NULL;

    switch (cmd) {
        case VFMW_CID_GET_CAPABILITY:
        case VFMW_CID_GET_CHAN_MEMSIZE:
        case VFMW_CID_ALLOC_SEC_BUF:
        case VFMW_CID_FREE_SEC_BUF:
            *chan_id = 0;
            break;

        case VFMW_CID_CREATE_CHAN:
            /* for specify chanid */
            chan = vadpt_chan(*chan_id);
            if (chan != TD_NULL && chan->state == VFMW_CHAN_STATE_NULL) {
                chan->state = VFMW_CHAN_STATE_WAIT;
                return EXT_SUCCESS;
            }

            *chan_id = vadpt_new_chan_id();
            vfmw_check_ret(*chan_id != -1, EXT_FAILURE);
            chan = vadpt_chan(*chan_id);
            vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);
            chan->state = VFMW_CHAN_STATE_WAIT;
            break;
        default:
            break;
    }

    return EXT_SUCCESS;
}

#ifndef VFMW_LOW_POWER_SUPPORT
STATIC td_s32 vadpt_process_hardware(attr_id index)
{
    td_s32 id;
    vfmw_entry *entry = TD_NULL;
    vfmw_chan *chan = TD_NULL;

    entry = vadpt_entry();
    if (entry->ref_cnt != 0) {
        return EXT_SUCCESS;
    }

    for (id = 0; id < VFMW_CHAN_NUM; id++) {
        chan = vadpt_chan(id);
        if (chan != TD_NULL && chan->state != VFMW_CHAN_STATE_NULL) {
            return EXT_SUCCESS;
        }
    }

    return pdt_set_attr(index, TD_NULL); /* for codex no pass null */
}
#endif

#ifdef VFMW_STREAM_SUPPORT
STATIC td_s32 vadpt_queue_stream(td_s32 chan_id, td_void *args)
{
    td_s32 ret;
    td_u32 time;
    vfmw_chan *chan = TD_NULL;

    vfmw_check_ret(os_get_time_us != TD_NULL, EXT_FAILURE);
    time = os_get_time_us();
    chan = vadpt_chan(chan_id);

    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    ret = stream_queue_buffer(chan_id, (vfmw_stream_buf *)args);
    if (chan != TD_NULL &&
        ret == EXT_SUCCESS &&
        chan->kpi.questm.begin == 0) {
        chan->kpi.questm.begin = time;
        chan->kpi.questm.end = os_get_time_us();
        chan->kpi.questm.cost = chan->kpi.questm.end - chan->kpi.start.end;
    }

    return ret;
}
#endif

STATIC td_s32 vadpt_local_control_stream(td_s32 chan_id, vfmw_cid cmd_id, td_void *args, td_bool *is_local_case)
{
    td_s32 ret;

    *is_local_case = TD_TRUE;
    switch (cmd_id) {
#ifdef VFMW_STREAM_SUPPORT
        case VFMW_CID_QUEUE_STREAM:
            ret = vadpt_queue_stream(chan_id, args);
            break;
        case VFMW_CID_DEQUEUE_STREAM:
            ret = stream_dequeue_buffer(chan_id, (vfmw_stream_buf *)args);
            break;
        case VFMW_CID_INIT_ES_BUF:
            ret = stream_alloc_buffer(chan_id, args);
            break;
        case VFMW_CID_DEINIT_ES_BUF:
            ret = stream_free_buffer(chan_id);
            break;
        case VFMW_CID_BIND_ES_USR_ADDR:
            ret = stream_bind_usr_addr(chan_id, *((td_u64 *)args));
            break;
        case VFMW_CID_SEND_EOS:
            ret = stream_send_eos(chan_id);
            break;
        case VFMW_CID_SET_IDR_PTS:
            ret = stream_set_idr_pts(chan_id, *((td_s64 *)args));
            break;
#endif
        default:
            *is_local_case = TD_FALSE;
            ret = EXT_SUCCESS;
    }

    return ret;
}

STATIC td_s32 vadpt_local_control_part2(td_s32 chan_id, vfmw_cid cmd_id, td_void *args, td_bool *is_local_case)
{
    td_s32 ret;

    *is_local_case = TD_TRUE;
    ret = vadpt_local_control_stream(chan_id, cmd_id, args, is_local_case);

    return ret;
}

STATIC td_s32 vadpt_local_check_std(const vfmw_chan_cfg *cfg)
{
    pdt_license license = {0};
    const vfmw_product_info *pdt = TD_NULL;

    if (pdt_get_attr(ATTR_GET_LICENSE, &license) == EXT_SUCCESS &&
        ((cfg->vid_std == VFMW_AV1 && license.av1_en == 0) ||
         (cfg->vid_std == VFMW_AVS3 && license.avs3_en == 0))) {
        return EXT_FAILURE;
    }

    pdt = pdt_get_product_info();
    if ((pdt->opt_vp9_av1 == 1 && (cfg->vid_std == VFMW_VP9 || cfg->vid_std == VFMW_AV1)) ||
        (pdt->opt_real8_real9 == 1 && (cfg->vid_std == VFMW_REAL8 || cfg->vid_std == VFMW_REAL9))) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vadpt_local_control(td_s32 chan_id, vfmw_cid cmd_id, td_void *args, td_u32 len, td_bool *is_local_case)
{
    td_s32 ret = EXT_FAILURE;
    vfmw_chan_cfg *cfg = (vfmw_chan_cfg *)args;

    *is_local_case = TD_TRUE;

    vfmw_unused(len);
    switch (cmd_id) {
        case VFMW_CID_CFG_CHAN: {
            if (vadpt_local_check_std(cfg) != EXT_SUCCESS) {
                dprint(PRN_FATAL, "vfmw std %d not support\n", cfg->vid_std);
                ret = EXT_FAILURE;
            } else {
                *is_local_case = TD_FALSE;
            }
            break;
        }
        case VFMW_CID_SET_FRAME_RATE_TYPE: {
            vfmw_pts_frmrate *pts_frm_rate = (vfmw_pts_frmrate *)args;
            ret = pts_info_set_frm_rate(chan_id, pts_frm_rate);
            break;
        }
        case VFMW_CID_GET_FRAME_RATE_TYPE: {
            vfmw_pts_frmrate *pts_frm_rate = (vfmw_pts_frmrate *)args;
            ret = pts_info_get_frm_rate(chan_id, pts_frm_rate);
            break;
        }
#ifdef VFMW_VPP_SUPPORT
        case VFMW_CID_CHECK_FRM_STATE: {
            ret = vpp_check_frame_state(chan_id, args, len);
            break;
        }
#endif
        default: {
            ret = vadpt_local_control_part2(chan_id, cmd_id, args, is_local_case);
            break;
        }
    }

    return ret;
}


#ifdef VFMW_STREAM_SUPPORT
STATIC td_s32 vadpt_read_stream(td_s32 chan_id, vfmw_stream_data *packet)
{
    td_s32 ret;
    vfmw_stream_buf buffer = {0};

    ret = stream_read_buffer(chan_id, &buffer);
    if (ret == (td_s32)EXT_FAILURE) {
        return EXT_FAILURE;
    }

    vfmw_check_sec_func(memset_s(packet, sizeof(vfmw_stream_data), 0, sizeof(vfmw_stream_data)));

    if (ret == STREAM_EOS) {
        packet->is_stream_end_flag = 1;
        packet->is_not_last_packet_flag = 0;
        packet->pts = VFMW_INVALID_PTS;
    } else {
        packet->is_not_last_packet_flag = !buffer.is_end_of_frm;
    }

    packet->vir_addr = buffer.kern_vir_addr;
    packet->phy_addr = buffer.phy_addr;
    packet->pts = buffer.pts;
    packet->dv_dual_layer = buffer.is_dual_layer;
    packet->length = buffer.length;
    packet->disp_enable_flag = buffer.disp_enable_flag;
    packet->disp_frame_distance = buffer.disp_frm_distance;
    packet->disp_time = buffer.disp_time;
    packet->distance_before_first_frame = buffer.distance_before_first_frame;
    packet->gop_num = buffer.gop_num;
    packet->index = buffer.index;

    return EXT_SUCCESS;
}

STATIC td_s32 vadpt_release_stream(td_s32 chan_id, const vfmw_stream_data *packet)
{
    vfmw_stream_buf buffer = {0};

    if (packet->is_stream_end_flag == 1) {
        return EXT_SUCCESS;
    }

    buffer.phy_addr = packet->phy_addr;
    buffer.kern_vir_addr = packet->vir_addr;
    buffer.length = packet->length;

    return stream_release_buffer(chan_id, &buffer);
}
#endif

STATIC td_void vadpt_set_dec_perf(td_s32 chan_id, vfmw_image *image)
{
    pdt_perf perf = {0};
    const vfmw_product_info *pdt = TD_NULL;
    pdt = pdt_get_product_info();
    if (pdt->vdh_clk_dyn == 0) {
        return;
    }
    perf.chan_id = chan_id;
    if (image != TD_NULL) {
        perf.img_wid = image->image_width;
        perf.img_hig = image->image_height;
        perf.frm_rate = image->frame_rate;
    }

    pdt_set_attr(ATTR_SET_PERF, &perf);
}

STATIC td_void vadpt_bef_create_chan(td_s32 chan_id, td_void *args)
{
    vfmw_chan *chan;

    vfmw_unused(args);
    vfmw_check(os_get_time_us != TD_NULL);
    chan = vadpt_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_FATAL, "Get chan %d failed!\n", chan_id);
        return;
    }

    vfmw_check_sec_func(memset_s(&chan->kpi, sizeof(vfmw_kpi), 0, sizeof(vfmw_kpi)));
    chan->kpi.create.begin = os_get_time_us();

    vadpt_set_dec_perf(chan_id, TD_NULL);

    return;
}

STATIC td_void vadpt_bef_config_chan(td_s32 chan_id, td_void *args)
{
    vfmw_chan *chan = TD_NULL;
    pdt_license license = {0};
#ifdef VFMW_STREAM_SUPPORT
    vfmw_stream_intf *intf = TD_NULL;
#endif
    vfmw_entry *entry = vadpt_entry();

    vfmw_chan_cfg *cfg = (vfmw_chan_cfg *)args;

    vfmw_check(cfg != TD_NULL);
    vfmw_check(os_get_time_us != TD_NULL);

    chan = vadpt_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_FATAL, "Get chan %d failed!\n", chan_id);
        return;
    }

    chan->kpi.config.begin = os_get_time_us();

    if (cfg->sample_frd_enable == TD_TRUE) {
        pts_info_enable_sample_frd(chan_id);
    } else {
        pts_info_disable_sample_frd(chan_id);
    }

    chan->alloc_type = cfg->alloc_type;

    if (entry->crc_intf.crc_set_config != TD_NULL) {
        entry->crc_intf.crc_set_config(chan_id, cfg);
    }

    cfg->private_data = 0;
#ifdef VFMW_STREAM_SUPPORT
        intf = osal_vmalloc(sizeof(vfmw_stream_intf));
        if (intf == TD_NULL) {
            dprint(PRN_FATAL, "Chan %d alloc virmem for stream intf fail!\n", chan_id);
            return;
        }
        intf->stream_provider_inst_id = chan_id;
        intf->read_stream = vadpt_read_stream;
        intf->release_stream = vadpt_release_stream;
        cfg->private_data = ptr_uint64(intf);
#endif

    if (pdt_get_attr(ATTR_GET_LICENSE, &license) == EXT_SUCCESS) {
        cfg->max_core_num = license.core_num;
    }

    return;
}

STATIC td_void vadpt_bef_start_chan(td_s32 chan_id, td_void *args)
{
    vfmw_chan *chan = TD_NULL;

    vfmw_unused(args);
    vfmw_check(os_get_time_us != TD_NULL);

    chan = vadpt_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_FATAL, "Get chan %d failed!\n", chan_id);
        return;
    }

    chan->kpi.start.begin = os_get_time_us();
    chan->start_cnt++;

    return;
}

STATIC td_void vadpt_bef_stop_chan(td_s32 chan_id, td_void *args)
{
    vfmw_chan *chan = TD_NULL;

    vfmw_unused(args);
    vfmw_check(os_get_time_us != TD_NULL);

    chan = vadpt_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_FATAL, "Get chan %d failed!\n", chan_id);
        return;
    }

    chan->kpi.stop.begin = os_get_time_us();
    chan->stop_cnt++;

    return;
}

STATIC td_void vadpt_bef_reset_chan(td_s32 chan_id, td_void *args)
{
    vfmw_chan *chan = TD_NULL;

    vfmw_unused(args);
    vfmw_check(os_get_time_us != TD_NULL);

    chan = vadpt_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_FATAL, "Get chan %d failed!\n", chan_id);
        return;
    }

#ifdef VFMW_VPP_SUPPORT
    vpp_reset(chan_id);
#endif

    chan->kpi.reset.begin = os_get_time_us();
    chan->reset_cnt++;

    return;
}

STATIC td_void vadpt_bef_flush_chan(td_s32 chan_id, td_void *args)
{
    vfmw_chan *chan = TD_NULL;

    vfmw_unused(args);

    chan = vadpt_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_FATAL, "Get chan %d failed!\n", chan_id);
        return;
    }

    chan->flush_cnt++;

    return;
}

STATIC td_void vadpt_bef_destroy_chan(td_s32 chan_id, td_void *args)
{
    vfmw_chan *chan;
    vfmw_entry *entry = vadpt_entry();

    vfmw_unused(args);
    vfmw_check(os_get_time_us != TD_NULL);
    chan = vadpt_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_FATAL, "Get chan %d failed!\n", chan_id);
        return;
    }

    chan->kpi.destroy.begin = os_get_time_us();

    pts_info_free(chan_id);

    if (entry->crc_intf.crc_destroy_instance != TD_NULL &&
        entry->crc_intf.crc_destroy_instance(chan_id) != EXT_SUCCESS) {
        dprint(PRN_ERROR, "destroy CRC inst %d failed!\n", chan_id);
    }

    img_info_destroy(chan_id);

#ifdef VFMW_VPP_SUPPORT
    vpp_destroy(chan_id);
#endif

    vadpt_set_dec_perf(chan_id, TD_NULL);

    return;
}

STATIC td_void vadpt_bef_bind_es_buffer(td_s32 chan_id, td_void *args)
{
    vfmw_chan *chan;

    chan = vadpt_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return;
    }

#ifdef VFMW_STREAM_SUPPORT
    if (((vfmw_stream_buf *)args)->phy_addr == 0) {
        if (stream_get_es_info(chan_id, (vfmw_stream_buf *)args) != EXT_SUCCESS) {
            dprint(PRN_FATAL, "Get chan %d es info fail!\n", chan_id);
            return;
        }
    } else if (chan->is_sec) {
        if (stream_attach_es_buf(chan_id, (vfmw_stream_buf *)args) != EXT_SUCCESS) {
            dprint(PRN_FATAL, "attach chan %d es buf fail!\n", chan_id);
            return;
        }
    }
#endif
    vdec_record_chr_event(chan_id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_BIND_INPUT_BUF_OK);

    return;
}

#ifdef VFMW_VPP_SUPPORT
STATIC td_void vadpt_vpp_create(td_s32 chan_id, td_void *args)
{
    td_s32 ret;
    vfmw_entry *entry = vadpt_entry();
    vpp_create_param param = {0};
    vfmw_chan_option *chan_option = TD_NULL;
    vfmw_chan *chan = TD_NULL;

    chan = vadpt_chan(chan_id);
    vfmw_check(chan != TD_NULL);

    chan_option = (vfmw_chan_option *)args;
    if (chan_option != TD_NULL) {
        switch (chan_option->adpt_type) {
            case ADPT_TYPE_VDEC:
                param.work_mode = QUERY_MODE;
                param.event_report = entry->event_report_vdec;
                break;

            default:
                dprint(PRN_FATAL, "adpt_type %d err\n", chan_option->adpt_type);
                return;
        }
    }

    chan->work_mode = param.work_mode;

    ret = vpp_create(chan_id, &param);
    if (ret != EXT_SUCCESS) {
        dprint(PRN_FATAL, "vpp_create chan %d failed!\n", chan_id);
    }
}
#endif

#ifdef VFMW_STREAM_SUPPORT
STATIC td_void vadpt_stream_create(td_s32 chan_id, td_void *args)
{
    vfmw_chan_option *chan_option = (vfmw_chan_option *)args;

    vfmw_check(args != TD_NULL);
    (td_void)stream_create_chan(chan_id, chan_option);
}
#endif

STATIC td_void vadpt_aft_create_chan(td_s32 chan_id, td_void *args, td_s32 result)
{
    vfmw_chan *chan = TD_NULL;
    vfmw_entry *entry = vadpt_entry();
    vfmw_check(os_get_time_us != TD_NULL);

    if (result != EXT_SUCCESS) {
        if ((chan = vadpt_chan(chan_id)) != TD_NULL) {
            chan->state = VFMW_CHAN_STATE_NULL;
        }
        return;
    }

    pts_info_alloc(chan_id);

    if (entry->crc_intf.crc_create_instance != TD_NULL &&
        entry->crc_intf.crc_create_instance(chan_id) != EXT_SUCCESS) {
        dprint(PRN_ERROR, "create CRC inst %d failed!\n", chan_id);
    }

    chan = vadpt_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return;
    }

    chan->state = VFMW_CHAN_STATE_CREATE;
    chan->frm_cnt = 0;
    chan->que_cnt = 0;
    entry->chan_cnt++;
    chan->work_mode = QUERY_MODE;

    if (args != TD_NULL) {
        vfmw_chan_option *opt = (vfmw_chan_option *)args;
        chan->mod_handle = opt->vdec_handle;
        chan->ssm_handle = opt->ssm_handle;
        chan->adpt_type = opt->adpt_type;
    }

#ifdef VFMW_VPP_SUPPORT
    vadpt_vpp_create(chan_id, args);
#endif

#ifdef VFMW_STREAM_SUPPORT
    vadpt_stream_create(chan_id, args);
#endif

    chan->kpi.create.end = os_get_time_us();
    chan->kpi.create.cost = chan->kpi.create.end - chan->kpi.create.begin;

    return;
}

STATIC td_void vadpt_aft_config_chan(td_s32 chan_id, td_void *args, td_s32 result)
{
    vfmw_chan *chan = TD_NULL;
    vfmw_chan_cfg *cfg = (vfmw_chan_cfg *)args;

    if (result != EXT_SUCCESS) {
        return;
    }

    chan = vadpt_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_FATAL, "Get chan %d failed!\n", chan_id);
        return;
    }

#ifdef VFMW_VPP_SUPPORT
    vpp_config(chan_id, cfg);
#endif

#ifdef VFMW_STREAM_SUPPORT
    stream_config(chan_id, cfg);

    if (cfg->private_data != 0) {
        osal_vfree(uint64_ptr(cfg->private_data));
        cfg->private_data = 0;
    }
#endif

    chan->kpi.config.end = os_get_time_us();
    chan->kpi.config.cost = chan->kpi.config.end - chan->kpi.config.begin;

    return;
}

STATIC td_void vadpt_aft_start_chan(td_s32 chan_id, td_void *args, td_s32 result)
{
    vfmw_chan *chan = TD_NULL;

    vfmw_unused(args);

    if (result != EXT_SUCCESS) {
        return;
    }
    vfmw_check(os_get_time_us != TD_NULL);

    chan = vadpt_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_FATAL, "Get chan %d failed!\n", chan_id);
        return;
    }

#ifdef VFMW_VPP_SUPPORT
    vpp_start(chan_id);
#endif

    chan->kpi.start.end = os_get_time_us();
    chan->kpi.start.cost = chan->kpi.start.end - chan->kpi.start.begin;

    return;
}

STATIC td_void vadpt_aft_stop_chan(td_s32 chan_id, td_void *args, td_s32 result)
{
    vfmw_chan *chan = TD_NULL;

    vfmw_unused(args);

    if (result != EXT_SUCCESS) {
        return;
    }
    vfmw_check(os_get_time_us != TD_NULL);

    chan = vadpt_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_FATAL, "Get chan %d failed!\n", chan_id);
        return;
    }

#ifdef VFMW_VPP_SUPPORT
    vpp_stop(chan_id);
#endif

    chan->kpi.stop.end = os_get_time_us();
    chan->kpi.stop.cost = chan->kpi.stop.end - chan->kpi.stop.begin;

    return;
}

STATIC td_void vadpt_aft_reset_chan(td_s32 chan_id, td_void *args, td_s32 result)
{
    vfmw_chan *chan = TD_NULL;

    vfmw_unused(args);

    pts_info_reset(chan_id);

    if (result != EXT_SUCCESS) {
        return;
    }
    vfmw_check(os_get_time_us != TD_NULL);

    chan = vadpt_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_FATAL, "Get chan %d failed!\n", chan_id);
        return;
    }

#ifdef VFMW_STREAM_SUPPORT
    stream_reset(chan_id);
#endif

    chan->kpi.reset.end = os_get_time_us();
    chan->kpi.reset.cost = chan->kpi.reset.end - chan->kpi.reset.begin;

    return;
}

STATIC td_void vadpt_aft_flush_chan(td_s32 chan_id, td_void *args, td_s32 result)
{
    vfmw_chan *chan = TD_NULL;
    vfmw_flush_type flush_type = *((vfmw_flush_type *)args);

    vfmw_unused(args);

    pts_info_reset(chan_id);

    if (result != EXT_SUCCESS) {
        return;
    }

    chan = vadpt_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_FATAL, "Get chan %d failed!\n", chan_id);
    }

#ifdef VFMW_VPP_SUPPORT
    if (flush_type == VFMW_FLUSH_OUT || flush_type == VFMW_FLUSH_ALL) {
        vpp_reset(chan_id);
    }
#endif

#ifdef VFMW_STREAM_SUPPORT
    if (flush_type == VFMW_FLUSH_IN || flush_type == VFMW_FLUSH_ALL) {
        stream_reset(chan_id);
    }
#endif

    return;
}

STATIC td_void vadpt_aft_flush_post(td_s32 chan_id, td_void *args, td_s32 result)
{
    vfmw_unused(args);
    vfmw_unused(result);
    vfmw_unused(chan_id);
#ifdef VFMW_VPP_SUPPORT
    (td_void)vpp_reset(chan_id);
#endif

    return;
}

STATIC td_void vadpt_aft_destroy_chan(td_s32 chan_id, td_void *args, td_s32 result)
{
    vfmw_chan *chan = TD_NULL;
    vfmw_entry *entry = vadpt_entry();
#ifdef VFMW_VMM_SUPPORT
    vmm_rls_burst burst = { 0 };
    vmm_cancel_cmd cancle_cmd = { 0 };
#endif
    vfmw_unused(args);

    if (result != EXT_SUCCESS) {
        return;
    }
    vfmw_check(os_get_time_us != TD_NULL);

    chan = vadpt_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return;
    }

#ifdef VFMW_STREAM_SUPPORT
    (td_void)stream_destroy_chan(chan_id);
#endif

#ifdef VFMW_VMM_SUPPORT
    burst.priv_id = chan_id;
    burst.priv_mask = 0xffff; /* all vmm mem */
    cancle_cmd.cmd_id = VMM_CMD_ALLOC;
    cancle_cmd.priv_id = chan_id;
    cancle_cmd.priv_mask = 0xffff;

    if (vmm_send_command(VMM_CMD_RELEASE_BURST, VMM_CMD_PRIO_MIN, &burst) != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s VMM_RELEASE_BURST failed!\n", __func__);
    }

    if (vmm_send_command(VMM_CMD_CANCLE, VMM_CMD_PRIO_MAX, &cancle_cmd) != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s VMM_CMD_CANCLE failed!\n", __func__);
    }

    if (vmm_send_command(VMM_CMD_CANCLE, VMM_CMD_PRIO_MAX, &cancle_cmd) != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s VMM_CMD_CANCLE failed!\n", __func__);
    }
#endif

    entry->chan_cnt--;
    chan->state = VFMW_CHAN_STATE_NULL;

    chan->kpi.destroy.end = os_get_time_us();
    chan->kpi.destroy.cost = chan->kpi.destroy.end - chan->kpi.destroy.begin;

    return;
}

STATIC td_void vadpt_aft_get_chan_state(td_s32 chan_id, td_void *args, td_s32 result)
{
    vfmw_chan *chan = TD_NULL;

    if (result != EXT_SUCCESS) {
        return;
    }

    chan = vadpt_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return;
    }

#ifdef VFMW_STREAM_SUPPORT
    (td_void)stream_get_chan_status(chan_id, (vfmw_chan_info *)args);
#endif

#ifdef VFMW_VPP_SUPPORT
    (td_void)vpp_get_chan_status(chan_id, (vfmw_chan_info *)args);
#endif

    pts_get_info(chan_id, (vfmw_chan_info *)args);

    return;
}

STATIC td_void vadpt_aft_set_evt_map(td_s32 chan_id, const td_void *args, td_s32 result)
{
    vfmw_chan *chan = TD_NULL;

    if (result != EXT_SUCCESS) {
        return;
    }

    chan = vadpt_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return;
    }

#ifdef VFMW_VPP_SUPPORT
    {
        td_s32 ret;
        ret = vpp_set_evt_map(chan_id, *((td_u64 *)args));
        if (ret != EXT_SUCCESS) {
            dprint(PRN_ERROR, "vpp_set_evt_map fail!\n");
        }
    }
#endif

#ifdef VFMW_STREAM_SUPPORT
    {
        td_s32 ret;
        ret = stream_set_evt_map(chan_id, *((td_u64 *)args));
        if (ret != EXT_SUCCESS) {
            dprint(PRN_ERROR, "stream_set_evt_map fail!\n");
        }
    }
#endif
}

STATIC td_s32 vadpt_get_image(td_s32 chan_id, td_void *image);
STATIC td_s32 vadpt_release_image(td_s32 chan_id, const td_void *image);

STATIC td_void vadpt_aft_end_pic(td_s32 chan_id, td_void *args, td_s32 result)
{
    td_s32 ret;
    vfmw_image img = {0};
    vfmw_chan *chan = TD_NULL;

    vfmw_unused(args);

    if (result != EXT_SUCCESS) {
        return;
    }

    chan = vadpt_chan(chan_id);
    vfmw_check(chan != TD_NULL);

    ret = vadpt_get_image(chan_id, &img);
    if (ret == EXT_SUCCESS) {
        vadpt_release_image(chan_id, &img);
    }

    return;
}

STATIC td_void vadpt_bef_bind_ext_buffer(td_s32 chan_id, td_void *args)
{
#ifdef VFMW_SEC_SUPPORT
    td_s32 ret;
    td_u64 sec_info_addr = 0;
    vfmw_chan *chan = TD_NULL;
    ext_drv_ssm_buf_attach_info info = {0};
    vfmw_ext_buf *buffer = (vfmw_ext_buf *)args;

    vfmw_check(args != TD_NULL);
    vfmw_check(chan_id < VFMW_CHAN_NUM);
    chan = vadpt_chan(chan_id);
    vfmw_check(chan != TD_NULL);

    if (chan->is_sec == 0) {
        return;
    }

    info.module_handle = chan->mod_handle;
    info.session_handle = chan->ssm_handle;
    info.buf_id = EXT_SSM_BUFFER_ID_VID_FRM_BUF;
    info.dma_buf_addr = buffer->ext_frm_buf.dma_buf;

    ret = ext_drv_ssm_attach_buffer(info, &sec_info_addr);
    if (ret != EXT_SUCCESS) {
        dprint(PRN_FATAL, "vfmw attach extbuf fail\n");
        dprint(PRN_FATAL, "mod %x ssm %x bufid %x dma %llx phy %llx\n",
            info.module_handle, info.session_handle,
            info.buf_id, info.dma_buf_addr, buffer->ext_frm_buf.phy_addr);
    }
    buffer->ext_frm_buf.sec_info = sec_info_addr;
#else
    vfmw_unused(chan_id);
    vfmw_unused(args);
#endif
}

STATIC td_void vadpt_before_control(td_s32 chan_id, vfmw_cid cmd_id, td_void *args)
{
    switch (cmd_id) {
        case VFMW_CID_CREATE_CHAN:
            vadpt_bef_create_chan(chan_id, args);
            break;
        case VFMW_CID_CFG_CHAN:
            vadpt_bef_config_chan(chan_id, args);
            break;
        case VFMW_CID_START_CHAN:
            vadpt_bef_start_chan(chan_id, args);
            break;
        case VFMW_CID_STOP_CHAN:
            vadpt_bef_stop_chan(chan_id, args);
            break;
        case VFMW_CID_RESET_CHAN:
        case VFMW_CID_RESET_CHAN_WITH_OPTION:
            vadpt_bef_reset_chan(chan_id, args);
            break;
        case VFMW_CID_FLUSH_CHAN:
            vadpt_bef_flush_chan(chan_id, args);
            break;
        case VFMW_CID_DESTROY_CHAN:
            vadpt_bef_destroy_chan(chan_id, args);
            break;
        case VFMW_CID_BIND_ES_BUF:
            vadpt_bef_bind_es_buffer(chan_id, args);
            break;
        case VFMW_CID_BIND_EXTERN_BUFFER:
            vadpt_bef_bind_ext_buffer(chan_id, args);
            break;
        default:
            break;
    }

    return;
}

STATIC td_void vadpt_after_control(td_s32 chan_id, vfmw_cid cmd_id, td_void *args, td_s32 result)
{
    switch (cmd_id) {
        case VFMW_CID_CREATE_CHAN:
            vadpt_aft_create_chan(chan_id, args, result);
            break;
        case VFMW_CID_CFG_CHAN:
            vadpt_aft_config_chan(chan_id, args, result);
            break;
        case VFMW_CID_START_CHAN:
            vadpt_aft_start_chan(chan_id, args, result);
            break;
        case VFMW_CID_STOP_CHAN:
            vadpt_aft_stop_chan(chan_id, args, result);
            break;
        case VFMW_CID_RESET_CHAN:
        case VFMW_CID_RESET_CHAN_WITH_OPTION:
            vadpt_aft_reset_chan(chan_id, args, result);
            break;
        case VFMW_CID_FLUSH_CHAN:
            vadpt_aft_flush_chan(chan_id, args, result);
            break;
        case VFMW_CID_FLUSH_POST_PROCESS:
            vadpt_aft_flush_post(chan_id, args, result);
            break;
        case VFMW_CID_DESTROY_CHAN:
            vadpt_aft_destroy_chan(chan_id, args, result);
            break;
        case VFMW_CID_GET_CHAN_STATE:
            vadpt_aft_get_chan_state(chan_id, args, result);
            break;
        case VFMW_CID_SET_EVENT_MAP:
            vadpt_aft_set_evt_map(chan_id, args, result);
            break;
        case VFMW_CID_END_PIC:
            vadpt_aft_end_pic(chan_id, args, result);
            break;
        default:
            break;
    }

    return;
}

STATIC td_void vadpt_save_image_yuv(vfmw_chan *chan, td_void *image)
{
    vfmw_dflt *dflt = vadpt_get_dflt();
    vfmw_entry *entry = TD_NULL;

    entry = vadpt_entry();

    if (chan->yuv == TD_NULL && chan->mtdt == TD_NULL) {
        /* avoid is_sec err print */
        return;
    }

    if (chan->is_sec == TD_TRUE) {
        dprint(PRN_ERROR, "%s not support saving secure image yuv.\n", __func__);
        return;
    }

    if (chan->frm_cnt < dflt->save_begin || chan->frm_cnt > dflt->save_end) {
        dprint(PRN_ALWS, "%s current frame count %d not in the range[%d, %d].\n", __func__,
               chan->frm_cnt, dflt->save_begin, dflt->save_end);
        return;
    }

    /* REPORT_MODE(OMX path) get image ,have vpp_sema locked that lead to dead lock when reset/stop/start cmd */
    os_sema_down(entry->dbg_sema);

    if (chan->yuv != TD_NULL) {
        dprint(PRN_ALWS, "chan %d frame %d yuv:\n", chan->id, chan->frm_cnt);
        dbg_save_yuv(chan->id, image, chan->yuv);
    }

    os_sema_up(entry->dbg_sema);

    return;
}

STATIC td_s32 vadpt_process_get_image_success(vfmw_chan *chan, td_void *image)
{
    td_s32 ret = EXT_FAILURE;
    vfmw_entry *entry = vadpt_entry();

    vfmw_unused(ret);
    img_info_convert(chan->id, image);

#ifdef VFMW_VMM_SUPPORT
    if (chan->alloc_type == VFMW_FRM_ALLOC_INNER) {
        ret = vadpt_vmm_set_image_out(image);
        if (ret != EXT_SUCCESS) {
            dprint(PRN_ERROR, "%s vadpt_vmm_set_image_out failed.\n", __func__);
            return EXT_FAILURE;
        }
    }
#endif

    if (chan->type != VFMW_TYPE_USER) {
        vadpt_set_dec_perf(chan->id, image);
    }

    if (entry->crc_intf.crc_put_image != TD_NULL) {
        entry->crc_intf.crc_put_image(chan->id, image);
    }

    vadpt_save_image_yuv(chan, image);

    chan->frm_cnt++;

    return EXT_SUCCESS;
}

STATIC td_s32 vadpt_process_release_image_success(vfmw_chan *chan, const td_void *image)
{
#ifdef VFMW_VMM_SUPPORT
    if (chan->alloc_type == VFMW_FRM_ALLOC_INNER) {
        (td_void)vadpt_vmm_set_image_in(image);
    }
#else
    vfmw_unused(chan);
    vfmw_unused(image);
#endif

    return EXT_SUCCESS;
}

STATIC td_void vadpt_init_entry(td_void)
{
    td_s32 id;
    vfmw_entry *entry = TD_NULL;

    entry = vadpt_entry();
    entry->ref_cnt = 0;
    os_sema_init(&entry->sema);

    for (id = 0; id < VFMW_CHAN_NUM; id++) {
        os_sema_init(&(entry->chan[id].sema));
    }
    os_sema_init(&entry->dbg_sema);
}

STATIC td_void vadapt_set_callback(vfmw_adpt_type type, fun_vfmw_event_report event_report)
{
    vfmw_entry *entry = TD_NULL;
    td_s32 ret = EXT_FAILURE;

    entry = vadpt_entry();
    vfmw_check(entry != TD_NULL);
    vfmw_check(event_report != TD_NULL);

    switch (type) {
        case ADPT_TYPE_VDEC:
            entry->event_report_vdec = event_report;
            break;

        default:
            dprint(PRN_FATAL, "%s unkown adapter type: %d\n", __func__, type);
            return;
    }

#ifdef VFMW_LOCAL_SUPPORT
    ret = local_set_callback(type, event_report);
#endif
#ifdef VFMW_USER_SUPPORT
    ret = user_set_callback(type, event_report);
#endif
#ifdef VFMW_STREAM_SUPPORT
    ret = stream_set_callback(type, event_report);
#endif
    if (ret != EXT_SUCCESS) {
        dprint(PRN_FATAL, "%s set event type %d failed\n", __func__, type);
    }

    return;
}

STATIC td_void vadapt_close_module(td_void)
{
    vfmw_entry *entry = vadpt_entry();

#ifdef VFMW_STREAM_SUPPORT
    stream_close();
#endif

#ifdef VFMW_VPP_SUPPORT
    vpp_deinit();
#endif

    if (entry->crc_intf.crc_exit != TD_NULL) {
        entry->crc_intf.crc_exit();
    }

#ifdef VFMW_IPT_SUPPORT
    ipt_close();
#endif

#ifdef VFMW_LOCAL_SUPPORT
    (td_void)local_close();
#endif

#ifdef VFMW_USER_SUPPORT
    (td_void)user_close();
#endif

#ifdef VFMW_VMM_SUPPORT
    vmm_mod_exit();
#endif

    return;
}

STATIC td_s32 vadapt_init_module(td_void *args)
{
    td_s32 ret;
    vfmw_entry *entry = vadpt_entry();

    ret = EXT_FAILURE;
#ifdef VFMW_LOCAL_SUPPORT
    ret = local_init(args, sizeof(vfmw_init_param));
#endif
#ifdef VFMW_USER_SUPPORT
    ret = user_init(args, sizeof(vfmw_init_param));
#endif

    if (entry->crc_intf.crc_init_calc_env != TD_NULL) {
        entry->crc_intf.crc_init_calc_env();
    }

#ifdef VFMW_STREAM_SUPPORT
    stream_init((vfmw_init_param *)args);
#endif

    return ret;
}

STATIC td_s32 vadapt_control_type(td_s32 chan_id, td_s32 cmd, td_void *args, td_u32 len)
{
    td_s32 ret;
    vfmw_type type;

    vfmw_unused(len);
    type = vadpt_get_vfmw_type(chan_id, cmd, args);

    switch (type) {
#ifdef VFMW_LOCAL_SUPPORT
        case VFMW_TYPE_LOCAL:
            ret = local_control(chan_id, cmd, args, len);
            break;
#endif
#ifdef VFMW_USER_SUPPORT
        case VFMW_TYPE_USER:
            ret = user_control(chan_id, cmd, args, len);
            break;
#endif
        default:
            ret = EXT_FAILURE;
            dprint(PRN_ERROR, "vfmw adapter can not support type %d\n", type);
            break;
    }

    return ret;
}

STATIC td_s32 vadpt_release_image(td_s32 chan_id, const td_void *image)
{
    td_s32 ret;
    vfmw_chan *chan = TD_NULL;

    vfmw_check_ret(vadpt_get_load(), EXT_FAILURE);

    chan = vadpt_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    ret = EXT_FAILURE;
    switch (chan->type) {
#ifdef VFMW_LOCAL_SUPPORT
        case VFMW_TYPE_LOCAL:
            ret = local_release_image(chan_id, image);
            break;
#endif
#ifdef VFMW_USER_SUPPORT
        case VFMW_TYPE_USER:
            ret = user_release_image(chan_id, image);
            break;
#endif
        default:
            dprint(PRN_ERROR, "vfmw adapter can not support type %d\n", chan->type);
            break;
    }

    if (ret == EXT_SUCCESS) {
        (td_void)vadpt_process_release_image_success(chan, image);
    }

    return ret;
}

STATIC td_s32 vadpt_get_image(td_s32 chan_id, td_void *image)
{
    td_s32 ret;
    vfmw_chan *chan = TD_NULL;

    vfmw_check_ret(vadpt_get_load(), EXT_FAILURE);

    chan = vadpt_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    ret = EXT_FAILURE;
    switch (chan->type) {
#ifdef VFMW_LOCAL_SUPPORT
        case VFMW_TYPE_LOCAL:
            ret = local_get_image(chan_id, image);
            break;
#endif
#ifdef VFMW_USER_SUPPORT
        case VFMW_TYPE_USER:
            ret = user_get_image(chan_id, image);
            break;
#endif
        default:
            dprint(PRN_ERROR, "vfmw adapter can not support type %d\n", chan->type);
            break;
    }

    if (ret == EXT_SUCCESS) {
        ret = vadpt_process_get_image_success(chan, image);
        if (ret != EXT_SUCCESS) {
            (td_void)vadpt_release_image(chan->id, image);
        }
    }

    return ret;
}

#ifdef VFMW_VPP_SUPPORT
STATIC td_s32 vadpt_check_image(td_s32 chan_id, td_void *image)
{
    td_s32 ret;
    vfmw_chan *chan = TD_NULL;

    vfmw_check_ret(image != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(vadpt_get_load(), EXT_FAILURE);

    chan = vadpt_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    ret = EXT_FAILURE;
    switch (chan->type) {
#ifdef VFMW_LOCAL_SUPPORT
        case VFMW_TYPE_LOCAL:
            ret = local_check_image(chan_id, image);
            break;
#endif
        default:
            dprint(PRN_ERROR, "%s type %d error\n", __func__, chan->type);
            break;
    }

    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return ret;
}

STATIC td_s32 vadpt_check_eos(td_s32 chan_id, td_s32 *flag)
{
    td_s32 ret;
    vfmw_chan *chan = TD_NULL;

    vfmw_unused(flag);
    chan = vadpt_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    ret = EXT_FAILURE;
    switch (chan->type) {
#ifdef VFMW_LOCAL_SUPPORT
        case VFMW_TYPE_LOCAL:
            ret = local_check_eos(chan_id, flag);
            break;
#endif
        default:
            dprint(PRN_ERROR, "%s type %d error\n", __func__, chan->type);
            break;
    }

    return ret;
}
#endif

#ifdef VFMW_VPP_SUPPORT
STATIC td_void vadpt_open_vpp_support(td_void)
{
    vpp_init_param param = {0};

    param.get_image = vadpt_get_image;
    param.rls_image = vadpt_release_image;
    param.chk_image = vadpt_check_image;
    param.chk_eos = vadpt_check_eos;
    vpp_init(&param);
}
#endif

STATIC td_bool vadpt_is_need_print_fatal(vfmw_cid cmd)
{
    td_u32 i;
    const vfmw_cid cmd_list[] = {
        VFMW_CID_CREATE_CHAN, VFMW_CID_DESTROY_CHAN, VFMW_CID_GET_CHAN_CFG, VFMW_CID_CFG_CHAN, VFMW_CID_START_CHAN,
        VFMW_CID_STOP_CHAN, VFMW_CID_RESET_CHAN, VFMW_CID_SET_STREAM_INTF, VFMW_CID_GET_CHAN_MEMSIZE,
        VFMW_CID_RELEASE_STREAM, VFMW_CID_RESET_CHAN_WITH_OPTION, VFMW_CID_INIT_ES_BUF,
        VFMW_CID_BIND_ES_USR_ADDR, VFMW_CID_FLUSH_CHAN, VFMW_CID_FLUSH_POST_PROCESS
    };

    for (i = 0; i < sizeof(cmd_list) / sizeof(vfmw_cid); i++) {
        if (cmd == cmd_list[i]) {
            return TD_TRUE;
        }
    }

    return TD_FALSE;
}

#ifdef VT_SUPPORT
    td_s32 vt_mod_init(td_void);
#endif

#ifndef VFMW_LOW_POWER_SUPPORT
STATIC td_s32 vadpt_power_on(td_void)
{
    td_s32 ret;

    ret = vadpt_process_hardware(ATTR_ENABLE_VDH);
    vfmw_check_ret(ret == EXT_SUCCESS, EXT_FAILURE);

    ret = vadpt_process_hardware(ATTR_RESET_VDH);
    vfmw_check_goto(ret == EXT_SUCCESS, err1);

    return EXT_SUCCESS;
err1:
    (td_void)vadpt_process_hardware(ATTR_DISABLE_VDH);
    return EXT_FAILURE;
}
#endif

/* ==== extern function define ==== */
td_s32 vadpt_open(td_void)
{
    vfmw_entry *entry = vadpt_entry();

    os_intf_init();
    (td_void)dbg_init();

    pdt_init_product_info();

#ifdef VDEC_PROC_SUPPORT
    os_proc_init();
    (td_void)vfmw_adpt_proc_create();
#endif

    vadpt_init_entry();

#ifdef VFMW_VMM_SUPPORT
    vmm_mod_init(vadpt_vmm_call_back);
#endif

#ifdef VFMW_LOCAL_SUPPORT
    (td_void)local_open();
#endif

#ifdef VFMW_USER_SUPPORT
    (td_void)user_open();
#endif

    if (entry->crc_intf.crc_init != TD_NULL) {
        entry->crc_intf.crc_init();
    }

#ifdef VFMW_IPT_SUPPORT
    ipt_open();
#endif

    img_info_init();

#ifdef VFMW_STREAM_SUPPORT
    stream_open1();
#endif

#ifdef VFMW_VPP_SUPPORT
    vadpt_open_vpp_support();
#endif

#ifdef VT_SUPPORT
    vt_mod_init();
#endif

    vadpt_set_load(TD_TRUE);

    return EXT_SUCCESS;
}

td_s32 vadpt_close(td_void)
{
    td_s32 id;
    vfmw_entry *entry = TD_NULL;

#ifdef VT_SUPPORT
    {
        td_s32 vt_mod_exit(td_void);
        vt_mod_exit();
    }
#endif

    vadapt_close_module();

#ifdef VDEC_PROC_SUPPORT
    (td_void)vfmw_adpt_proc_destroy();
    os_proc_exit();
#endif

    entry = vadpt_entry();

    os_sema_exit(entry->sema);

    for (id = 0; id < VFMW_CHAN_NUM; id++) {
        os_sema_exit(entry->chan[id].sema);
    }
    os_sema_exit(entry->dbg_sema);

    img_info_exit();

    pdt_deinit_product_info();

    (td_void)dbg_deinit();
    (td_void)os_intf_exit();

    vadpt_set_load(TD_FALSE);

    return EXT_SUCCESS;
}

td_s32 vadpt_init(td_void *args)
{
    td_s32 id;
    td_s32 ret;
    vfmw_entry *entry = TD_NULL;
    vfmw_init_param *param = TD_NULL;

    vfmw_check_ret(vadpt_get_load(), EXT_FAILURE);
    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(os_get_time_us != TD_NULL, EXT_FAILURE);

    vadpt_sema_down();

    entry = vadpt_entry();
    entry->init_kpi.begin = os_get_time_us();

    if (entry->ref_cnt >= VFMW_UINT_MAX) {
        ret = EXT_FAILURE;
        goto out;
    } else if (entry->ref_cnt >= 1) { /* 1: init cnt */
        ret = EXT_SUCCESS;
        goto out;
    }

    ret = vadapt_init_module(args);

#ifndef VFMW_LOW_POWER_SUPPORT
    ret = vadpt_power_on();
    vfmw_check_goto(ret == EXT_SUCCESS, err0);
#endif

    entry->state = VFMW_STATE_OPEN;
    for (id = 0; id < VFMW_CHAN_NUM; id++) {
        entry->chan[id].state = VFMW_CHAN_STATE_NULL;
        entry->chan[id].id = id;
    }

out:
    if (ret == EXT_SUCCESS) {
        entry->ref_cnt++;
        param = (vfmw_init_param *)args;
        vadapt_set_callback(param->adpt_type, param->fn_callback);
    }

    entry->init_kpi.end = os_get_time_us();
    entry->init_kpi.cost = entry->init_kpi.end - entry->init_kpi.begin;

    vadpt_sema_up();

    return ret;

#ifndef VFMW_LOW_POWER_SUPPORT
err0:
    vadapt_close_module();
    vadpt_sema_up();
    return EXT_FAILURE;
#endif
}

td_s32 vadpt_exit(const td_void *args)
{
    td_s32 ret = EXT_FAILURE;
    vfmw_entry *entry = TD_NULL;

    vfmw_unused(args);
    vfmw_check_ret(os_get_time_us != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(vadpt_get_load(), EXT_FAILURE);

    vadpt_sema_down();

    entry = vadpt_entry();
    entry->exit_kpi.begin = os_get_time_us();

    if (entry->ref_cnt == 0) {
        ret = EXT_FAILURE;
        goto out;
    } else if (entry->ref_cnt > 1) { /* 1: init cnt */
        ret = EXT_SUCCESS;
        goto out;
    }

#ifdef VFMW_LOCAL_SUPPORT
    ret = local_exit(args);
#endif

    if (entry->crc_intf.crc_exit_calc_env != TD_NULL) {
        entry->crc_intf.crc_exit_calc_env();
    }

#ifdef VFMW_STREAM_SUPPORT
    stream_exit();
#endif

    entry->state = VFMW_STATE_NULL;

#ifndef VFMW_LOW_POWER_SUPPORT
    ret = vadpt_process_hardware(ATTR_DISABLE_VDH);
#endif

out:
    if (ret == EXT_SUCCESS) {
        entry->ref_cnt--;
    }

    entry->exit_kpi.end = os_get_time_us();
    entry->exit_kpi.cost = entry->exit_kpi.end - entry->exit_kpi.begin;

    vadpt_sema_up();

    return ret;
}

td_s32 vadpt_suspend(const td_void *args)
{
    td_s32 ret;
    vfmw_entry *entry = vadpt_entry();

    vfmw_unused(args);

    vfmw_check_ret(os_get_time_us != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(vadpt_get_load(), EXT_FAILURE);

    entry->susp_kpi.begin = os_get_time_us();

    vadpt_sema_down();

    if (entry->suspend != 0) {
        vadpt_sema_up();
        return EXT_SUCCESS;
    }

    ret = EXT_SUCCESS;

#ifdef VFMW_LOCAL_SUPPORT
    ret = local_suspend(args);
#endif

    (td_void)pdt_suspend(TD_NULL);

    entry->suspend = 1;

    vadpt_sema_up();

    entry->susp_kpi.end = os_get_time_us();
    entry->susp_kpi.cost = entry->susp_kpi.end - entry->susp_kpi.begin;
    entry->suspend_cnt++;

    osal_printk("vfmw suspend ok\n");

    return ret;
}

td_s32 vadpt_resume(const td_void *args)
{
    td_s32 ret;
    td_bool dec_flag;

    vfmw_entry *entry = vadpt_entry();

    vfmw_unused(args);
    entry->resu_kpi.begin = os_get_time_us();

    vfmw_check_ret(vadpt_get_load(), EXT_FAILURE);

    vadpt_sema_down();

    if (entry->suspend == 0) {
        vadpt_sema_up();
        return EXT_SUCCESS;
    }

    ret = EXT_SUCCESS;

    dec_flag = entry->chan_cnt != 0;
    (td_void)pdt_resume(&dec_flag);

#ifdef VFMW_LOCAL_SUPPORT
    ret = local_resume(args);
#endif

    entry->suspend = 0;

    vadpt_sema_up();

    entry->resu_kpi.end = os_get_time_us();
    entry->resu_kpi.cost = entry->resu_kpi.end - entry->resu_kpi.begin;
    entry->resume_cnt++;

    osal_printk("vfmw resume ok\n");

    return ret;
}

td_s32 vadpt_get_frame(td_s32 chan_id, td_void *frame, td_void *ext_frm_info)
{
    td_s32 ret;
    td_u32 time;
    vfmw_chan *chan = TD_NULL;
    vfmw_entry *entry = vadpt_entry();

    vfmw_check_ret(vadpt_get_load(), EXT_FAILURE);

    chan = vadpt_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(os_get_time_us != TD_NULL, EXT_FAILURE);

    time = os_get_time_us();
    vfmw_unused(ext_frm_info);

    if (entry->crc_intf.is_register == TD_TRUE) {
        ret = vadpt_get_image(chan_id, frame);
    } else {
        ret = vpp_receive_frame(chan_id, frame);
    }

    if (ret == EXT_SUCCESS &&
        chan->kpi.getfrm.begin == 0) {
        chan->kpi.getfrm.begin = time;
        chan->kpi.getfrm.end = os_get_time_us();
        chan->kpi.getfrm.cost = chan->kpi.getfrm.end - chan->kpi.questm.end;
    }

    return ret;
}

td_s32 vadpt_release_frame(td_s32 chan_id, const td_void *frame)
{
    vfmw_chan *chan = TD_NULL;
    vfmw_entry *entry = vadpt_entry();
    vfmw_check_ret(vadpt_get_load(), EXT_FAILURE);

    chan = vadpt_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    if (entry->crc_intf.is_register == TD_TRUE) {
        return vadpt_release_image(chan_id, frame);
    } else {
        return vpp_release_frame(chan_id, frame);
    }
}

td_s32 vadpt_control(td_s32 chan_id, td_s32 cmd, td_void *args, td_u32 len)
{
    td_s32 ret;
    vfmw_entry *entry = TD_NULL;
    td_bool local_case = TD_FALSE;

    vfmw_check_ret(vadpt_get_load(), EXT_FAILURE);
    vfmw_check_ret(vadpt_is_chan_valid(chan_id, cmd) == TD_TRUE, EXT_FAILURE);

    ret = vadpt_local_control(chan_id, cmd, args, len, &local_case);
    if (local_case == TD_TRUE) {
        return ret;
    }

    vadpt_sema_down();
    ret = vadpt_get_chan_id(cmd, &chan_id);
    vadpt_sema_up();
    if (ret != EXT_SUCCESS) {
        dprint(PRN_FATAL, "error: %s vadpt_get_chan_id error\n", __func__);
        return EXT_FAILURE;
    }
    entry = vadpt_entry();

    os_sema_down(entry->chan[chan_id].sema);

    vadpt_before_control(chan_id, cmd, args);

    ret = vadapt_control_type(chan_id, cmd, args, len);

    vadpt_after_control(chan_id, cmd, args, ret);

    if (ret != EXT_SUCCESS && (vadpt_is_need_print_fatal((vfmw_cid)cmd) == TD_TRUE)) {
        dprint(PRN_FATAL, "chan %d vfmw control %d fail!\n", chan_id, cmd);
    }

    os_sema_up(entry->chan[chan_id].sema);

    return ret;
}

td_s32 vadpt_register_crc_intf(vfmw_crc_intf *crc_intf)
{
    vfmw_entry *entry = vadpt_entry();

    if (entry->crc_intf.is_register == TD_TRUE) {
        return EXT_SUCCESS;
    }

    vfmw_check_ret(crc_intf->crc_init != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(crc_intf->crc_exit != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(crc_intf->crc_create_instance != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(crc_intf->crc_destroy_instance != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(crc_intf->crc_set_config != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(crc_intf->crc_put_image != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(crc_intf->crc_init_calc_env != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(crc_intf->crc_exit_calc_env != TD_NULL, EXT_FAILURE);

    entry->crc_intf.crc_init = crc_intf->crc_init;
    entry->crc_intf.crc_exit = crc_intf->crc_exit;
    entry->crc_intf.crc_create_instance = crc_intf->crc_create_instance;
    entry->crc_intf.crc_destroy_instance = crc_intf->crc_destroy_instance;
    entry->crc_intf.crc_set_config = crc_intf->crc_set_config;
    entry->crc_intf.crc_put_image = crc_intf->crc_put_image;
    entry->crc_intf.crc_init_calc_env = crc_intf->crc_init_calc_env;
    entry->crc_intf.crc_exit_calc_env = crc_intf->crc_exit_calc_env;
    entry->crc_intf.is_register = TD_TRUE;

    return EXT_SUCCESS;
}

td_void vadpt_unregister_crc_intf(td_void)
{
    vfmw_entry *entry = vadpt_entry();

    (td_void)memset_s(&entry->crc_intf, sizeof(vfmw_crc_intf), 0, sizeof(vfmw_crc_intf));
    entry->crc_intf.is_register = TD_FALSE;
}
