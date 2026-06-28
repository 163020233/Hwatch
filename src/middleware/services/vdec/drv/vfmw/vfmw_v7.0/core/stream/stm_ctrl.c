/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "stm_ctrl.h"
#include "stm_base.h"
#include "stm_dev.h"
#include "stm_log.h"
#include "stm_intf.h"
#include "dbg.h"
#include "stm_log.h"
#include "stm_raw.h"
#include "stm_seg.h"
#include "vfmw_define.h"
#include "vdec_chr.h"

#define STM_CHECK 0x1234abcd

/* variable define */
static stm_ops g_stm_ops_info = {
    .open = stm_ctrl_open,
    .close = stm_ctrl_close,
    .suspend = stm_ctrl_suspend,
    .resume = stm_ctrl_resume,
    .wakeup = stm_ctrl_wake_up,
    .receive_seg = stm_ctrl_receive_seg,
    .release_seg = stm_ctrl_release_seg,
    .set_ref_cnt = stm_ctrl_set_ref_cnt,
    .control = stm_ctrl_control,
    .get_mem_need_size = stm_ctrl_get_mem_need_size,
};

static stm_entry g_stm_entry_info = {
    .reserve1 = STM_CHECK,
    .ops = &g_stm_ops_info,
    .ctx_size = sizeof(stm_entry),
    .bypass_chan = -1,
    .reserve2 = STM_CHECK,
};

typedef td_s32 (*STM_CID_HANDLER)(td_s32, td_void *args);

typedef struct {
    stm_cmd cmd_id;
    STM_CID_HANDLER handler;
} stm_cid_node;

/* inner function define */
stm_entry *stm_ctrl_get_entry(td_void)
{
    return &g_stm_entry_info;
}

#ifdef VT_SUPPORT
td_s32 stm_ctrl_check_error(td_void)
{
    stm_entry *entry;

    entry = stm_ctrl_get_entry();

    return ((entry->reserve1 == STM_CHECK) &&
            (entry->reserve2 == STM_CHECK));
}
#endif

stm_thread *stm_ctrl_get_thread(td_void)
{
    return stm_ctrl_get_entry()->thread;
}

stm_dev *stm_ctrl_get_dev(td_s32 dev_id)
{
    return &(stm_ctrl_get_entry()->dev[dev_id]);
}

td_void stm_ctrl_lock(td_ulong *lock_flag)
{
    vfmw_check(lock_flag != TD_NULL);

    os_spin_lock(stm_ctrl_get_entry()->lock, lock_flag);
}

td_void stm_ctrl_unlock(td_ulong *lock_flag)
{
    vfmw_check(lock_flag != TD_NULL);

    os_spin_unlock(stm_ctrl_get_entry()->lock, lock_flag);
}

td_void stm_ctrl_sema_up(td_void)
{
    os_sema_up(stm_ctrl_get_entry()->sema);
}

STATIC td_s32 stm_ctrl_sema_try(td_void)
{
    vfmw_check_ret(os_sema_try != TD_NULL, OSAL_ERR);
    return os_sema_try(stm_ctrl_get_entry()->sema);
}

td_void stm_ctrl_sema_down(td_void)
{
    os_sema_down(stm_ctrl_get_entry()->sema);
}

STATIC td_void stm_ctrl_adjust_wait_time(td_void)
{
    if (stm_chan_need_lowldy() == STM_OK) {
        stm_thread_set_wait(1);
    } else {
        stm_thread_set_wait(0);
    }
}

STATIC td_s32 stm_ctrl_thread_start(stm_thread *thread, td_u32 thread_unid)
{
    td_s32 ret;

    stm_thread_lock();

    thread->pos = THD_START_POS1;
    thread->state = STM_THREAD_START;

    thread->pos = THD_START_POS2;
    thread->run_cnt++;

    if (thread_unid != thread->unid) {
        dprint(PRN_ERROR, "stm thread unid changed!\n");
        stm_thread_unlock();
        return STM_ERR;
    }

    if (thread->cmd == STM_THREAD_STOP) {
        stm_thread_unlock();
        return STM_STOP;
    } else if (thread->cmd == STM_THREAD_DESTROY) {
        stm_dev_try_close_device();
        stm_thread_unlock();
        return STM_DESTORY;
    }
    stm_thread_unlock();

    ret = stm_ctrl_sema_try();
    if (ret == OSAL_OK) {
        thread->pos = THD_START_POS3;
        ret = stm_chan_run_proc();
        if (ret == STM_OK) {
            thread->pos = THD_START_POS4;
            (td_void)stm_dev_run_proc();
        }
        stm_ctrl_sema_up();
    }

    thread->pos = THD_START_POS5;
    stm_ctrl_adjust_wait_time();

    /* Thread run before stm_thread_create finish */
    if (stm_thread_wait() == STM_ERR) {
        dprint(PRN_ERROR, "stm thread wait err!\n");
        os_msleep(1);
    }

    stm_thread_sleep();

    return STM_OK;
}

