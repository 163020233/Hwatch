/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "dec_ctrl.h"
#include "dec_base.h"
#include "dec_chn.h"
#include "dec_dev.h"
#include "dbg.h"
#include "vdec_chr.h"

static dec_entry g_dec_entry = {
    .dec_check_0 = DEC_CHECK,
    .dec_check_1 = DEC_CHECK,
};

dec_entry *dec_ctrl_get_entry(td_void)
{
    return &g_dec_entry;
}

dec_thread_pool *dec_ctrl_get_thread_pool(td_void)
{
    return &(g_dec_entry.thread_pool);
}

VFMW_ACC_BASE dec_chan_pool *dec_ctrl_get_chan_pool(td_void)
{
    return &(g_dec_entry.chan_pool);
}

VFMW_ACC_BASE dec_dev_pool *dec_ctrl_get_dev_pool(td_void)
{
    return &(g_dec_entry.dev_pool);
}

dec_thread *dec_ctrl_get_thread(td_u32 thread_id)
{
    dec_thread *thread = TD_NULL;

    dec_thread_get_thread(thread_id, &thread);

    return thread;
}

dec_chan *dec_ctrl_get_chan(td_s32 chan_id)
{
    return dec_chan_get_chan(chan_id);
}

dec_dev_info *dec_ctrl_get_dev(td_u32 dev_id)
{
    dec_dev_info *dev = TD_NULL;

    if (dec_dev_get_dev((td_u16)dev_id, &dev) != DEC_OK) {
        dprint(PRN_ERROR, "%s return error!\n", __func__);
    }
    return dev;
}

STATIC td_void dec_ctrl_get_chan_num(dec_chan_type_num *num)
{
    dec_chan_get_chan_num(num);

    return;
}

td_s32 dec_ctrl_entry_init(td_void)
{
    dec_entry *entry = TD_NULL;

    entry = dec_ctrl_get_entry();

    os_sema_init(&entry->sema);

    dec_thread_init();
    dec_chan_init();
    dec_dev_init();

    entry->sleep_time = 5; /* 5 num */
    entry->wait_time = 0;
    entry->is_over_load = 0;
    entry->check_interval = 1000; /* 1000 ms */
    entry->bypass_chan = -1;
    return DEC_OK;
}

td_s32 dec_ctrl_entry_exit(td_void)
{
    dec_entry *entry = TD_NULL;

    entry = dec_ctrl_get_entry();

    dec_thread_deinit();
    dec_chan_deinit();
    dec_dev_deinit();

    entry->sleep_time = 0;
    entry->wait_time = 0;
    os_sema_exit(entry->sema);

    return DEC_OK;
}

td_s32 dec_ctrl_open(td_void)
{
    td_s32 ret;

    ret = dec_ctrl_entry_init();
    if (ret != DEC_OK) {
        goto out;
    }

    return DEC_OK;

out:

    ret = dec_ctrl_close();

    return ret;
}

td_s32 dec_ctrl_close(td_void)
{
    td_s32 ret;

    ret = dec_ctrl_entry_exit();
    if (ret != DEC_OK) {
        dprint(PRN_ERROR, "%s err\r\n", __func__);
    }

    return ret;
}

td_s32 dec_ctrl_suspend(td_void)
{
    dec_entry *entry = TD_NULL;
    td_s32 ret;

    entry = dec_ctrl_get_entry();
    os_sema_down(entry->sema);

    ret = dec_thread_stop_all();
    if (ret != DEC_OK) {
        dprint(PRN_ERROR, "dec_thread_stop_all err\r\n");
        goto out;
    }

    ret = dec_dev_suspend();
    if (ret != DEC_OK) {
        dprint(PRN_ERROR, "dec_dev_suspend err\r\n");
        goto out;
    }

    os_sema_up(entry->sema);
    vdec_record_chr_event(-1, MT_VDEC_SUSPEND, MT_VDEC_VFMW_SUSPEND, MT_VDEC_VFMW_DEC_SUSPEND_OK);

    return ret;
out:
    os_sema_up(entry->sema);
    return DEC_ERR;
}

td_s32 dec_ctrl_resume(td_void)
{
    dec_entry *entry = TD_NULL;
    td_s32 ret;

    entry = dec_ctrl_get_entry();
    os_sema_down(entry->sema);

    ret = dec_thread_start_all();
    if (ret != DEC_OK) {
        dprint(PRN_ERROR, "dec_thread_start_all err\r\n");
        goto out;
    }

    ret = dec_dev_resume();
    if (ret != DEC_OK) {
        dprint(PRN_ERROR, "dec_dev_resume err\r\n");
        goto out;
    }

    os_sema_up(entry->sema);
    vdec_record_chr_event(-1, MT_VDEC_RESUME, MT_VDEC_VFMW_RESUME, MT_VDEC_VFMW_DEC_RESUME_OK);

    return ret;
out:
    os_sema_up(entry->sema);
    return DEC_ERR;
}

VFMW_ACC_BASE td_s32 dec_ctrl_wakeup(td_s32 chan_id)
{
    dec_entry *entry = TD_NULL;
    td_s32 ret;

    vfmw_unused(chan_id);
    entry = dec_ctrl_get_entry();
    os_sema_down(entry->sema);

    ret = dec_thread_wakeup();
    os_sema_up(entry->sema);

    return ret;
}

