/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "drv_vdec.h"
#include "drv_vdec_sdk.h"
#include "soc_osal.h"
#include "drv_vdec_proc.h"
#include "drv_vdec_vfmw.h"
#include "vdec_priv_type.h"
#include "securec.h"
#include "vdec_chr.h"
#ifdef VDEC_HEIF_SUPPORT
#include "drv_vdec_heif.h"
#endif

typedef enum {
    VDEC_CHAN_STATE_NULL = 0,
    VDEC_CHAN_STATE_CREATE,
    VDEC_CHAN_STATE_START,
    VDEC_CHAN_STATE_STOP,
} drv_vdec_chan_state;

typedef enum {
    VDEC_WAIT_PARAM_PVR_INDEX = 0,
    VDEC_WAIT_PARAM_FRM_RATE_INDEX,
    VDEC_WAIT_PARAM_SPEED_INDEX,
    VDEC_WAIT_PARAM_START_PTS_INDEX,
    VDEC_WAIT_PARAM_MAX_INDEX
} drv_vdec_wait_param_index;

typedef struct {
    td_u32 create_time;
    td_u32 config_time;
    td_u32 start_time;
    td_u32 stop_time;
    td_u32 reset_time;
    td_u32 flush_time;
    td_u32 destroy_time;
    td_u32 reg_event_time;
} drv_vdec_kpi;

typedef struct {
    td_u32 try_que_stm_num;
    td_u32 que_stm_num;
    td_u32 try_deq_stm_num;
    td_u32 deq_stm_num;
    td_u32 try_acq_frm_num;
    td_u32 acq_frm_num;
    td_u32 try_rel_frm_num;
    td_u32 rel_frm_num;
} drv_vdec_statis;

typedef struct {
    ext_vdec_frame vdec_frame;
    struct osal_list_head node;
} drv_vdec_cycle_frame;

#define VDEC_CYCLE_MAX_NUM (3)
typedef struct {
    td_s32 vdec_id;
    td_s32 vfmw_id;
    td_handle vdec_handle;
    td_handle user_handle;
    td_u64 event_map;
    ext_vdec_attr attr;
    vdec_vfmw_param ext_attr;
    vdec_osal_sema sema;
    drv_vdec_chan_state status;
    drv_vdec_statis statis;
    fn_drv_vdec_callback callback;
    td_void *unique;
    vdec_osal_event wait_frm_event;
    vdec_osal_event wait_event;
    td_u32 pvr_buf_num;
    td_u32 new_frm_evt;
    td_bool is_stream_buffer_init;

    td_u64 event_valid_map;
    ext_vdec_event listen_event[VDEC_LINSTEN_NUM];

    td_u32 idx;
    td_u32 cycle_mode;
    struct osal_list_head list;
    drv_vdec_cycle_frame frame[VDEC_CYCLE_MAX_NUM];
    td_u32 low_power_test_mode;
#ifdef VDEC_HEIF_SUPPORT
    ext_vdec_heif_select ori_select;
    ext_vdec_heif_select act_select;
    drv_vdec_heif_info heif_info;
#endif
} drv_vdec_chan;

typedef struct {
    td_u32 chan_num;
    vdec_osal_sema sema;
    drv_vdec_chan *chan_ptr[VDEC_MAX_CHAN_NUM];
    drv_vdec_kpi kpi_info[VDEC_MAX_CHAN_NUM];
    td_u32 open_cnt;
} drv_vdec_entry;

/* recive frame cycles in 1 second */
#define VDEC_MAX_IFRAME_RECV_FRM_TIME  (1 * 1000 * 1000)
#define VDEC_INVALIED_INDEX  (-1)
static drv_vdec_entry g_drv_vdec_entry = {0};

STATIC drv_vdec_entry* drv_vdec_get_entry(td_void)
{
    return &g_drv_vdec_entry;
}

STATIC drv_vdec_chan* drv_vdec_get_chan(td_s32 chan_id)
{
    drv_vdec_entry *entry = drv_vdec_get_entry();
    return entry->chan_ptr[chan_id];
}

STATIC td_void drv_vdec_lock_entry(td_void)
{
    drv_vdec_entry *entry = drv_vdec_get_entry();
    vdec_osal_sema_down(&entry->sema);
}

STATIC td_void drv_vdec_unlock_entry(td_void)
{
    drv_vdec_entry *entry = drv_vdec_get_entry();
    vdec_osal_sema_up(&entry->sema);
}

STATIC td_void drv_vdec_lock_chan(td_s32 chan_id)
{
    drv_vdec_chan *chan = drv_vdec_get_chan(chan_id);
    vdec_osal_sema_down(&chan->sema);
}

STATIC td_void drv_vdec_unlock_chan(td_s32 chan_id)
{
    drv_vdec_chan *chan = drv_vdec_get_chan(chan_id);
    vdec_osal_sema_up(&chan->sema);
}

STATIC td_void drv_vdec_clear_chan(drv_vdec_chan *chan)
{
    chan->vfmw_id = VDEC_INVALID_HANDLE;
    chan->event_map = EXT_VDEC_EVT_UNKNOWN;
    chan->event_valid_map = EXT_VDEC_EVT_UNKNOWN;
    chan->callback = TD_NULL;
    (td_void)memset_s(&chan->statis, sizeof(drv_vdec_statis), 0, sizeof(drv_vdec_statis));
    (td_void)memset_s(&chan->ext_attr, sizeof(vdec_vfmw_param), 0, sizeof(vdec_vfmw_param));
    chan->ext_attr.frame_rate.type = EXT_VDEC_FR_TYPE_MAX;
    chan->ext_attr.is_pvr_info_valid = TD_FALSE;
    chan->ext_attr.start_pts.pts = VDEC_INVALID_PTS;
    chan->ext_attr.speed_info.direct = EXT_VDEC_SPEED_INALID;
    chan->ext_attr.frm_3d_type = -1;
    chan->pvr_buf_num = 0;
    chan->new_frm_evt = 0;
}

STATIC td_s32 drv_vdec_event_id_2_index(td_u64 evt_id)
{
    td_s32 index = VDEC_INVALIED_INDEX;

    if (evt_id <= EXT_VDEC_EVT_UNKNOWN || evt_id >= EXT_VDEC_EVT_ALL) {
        return index;
    }

    for (index = 0; index < VDEC_LINSTEN_NUM; index++) {
        if ((1 << index) == evt_id) {
            return index;
        }
    }

    return VDEC_INVALIED_INDEX;
}

STATIC td_s32 drv_vdec_event_ext_param_err(td_s32 chan_id, td_u32 evt_id, const td_void *param)
{
    td_s32 temp_ret;
    td_s32 index;
    drv_vdec_chan *chan = TD_NULL;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);

    chan = drv_vdec_get_chan(chan_id);
    index = drv_vdec_event_id_2_index(evt_id);
    if (index != VDEC_INVALIED_INDEX) {
        temp_ret = memcpy_s(&chan->listen_event[index].event_param,
            sizeof(ext_vdec_event) - sizeof(ext_vdec_event_id),
            param, sizeof(chan->listen_event[index].event_param));
        if (temp_ret != EOK) {
            return EXT_FAILURE;
        }
        chan->event_valid_map |= evt_id;
        vdec_osal_event_give(&chan->wait_event);
    }
    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_event_call_back(td_s32 chan_id, td_u32 evt_id, const td_void *param, td_u32 size)
{
    td_s32 ret = EXT_FAILURE;
    drv_vdec_chan *chan = TD_NULL;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);

    chan = drv_vdec_get_chan(chan_id);
    if ((evt_id & chan->event_map) != 0) {
        ret = EXT_SUCCESS;
        if (chan->callback != TD_NULL) {
            ret = chan->callback(chan->user_handle, evt_id, param, size);
        } else {
            ret = drv_vdec_event_ext_param_err(chan_id, evt_id, param);
            vdec_check_ret(ret == EXT_SUCCESS);
        }
    } else {
        vdec_print_dbg("chan %d event %d is not in set in map %x!\n", chan_id, evt_id, chan->event_map);
    }

    if (evt_id == EXT_VDEC_EVT_NEW_FRAME) {
        chan->new_frm_evt++;
        vdec_osal_event_give(&chan->wait_frm_event);
    }

    return ret;
}