STATIC td_s32 stm_ctrl_thread_stop(stm_thread *thread, td_u32 thread_unid)
{
    const td_u32 stop_time = 10; /* 10 is sleep time */

    thread->pos = THD_STOP_POS1;
    thread->state = STM_THREAD_STOP;

    thread->pos = THD_STOP_POS2;
    thread->run_cnt++;

    if (thread_unid != thread->unid) {
        dprint(PRN_ERROR, "stm thread unid changed!\n");
        return STM_ERR;
    }

    if (thread->cmd == STM_THREAD_START) {
        return STM_START;
    } else if (thread->cmd == STM_THREAD_DESTROY) {
        return STM_DESTORY;
    } else {
        os_msleep(stop_time);
    }

    return STM_OK;
}

STATIC td_s32 stm_ctrl_thread_destroy(stm_thread *thread, td_u32 thread_unid)
{
    thread->pos = THD_DSTY_POS1;

    stm_thread_lock();
    if (thread_unid == thread->unid) {
        stm_thread_clean();
    } else {
        stm_thread_unlock();
        dprint(PRN_ERROR, "stm thread unid changed!\n");
        return STM_ERR;
    }
    stm_thread_unlock();

    return STM_OK;
}

STATIC td_s32 stm_ctrl_thread_func(td_void *data)
{
    td_s32 ret;
    stm_thread *thread = TD_NULL;
    td_u32 thread_unid;
    td_u32 curr_time, end_time, cost_time;

    vfmw_unused(data);
    vfmw_check_ret(os_get_time_us != TD_NULL, STM_ERR);

    thread = stm_ctrl_get_thread();
    thread_unid = thread->unid;

start :
    ret = STM_OK;
    while (ret == STM_OK) {
        curr_time = os_get_time_us();
        ret = stm_ctrl_thread_start(thread, thread_unid);
        end_time = os_get_time_us();
        cost_time = (end_time > curr_time) ? (end_time - curr_time) : 0;
        thread->max_time = (thread->max_time < cost_time) ? cost_time : thread->max_time;
    }
    if (ret == STM_STOP) {
        goto stop;
    } else if (ret == STM_DESTORY) {
        goto destroy;
    }

    return STM_ERR;
stop :
    ret = STM_OK;
    while (ret == STM_OK) {
        ret = stm_ctrl_thread_stop(thread, thread_unid);
    }

    if (ret == STM_START) {
        goto start;
    } else if (ret == STM_DESTORY) {
        goto destroy;
    }

    return STM_ERR;
destroy :
    ret = stm_ctrl_thread_destroy(thread, thread_unid);

    return ret;
}

STATIC td_s32 stm_ctrl_destroy_chan(td_s32 chan_id, td_void *args)
{
    td_s32 chan_is_sec;
    td_s32 ret;
    stm_chan_type_num num = { 0 };
    stm_chan *chan = TD_NULL;

    vfmw_unused(args);
    chan = STM_CTRL_GET_CHAN(chan_id);
    vfmw_check_ret_print(chan != TD_NULL, STM_ERR, "%s chan %d is null\n", __func__, chan_id);

    chan_is_sec = chan->is_sec;

    ret = stm_chan_destroy(chan_id);
    vfmw_check_ret_print(ret == STM_OK, STM_ERR, "%s destroy chan %d error\n", __func__, chan_id);

    stm_chan_get_num(&num);

    if (STM_DEV_NUM > 1) {
        if (((num.no_sec > 0) && (chan_is_sec == 0)) ||
            ((num.sec > 0) && (chan_is_sec == 1))) {
            return STM_OK;
        }
    }

    stm_dev_try_close_device();

    if ((num.no_sec == 0) && (num.sec == 0)) {
        ret = stm_thread_destroy();
        if (ret != STM_OK) {
            dprint(PRN_ERROR, "%s destroy thread error\n", __func__);
            return STM_ERR;
        }
    }

    return ret;
}