td_s32 dec_ctrl_create_chan(td_s32 chan_id, const td_void *args)
{
    td_s32 ret;
    dec_chan_type_num num = { 0 };
    dec_create_info *param = (dec_create_info *)args;

    (td_void)param; /* avoid to build err when printing is not supported. */
    vfmw_check_ret(args != TD_NULL, DEC_ERR);

    dec_ctrl_get_chan_num(&num);
    if (num.no_sec == 0 && num.sec == 0) {
        ret = dec_thread_create(dec_thread_routine); /* block */
        if (ret != DEC_OK) {
            goto exit;
        }
        vdec_record_chr_event(chan_id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_CREATE_DEC_THREAD_OK);
    }

    ret = dec_chan_create(chan_id, args);
    if (ret != DEC_OK) {
        goto exit;
    }
    vdec_record_chr_event(chan_id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_CREATE_DEC_OK);

    return DEC_OK;

exit:

    if (num.no_sec == 0 && num.sec == 0) { /* the last channel */
        dec_thread_destroy(0);             /* block */
    }

    return DEC_ERR;
}

td_s32 dec_ctrl_config_chan(td_s32 chan_id, const td_void *args)
{
    td_s32 ret;

    ret = dec_chan_config(chan_id, args);

    return ret;
}

td_s32 dec_ctrl_start_chan(td_s32 chan_id, const td_void *args)
{
    td_s32 ret;

    vfmw_unused(args);
    ret = dec_chan_start(chan_id);
    vdec_record_chr_event(chan_id, MT_VDEC_START, MT_VDEC_VFMW_START, MT_VFMW_DEC_START_OK);

    return ret;
}

td_s32 dec_ctrl_stop_chan(td_s32 chan_id, const td_void *args)
{
    td_s32 ret;

    vfmw_unused(args);
    ret = dec_chan_stop(chan_id);
    vdec_record_chr_event(chan_id, MT_VDEC_STOP, MT_VDEC_VFMW_STOP, MT_VFMW_DEC_STOP_OK);

    return ret;
}

td_s32 dec_ctrl_reset_chan(td_s32 chan_id, const td_void *args)
{
    td_s32 ret;

    vfmw_unused(args);
    ret = dec_chan_reset(chan_id);
    vdec_record_chr_event(chan_id, MT_VDEC_RESET, MT_VDEC_VFMW_RESET, MT_VFMW_DEC_RESET_OK);

    return ret;
}

td_s32 dec_ctrl_destroy_chan(td_s32 chan_id, const td_void *args)
{
    td_s32 ret;
    dec_chan_type_num num = { 0 };
    dec_chan *chan = TD_NULL;

    vfmw_unused(args);
    chan = dec_chan_get_chan(chan_id);
    if (chan == TD_NULL) {
        return DEC_ERR;
    }

    ret = dec_chan_destroy(chan_id);
    if (ret != DEC_OK) {
    }

    dec_chan_try_refresh_bin();
    dec_chan_try_close_dev();

    dec_ctrl_get_chan_num(&num);
    if (num.no_sec == 0 && num.sec == 0) {
        ret = dec_thread_destroy(0); /* block */
        if (ret != DEC_OK) {
        }
    }

    return ret;
}

STATIC td_s32 dec_ctrl_get_info(td_s32 chan_id, const td_void *args)
{
    return dec_chan_get_info(chan_id, args);
}

td_s32 dec_ctrl_get_state(td_void)
{
    return DEC_ERR;
}

td_s32 dec_ctrl_control(td_s32 chan_id, td_u32 cid, td_void *args)
{
    dec_entry *entry = TD_NULL;
    dec_cmd_handler handler = TD_NULL;
    td_s32 idx = 0;
    td_s32 ret;
    td_s32 table_size;

    STATIC const dec_cid_node s_dec_handler_table[] = {
        { DEC_CID_CREATE_CHAN,  dec_ctrl_create_chan },
        { DEC_CID_START_CHAN,   dec_ctrl_start_chan },
        { DEC_CID_CONFIG_CHAN,  dec_ctrl_config_chan },
        { DEC_CID_STOP_CHAN,    dec_ctrl_stop_chan },
        { DEC_CID_RESET_CHAN,   dec_ctrl_reset_chan },
        { DEC_CID_DESTROY_CHAN, dec_ctrl_destroy_chan },
        { DEC_CID_GET_INFO,     dec_ctrl_get_info },
        { DEC_CID_MAX,          TD_NULL }
    };

    entry = dec_ctrl_get_entry();
    table_size = (td_s32)(sizeof(s_dec_handler_table) / sizeof(s_dec_handler_table[0]));

    os_sema_down(entry->sema);

    while (1) {
        if (idx < table_size &&
            s_dec_handler_table[idx].cmd_id == DEC_CID_MAX) {
            break;
        }

        if (idx < table_size &&
            s_dec_handler_table[idx].cmd_id == cid) {
            handler = s_dec_handler_table[idx].handler;
            break;
        }

        idx++;
    }

    if (handler != TD_NULL) {
        ret = handler(chan_id, args);
    } else {
        dprint(PRN_ERROR, "Unsupport CID %d\n", cid);
        ret = DEC_ERR;
    }

    os_sema_up(entry->sema);

    return ret;
}

td_s32 dec_ctrl_get_mem_need_size(vfmw_svr_mem_need *mem)
{
    dec_dev_get_mem_need_size(mem);

    return DEC_OK;
}

td_s32 dec_ctrl_process(td_void)
{
    dec_chan_routine();

    return DEC_OK;
}