STATIC td_s32 drv_vdec_set_low_delay(drv_vdec_chan *chan, td_bool low_delay)
{
    td_s32 ret = EXT_SUCCESS;

    chan->ext_attr.enable_low_delay = low_delay;

    if (chan->vfmw_id != (td_s32)VDEC_INVALID_HANDLE) {
        ret = vdec_vfmw_set_low_delay(chan->vfmw_id, low_delay);
    }
    vdec_record_chr_event(chan->vdec_id, MT_VDEC_SET_PARAM, MT_VDEC_SET_HW_PARAM, MT_VDEC_SET_HW_LOW_DELAY_PARAM);

    return ret;
}

STATIC td_s32 drv_vdec_get_low_delay(drv_vdec_chan *chan, td_bool *low_delay)
{
    td_s32 ret = EXT_SUCCESS;

    if (chan->vfmw_id != (td_s32)VDEC_INVALID_HANDLE) {
        ret = vdec_vfmw_get_low_delay(chan->vfmw_id, low_delay);
    }
    vdec_record_chr_event(chan->vdec_id, MT_VDEC_GET_PARAM, MT_VDEC_GET_HW_PARAM, MT_VDEC_GET_HW_LOW_DELAY_PARAM);
    return ret;
}

STATIC td_s32 drv_vdec_set_frm_rate(drv_vdec_chan *chan, ext_vdec_frame_rate *frame_rate)
{
    td_s32 ret = EXT_SUCCESS;

    (td_void)memcpy_s(&chan->ext_attr.frame_rate, sizeof(ext_vdec_frame_rate), frame_rate, sizeof(ext_vdec_frame_rate));

    if (chan->vfmw_id != (td_s32)VDEC_INVALID_HANDLE) {
        ret = vdec_vfmw_set_frame_rate(chan->vfmw_id, frame_rate);
    }
    vdec_record_chr_event(chan->vdec_id, MT_VDEC_SET_PARAM, MT_VDEC_SET_HW_PARAM, MT_VDEC_SET_HW_FRM_RATE_PARAM);

    return ret;
}

STATIC td_s32 drv_vdec_get_frm_rate(drv_vdec_chan *chan, ext_vdec_frame_rate *frame_rate)
{
    td_s32 ret;

    if (chan->vfmw_id != (td_s32)VDEC_INVALID_HANDLE) {
        ret = vdec_vfmw_get_frame_rate(chan->vfmw_id, frame_rate);
    } else {
        (td_void)memcpy_s(frame_rate, sizeof(ext_vdec_frame_rate), &chan->ext_attr.frame_rate,
            sizeof(ext_vdec_frame_rate));
        ret = EXT_SUCCESS;
    }
    vdec_record_chr_event(chan->vdec_id, MT_VDEC_GET_PARAM, MT_VDEC_GET_HW_PARAM, MT_VDEC_GET_HW_FRM_RATE_PARAM);

    return ret;
}

td_s32 drv_vdec_get_feature(td_s32 chan_id, ext_vdec_feature *feature)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    vdec_check_goto(chan->status != VDEC_CHAN_STATE_NULL && chan->vfmw_id != (td_s32)VDEC_INVALID_HANDLE, exit);

    ret = vdec_vfmw_get_feature(chan->vfmw_id, feature);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    feature->standard = chan->attr.standard;

    drv_vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;
exit:
    drv_vdec_unlock_chan(chan_id);
    return EXT_FAILURE;
}

td_s32 drv_vdec_get_status(td_s32 chan_id, ext_vdec_status *status)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    vdec_check_goto(chan->status != VDEC_CHAN_STATE_NULL && chan->vfmw_id != (td_s32)VDEC_INVALID_HANDLE, exit);

    ret = vdec_vfmw_get_status(chan->vfmw_id, status);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    drv_vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;
exit:
    drv_vdec_unlock_chan(chan_id);
    return EXT_FAILURE;
}

STATIC td_s32 drv_vdec_create_vfmw_chan(drv_vdec_chan *chan)
{
    td_s32 ret;
    vdec_vfmw_create_param param = {0};

    ret = vdec_vfmw_init();
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("init vfmw error!\n");
        return EXT_FAILURE;
    }

    param.vdec_handle = chan->vdec_handle;
    param.vdec_id = chan->vdec_id;
    param.attr = &(chan->attr);
    param.callback = drv_vdec_event_call_back;
    ret = vdec_vfmw_create(&chan->vfmw_id, &param);
    if (ret != EXT_SUCCESS) {
        (td_void)vdec_vfmw_exit();
        vdec_print_alert("vdec %d create vfmw failed!\n", chan->vdec_id);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_chan_init(drv_vdec_chan *chan, td_s32 chan_id, td_void *private_data)
{
    chan->vdec_id = chan_id;
    chan->vfmw_id = (td_s32)VDEC_INVALID_HANDLE;
    vdec_osal_sema_init(&chan->sema);
    vdec_osal_event_init(&chan->wait_frm_event);
    vdec_osal_event_init(&chan->wait_event);
    chan->status = VDEC_CHAN_STATE_CREATE;
    chan->unique = private_data;
    chan->cycle_mode = TD_FALSE;
    chan->low_power_test_mode = TD_FALSE;

#ifdef VDEC_PROC_SUPPORT
    if (vdec_drv_proc_get_cycle_mode(chan_id) == TD_TRUE) {
        chan->idx = 0;
        chan->cycle_mode = TD_TRUE;
        OSAL_INIT_LIST_HEAD(&chan->list);
    }
    if (vdec_drv_proc_get_low_power_test_mode(chan_id) == TD_TRUE) {
        chan->low_power_test_mode = TD_TRUE;
    }
#endif
    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_chan_exit(drv_vdec_chan *chan)
{
    td_s32 ret;

    if (chan->vfmw_id != (td_s32)VDEC_INVALID_HANDLE) {
        if (chan->is_stream_buffer_init == TD_TRUE) {
            ret = vdec_vfmw_deinit_input_buf(chan->vfmw_id);
            if (ret != EXT_SUCCESS) {
                vdec_print_alert("chan %d vdec_vfmw_deinit_input_buf fail!\n", chan->vfmw_id);
                return EXT_FAILURE;
            }
            chan->is_stream_buffer_init = TD_FALSE;
        }

        ret = vdec_vfmw_destroy(chan->vfmw_id);
        if (ret != EXT_SUCCESS) {
            vdec_print_alert("vdec %d destroy vfmw %d failed!\n", chan->vdec_id, chan->vfmw_id);
            return EXT_FAILURE;
        }

        ret = vdec_vfmw_exit();
        if (ret != EXT_SUCCESS) {
            vdec_print_alert("vdec exit vfmw fail!\n");
            return EXT_FAILURE;
        }
    }

    drv_vdec_clear_chan(chan);
    chan->status = VDEC_CHAN_STATE_NULL;
    chan->unique = TD_NULL;

    vdec_osal_event_exit(&chan->wait_frm_event);
    vdec_osal_event_exit(&chan->wait_event);
    vdec_osal_sema_exit(&chan->sema);

    return EXT_SUCCESS;
}

td_void drv_vdec_init(td_void)
{
    td_s32 i;
    drv_vdec_entry *entry = drv_vdec_get_entry();

    vdec_osal_sema_init(&entry->sema);
    for (i = 0; i < VDEC_MAX_CHAN_NUM; i++) {
        entry->chan_ptr[i] = TD_NULL;
    }
}

td_void drv_vdec_deinit(td_void)
{
    td_s32 i;
    drv_vdec_entry *entry = drv_vdec_get_entry();

    vdec_osal_sema_exit(&entry->sema);

    for (i = 0; i < VDEC_MAX_CHAN_NUM; i++) {
        if (entry->chan_ptr[i] != TD_NULL) {
            osal_vfree(entry->chan_ptr[i]);
            entry->chan_ptr[i] = TD_NULL;
        }
    }
}

td_s32 drv_vdec_open(td_void)
{
    td_s32 ret;
    drv_vdec_entry *entry = drv_vdec_get_entry();

    drv_vdec_lock_entry();

    if (entry->open_cnt == 0) {
        ret = vdec_vfmw_open();
        if (ret != EXT_SUCCESS) {
            drv_vdec_unlock_entry();
            vdec_print_alert("open vfmw fail!\n");
            return EXT_FAILURE;
        }
    }
    vdec_record_chr_event(-1, MT_VDEC_CREATE, MT_VDEC_OPEN_OK, MT_VDEC_INFO_3_DEFAULT);

    entry->open_cnt++;

    drv_vdec_unlock_entry();

    return EXT_SUCCESS;
}

td_s32 drv_vdec_close(td_void)
{
    td_s32 ret;
    drv_vdec_entry *entry = drv_vdec_get_entry();

    drv_vdec_lock_entry();

    if (entry->open_cnt == 1) {
        if (entry->chan_num > 0) {
            drv_vdec_unlock_entry();
            vdec_print_alert("%u chans do not destroy!\n", entry->chan_num);
            return EXT_FAILURE;
        }

        ret = vdec_vfmw_close();
        if (ret != EXT_SUCCESS) {
            drv_vdec_unlock_entry();
            vdec_print_alert("close vfmw fail!\n");
            return EXT_FAILURE;
        }
    }

    if (entry->open_cnt > 0) {
        entry->open_cnt--;
    }

    drv_vdec_unlock_entry();

    return EXT_SUCCESS;
}

td_s32 drv_vdec_create(td_s32 *chan_id, td_void *private_data)
{
    td_s32 i;
    drv_vdec_chan *chan = TD_NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();
    td_u32 begin;
    td_u32 end;

    vdec_check_ret(chan_id != TD_NULL);

    begin = vdec_osal_get_time();

    drv_vdec_lock_entry();

    for (i = 0; i < VDEC_MAX_CHAN_NUM; i++) {
        if (entry->chan_ptr[i] == TD_NULL) {
            break;
        }
    }

    if (i >= VDEC_MAX_CHAN_NUM) {
        drv_vdec_unlock_entry();
        vdec_print_alert("no free vdec chan!\n");
        return EXT_FAILURE;
    }

    entry->chan_ptr[i] = (drv_vdec_chan *)osal_vmalloc(sizeof(drv_vdec_chan));
    if (entry->chan_ptr[i] == TD_NULL) {
        drv_vdec_unlock_entry();
        vdec_print_err("alloc drv_vdec_chan failed!\n");
        return EXT_FAILURE;
    }
    (td_void)memset_s(entry->chan_ptr[i], sizeof(drv_vdec_chan), 0, sizeof(drv_vdec_chan));
    chan = entry->chan_ptr[i];

    drv_vdec_chan_init(chan, i, private_data);

    entry->chan_num++;
    *chan_id = i;
    drv_vdec_unlock_entry();

    end = vdec_osal_get_time();
    entry->kpi_info[i].create_time = end - begin;

    return EXT_SUCCESS;
}

td_s32 drv_vdec_destroy(td_s32 chan_id)
{
    drv_vdec_chan *chan = TD_NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();
    td_u32 begin;
    td_u32 end;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);

    begin = vdec_osal_get_time();
    drv_vdec_lock_entry();

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status == VDEC_CHAN_STATE_NULL || chan->status == VDEC_CHAN_STATE_START) {
        vdec_print_alert("vdec %d state %d(state_null) error!\n", chan_id, chan->status);
        drv_vdec_unlock_entry();
        return EXT_FAILURE;
    }

    if (drv_vdec_chan_exit(chan) != EXT_SUCCESS) {
        drv_vdec_unlock_entry();
        return EXT_FAILURE;
    }

    entry->chan_num--;
    osal_vfree(entry->chan_ptr[chan_id]);
    entry->chan_ptr[chan_id] = TD_NULL;

    drv_vdec_unlock_entry();

    end = vdec_osal_get_time();
    entry->kpi_info[chan_id].destroy_time = end - begin;

    return EXT_SUCCESS;
}