STATIC td_s32 stm_ctrl_create_chan(td_s32 chan_id,  td_void *args)
{
    td_s32 ret;
    stm_chan *chan = TD_NULL;
    stm_chan_type_num num = { 0 };
    stm_create_info *param = (stm_create_info *)args;

    vfmw_check_ret(args != TD_NULL, STM_ERR);

    chan = STM_CTRL_GET_CHAN(chan_id);
    if (chan == TD_NULL || chan->handle != TD_NULL) {
        dprint(PRN_FATAL, "%s chan check error\n", __func__);
        return STM_ERR;
    }

    ret = stm_chan_create(chan_id, args);
    if (ret != STM_OK) {
        dprint(PRN_FATAL, "%s create chan error\n", __func__);
        return STM_ERR;
    }

    stm_chan_get_num(&num);

    if (((num.no_sec) > 1 && (param->is_sec == 0)) ||
        ((num.sec) > 1 && (param->is_sec == 1))) {
        return STM_OK;
    }

    if (((num.no_sec == 1) && (num.sec == 0)) || ((num.no_sec == 0) && (num.sec == 1))) {
        ret = stm_thread_create(stm_ctrl_thread_func);
        if (ret != STM_OK) {
            dprint(PRN_FATAL, "%s create thread error\n", __func__);
            goto DESTROY;
        }
        vdec_record_chr_event(chan_id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_CREATE_STM_THREAD_OK);
    }

    return ret;

DESTROY:

    ret = stm_ctrl_destroy_chan(chan_id, TD_NULL);
    if (ret != STM_OK) {
        dprint(PRN_FATAL, "%s destroy chan %d error\n", __func__, chan_id);
    }

    return STM_ERR;
}

STATIC td_s32 stm_ctrl_cfg_chan(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    vfmw_check_ret(args != TD_NULL, STM_ERR);

    ret = stm_chan_config(chan_id, args);

    return ret;
}

STATIC td_s32 stm_ctrl_cfg_ext_intf(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    vfmw_check_ret(args != TD_NULL, STM_ERR);

    ret = stm_chan_cfg_ext_intf(chan_id, args);

    return ret;
}

STATIC td_s32 stm_ctrl_start_chan(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    vfmw_unused(args);
#ifdef VDEC_PROC_SUPPORT
    if (stm_log_need_save_seek() == TD_TRUE) {
        stm_log_start_save_raw(chan_id);
    }
#endif
    ret = stm_chan_start(chan_id);
    if (ret != STM_OK) {
        dprint(PRN_FATAL, "stm start chan(%d) error!\n", chan_id);
        return STM_ERR;
    }

    ret = stm_thread_wakeup();
    if (ret != STM_OK) {
        dprint(PRN_FATAL, "stm thread wakeup error!\n");
        return STM_ERR;
    }

    ret = stm_thread_start();
    if (ret != STM_OK) {
        dprint(PRN_FATAL, "stm thread start error!\n");
        return STM_ERR;
    }
    vdec_record_chr_event(chan_id, MT_VDEC_START, MT_VDEC_VFMW_START, MT_VFMW_STM_START_OK);

    return ret;
}

STATIC td_s32 stm_ctrl_reset_chan(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    vfmw_unused(args);
    ret = stm_chan_reset(chan_id);
    vdec_record_chr_event(chan_id, MT_VDEC_RESET, MT_VDEC_VFMW_RESET, MT_VFMW_STM_RESET_OK);

    return ret;
}

STATIC td_s32 stm_ctrl_stop_chan(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    vfmw_unused(args);
    ret = stm_chan_stop(chan_id);

#ifdef VDEC_PROC_SUPPORT
    if (stm_log_need_save_seek() == TD_TRUE) {
        stm_log_stop_save_raw(chan_id);
    }
#endif
    vdec_record_chr_event(chan_id, MT_VDEC_STOP, MT_VDEC_VFMW_STOP, MT_VFMW_STM_STOP_OK);

    return ret;
}

STATIC td_s32 stm_ctrl_get_seg_fresh_num(td_s32 chan_id, td_void *args)
{
    vfmw_unused(args);
    return stm_seg_get_fresh_num(chan_id);
}