td_s32 drv_vdec_start(td_s32 chan_id)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();
    td_u32 begin;
    td_u32 end;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);
    begin = vdec_osal_get_time();

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status == VDEC_CHAN_STATE_NULL || chan->vfmw_id == (td_s32)VDEC_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec %d status(%d) or vfmw_id(%d) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        return EXT_FAILURE;
    }

    if (chan->status == VDEC_CHAN_STATE_START) {
        drv_vdec_unlock_chan(chan_id);
        return EXT_SUCCESS;
    }

    ret = vdec_vfmw_start(chan->vfmw_id);
    if (ret != EXT_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec %d start vfmw %d failed!\n", chan_id, chan->vfmw_id);
        return EXT_FAILURE;
    }

    chan->status = VDEC_CHAN_STATE_START;

    drv_vdec_unlock_chan(chan_id);

    end = vdec_osal_get_time();
    entry->kpi_info[chan_id].start_time = end - begin;

    return EXT_SUCCESS;
}

td_s32 drv_vdec_stop(td_s32 chan_id)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();
    td_u32 begin;
    td_u32 end;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);

    begin = vdec_osal_get_time();

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status == VDEC_CHAN_STATE_NULL || chan->vfmw_id == (td_s32)VDEC_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec %d status(%d) or vfmw_id(%d) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        return EXT_FAILURE;
    }

    if (chan->status == VDEC_CHAN_STATE_STOP) {
        drv_vdec_unlock_chan(chan_id);
        return EXT_SUCCESS;
    }

    ret = vdec_vfmw_stop(chan->vfmw_id);
    if (ret != EXT_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec %d stop vfmw %d failed!\n", chan_id, chan->vfmw_id);
        return EXT_FAILURE;
    }

    vdec_osal_event_give(&chan->wait_frm_event);
    chan->status = VDEC_CHAN_STATE_STOP;

    drv_vdec_unlock_chan(chan_id);
    end = vdec_osal_get_time();
    entry->kpi_info[chan_id].stop_time = end - begin;

    return EXT_SUCCESS;
}

td_s32 drv_vdec_reset(td_s32 chan_id)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();
    td_u32 begin;
    td_u32 end;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);

    begin = vdec_osal_get_time();

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status != VDEC_CHAN_STATE_STOP || chan->vfmw_id == (td_s32)VDEC_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec %d status(%d) or vfmw_id(%d) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        return EXT_FAILURE;
    }

    if (chan->is_stream_buffer_init == TD_TRUE) {
        ret = vdec_vfmw_deinit_input_buf(chan->vfmw_id);
        if (ret != EXT_SUCCESS) {
            vdec_print_alert("chan %d vdec_vfmw_deinit_input_buf fail!\n", chan->vfmw_id);
            drv_vdec_unlock_chan(chan_id);
            return EXT_FAILURE;
        }
        chan->is_stream_buffer_init = TD_FALSE;
    }

    ret = vdec_vfmw_destroy(chan->vfmw_id);
    if (ret != EXT_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec %d destroy vfmw %d failed!\n", chan_id, chan->vfmw_id);
        return EXT_FAILURE;
    }

    ret = vdec_vfmw_exit();
    if (ret != EXT_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec exit vfmw fail!\n");
        return EXT_FAILURE;
    }

    drv_vdec_clear_chan(chan);

    vdec_osal_event_give(&chan->wait_frm_event);
    drv_vdec_unlock_chan(chan_id);

    end = vdec_osal_get_time();
    entry->kpi_info[chan_id].reset_time = end - begin;

    return EXT_SUCCESS;
}