STATIC td_s32 stm_ctrl_get_info(td_s32 chan_id, td_void *args)
{
    stm_info *info = (stm_info *)args;
    raw_list_info *raw_list = TD_NULL;
    seg_list *seg_list_info = TD_NULL;
    stm_raw_info *raw_packet = TD_NULL;
    stm_chan *chan = TD_NULL;

    vfmw_check_ret(args != TD_NULL, STM_ERR);

    chan = STM_CTRL_GET_CHAN(chan_id);
    vfmw_check_ret(chan != TD_NULL, STM_ERR);

    raw_packet = &(chan->raw_list->raw_tmp);
    vfmw_check_ret(raw_packet != TD_NULL, STM_ERR);

    raw_list = STM_CTRL_GET_CHAN(chan_id)->raw_list;
    vfmw_check_ret(raw_list != TD_NULL, STM_ERR);

    seg_list_info = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
    vfmw_check_ret(seg_list_info != TD_NULL, STM_ERR);

    info->fresh_seg_num = stm_seg_get_fresh_num(chan_id);
    info->fresh_seg_size = stm_seg_get_fresh_size(chan_id);
    info->bit_rate = stm_raw_get_bit_rate(chan_id);
    info->raw_new_flag = (raw_list->fresh_num != 0) ? TD_TRUE : TD_FALSE;
    info->stm_seg_full = stm_seg_is_full(seg_list_info);
    info->raw_recv_try = raw_list->recv_try_cnt;
    info->raw_recv_ok = raw_list->recv_ok_cnt;
    info->raw_rels_try = raw_list->rels_try_cnt;
    info->raw_rels_ok = raw_list->rels_ok_cnt;
    info->raw_fresh_num = (td_u32)stm_raw_get_fresh_num(chan_id);
    info->seg_recv_try = seg_list_info->recv_try_cnt;
    info->seg_recv_ok = seg_list_info->recv_ok_cnt;
    info->seg_fresh_num = (td_u32)info->fresh_seg_num;
    if (chan->eos == STM_EOS_DONE) {
        info->eos_flag = TD_TRUE;
    } else {
        info->eos_flag = TD_FALSE;
    }

    return STM_OK;
}

STATIC td_s32 stm_ctrl_set_es_invalid(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    vfmw_unused(args);
    ret = stm_chan_set_es_invalid(chan_id);

    return ret;
}

STATIC td_s32 stm_ctrl_flush_chan(td_s32 chan_id, td_void *args)
{
    td_s32 ret;

    vfmw_unused(args);
    ret = stm_raw_deleta_all(chan_id);

    return ret;
}

STATIC td_s32 stm_ctrl_entry_init(td_void)
{
    td_s32 id;
    stm_entry *entry = TD_NULL;

    vfmw_check_ret(os_event_init != TD_NULL, STM_ERR);
    entry = stm_ctrl_get_entry();

    vfmw_check_sec_func(memset_s(entry->dev, sizeof(entry->dev), 0, sizeof(entry->dev)));
    vfmw_check_sec_func(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));
    vfmw_check_sec_func(memset_s(entry->thread, sizeof(entry->thread), 0, sizeof(entry->thread)));

    os_event_init(&entry->event, 0);
    os_sema_init(&entry->sema);
    os_sema_init(&entry->raw_sema);
    os_sema_init(&entry->seg_sema);
    os_spin_lock_init(&entry->lock);
    stm_thread_init();
    stm_dev_init();

    for (id = 0; id < STM_CHAN_NUM; id++) {
        os_spin_lock_init(&entry->chan[id].lock);
    }

    entry->sleep_time = 5; /* 5 num */
    entry->wait_time = 0;
    entry->state = STM_STATE_RUN;

    return STM_OK;
}

STATIC td_s32 stm_ctrl_entry_exit(td_void)
{
    td_s32 id;
    stm_entry *entry = TD_NULL;

    entry = stm_ctrl_get_entry();

    entry->state = STM_STATE_NULL;

    os_event_exit(entry->event);
    os_sema_exit(entry->sema);
    os_sema_exit(entry->raw_sema);
    os_sema_exit(entry->seg_sema);
    os_spin_lock_exit(entry->lock);
    stm_thread_exit();
    stm_dev_exit();

    for (id = 0; id < STM_CHAN_NUM; id++) {
        os_spin_lock_exit(entry->chan[id].lock);
    }

    vfmw_check_sec_func(memset_s(entry->dev, sizeof(entry->dev), 0, sizeof(entry->dev)));
    vfmw_check_sec_func(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));
    vfmw_check_sec_func(memset_s(entry->thread, sizeof(entry->thread), 0, sizeof(entry->thread)));

    entry->sleep_time = 0;
    entry->wait_time = 0;
    return STM_OK;
}