td_s32 drv_vdec_flush(td_s32 chan_id, const ext_vdec_opt_flush *option)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();
    td_u32 begin;
    td_u32 end;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);
    vdec_check_ret(option != TD_NULL);

    begin = vdec_osal_get_time();

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status != VDEC_CHAN_STATE_STOP || chan->vfmw_id == (td_s32)VDEC_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec %d status(%d) or vfmw_id(%d) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        return EXT_FAILURE;
    }

    ret = vdec_vfmw_flush(chan->vfmw_id, option);
    if (ret != EXT_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec %d flush vfmw %d failed!\n", chan_id, chan->vfmw_id);
        return EXT_FAILURE;
    }

    vdec_osal_event_give(&chan->wait_frm_event);
    drv_vdec_unlock_chan(chan_id);

    end = vdec_osal_get_time();
    entry->kpi_info[chan_id].flush_time = end - begin;

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_check_user_space_addr(td_u8 *addr_start, td_u32 len)
{
    td_u8 check;
    td_u8 *addr_end = TD_NULL;

    if (len == 0) {
        return EXT_FAILURE;
    }

    addr_end = addr_start + len - 1;
    if (osal_copy_from_user(&check, addr_end, 1) != 0) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_vfmw_start(drv_vdec_chan *chan)
{
    td_s32 ret;

    ret = vdec_vfmw_start(chan->vfmw_id);
    vdec_check_ret(ret == EXT_SUCCESS);

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_decode_fill_stream(const drv_vdec_decode_param *param, ext_vdec_stream *stream_info)
{
    td_s32 ret;
    td_u8 *stream_buf = TD_NULL;

    stream_buf = stream_info->vir_addr;
    vdec_check_ret(stream_buf != TD_NULL);

    ret = (td_s32)vdec_copy_from_user(stream_buf, param->stream->vir_addr, param->stream->length);
    vdec_check_ret(ret == EXT_SUCCESS);

    stream_info->is_end_of_frame = TD_TRUE;

    return EXT_SUCCESS;
}

#ifdef VDEC_HEIF_SUPPORT
STATIC td_u32 drv_vdec_get_heif_stream_length(const drv_vdec_decode_param *param, drv_vdec_heif_info *heif_info, ext_vdec_heif_select *select)
{
    td_s32 ret;
    td_u32 expect_len = 0;
    drv_vdec_heif_stream heif_stream = {0};

    heif_stream.start = param->stream->vir_addr;
    heif_stream.length = param->stream->length;
    ret = drv_vdec_heif_parse_info(&heif_stream, heif_info);
    vdec_check_ret_val(ret == EXT_SUCCESS, 0);

    if (*select == EXT_VDEC_HEIF_THUMBNAIL && heif_info->thmb_pic.total_stream_len == 0) {
        vdec_print_info("select thumbnail image but contain no thumbnail, take original image instead.\n");
        *select = EXT_VDEC_HEIF_ORIGINAL;
    }
    expect_len = (*select == EXT_VDEC_HEIF_ORIGINAL) ? heif_info->orig_pic.total_stream_len : heif_info->thmb_pic.total_stream_len;

    return expect_len;
}
#endif

STATIC td_s32 drv_vdec_vfmw_send_stream(drv_vdec_chan *chan, const drv_vdec_decode_param *param)
{
    td_s32 ret;
    ext_vdec_stream stream_info = {0};
    ext_vdec_opt_get_stream deq_opt = {0};
    td_u32 expect_len = param->stream->length;

#ifdef VDEC_HEIF_SUPPORT
    chan->ori_select = chan->act_select = param->option.heif.select;
    if (chan->attr.standard == EXT_VDEC_STD_HEIF) {
        expect_len = drv_vdec_get_heif_stream_length(param, &chan->heif_info, &chan->act_select);
    }
#endif

    vdec_check_ret(expect_len > 0);

    deq_opt.expect_size = expect_len;
    ret = vdec_vfmw_get_stream(chan->vfmw_id, &deq_opt, &stream_info);
    vdec_check_ret(ret == EXT_SUCCESS);

    if (chan->attr.standard == EXT_VDEC_STD_HEIF) {
#ifdef VDEC_HEIF_SUPPORT
        drv_vdec_heif_stream h265_stream = {0};

        h265_stream.start = stream_info.vir_addr;
        h265_stream.length = stream_info.length;
        ret = drv_vdec_heif_fill_stream(chan->act_select, &chan->heif_info, &h265_stream);
        vdec_check_goto(ret == EXT_SUCCESS, err_exit);
#endif
    } else {
        ret = drv_vdec_decode_fill_stream(param, &stream_info);
        vdec_check_goto(ret == EXT_SUCCESS, err_exit);
    }

    ret = vdec_vfmw_put_stream(chan->vfmw_id, &stream_info);
    vdec_check_ret(ret == EXT_SUCCESS);
    vdec_record_chr_event(chan->vdec_id, MT_VDEC_I_FRAME_DECODE, MT_VDEC_VFMW_I_DECODE, MT_VDEC_VFMW_I_PUT_STM_OK);

    ret = vdec_vfmw_send_eos(chan->vfmw_id);
    vdec_check_ret(ret == EXT_SUCCESS);
    vdec_record_chr_event(chan->vdec_id, MT_VDEC_I_FRAME_DECODE, MT_VDEC_VFMW_I_DECODE, MT_VDEC_VFMW_I_SEND_EOS_OK);

    return EXT_SUCCESS;

err_exit:
    (td_void)vdec_vfmw_put_stream(chan->vfmw_id, &stream_info);

    return EXT_FAILURE;
}

STATIC td_s32 drv_vdec_vfmw_receive_frame(const drv_vdec_chan *chan, drv_vdec_decode_param *param)
{
    td_s32 ret;
    td_u32 time_begin, time_tmp;
    ext_vdec_ext_frm_info ext_frm_info;
    td_u32 time_out_us = (td_u32)param->option.time_out * 1000;

    time_begin = vdec_osal_get_time();

    do {
        ret = vdec_vfmw_acquire_frame(chan->vfmw_id, TD_NULL, param->frame, &ext_frm_info);
        if (ret == EXT_SUCCESS) {
            break;
        }

        time_tmp = vdec_osal_get_time();
        time_begin = (time_tmp <= time_begin) ? time_tmp : time_begin;

        if (time_tmp - time_begin >= time_out_us) {
            break;
        }

        vdec_osal_msleep(1);
    } while (1);

    if (ret != EXT_SUCCESS) {
        vdec_print_err("I frame decode recv frame failed cost %u us, time_out %u us\n", time_tmp - time_begin, time_out_us);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_void drv_vdec_decode_finish_work(const drv_vdec_chan *chan, drv_vdec_decode_param *param)
{
#ifdef VDEC_HEIF_SUPPORT
    if (chan->attr.standard == EXT_VDEC_STD_HEIF) {
        param->frame->standard = EXT_VDEC_STD_HEIF;
        param->frame->orientation = chan->heif_info.format.orientation;
    }
#else
    (td_void)(chan);
    (td_void)(param);
#endif
}

STATIC td_void drv_vdec_vfmw_stop(drv_vdec_chan *chan)
{
    (td_void)vdec_vfmw_stop(chan->vfmw_id);
    (td_void)vdec_vfmw_reset(chan->vfmw_id);
}

td_s32 drv_vdec_decode(td_s32 chan_id, drv_vdec_decode_param *param)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM && param != TD_NULL);

    ret = drv_vdec_check_user_space_addr(param->stream->vir_addr, param->stream->length);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("%s can not read\n", __func__);
        return EXT_FAILURE;
    }

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    vdec_check_goto(chan != TD_NULL, exit_0);
    vdec_check_goto(chan->vfmw_id != (td_s32)VDEC_INVALID_HANDLE, exit_0);

    ret = drv_vdec_vfmw_start(chan);
    vdec_check_goto(ret == EXT_SUCCESS, exit_0);
    vdec_record_chr_event(chan_id, MT_VDEC_I_FRAME_DECODE, MT_VDEC_VFMW_I_DECODE, MT_VDEC_VFMW_I_START_OK);

    ret = drv_vdec_vfmw_send_stream(chan, param);
    vdec_check_goto(ret == EXT_SUCCESS, exit_1);
    vdec_record_chr_event(chan->vdec_id, MT_VDEC_I_FRAME_DECODE, MT_VDEC_VFMW_I_DECODE, MT_VDEC_VFMW_I_DEQUEUE_OK);

    ret = drv_vdec_vfmw_receive_frame(chan, param);
    vdec_check_goto(ret == EXT_SUCCESS, exit_1);
    vdec_record_chr_event(chan->vdec_id, MT_VDEC_I_FRAME_DECODE, MT_VDEC_VFMW_I_DECODE, MT_VDEC_VFMW_I_RECIEVE_OK);

    drv_vdec_decode_finish_work(chan, param);

exit_1:
    drv_vdec_vfmw_stop(chan);

exit_0:
    drv_vdec_unlock_chan(chan_id);

    return ret;
}

td_s32 drv_vdec_get_attr(td_s32 chan_id, ext_vdec_attr *attr)
{
    drv_vdec_chan *chan = TD_NULL;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);
    vdec_check_ret(attr != TD_NULL);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status == VDEC_CHAN_STATE_NULL) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec %d state %d(state_null) error!\n", chan_id, chan->status);
        return EXT_FAILURE;
    }

    (td_void)memcpy_s(attr, sizeof(ext_vdec_attr), &chan->attr, sizeof(chan->attr));

    drv_vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;
}