/* export function define */
stm_ops *stm_ctrl_get_ops(td_void)
{
    return &g_stm_ops_info;
}

td_s32 stm_ctrl_open(td_void)
{
    td_s32 ret;

    ret = stm_ctrl_entry_init();

    return ret;
}

td_s32 stm_ctrl_close(td_void)
{
    td_s32 ret;

    ret = stm_ctrl_entry_exit();

    return ret;
}

td_s32 stm_ctrl_suspend(td_void)
{
    td_s32 ret;
    stm_thread *thread = TD_NULL;

    stm_ctrl_sema_down();

    thread = stm_ctrl_get_thread();
    if (thread == TD_NULL || thread->handle == TD_NULL) {
        ret = STM_OK;
        goto OUT;
    }

    ret = stm_thread_stop();
    if (ret != STM_OK) {
        ret = STM_ERR;
        goto OUT;
    }

    ret = stm_dev_suspend();
    if (ret != STM_OK) {
        ret = STM_ERR;
        goto OUT;
    }

OUT:

    stm_ctrl_sema_up();
    vdec_record_chr_event(-1, MT_VDEC_SUSPEND, MT_VDEC_VFMW_SUSPEND, MT_VDEC_VFMW_STM_SUSPEND_OK);

    return ret;
}

td_s32 stm_ctrl_resume(td_void)
{
    td_s32 ret;
    stm_thread *thread = TD_NULL;

    stm_ctrl_sema_down();

    thread = stm_ctrl_get_thread();
    if (thread == TD_NULL || thread->handle == TD_NULL) {
        ret = STM_OK;
        goto OUT;
    }

    ret = stm_dev_resume();
    if (ret != STM_OK) {
        ret = STM_ERR;
        goto OUT;
    }

    ret = stm_thread_start();
    if (ret != STM_OK) {
        ret = STM_ERR;
        goto OUT;
    }

OUT:

    stm_ctrl_sema_up();
    vdec_record_chr_event(-1, MT_VDEC_RESUME, MT_VDEC_VFMW_RESUME, MT_VDEC_VFMW_STM_RESUME_OK);

    return ret;
}

td_s32 stm_ctrl_wake_up(td_void)
{
    td_s32 ret = STM_OK;
    td_s32 run_cnt;
    stm_thread *thread = TD_NULL;
    STATIC td_s32 pre_run_cnt = 0;

    thread = stm_ctrl_get_thread();
    if (thread->handle == TD_NULL) {
        return STM_ERR;
    }

    run_cnt = (td_s32)thread->run_cnt;

    if (pre_run_cnt != run_cnt &&
        thread->pos == THD_START_POS5) {
        pre_run_cnt = run_cnt;
        ret = stm_thread_wakeup();
    }

    return ret;
}

td_s32 stm_ctrl_receive_seg(td_s32 chan_id, stm_seg_info *seg)
{
    td_s32 ret;
    stm_chan *chan = TD_NULL;

    STM_ASSERT_CHAN_ID(chan_id, STM_ERR);
    vfmw_check_ret(seg != TD_NULL, STM_ERR);

    stm_ctrl_sema_down();

    chan = STM_CTRL_GET_CHAN(chan_id);
    if (chan->handle == TD_NULL) {
        ret = STM_ERR;
        goto OUT;
    }

    ret = stm_dev_raw_to_seg(chan_id, seg);
    if (ret != STM_OK) {
        ret = STM_ERR;
        goto OUT;
    }

    ret = stm_seg_receive(chan_id, seg);

OUT:

    stm_ctrl_sema_up();

    return ret;
}

td_s32 stm_ctrl_release_seg(td_s32 chan_id, td_s32 seg_id)
{
    td_s32 ret;
    stm_chan *chan = TD_NULL;

    STM_ASSERT_CHAN_ID(chan_id, STM_ERR);

    chan = STM_CTRL_GET_CHAN(chan_id);
    if (chan->handle == TD_NULL) {
        ret = STM_ERR;
        goto OUT;
    }

    ret = stm_seg_release(chan_id, seg_id);

OUT:

    return ret;
}

td_s32 stm_ctrl_control(td_s32 chan_id, stm_cmd cmd, td_void *arg)
{
    STM_CID_HANDLER handler = TD_NULL;
    td_u32 idx = 0;
    td_s32 ret = STM_ERR;
    stm_chan *chan = TD_NULL;

    STATIC const stm_cid_node stm_handler_table[] = {
        { STM_CMD_CREATE_CHAN,    stm_ctrl_create_chan },
        { STM_CMD_DESTROY_CHAN,   stm_ctrl_destroy_chan },
        { STM_CMD_START_CHAN,     stm_ctrl_start_chan },
        { STM_CMD_STOP_CHAN,      stm_ctrl_stop_chan },
        { STM_CMD_RESET_CHAN,     stm_ctrl_reset_chan },
        { STM_CMD_CONFIG_CHAN,    stm_ctrl_cfg_chan },
        { STM_CMD_CFG_EXT_INTF,   stm_ctrl_cfg_ext_intf },
        { STM_CMD_FLUSH_CHAN,     stm_ctrl_flush_chan },
        { STM_CMD_GET_SEG_NUM,    stm_ctrl_get_seg_fresh_num },
        { STM_CMD_GET_INFO,       stm_ctrl_get_info },
        { STM_CMD_SET_ES_INVALID, stm_ctrl_set_es_invalid },
        { STM_CMD_MAX,            TD_NULL }
    };

    STM_ASSERT_CHAN_ID(chan_id, STM_ERR);

    stm_ctrl_sema_down();

    if (cmd != STM_CMD_CREATE_CHAN) {
        chan = STM_CTRL_GET_CHAN(chan_id);
        if (chan->handle == TD_NULL) {
            dprint(PRN_FATAL, "%s chan is null \n", __func__);
            ret = STM_ERR;
            goto OUT;
        }
    }

    while (idx < (sizeof(stm_handler_table) / sizeof(stm_cid_node))) {
        if (stm_handler_table[idx].cmd_id == STM_CMD_MAX) {
            break;
        }

        if (stm_handler_table[idx].cmd_id == cmd) {
            handler = stm_handler_table[idx].handler;
            break;
        }

        idx++;
    }

    if (handler != TD_NULL) {
        ret = handler(chan_id, arg);
    } else {
        dprint(PRN_FATAL, "%s unsupport cid %d\n", __func__, cmd);
        ret = STM_ERR;
    }

OUT:
    stm_ctrl_sema_up();

    return ret;
}

td_s32 stm_ctrl_delete_raw(td_s32 chan_id)
{
    raw_list_info *raw_list = TD_NULL;
    stm_chan *chan = TD_NULL;

    chan = STM_CTRL_GET_CHAN(chan_id);
    raw_list = chan->raw_list;

    raw_list->delete_num++;

    return STM_OK;
}

td_s32 stm_ctrl_get_mem_need_size(vfmw_svr_mem_need *mem)
{
    vfmw_check_ret(mem != TD_NULL, STM_ERR);

    mem->chan_cache_size += vfmw_align_up((td_u32)sizeof(raw_list_info), VFMW_CTX_ALIGN) +
                            vfmw_align_up((td_u32)sizeof(seg_list), VFMW_CTX_ALIGN);
    mem->chan_no_cache_size += vfmw_align_up(STM_EOS_SIZE, VFMW_DEV_ALIGN);

    vfmw_check_ret((stm_dev_hal_size(0)) < VFMW_MAX_UINT32, STM_ERR);
    mem->hal_size += vfmw_align_up(stm_dev_hal_size(0), VFMW_DEV_ALIGN);

    return STM_OK;
}

td_s32 stm_ctrl_process(td_void)
{
    if (stm_ctrl_sema_try() == OSAL_OK) {
        (td_void)stm_chan_run_proc();

        (td_void)stm_dev_run_proc();

        stm_ctrl_sema_up();
    }

    return STM_OK;
}

td_bool stm_ctrl_seg_has_fresh(td_s32 chan_id)
{
    seg_list *seg_list_info = TD_NULL;

    seg_list_info = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;

    return stm_seg_has_fresh(seg_list_info);
}

td_s32 stm_ctrl_set_ref_cnt(td_s32 chan_id, td_s32 seg_id, td_s32 ref_cnt)
{
    td_s32 ret;
    stm_chan *chan = TD_NULL;

    STM_ASSERT_CHAN_ID(chan_id, STM_ERR);

    chan = STM_CTRL_GET_CHAN(chan_id);
    if (chan->handle == TD_NULL) {
        ret = STM_ERR;
        goto OUT;
    }

    ret = stm_seg_set_ref_cnt(chan_id, seg_id, ref_cnt);

OUT:

    return ret;
}