td_s32 drv_vdec_set_attr(td_s32 chan_id, td_handle vdec_handle, const ext_vdec_attr *attr)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();
    td_u32 begin;
    td_u32 end;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);
    vdec_check_ret(attr != TD_NULL);

    begin = vdec_osal_get_time();

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status == VDEC_CHAN_STATE_NULL || chan->status == VDEC_CHAN_STATE_START) {
        vdec_print_alert("chan %d status %d error!\n", chan_id, chan->status);
        goto exit;
    }

    chan->vdec_handle = vdec_handle;
    (td_void)memcpy_s(&chan->attr, sizeof(chan->attr), attr, sizeof(ext_vdec_attr));

    if (chan->vfmw_id == (td_s32)VDEC_INVALID_HANDLE) {
        ret = drv_vdec_create_vfmw_chan(chan);
        if (ret != EXT_SUCCESS) {
            vdec_print_alert("create vfmw chan failed!\n");
            goto exit;
        }
    }

    if (chan->is_stream_buffer_init == TD_FALSE) {
        ret = vdec_vfmw_init_input_buf(chan->vfmw_id, attr->max_stream_size);
        if (ret != EXT_SUCCESS) {
            vdec_print_alert("chan %d vdec_vfmw_init_input_buf fail!\n", chan->vfmw_id);
            goto exit;
        }
        chan->is_stream_buffer_init = TD_TRUE;
    }

    ret = vdec_vfmw_set_attr(chan->vfmw_id, &chan->attr, &chan->ext_attr);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("chan %d set attr fail!\n", chan->vfmw_id);
        goto exit;
    }

    drv_vdec_unlock_chan(chan_id);

    end = vdec_osal_get_time();
    entry->kpi_info[chan_id].config_time = end - begin;

    return EXT_SUCCESS;
exit:
    drv_vdec_unlock_chan(chan_id);
    return EXT_FAILURE;
}

td_s32 drv_vdec_register_event(td_s32 chan_id, td_u64 event_map)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();
    td_u32 begin;
    td_u32 end;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);

    begin = vdec_osal_get_time();

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    chan->event_map = event_map;

    ret = vdec_vfmw_set_event_map(chan->vfmw_id, event_map);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("chan %d set event map %llx fail!\n", chan_id, event_map);
        drv_vdec_unlock_chan(chan_id);
        return EXT_FAILURE;
    }

    drv_vdec_unlock_chan(chan_id);

    end = vdec_osal_get_time();
    entry->kpi_info[chan_id].reg_event_time = end - begin;

    return EXT_SUCCESS;
}

STATIC int drv_vdec_listen_event_wait(td_s32 chan_id, const ext_vdec_opt_listen *option, ext_vdec_event *event)
{
    td_s32 index;
    td_u32 time_out = (td_u32)option->time_out;

    drv_vdec_chan *chan = TD_NULL;
    chan = drv_vdec_get_chan(chan_id);

    if (time_out != 0) {
        vdec_osal_event_wait_interruptible(&chan->wait_event, time_out);
    }
    drv_vdec_lock_chan(chan_id);
    if (chan->status == VDEC_CHAN_STATE_NULL || chan->vfmw_id == (td_s32)VDEC_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec %d may destoryed!\n", chan_id);
        return EXT_FAILURE;
    }

    if (((td_u32)event->event_id & chan->event_map) == 0) {
        vdec_print_alert("vdec %d listen unregistered event(0x%x) event_map(%llx) !\n",
            chan_id, event->event_id, chan->event_map);
        drv_vdec_unlock_chan(chan_id);
        return EXT_FAILURE;
    }

    index = drv_vdec_event_id_2_index(event->event_id);
    vdec_check_goto(index >= 0 && index < VDEC_LINSTEN_NUM, err);
    if ((chan->event_valid_map & (td_u32)event->event_id) != 0) {
        chan->event_valid_map &= ~((td_u32)event->event_id);
        event->event_param = chan->listen_event[index].event_param;
        drv_vdec_unlock_chan(chan_id);
        return EXT_SUCCESS;
    }
err:
    drv_vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 drv_vdec_listen_event(td_s32 chan_id, const ext_vdec_opt_listen *option, ext_vdec_event *event)
{
    td_s32 index;
    drv_vdec_chan *chan = TD_NULL;

    vdec_check_ret(option != TD_NULL);
    vdec_check_ret(event != TD_NULL);
    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);

    drv_vdec_lock_chan(chan_id);
    chan = drv_vdec_get_chan(chan_id);
    if (((td_u32)event->event_id & chan->event_map) == 0) {
        vdec_print_alert("vdec %d listen unregistered event(%d) event_map(%llu) !\n",
            chan_id, event->event_id, chan->event_map);
        drv_vdec_unlock_chan(chan_id);
        return EXT_FAILURE;
    }

    if (chan->status == VDEC_CHAN_STATE_NULL || chan->vfmw_id == (td_s32)VDEC_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec %d status(%d) or vfmw_id(%d) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        return EXT_FAILURE;
    }

    index = drv_vdec_event_id_2_index(event->event_id);
    if ((chan->event_valid_map & (td_u32)event->event_id) != 0 && (index != VDEC_INVALIED_INDEX)) {
        chan->event_valid_map &= ~((td_u32)event->event_id);
        event->event_param = chan->listen_event[index].event_param;
        drv_vdec_unlock_chan(chan_id);
        return EXT_SUCCESS;
    }
    drv_vdec_unlock_chan(chan_id);

    return drv_vdec_listen_event_wait(chan_id, option, event);
}

td_s32 drv_vdec_put_stream(td_s32 chan_id, const ext_vdec_stream *stream)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);
    vdec_check_ret(stream != TD_NULL);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    chan->statis.try_que_stm_num++;

    if (chan->status == VDEC_CHAN_STATE_NULL || chan->vfmw_id == (td_s32)VDEC_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec %d status(%d) or vfmw_id(%d) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        return EXT_FAILURE;
    }

    ret = vdec_vfmw_put_stream(chan->vfmw_id, stream);
    if (ret != EXT_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec %d queue vfmw %d stream failed!\n", chan_id, chan->vfmw_id);
        return EXT_FAILURE;
    }

    if (chan->statis.que_stm_num <= 1 && chan->statis.acq_frm_num <= 1) {
        ret = vdec_vfmw_wakeup(chan->vfmw_id, TD_NULL, 0);
        if (ret != EXT_SUCCESS) {
            vdec_print_dbg("vdec %d wakeup vfmw %d failed!\n", chan_id, chan->vfmw_id);
        }
    }

    chan->statis.que_stm_num++;

    drv_vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;
}

td_s32 drv_vdec_get_stream(td_s32 chan_id, const ext_vdec_opt_get_stream *option, ext_vdec_stream *stream)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);
    vdec_check_ret(option != TD_NULL);
    vdec_check_ret(stream != TD_NULL);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    chan->statis.try_deq_stm_num++;

    if (chan->status == VDEC_CHAN_STATE_NULL || chan->vfmw_id == (td_s32)VDEC_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec %d status(%d) or vfmw_id(%d) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        return EXT_FAILURE;
    }

    if (chan->low_power_test_mode == TD_TRUE) {
        drv_vdec_unlock_chan(chan_id);
        return EXT_FAILURE;
    }

    ret = vdec_vfmw_get_stream(chan->vfmw_id, option, stream);
    if (ret != EXT_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_dbg("vdec %d dequeue vfmw %d stream failed!\n", chan_id, chan->vfmw_id);
        return EXT_FAILURE;
    }

    chan->statis.deq_stm_num++;

    drv_vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_process_hw_error(ext_vdec_frame *frame, drv_vdec_chan *chan)
{
    if (frame->error_ratio != 0 && (chan->event_map & EXT_VDEC_EVT_ERR_DECODE) != 0) {
        td_s32 index = drv_vdec_event_id_2_index(EXT_VDEC_EVT_ERR_DECODE);
        vdec_check_ret(index != VDEC_INVALIED_INDEX);
        chan->listen_event[index].event_param.error_decode.error_type = EXT_VDEC_ERROR_UNDEFINED;
        chan->listen_event[index].event_param.error_decode.pts = frame->src_pts;
        chan->event_valid_map |= EXT_VDEC_EVT_ERR_DECODE;
        vdec_osal_event_give(&chan->wait_event);
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_acquire_frame_wait(td_s32 chan_id, const ext_vdec_opt_acq_frame *option, td_void *frame,
    ext_vdec_ext_frm_info *ext_frm_info, vdec_osal_event *wait_frm_event)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;

    vdec_check_ret(option != TD_NULL);
    vdec_check_ret(frame != TD_NULL);
    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);
    vdec_check_ret(wait_frm_event != TD_NULL);

    ret = vdec_osal_event_wait(wait_frm_event, (td_u32)option->time_out);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status == VDEC_CHAN_STATE_NULL || chan->vfmw_id == (td_s32)VDEC_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec %d status(%d) or vfmw_id(%d) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        return EXT_FAILURE;
    }

    ret = vdec_vfmw_acquire_frame(chan->vfmw_id, option, frame, ext_frm_info);
    if (ret != EXT_SUCCESS) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_dbg("vdec %d acquire vfmw %d frame failed!\n", chan_id, chan->vfmw_id);
        return EXT_FAILURE;
    }

    chan->statis.acq_frm_num++;

    ret = drv_vdec_process_hw_error((ext_vdec_frame *)frame, chan);
    vdec_check_goto(ret == EXT_SUCCESS, err);

    drv_vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;
err:
    drv_vdec_unlock_chan(chan_id);
    return EXT_FAILURE;
}

STATIC td_s32 drv_vdec_del_cycle_list(drv_vdec_chan *chan, ext_vdec_frame *frame)
{
    drv_vdec_cycle_frame *cycle_frame = TD_NULL;

    if (osal_list_empty(&chan->list) == 1) {
        return EXT_FAILURE;
    }

    cycle_frame = osal_list_first_entry(&chan->list, drv_vdec_cycle_frame, node);
    vdec_check_ret(cycle_frame != TD_NULL);
    osal_list_del(&cycle_frame->node);

    (td_void)memcpy_s(frame, sizeof(ext_vdec_frame), &cycle_frame->vdec_frame, sizeof(ext_vdec_frame));

    return EXT_SUCCESS;
}

STATIC td_void drv_vdec_add_cycle_list(drv_vdec_chan *chan, ext_vdec_frame *frame)
{
    td_s32 i;
    if (chan->idx < VDEC_CYCLE_MAX_NUM) {
        (td_void)memcpy_s(&chan->frame[chan->idx].vdec_frame, sizeof(ext_vdec_frame), frame, sizeof(ext_vdec_frame));
        osal_list_add_tail(&(chan->frame[chan->idx].node), &chan->list);
        chan->idx++;
    } else {
        for (i = 0; i < VDEC_CYCLE_MAX_NUM; i++) {
            if (frame->frame_addr.start_addr == chan->frame[i].vdec_frame.frame_addr.start_addr) {
                osal_list_add_tail(&(chan->frame[i].node), &chan->list);
            }
        }
    }
}

/* Output a fake frame buffer for low power test */
STATIC td_void drv_vdec_low_power_test_process(drv_vdec_chan *chan, ext_vdec_frame *frame)
{
    td_u32 start_addr = 0xFFFFFFFF; /* Confirm a valid value based on the product */

    frame->frame_index = chan->statis.acq_frm_num;
    frame->width = 466; /* 466 is test num */
    frame->height = 466; /* 466 is test num */
    frame->aspect_width = 466; /* 466 is test num */
    frame->aspect_height = 466; /* 466 is test num */
    frame->pts = 1000 * 40 * frame->frame_index; /* 1000 40 is test num */
    frame->src_pts = frame->pts;
    frame->frame_rate = 25 * 1000; /* 25 1000 is test num */
    frame->src_frame_rate = frame->frame_rate;
    frame->error_ratio = 0;
    frame->splice_info.type = EXT_VDEC_SPLICE_NONE;
    frame->standard = EXT_VDEC_STD_H264;
    frame->bit_depth = EXT_VDEC_BIT_DEPTH_8;
    frame->frame_addr.start_addr = start_addr;
    frame->frame_addr.offset_y = 0;
    frame->frame_addr.offset_c = frame->width * frame->height;
    frame->frame_addr.stride_y = 480; /* 480 is test num */
    frame->frame_addr.stride_c = 480; /* 480 is test num */
    frame->frame_addr.vir_addr_y = (td_u8 *)(td_uintptr_t)start_addr;
    frame->frame_addr.vir_addr_c = frame->frame_addr.vir_addr_y + frame->frame_addr.offset_c;
}

td_s32 drv_vdec_acquire_frame(td_s32 chan_id, const ext_vdec_opt_acq_frame *option, ext_vdec_frame *frame,
    ext_vdec_ext_frm_info *ext_frm_info)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;
    vdec_osal_event *wait_frm_event = TD_NULL;

    vdec_check_ret((option != TD_NULL) && (frame != TD_NULL) && (chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM));

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    chan->statis.try_acq_frm_num++;

    if (chan->status == VDEC_CHAN_STATE_NULL || chan->vfmw_id == (td_s32)VDEC_INVALID_HANDLE || chan->attr.standard == EXT_VDEC_STD_HEIF) {
        vdec_print_alert("vdec %d status(%d) or vfmw_id(%d) or standard(heif) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        goto err;
    }

    if (chan->low_power_test_mode == TD_TRUE) {
        drv_vdec_low_power_test_process(chan, frame);
        goto out;
    }

    vdec_osal_event_init_cond(&chan->wait_frm_event);
    if (chan->cycle_mode == TD_TRUE && chan->statis.acq_frm_num >= VDEC_CYCLE_MAX_NUM) {
        ret = drv_vdec_del_cycle_list(chan, (ext_vdec_frame *)frame);
    } else {
        ret = vdec_vfmw_acquire_frame(chan->vfmw_id, option, frame, ext_frm_info);
    }

    if (ret != EXT_SUCCESS) {
        if (option->time_out > 0 || option->time_out == -1) {
            wait_frm_event = &chan->wait_frm_event;
            drv_vdec_unlock_chan(chan_id);
            ret = drv_vdec_acquire_frame_wait(chan_id, option, frame, ext_frm_info, wait_frm_event);
            return ret;
        } else {
            vdec_print_dbg("vdec %d acquire vfmw %d frame failed!\n", chan_id, chan->vfmw_id);
            goto err;
        }
    }

#ifdef VDEC_PROC_SUPPORT
    if (vdec_drv_proc_get_bypass(chan_id) == TD_TRUE) {
        vdec_vfmw_release_frame(chan->vfmw_id, frame);
        goto err;
    }
#endif

out:
    chan->statis.acq_frm_num++;

    ret = drv_vdec_process_hw_error((ext_vdec_frame *)frame, chan);
    vdec_check_goto(ret == EXT_SUCCESS, err);

    drv_vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;

err:
    drv_vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 drv_vdec_release_frame(td_s32 chan_id, const ext_vdec_frame *frame)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;

    vdec_check_ret(frame != TD_NULL);
    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    chan->statis.try_rel_frm_num++;

    if (chan->status == VDEC_CHAN_STATE_NULL || chan->vfmw_id == (td_s32)VDEC_INVALID_HANDLE) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec %d status(%d) or vfmw_id(%d) invalid!\n", chan_id, chan->status, chan->vfmw_id);
        return EXT_FAILURE;
    }

    if (chan->low_power_test_mode == TD_TRUE) {
        goto out;
    }

    if (chan->cycle_mode == TD_TRUE) {
        drv_vdec_add_cycle_list(chan, (ext_vdec_frame *)frame);
    } else {
        ret = vdec_vfmw_release_frame(chan->vfmw_id, frame);
        if (ret != EXT_SUCCESS) {
            drv_vdec_unlock_chan(chan_id);
            vdec_print_alert("vdec %d release vfmw %d frame failed!\n", chan_id, chan->vfmw_id);
            return EXT_FAILURE;
        }
    }

out:
    chan->statis.rel_frm_num++;

    drv_vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;
}

td_s32 drv_vdec_get_param(td_s32 chan_id, ext_vdec_param *param)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);
    vdec_check_ret(param != TD_NULL);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    vdec_check_goto(chan->status != VDEC_CHAN_STATE_NULL, err);

    ret = drv_vdec_get_low_delay(chan, &param->low_delay);
    vdec_check_goto(ret == EXT_SUCCESS, err);

    ret = drv_vdec_get_frm_rate(chan, &param->frame_rate);
    vdec_check_goto(ret == EXT_SUCCESS, err);

    drv_vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;
err:
    drv_vdec_unlock_chan(chan_id);
    return EXT_FAILURE;
}

td_s32 drv_vdec_set_param(td_s32 chan_id, ext_vdec_param *param)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);
    vdec_check_ret(param != TD_NULL);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    vdec_check_goto(chan->status != VDEC_CHAN_STATE_NULL, err);

    ret = drv_vdec_set_low_delay(chan, param->low_delay);
    vdec_check_goto(ret == EXT_SUCCESS, err);

    ret = drv_vdec_set_frm_rate(chan, &param->frame_rate);
    vdec_check_goto(ret == EXT_SUCCESS, err);

    drv_vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;
err:
    drv_vdec_unlock_chan(chan_id);
    return EXT_FAILURE;
}

td_s32 drv_vdec_is_std_support(ext_vdec_opt_is_std_support *support)
{
    return vdec_vfmw_is_std_support(support);
}

td_s32 drv_vdec_get_cap(ext_vdec_capability *cap)
{
    return vdec_vfmw_get_cap(cap);
}

td_s32 drv_vdec_send_eos(td_s32 chan_id)
{
    td_s32 ret = EXT_SUCCESS;
    drv_vdec_chan *chan = TD_NULL;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);
    drv_vdec_lock_chan(chan_id);
    chan = drv_vdec_get_chan(chan_id);
    vdec_check_goto(chan->status != VDEC_CHAN_STATE_NULL, err);

    if (chan->vfmw_id != (td_s32)VDEC_INVALID_HANDLE) {
        ret = vdec_vfmw_send_eos(chan->vfmw_id);
    }
    drv_vdec_unlock_chan(chan_id);

    return ret;
err:
    drv_vdec_unlock_chan(chan_id);
    return EXT_FAILURE;
}

td_s32 drv_vdec_bind_frm_buffer(td_s32 chan_id, ext_vdec_frame_buf *buffer)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;

    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);
    vdec_check_ret(buffer != TD_NULL);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    vdec_check_goto(chan->status != VDEC_CHAN_STATE_NULL, err);

    ret = vdec_vfmw_bind_extern_frame_buffer(chan_id, buffer);
    vdec_check_goto(ret == EXT_SUCCESS, err);

    drv_vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;
err:
    drv_vdec_unlock_chan(chan_id);
    return EXT_FAILURE;
}

STATIC td_s32 drv_vdec_check_command(const drv_vdec_chan *chan, ext_vdec_cmd_id cmd)
{
    (td_void)(cmd); /* avoid to build err when printing is not supported. */
    if (chan->status == VDEC_CHAN_STATE_NULL || chan->vfmw_id == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("vdec %d cmd 0x%x status(%d) or vfmw_id(%d) invalid!\n",
            chan->vdec_id, cmd, chan->status, chan->vfmw_id);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 drv_vdec_command(td_s32 chan_id, ext_vdec_cmd_id cmd_id, td_u8 *param, td_u32 param_size)
{
    td_s32 ret;
    drv_vdec_chan *chan = TD_NULL;
    td_s32 vfmw_id = VDEC_INVALID_HANDLE;
    td_bool is_chn_valid;

    vdec_check_ret(param != TD_NULL);
    is_chn_valid = (chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM) ? TD_TRUE : TD_FALSE;

    if (is_chn_valid) {
        drv_vdec_lock_chan(chan_id);

        chan = drv_vdec_get_chan(chan_id);
        if (drv_vdec_check_command(chan, cmd_id) != EXT_SUCCESS) {
            drv_vdec_unlock_chan(chan_id);
            vdec_print_alert("chan %d check comamnd %d fail!\n", chan_id, cmd_id);
            return EXT_FAILURE;
        }

        vfmw_id = chan->vfmw_id;
    }

    ret = vdec_vfmw_command(vfmw_id, cmd_id, param, param_size);

    if (is_chn_valid) {
        drv_vdec_unlock_chan(chan_id);
    }

    return ret;
}

td_s32 drv_vdec_register_callback(td_s32 chan_id, td_handle user_handle, fn_drv_vdec_callback func)
{
    drv_vdec_chan *chan = TD_NULL;

    vdec_check_ret(func != TD_NULL);
    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status == VDEC_CHAN_STATE_NULL) {
        drv_vdec_unlock_chan(chan_id);
        vdec_print_alert("vdec %d status(%d) invalid!\n", chan_id, chan->status);
        return EXT_FAILURE;
    }

    chan->callback = func;
    chan->user_handle = user_handle;

    drv_vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;
}

td_s32 drv_vdec_suspend(td_void)
{
    td_s32 ret;

    drv_vdec_lock_entry();

    ret = vdec_vfmw_suspend();

    drv_vdec_unlock_entry();

    return ret;
}

td_s32 drv_vdec_resume(td_void)
{
    td_s32 ret;

    drv_vdec_lock_entry();

    ret = vdec_vfmw_resume();

    drv_vdec_unlock_entry();

    return ret;
}

td_s32 drv_vdec_freeze(td_void)
{
    td_s32 ret;

    drv_vdec_lock_entry();

    ret = vdec_vfmw_freeze();

    drv_vdec_unlock_entry();

    return ret;
}

td_s32 drv_vdec_restore(td_void)
{
    td_s32 ret;

    drv_vdec_lock_entry();

    ret = vdec_vfmw_restore();

    drv_vdec_unlock_entry();

    return ret;
}

td_void drv_vdec_release(const td_void *private_data)
{
    td_s32 i;
    drv_vdec_chan *chan = TD_NULL;
    drv_vdec_entry *entry = drv_vdec_get_entry();

    drv_vdec_lock_entry();

    for (i = 0; i < VDEC_MAX_CHAN_NUM; i++) {
        if (entry->chan_ptr[i] == TD_NULL) {
            continue;
        }
        drv_vdec_lock_chan(i);
        chan = drv_vdec_get_chan(i);
        if (chan->unique != private_data || chan->status == VDEC_CHAN_STATE_NULL) {
            drv_vdec_unlock_chan(i);
            continue;
        }

        if (chan->vfmw_id != (td_s32)VDEC_INVALID_HANDLE) {
            (td_void)vdec_vfmw_stop(chan->vfmw_id);
            if (chan->is_stream_buffer_init == TD_TRUE) {
                (td_void)vdec_vfmw_deinit_input_buf(chan->vfmw_id);
                chan->is_stream_buffer_init = TD_FALSE;
            }
            (td_void)vdec_vfmw_destroy(chan->vfmw_id);
            (td_void)vdec_vfmw_exit();
        }

        drv_vdec_clear_chan(chan);
        chan->status = VDEC_CHAN_STATE_NULL;
        chan->unique = TD_NULL;
        drv_vdec_unlock_chan(i);
        entry->chan_num--;
    }

    drv_vdec_unlock_entry();

    return;
}

#ifdef VDEC_PROC_SUPPORT
struct std_string {
    td_s32 std;
    td_char string[30];    /* 30, string max length. */
};

struct std_string g_std_str[] = {
    {EXT_VDEC_STD_H265,  "h265"},
    {EXT_VDEC_STD_H264,  "h264"},
    {EXT_VDEC_STD_MJPEG, "mjpeg"},
    {EXT_VDEC_STD_HEIF,  "heif"},
};

STATIC td_char* drv_vdec_get_standard(ext_vdec_standard standard)
{
    td_u32 i;
    td_u32 size;

    size = (td_u32)sizeof(g_std_str) / (td_u32)sizeof(struct std_string);

    for (i = 0; i < size; i++) {
        if (standard == (ext_vdec_standard)g_std_str[i].std) {
            return g_std_str[i].string;
        }
    }

    return "unknown";
}

STATIC td_void drv_vdec_proc_kpi_info_print(td_void *buf, td_u32 len, td_u32 *used, drv_vdec_entry *entry,
    td_s32 chan_id)
{
    drv_vdec_kpi *kpi_info = TD_NULL;

    kpi_info = &entry->kpi_info[chan_id];
    vdec_drv_proc_dump(buf, len, used, VP_KPI_HEAD);
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "create", kpi_info->create_time, "config", kpi_info->config_time);
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "start", kpi_info->start_time, "stop", kpi_info->stop_time);
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "reset", kpi_info->reset_time, "flush", kpi_info->flush_time);
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "reg_event", kpi_info->reg_event_time, "destroy",
        kpi_info->destroy_time);
}

STATIC td_char *vdec_cnvt_decmode_2_str(ext_vdec_decode_mode mode)
{
    td_char *dec_mode[] = {"normal", "ip", "i", "na"};
    td_u8 index;

    index = (mode >= EXT_VDEC_DEC_MODE_IPB && mode <= EXT_VDEC_DEC_MODE_MAX) ? mode : EXT_VDEC_DEC_MODE_MAX;

    return dec_mode[index];
}

STATIC td_char *vdec_cnvt_frame_mode_2_str(ext_vdec_frame_buf_mode mode)
{
    td_char *frame_buf_mode[] = {"internal", "external", "na"};
    td_u8 index;

    index = (mode >= EXT_VDEC_FRM_INTERNAL && mode <= EXT_VDEC_FRM_MAX) ? mode : EXT_VDEC_FRM_MAX;

    return frame_buf_mode[index];
}

STATIC td_char *vdec_cnvt_outorder_2_str(ext_vdec_output_order mode)
{
    td_char *out_order[] = {"display", "decode", "na"};
    td_u8 index;

    index = (mode >= EXT_VDEC_OUT_ORDER_DISPLAY && mode <= EXT_VDEC_OUT_ORDER_MAX) ? mode : EXT_VDEC_OUT_ORDER_MAX;

    return out_order[index];
}

STATIC td_char *vdec_cnvt_frame_layout_2_str(ext_vdec_frame_layout frame_layout)
{
    td_char *layout[] = {"auto", "linear", "na"};
    td_u8 index;

    index = (frame_layout >= EXT_VDEC_FRM_LAYOUT_AUTO && frame_layout <= EXT_VDEC_FRM_LAYOUT_MAX) ?
        frame_layout : EXT_VDEC_FRM_LAYOUT_MAX;

    return layout[index];
}

STATIC td_char *vdec_cnvt_outfmt_2_str(ext_vdec_yuv_format mode)
{
    td_char *out_fmt[] = {"420sp", "420sp_fix_8bit", "420sp_expand_16bit", "420sp_cmp", "420sp_afbc",
        "420sp_tile", "na"};
    td_u8 index;

    index = (mode >= EXT_VDEC_YUV_FMT_420SP && mode <= EXT_VDEC_YUV_FMT_MAX) ? mode : EXT_VDEC_YUV_FMT_MAX;

    return out_fmt[index];
}

STATIC td_void drv_vdec_read_chan_state(td_void *buf, td_u32 len, td_u32 *used, td_s32 chan_id)
{
    drv_vdec_chan *chan = TD_NULL;
    td_char *chan_state[] = {"null", "create", "start", "stop"};
    td_u32 state_num = (td_u32)(sizeof(chan_state) / sizeof(chan_state[0]));

    chan = drv_vdec_get_chan(chan_id);

    vdec_drv_proc_dump(buf, len, used, VP_VDEC_INFO_HEAD);
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "chan_id", chan_id, "vfmw_id", chan->vfmw_id);
    vdec_drv_proc_dump(buf, len, used, VP_S_S, "status", chan->status < state_num ? chan_state[chan->status] : "err",
        "standard", drv_vdec_get_standard(chan->attr.standard));
    vdec_drv_proc_dump(buf, len, used, VP_S_S, "dec_mode", vdec_cnvt_decmode_2_str(chan->attr.decode_mode),
        "out_order", vdec_cnvt_outorder_2_str(chan->attr.output_order));
    vdec_drv_proc_dump(buf, len, used, VP_S_S, "frame_layout", vdec_cnvt_frame_layout_2_str(chan->attr.frame_layout),
        "frame_buf_mode", vdec_cnvt_frame_mode_2_str(chan->attr.frame_buf_mode));
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "low_power_test_mode", chan->low_power_test_mode, "cycle_mode",
        chan->cycle_mode);
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "max_stream_size", chan->attr.max_stream_size, "max_stream_num",
        chan->attr.max_stream_num);
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "error_threshold", chan->attr.error_threshold, "extra_frame_num",
        chan->attr.extra_frame_num);
    vdec_drv_proc_dump(buf, len, used, VP_S_X, "out_format",
        vdec_cnvt_outfmt_2_str(chan->ext_attr.out_format.yuv_fmt), "event_map", (td_u32)chan->event_map);
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "lowdelay", chan->ext_attr.enable_low_delay, "is_max_dec_w_h_en",
        chan->attr.max_dec_w_h.is_max_dec_w_h_en);
    vdec_drv_proc_dump(buf, len, used, VP_U_U, "max_dec_width", chan->attr.max_dec_w_h.max_dec_width,
        "max_dec_height", chan->attr.max_dec_w_h.max_dec_height);
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "try_que_stm", chan->statis.try_que_stm_num, "que_stm_ok",
        chan->statis.que_stm_num);
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "try_deq_stm", chan->statis.try_deq_stm_num, "deq_stm_ok",
        chan->statis.deq_stm_num);
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "try_acq_frm", chan->statis.try_acq_frm_num, "acq_frm_ok",
        chan->statis.acq_frm_num);
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "try_rls_frm", chan->statis.try_rel_frm_num, "rls_frm_ok",
        chan->statis.rel_frm_num);
}

#ifdef VDEC_HEIF_SUPPORT
STATIC td_char *drv_vdec_heif_cnvt_select_2_str(ext_vdec_heif_select select)
{
    td_u8 index;
    td_char *select_type[] = {"primary", "thumbnail", "invalid"};

    index = (select >= EXT_VDEC_HEIF_ORIGINAL && select < EXT_VDEC_HEIF_MAX) ? select : EXT_VDEC_HEIF_MAX;

    return select_type[index];
}

STATIC td_char *drv_vdec_heif_cnvt_orient_2_str(ext_vdec_orientation orient)
{
    td_u8 index;
    td_char *orient_type[] = {"normal", "flip_hor", "rot_180", "flip_ver", "flip_hor_rot_270", "rot_90", "flip_hor_rot_90", "rot_270", "invalid"};

    index = (orient >= EXT_VDEC_ORI_NORMAL && orient < EXT_VDEC_ORI_MAX) ? orient : EXT_VDEC_HEIF_MAX;

    return orient_type[index];
}

td_void drv_vdec_read_heif_info(const drv_vdec_chan *chan, td_void *buf, td_u32 len, td_u32 *used)
{
    const drv_vdec_heif_info *heif_info = &chan->heif_info;
    const drv_vdec_heif_pic *orig_pic = &heif_info->orig_pic;
    const drv_vdec_heif_pic *thmb_pic = &heif_info->thmb_pic;

    vdec_drv_proc_dump(buf, len, used, VP_HEIF_HEAD);

    vdec_drv_proc_dump(buf, len, used, VP_S_S,
        "ori_select", drv_vdec_heif_cnvt_select_2_str(chan->ori_select),
        "act_select", drv_vdec_heif_cnvt_select_2_str(chan->act_select));
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "orig_stream_size", orig_pic->total_stream_len, "thmb_stream_size", thmb_pic->total_stream_len);
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "orig_width", orig_pic->width, "orig_height", orig_pic->height);
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "thmb_pic_width", thmb_pic->width, "thmb_pic_height", thmb_pic->height);
    vdec_drv_proc_dump(buf, len, used, VP_S_N, "orientation", drv_vdec_heif_cnvt_orient_2_str(heif_info->format.orientation), "");
}
#endif

td_s32 drv_vdec_read_proc(td_void *buf, td_u32 len, td_u32 *used, td_s32 chan_id)
{
    drv_vdec_entry *entry = drv_vdec_get_entry();
    drv_vdec_chan *chan = TD_NULL;

    vdec_check_ret((chan_id >= 0) && (chan_id < VDEC_MAX_CHAN_NUM) && (buf != TD_NULL));
    vdec_drv_proc_dump(buf, len, used, VP_DRV_HEAD);
    vdec_drv_proc_dump(buf, len, used, VP_S_D, "version", VDEC_VERSION, "chan_num", entry->chan_num);

    if (entry->chan_ptr[chan_id] == TD_NULL) {
        return EXT_SUCCESS;
    }

    drv_vdec_lock_chan(chan_id);

    chan = drv_vdec_get_chan(chan_id);
    if (chan->status != VDEC_CHAN_STATE_NULL) {
        drv_vdec_read_chan_state(buf, len, used, chan_id);
#ifdef VDEC_HEIF_SUPPORT
        if (chan->attr.standard == EXT_VDEC_STD_HEIF) {
            drv_vdec_read_heif_info(chan, buf, len, used);
        }
#endif
    }

    if ((td_u32)chan->vfmw_id != VDEC_INVALID_HANDLE) {
        vdec_vfmw_read_proc(buf, len, used, chan->vfmw_id);
    }

    drv_vdec_proc_kpi_info_print(buf, len, used, entry, chan_id);

    drv_vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;
}
#endif

td_s32 drv_vdec_vfmw_mod_init(td_void)
{
    td_s32 ret;

    drv_vdec_lock_entry();

    ret = vdec_vfmw_mod_init();

    drv_vdec_unlock_entry();

    return ret;
}

td_void drv_vdec_vfmw_mod_exit(td_void)
{
    drv_vdec_lock_entry();

    vdec_vfmw_mod_exit();

    drv_vdec_unlock_entry();

    return;
}
