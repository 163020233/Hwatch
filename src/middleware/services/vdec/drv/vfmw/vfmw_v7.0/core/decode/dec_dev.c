/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "dec_dev.h"
#include "dec_chn.h"
#include "dec_thread.h"
#include "dec_ctrl.h"
#include "dbg.h"
#include "vfmw_sys.h"
#include "vfmw_pdt.h"
#include "event.h"
#include "vdec_chr.h"
#if defined(MDMA_NUM) && (MDMA_NUM > 0)
#include "mdma_hal.h"
#endif
STATIC td_s32 dec_dev_get_msg_vdh(const td_void *dec_dev, td_u16 msg_id, td_void *msg);
STATIC td_s32 dec_dev_get_reg_vdh(const td_void *dec_dev, td_u16 reg_id, td_void *reg);
#if defined(MDMA_NUM) && (MDMA_NUM > 0)
STATIC td_s32 dec_dev_get_msg_mdma(const td_void *dev, td_u16 msg_id, td_void *msg);
STATIC td_s32 dec_dev_get_reg_mdma(const td_void *dev, td_u16 reg_id, td_void *reg);
#endif

static dec_dev_ops g_dec_dev_ops[] = {
    /* VDH */
    {
        .open = dec_hal_open_vdh,
        .close = dec_hal_close_vdh,
        .reset = dec_hal_reset_vdh,
        .resume = dec_hal_resume_vdh,
        .suspend = dec_hal_suspend_vdh,
        .init_irq = dec_hal_init_irq_vdh,
        .exit_irq = dec_hal_exit_irq_vdh,
        .config_msg = dec_hal_config_msg_vdh,
        .config_reg = dec_hal_config_reg_vdh,
        .start_reg = dec_hal_start_dec,
        .get_msg = dec_dev_get_msg_vdh,
        .get_reg = dec_dev_get_reg_vdh,
        .get_mem_need_size = dec_hal_get_mem_need_size,
    },
#if defined(MDMA_NUM) && (MDMA_NUM > 0)
    /* MDMA */
    {
        .open = mdma_hal_open,
        .close = mdma_hal_close,
        .reset = mdma_hal_reset,
        .init_irq = mdma_hal_init_irq,
        .exit_irq = mdma_hal_exit_irq,
        .config_msg = mdma_hal_config_msg,
        .config_reg = mdma_hal_config_reg,
        .start_reg = mdma_hal_start,
        .get_msg = dec_dev_get_msg_mdma,
        .get_reg = dec_dev_get_reg_mdma,
        .get_mem_need_size = mdma_hal_get_mem_need_size,
    }
#endif
};

td_s32 dec_dev_get_mem_need_size(vfmw_svr_mem_need *mem)
{
    dec_dev_ops *ops = TD_NULL;
    dec_dev_info *dev = TD_NULL;

    ops = &g_dec_dev_ops[(td_s16)DEC_DEV_TYPE_VDH - 1];
    if (dec_dev_get_dev(DEC_DEV_TYPE_VDH - 1, &dev) == DEC_OK) {
        ops->get_mem_need_size(dev, mem);
    }

#if defined(MDMA_NUM) && (MDMA_NUM > 0)
    ops = &g_dec_dev_ops[(td_s16)DEC_DEV_TYPE_MDMA - 1];
    if (dec_dev_get_dev(DEC_DEV_TYPE_MDMA - 1, &dev) == DEC_OK) {
        ops->get_mem_need_size(dev, mem);
    }
#endif

    return DEC_OK;
}

/* lock and unlock */
STATIC VFMW_ACC_BASE td_void dec_dev_lock_dev(dec_dev_info *dev)
{
    vfmw_unused(dev);

    return;
}

STATIC VFMW_ACC_BASE td_void dec_dev_unlock_dev(dec_dev_info *dev)
{
    vfmw_unused(dev);

    return;
}

/* init and deinit */
STATIC td_void dec_dev_init_dev_pool(td_void)
{
    td_u16 id;
    dec_dev_info *dev = TD_NULL;

    for (id = 0; id < DEC_DEV_NUM; id++) {
        if (dec_dev_get_dev(id, &dev) != DEC_OK) {
            continue;
        }

        if (id < VDH_NUM) {
            vfmw_check_sec_func(memset_s(dev, sizeof(dec_dev_info), 0, sizeof(dec_dev_info)));
            dev->dev_id = id;
            dev->type = DEC_DEV_TYPE_VDH;
            dev->ops = &g_dec_dev_ops[(td_s16)DEC_DEV_TYPE_VDH - 1];
        }
#if defined(MDMA_NUM) && (MDMA_NUM > 0)
        if (id >= VDH_NUM) {
            vfmw_check_sec_func(memset_s(dev, sizeof(dec_dev_info), 0, sizeof(dec_dev_info)));
            dev->dev_id = id;
            dev->type = DEC_DEV_TYPE_MDMA;
            dev->ops = &g_dec_dev_ops[(td_s16)DEC_DEV_TYPE_MDMA - 1];
        }
#endif

        os_sema_init(&dev->sema);
    }

    return;
}

STATIC td_void dec_dev_deinit_dev_pool(td_void)
{
    td_u32 id;
    dec_dev_info *dev = TD_NULL;

    for (id = 0; id < DEC_DEV_NUM; id++) {
        if (dec_dev_get_dev((td_u16)id, &dev) != DEC_OK) {
            continue;
        }

        os_sema_exit(dev->sema);

        if (id < VDH_NUM) {
            vfmw_check_sec_func(memset_s(dev, sizeof(dec_dev_info), 0, sizeof(dec_dev_info)));
        }
#if defined(MDMA_NUM) && (MDMA_NUM > 0)
        if (id >= VDH_NUM) {
            vfmw_check_sec_func(memset_s(dev, sizeof(dec_dev_info), 0, sizeof(dec_dev_info)));
        }
#endif
    }

    return;
}

/* reg and msg */
STATIC td_s32 dec_dev_get_msg_vdh(const td_void *dec_dev, td_u16 msg_id, td_void *msg)
{
    const dec_dev_info *dev = (dec_dev_info *)dec_dev;
    dec_dev_vdh *vdh = (dec_dev_vdh *)dev->handle;
    vdh_msg_pool_info *msg_pool = &vdh->vdh_msg_pool;
    vdh_msg_info **vdh_msg = (vdh_msg_info **)msg;

    if (msg_id >= msg_pool->msg_num) {
        return DEC_ERR;
    }

    *vdh_msg = &msg_pool->vdh_msg[msg_id];

    return DEC_OK;
}

STATIC td_s32 dec_dev_get_reg_vdh(const td_void *dec_dev, td_u16 reg_id, td_void *reg)
{
    const dec_dev_info *dev = (dec_dev_info *)dec_dev;
    dec_dev_vdh *vdh = (dec_dev_vdh *)dev->handle;
    vdh_reg_pool_info *reg_pool = &vdh->vdh_reg_pool;
    vdh_reg_info **vdh_reg = (vdh_reg_info **)reg;

    if (reg_id >= reg_pool->reg_num) {
        return DEC_ERR;
    }

    *vdh_reg = &reg_pool->vdh_reg[reg_id];

    return DEC_OK;
}

#if defined(MDMA_NUM) && (MDMA_NUM > 0)
STATIC td_s32 dec_dev_get_msg_mdma(const td_void *dec_dev, td_u16 msg_id, td_void *msg)
{
    const dec_dev_info *dev = (dec_dev_info *)dec_dev;
    dec_dev_mdma *mdma = (dec_dev_mdma *)dev->handle;
    mdma_msg_info **mama_msg = (mdma_msg_info **)msg;

    vfmw_unused(msg_id);

    *mama_msg = &mdma->msg;

    return DEC_OK;
}

STATIC td_s32 dec_dev_get_reg_mdma(const td_void *dec_dev, td_u16 reg_id, td_void *reg)
{
    const dec_dev_info *dev = (dec_dev_info *)dec_dev;
    dec_dev_mdma *mdma = (dec_dev_mdma *)dev->handle;
    mdma_reg_info **mdma_reg = (mdma_reg_info **)reg;

    vfmw_unused(reg_id);

    *mdma_reg = &mdma->reg;

    return DEC_OK;
}
#endif

STATIC td_s16 dec_dev_get_and_up_next_reg(vdh_reg_pool_info *reg_pool,
    td_u16 *next_reg, td_u8 used_flag, td_u16 *reg_id)
{
    vdh_reg_info *reg = TD_NULL;
    td_u16 rid;
    td_s16 ret;

    rid = *next_reg;
    reg = &reg_pool->vdh_reg[rid];

    if (reg->used == used_flag) {
        *next_reg = (rid + 1) % reg_pool->reg_num;
        *reg_id = rid;
        ret = DEC_OK;
    } else {
        ret = DEC_ERR;
    }

    return ret;
}

VFMW_ACC_BASE STATIC td_s16 dec_dev_get_next_cfg_reg(dec_dev_info *dev, td_u16 *reg_id)
{
    dec_dev_vdh *vdh = (dec_dev_vdh *)dev->handle;
    vdh_reg_pool_info *reg_pool = &vdh->vdh_reg_pool;
    td_s16 ret;

    dec_dev_lock_dev(dev);

    ret = dec_dev_get_and_up_next_reg(reg_pool, &reg_pool->next_cfg_reg, 0, reg_id);
    if (ret == DEC_OK) {
        vdh->vdh_reg_pool.vdh_reg[*reg_id].used = 1;
    }

    dec_dev_unlock_dev(dev);

    return ret;
}

STATIC td_s16 dec_dev_get_next_bsp_reg(dec_dev_info *dev, td_u16 *reg_id)
{
    dec_dev_vdh *vdh = (dec_dev_vdh *)dev->handle;
    vdh_reg_pool_info *reg_pool = &vdh->vdh_reg_pool;
    td_s16 ret;

    dec_dev_lock_dev(dev);

    ret = dec_dev_get_and_up_next_reg(reg_pool, &reg_pool->next_bsp_reg, 1, reg_id);
    if (ret != DEC_OK) {
        dprint(PRN_ERROR, "Vdh register error!\n");
    }

    dec_dev_unlock_dev(dev);

    return ret;
}

STATIC td_s16 dec_dev_get_cur_pxp_reg(dec_dev_info *dev, td_u16 *reg_id)
{
    dec_dev_vdh *vdh = (dec_dev_vdh *)dev->handle;
    vdh_reg_pool_info *reg_pool = &vdh->vdh_reg_pool;
    td_s16 ret = DEC_ERR;
    td_u16 rid;

    dec_dev_lock_dev(dev);

    if (dev->type == DEC_DEV_TYPE_MDMA) {
        *reg_id = 0;
        ret = DEC_OK;
    } else {
        rid = reg_pool->next_pxp_reg;
        if (reg_pool->vdh_reg[rid].used == 1) {
            *reg_id = rid;
            ret = DEC_OK;
        }
    }

    dec_dev_unlock_dev(dev);

    return ret;
}

STATIC td_s16 dec_dev_update_pxp_reg(dec_dev_info *dev)
{
    dec_dev_vdh *vdh = (dec_dev_vdh *)dev->handle;
    vdh_reg_pool_info *reg_pool = &vdh->vdh_reg_pool;
    td_s16 ret = DEC_ERR;
    td_u16 rid;

    dec_dev_lock_dev(dev);

    if (dev->type == DEC_DEV_TYPE_MDMA) {
        ret = DEC_OK;
    } else if (dev->type == DEC_DEV_TYPE_VDH) {
        rid = reg_pool->next_pxp_reg;
        if (reg_pool->vdh_reg[rid].used == 1) {
            reg_pool->next_pxp_reg = (rid + 1) % reg_pool->reg_num;
            ret = DEC_OK;
        }
    }

    dec_dev_unlock_dev(dev);

    return ret;
}

VFMW_ACC_BASE STATIC td_s16 dec_dev_get_next_cfg_msg(dec_dev_info *dev, td_u16 *msg_id)
{
    dec_dev_vdh *vdh = TD_NULL;
    vdh_msg_pool_info *msg_pool = TD_NULL;
    vdh_msg_info *msg = TD_NULL;
    td_u16 mid, i;
    td_s16 ret = DEC_ERR;

    vfmw_check_ret(dev != TD_NULL, DEC_ERR);
    vfmw_check_ret(msg_id != TD_NULL, DEC_ERR);

    vdh = (dec_dev_vdh *)dev->handle;

    vfmw_check_ret(vdh != TD_NULL, DEC_ERR);

    msg_pool = &vdh->vdh_msg_pool;

    dec_dev_lock_dev(dev);

    for (i = 0; i < msg_pool->msg_num; i++) {
        mid = msg_pool->next_cfg_msg;
        if (mid < VDH_MSG_NUM) {
            msg = &msg_pool->vdh_msg[mid];
        }
        msg_pool->next_cfg_msg = (mid + 1) % msg_pool->msg_num;

        vfmw_check_ret(msg != TD_NULL, DEC_ERR);
        if (msg->used == 0) {
            msg->used = 1;
            *msg_id = mid;
            ret = DEC_OK;
            break;
        }
    }

    if (i >= msg_pool->msg_num) {
        /* set block cond that all message are busy */
        ret = DEC_ERR;
    }

    dec_dev_unlock_dev(dev);

    return ret;
}

STATIC td_s32 dec_dev_get_chan_id(dec_dev_info *dev, td_u16 reg_id, td_s32 *chan_id)
{
    dec_dev_vdh *vdh = TD_NULL;
    dec_dev_mdma *mdma = TD_NULL;

    *chan_id = 0xffff;

    if (dev->type == DEC_DEV_TYPE_VDH) {
        vdh = (dec_dev_vdh *)dev->handle;
        if (vdh->vdh_reg_pool.vdh_reg[reg_id].used == 1) {
            *chan_id = (td_u16)vdh->vdh_reg_pool.vdh_reg[reg_id].chan_id;
        }
    } else {
        mdma = (dec_dev_mdma *)dev->handle;
        if (mdma->used == 1) {
            *chan_id = mdma->chan_id;
        }
    }

    return (*chan_id == 0xffff) ? DEC_ERR : DEC_OK;
}

STATIC td_void dec_dev_stat_work_load(dec_dev_info *dev)
{
    td_u32 cur_time, peroid, dev_busy_flag;
    dec_dev_vdh *vdh = TD_NULL;
    dec_dev_mdma *mdma = TD_NULL;

    vfmw_check(os_get_time_us != TD_NULL);
    if (dev->type == DEC_DEV_TYPE_VDH) {
        vdh = (dec_dev_vdh *)dev->handle;
        dev_busy_flag = (vdh->pxp_int_cnt < vdh->reg_cfg_cnt) ? 1 : 0;
    } else {
        mdma = (dec_dev_mdma *)dev->handle;
        dev_busy_flag = (mdma->int_cnt < mdma->reg_cfg_cnt) ? 1 : 0;
    }

    cur_time = os_get_time_us();
    dev->isr_schd_intval[dev->dec_frm_cnt % DEC_DEBUG_CNT] = cur_time - dev->isr_last_time;
    dev->isr_last_time = cur_time;

    dev->work_time = cur_time - dev->last_start_time;
    dev->work_max_time = dev->work_max_time > dev->work_time
                         ? dev->work_max_time : dev->work_time;

    if (dev->update_step_time == 1 || dev->step_start_time == 0) {
        dev->step_start_time = dev->last_start_time;
        dev->update_step_time = 0;
    }

    peroid = cur_time - dev->last_count_time;
    if (peroid > 1000 * 1000) { /* 1000 :a number 1000 :a number */
        dev->peroid_work_time += (cur_time - dev->step_start_time);
        dev->kilo_peroid_bsp_cycle += (dev->back_up.bsp_cycle);
        dev->kilo_peroid_pxp_cycle += (dev->back_up.pxp_cycle);
        dev->step_start_time = cur_time;
        dev->ratio = dev->peroid_work_time * 1000 / peroid; /* 1000 :a number */
        dev->avg_bsp_cycle = (dev->peroid_frm_cnt > 0) ?
            ((dev->kilo_peroid_bsp_cycle * 1000) / dev->peroid_frm_cnt) : 0; /* 1000 :a number */
        dev->avg_pxp_cycle = (dev->peroid_frm_cnt > 0) ?
            ((dev->kilo_peroid_pxp_cycle * 1000) / dev->peroid_frm_cnt) : 0; /* 1000 :a number */
        dev->peroid_work_time = 0;
        dev->kilo_peroid_bsp_cycle = 0;
        dev->kilo_peroid_pxp_cycle = 0;
        dev->peroid_frm_cnt = 0;
        dev->last_count_time = os_get_time_us();
    } else {
        if (dev_busy_flag == 0) {
            dev->peroid_work_time += (cur_time - dev->step_start_time);
        }

        dev->kilo_peroid_bsp_cycle += (dev->back_up.bsp_cycle);
        dev->kilo_peroid_pxp_cycle += (dev->back_up.pxp_cycle);
        dev->peroid_frm_cnt++;
    }

    if (dev_busy_flag == 0) {
        dev->update_step_time = 1;
    }
}

STATIC td_void dec_dev_int_cnt_inc(dec_dev_info *dev, td_s32 reg_id)
{
    dec_dev_vdh *vdh = TD_NULL;
    dec_dev_mdma *mdma = TD_NULL;
    td_s32 reg = reg_id;

    /*
    dev and handle not null ensured by caller
    when bsp going to use , add another input param to differ
    */
    if (dev->type == DEC_DEV_TYPE_VDH) {
        if (dec_hal_control(dev->dev_id, DHAL_CID_IS_VDH_INT, &reg, TD_NULL) == DEC_OK) {
            vdh = (dec_dev_vdh *)dev->handle;
            vdh->pxp_int_cnt++;
        }
    } else if (dev->type == DEC_DEV_TYPE_MDMA) {
        mdma = (dec_dev_mdma *)dev->handle;
        mdma->int_cnt++;
    } else {
        dprint(PRN_ERROR, "%s type error is %d\n", __func__, dev->type);
    }
}
/* ISR Process */
STATIC td_void dec_dev_bsp_int_process(td_u16 dev_id)
{
    dec_dev_info *dev = TD_NULL;
    dec_dev_vdh *vdh = TD_NULL;
    dec_chan *chan = TD_NULL;
    dec_chan_task *task = TD_NULL;
    td_u16 reg_id = 0;
    td_s32 chan_id = 0;
    td_ulong lock_flag;

    vfmw_unused(dev_id);
    /* Get Dev */
    if (dec_dev_get_dev(dev_id, &dev) != DEC_OK) {
        dprint(PRN_ERROR, "%s GetDev error\n", __func__);
        return;
    }

    vdh = (dec_dev_vdh *)dev->handle;
    if (vdh == TD_NULL) {
        dprint(PRN_ERROR, "%s VDH null\n", __func__);
        return;
    }

    vdh->bsp_int_cnt++;

    /* Get Reg */
    if (dec_dev_get_next_bsp_reg(dev, &reg_id) != DEC_OK) {
        dprint(PRN_ERROR, "%s get_reg error\n", __func__);
        return;
    }

    /* Get Chan and task */
    if (dec_dev_get_chan_id(dev, reg_id, &chan_id) != DEC_OK) {
        dprint(PRN_ERROR, "%s dev %d get chan_id error\n", __func__, reg_id);
        return;
    }

    dec_chan_lock_chan(chan_id, &lock_flag);

    chan = dec_ctrl_get_chan(chan_id);
    if (chan == TD_NULL ||
        chan->state == DEC_CHAN_STATE_NULL ||
        chan->state == DEC_CHAN_STATE_CANCEL) {
        dprint(PRN_ERROR, "%s Chan%d not valid\n", __func__, chan_id);
        goto exit;
    }

    if (dec_chan_get_bsp_task(chan, &task) != DEC_OK) {
        goto exit;
    }

    if (task->state == DEC_TASK_DECODING) { /* if task is normal */
        dec_chan_release_task_seg(chan, task);
    } else if (task->state == DEC_TASK_TEMPCANCEL) { /* if task have cancelled temporarily */
    } else if (task->state == DEC_TASK_PERMCANCEL) { /* if task have cancelled permanently */
        dec_chan_release_task_seg(chan, task);
    }

exit:

    dec_chan_unlock_chan(chan_id, &lock_flag);

    return;
}

STATIC td_void dec_dev_check_id(td_s32 *pdev_id)
{
#if defined(MDMA_NUM) && (MDMA_NUM > 0)
    td_s32 dev_id = VDH_NUM;
    dec_dev_info *dev = TD_NULL;
    dec_dev_mdma *mdma = TD_NULL;

    vfmw_unused(pdev_id);
    if (dec_dev_get_dev(dev_id, &dev) != DEC_OK) {
        return;
    }

    mdma = (dec_dev_mdma *)dev->handle;
    if (mdma != TD_NULL &&
        mdma->irq_num == 0 &&
        mdma->used == 1) {
        *pdev_id = dev_id;
    }
#else
    vfmw_unused(pdev_id);
#endif
}

STATIC td_void dec_dev_decoding_post_proc(dec_chan *chan,
    dec_chan_task *task, td_u32 dec_state)
{
    if (dec_state == DEC_INT_STATE_BOTH ||
        dec_state == DEC_INT_STATE_OVER) {
        dec_chan_update_pxp_task(chan);
        dec_chan_release_task_seg(chan, task);
        dec_chan_decrease_dec_task_num(chan);
        dec_post_make_report(chan, task);
        dec_post_record_reg_info(chan, task);
    }

    if (dec_state == DEC_INT_STATE_BOTH ||
        dec_state == DEC_INT_STATE_PART) {
        dec_chan_mark_display_state(chan, task, 1);
    }

    dec_hal_control(task->dev_id, DHAL_CID_GET_PIX_SUM, chan, task);
    dec_hal_control(task->dev_id, DHAL_CID_GET_CYCLE, chan, task);

    if (dec_state == DEC_INT_STATE_BOTH ||
        dec_state == DEC_INT_STATE_OVER) {
        dec_chan_increase_dec_dps_num(chan);
        dec_chan_clear_ref_info(chan, task);
        dec_chan_release_task_dps(chan, task);
        dec_dev_release_msg(task->dev_id, task->msg_id);
    }
}

STATIC td_void dec_dev_temp_cancel_post_proc(dec_chan *chan, dec_chan_task *task)
{
    dec_chan_decrease_dec_task_num(chan);
    dec_chan_update_pxp_task(chan);
    chan->chan_task_pool.next_cfg_task = (chan->chan_task_pool.next_cfg_task + DEC_MAX_TASK_NUM - 1) %
                                         DEC_MAX_TASK_NUM;
    chan->chan_task_pool.cfg_task_num++;
    task->state = DEC_TASK_CFGMSGOK;

    if (chan->chan_task_pool.dec_task_num == 0) {
        dec_chan_adjust_ref_info(chan, TD_NULL);
        chan->chan_task_pool.next_pxp_task = chan->chan_task_pool.next_cfg_task;
        chan->state = DEC_CHAN_STATE_RUN;
    }
}

STATIC td_void dec_dev_perm_cancel_post_proc(dec_chan *chan, dec_chan_task *task)
{
    dec_chan_release_task_seg(chan, task);
    dec_chan_mark_display_state(chan, task, 0);
    dec_chan_release_task_dps(chan, task);
    if (dec_dev_release_msg(task->dev_id, task->msg_id) != DEC_OK) {
        dprint(PRN_ERROR, "chan %d release msg fail!\n", chan->chan_id);
    }
}

STATIC td_s32 dev_dev_sed_post_process(dec_chan *chan, dec_chan_task *task)
{
    dec_post_make_report(chan, task);
    if (task->dps->pic_ctb_num == task->dps->sed_ctb_num) {
        task->dps->sed_state = SED_SUCC;
    } else {
        if (task->dps->ldy_mode == LDY_SED) {
            dec_chan_clear_ref_info(chan, task);
        }
        dec_chan_update_pxp_task(chan);
        dec_chan_decrease_dec_task_num(chan);
        dec_chan_release_task_dps(chan, task);
        dec_dev_release_msg(task->dev_id, task->msg_id);
        return DEC_ERR;
    }

    return DEC_OK;
}

STATIC td_void dev_dev_post_get_dec_state(const dec_dev_info *dev, const dec_chan_task *task, td_u32 *dec_state)
{
    if (task->mem_param.low_delay_en == 0) {
        (*dec_state) = DEC_INT_STATE_BOTH;
    } else if (dev->back_up.int_flag == 1) {
        (*dec_state) = DEC_INT_STATE_OVER;
    } else if (dev->back_up.int_flag == 2) { /* 2 :a number */
        (*dec_state) = DEC_INT_STATE_PART;
    } else {
        (*dec_state) = DEC_INT_STATE_BOTH;
    }
}

STATIC td_void dec_dev_post_proc(const dec_dev_info *dev, td_s32 chan_id, td_u32 *keep_reg)
{
    td_ulong lock_flag;
    td_u32 dec_state = DEC_INT_STATE_NULL;
    dec_chan *chan = TD_NULL;
    dec_chan_task *task = TD_NULL;

    dec_chan_lock_chan(chan_id, &lock_flag);

    chan = dec_ctrl_get_chan(chan_id);
    if (chan == TD_NULL ||
        chan->state == DEC_CHAN_STATE_NULL) {
        dprint(PRN_WARN, "%s chan %d state err\n", __func__, chan_id);
        goto exit;
    }

    if (dec_chan_get_pxp_task(chan, &task) != DEC_OK) {
        dprint(PRN_ERROR, "%s GetTask error\n", __func__);
        goto exit;
    }

    dec_state = DEC_INT_STATE_BOTH;

    if (chan->state != DEC_CHAN_STATE_CANCEL &&
        task->dps->sed_state == SED_TRY && (dev_dev_sed_post_process(chan, task) != DEC_OK)) {
        goto exit;
    }

    dev_dev_post_get_dec_state(dev, task, &dec_state);

    if (task->state == DEC_TASK_DECODING) { /* if task is normal */
        dec_dev_decoding_post_proc(chan, task, dec_state);
    } else if (task->state == DEC_TASK_TEMPCANCEL) { /* if task have cancelled temporarily */
        dec_dev_temp_cancel_post_proc(chan, task);
    } else if (task->state == DEC_TASK_PERMCANCEL) { /* if task have cancelled permanently */
        dec_dev_perm_cancel_post_proc(chan, task);
    }

exit:

    if (dec_state == DEC_INT_STATE_PART) {
        *keep_reg = 1;
    }

    dec_chan_unlock_chan(chan_id, &lock_flag);

    return;
}

STATIC td_s32 dec_dev_get_dev_info(td_s32 dev_id, dec_dev_info **dev_p, td_u16 *reg_id)
{
    dec_dev_info *dev = TD_NULL;

    /* Get Dev */
    if (dec_dev_get_dev((td_u16)dev_id, dev_p) != DEC_OK) {
        dprint(PRN_ERROR, "%s GetDev error\n", __func__);
        return EXT_FAILURE;
    }

    dev = *dev_p;
    vfmw_check_ret(dev != TD_NULL, EXT_FAILURE);
    if (dev->handle == TD_NULL) {
        dprint(PRN_ERROR, "%s dev->handle null\n", __func__);
        return EXT_FAILURE;
    }

    /* Get Reg */
    if (dec_dev_get_cur_pxp_reg(dev, reg_id) != DEC_OK) {
        dprint(PRN_ERROR, "%s dev %d type %d get_reg error\n", __func__, dev_id, dev->type);
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

VFMW_ACC_BASE STATIC td_void dec_dev_pxp_int_process(td_s32 dev_id)
{
    td_s32 ret;
    dec_dev_info *dev = TD_NULL;
    td_u16 reg_id = 0;
    td_s32 chan_id = 0;
    td_u32 keep_reg = 0;
    td_u32 start_time, end_time;

    vfmw_check(os_get_time_us != TD_NULL);
    start_time = os_get_time_us();

    dec_thread_wakeup();

#ifdef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
    os_flush_all_cache();
#endif

    dec_dev_check_id(&dev_id);

    ret = dec_dev_get_dev_info(dev_id, &dev, &reg_id);
    if (ret != EXT_SUCCESS) {
        goto err;
    }
    ret = pdt_add_sleep_apb_veto(VFMW_ID_VDH);
    if (ret != EXT_SUCCESS) {
        goto err;
    }

    dec_dev_int_cnt_inc(dev, reg_id);

    dec_dev_stat_work_load(dev);

    dec_hal_read_int_info(dev, reg_id);

    dec_hal_clear_int_state(dev, reg_id);

    /* Get Chan and task */
    if (dec_dev_get_chan_id(dev, reg_id, &chan_id) != DEC_OK) {
        dprint(PRN_ERROR, "%s dev %d get chan_id error\n", __func__, reg_id);
        pdt_remove_sleep_apb_veto(VFMW_ID_VDH);
        goto out;
    }

    dec_dev_post_proc(dev, chan_id, &keep_reg);

    pdt_remove_sleep_apb_veto(VFMW_ID_VDH);

    end_time = os_get_time_us();
    dev->isr_cost_time[dev->dec_frm_cnt % DEC_DEBUG_CNT] = end_time - start_time;
    dev->isr_max_cost_time = ((td_s32)dev->isr_max_cost_time < (td_s32)(end_time - start_time))
                             ? (end_time - start_time) : dev->isr_max_cost_time;

    dev->dec_frm_cnt++;

out:
    if (keep_reg == 0) {
        pdt_remove_sleep_veto(VFMW_ID_VDH);
        dec_dev_update_pxp_reg(dev);
        dec_dev_release_reg((td_u16)dev_id, reg_id);
    }
    return;
err:
    pdt_remove_sleep_veto(VFMW_ID_VDH);
    return;
}

STATIC td_void dec_dev_mmu_int_process(td_u16 dev_id)
{
    dec_dev_info *dev = TD_NULL;

    /* Get Dev */
    if (dec_dev_get_dev(dev_id, &dev) != DEC_OK) {
        dprint(PRN_ERROR, "%s GetDev error\n", __func__);
        return;
    }

    if (dev->handle == TD_NULL) {
        dprint(PRN_ERROR, "%s dev->handle null\n", __func__);
        return;
    }

    dec_dev_lock_dev(dev);

    dec_dev_unlock_dev(dev);

    return;
}

/******************** interface *******************/
VFMW_ACC_BASE td_s32 dec_dev_get_dev(td_u16 dev_id, dec_dev_info **dev)
{
    dec_dev_pool *dev_pool = TD_NULL;

    vfmw_check_ret(dev != TD_NULL, DEC_ERR);

    if (dev_id >= DEC_DEV_NUM) {
        dprint(PRN_ERROR, "DecDev dev_id Error!\n");
        return DEC_ERR;
    }

    dev_pool = dec_ctrl_get_dev_pool();

    *dev = &(dev_pool->dev_inst[dev_id]);

    return DEC_OK;
}

td_void dec_dev_init(td_void)
{
    dec_dev_init_dev_pool();

    return;
}

td_void dec_dev_deinit(td_void)
{
    dec_dev_deinit_dev_pool();

    return;
}

td_s32 dec_dev_open(td_s32 is_sec)
{
    dec_dev_info *dev = TD_NULL;
    td_u16 dev_id;

    for (dev_id = 0; dev_id < DEC_DEV_NUM; dev_id++) {
        if (VDH_NUM > 1) {
            if ((is_sec == 0 && dev_id % VDH_NUM == 1) ||
                (is_sec == 1 && dev_id % VDH_NUM == 0)) {
                continue;
            }
        }

        if (dec_dev_get_dev(dev_id, &dev) != DEC_OK) {
            return DEC_ERR;
        }

        os_sema_down(dev->sema);

        if (dev->state != DEC_DEV_STATE_NULL) {
            os_sema_up(dev->sema);
            return DEC_OK;
        }

        if (dev->ops->open(dev) != DEC_OK) {
            goto err;
        }

        if (dev->ops->reset(dev) != DEC_OK) {
            goto err;
        }

        if (dev->ops->init_irq(dev) != DEC_OK) {
            goto err;
        }

        dev->state = DEC_DEV_STATE_RUN;
        os_sema_up(dev->sema);
    }

    return DEC_OK;

err:
    os_sema_up(dev->sema);
    return DEC_ERR;
}

STATIC td_s32 dec_dev_wait_int(td_s32 is_sec)
{
    td_u16 dev_id;
    dec_dev_info *dev = TD_NULL;
    dec_dev_vdh *vdh = TD_NULL;
    dec_dev_mdma *mdma = TD_NULL;

    for (dev_id = 0; dev_id < DEC_DEV_NUM; dev_id++) {
        if ((is_sec == 0 && dev_id % VDH_NUM == 1) || (is_sec == 1 && dev_id % VDH_NUM == 0)) {
            continue;
        }

        if (dec_dev_get_dev(dev_id, &dev) != DEC_OK) {
            return DEC_ERR;
        }

        if (dev->type == DEC_DEV_TYPE_VDH) {
            vdh = (dec_dev_vdh *)dev->handle;
            if ((vdh != TD_NULL) && (vdh->reg_cfg_cnt != vdh->pxp_int_cnt)) {
                return DEC_ERR;
            }
        } else {
            mdma = (dec_dev_mdma *)dev->handle;
            if ((mdma != TD_NULL) && (mdma->reg_cfg_cnt != mdma->int_cnt)) {
                return DEC_ERR;
            }
        }
    }

    return DEC_OK;
}

td_s32 dec_dev_close(td_s32 is_sec)
{
    td_s32 cnt;
    td_u16 dev_id;
    dec_dev_info *dev = TD_NULL;

    for (cnt = 0; cnt < 1000; cnt++) { /* 1000 :a number */
        if (dec_dev_wait_int(is_sec) == DEC_OK) {
            break;
        }

        os_msleep(1);
    }

    if (cnt >= 1000) { /* 1000 :a number */
        dprint(PRN_ERROR, "DecDev WaitInt Fail\n");
    }

    for (dev_id = 0; dev_id < DEC_DEV_NUM; dev_id++) {
        if (VDH_NUM > 1) {
            if ((is_sec == 0 && dev_id % VDH_NUM == 1) ||
                (is_sec == 1 && dev_id % VDH_NUM == 0)) {
                continue;
            }
        }

        vfmw_check_ret(dec_dev_get_dev(dev_id, &dev) == DEC_OK, DEC_ERR);

        os_sema_down(dev->sema);
        if (dev->state == DEC_DEV_STATE_NULL) {
            /* has not open */
            os_sema_up(dev->sema);
            return DEC_OK;
        }

        if (dev->ops->exit_irq(dev) != DEC_OK) {
            goto err;
        }

        if (dev->ops->reset(dev) != DEC_OK) {
            goto err;
        }

        vfmw_check_goto(dev->ops->close(dev) == DEC_OK, err);

        dev->state = DEC_DEV_STATE_NULL;
        os_sema_up(dev->sema);
    }

    return DEC_OK;

err:
    os_sema_up(dev->sema);
    return DEC_ERR;
}

td_s32 dec_dev_resume(td_void)
{
    dec_dev_info *dev = TD_NULL;
    td_u16 dev_id;

    for (dev_id = 0; dev_id < DEC_DEV_NUM; dev_id++) {
        if (dec_dev_get_dev(dev_id, &dev) != DEC_OK) {
            return DEC_ERR;
        }

        if (dev->state == DEC_DEV_STATE_NULL) {
            continue;
        }

        if (dev->ops->resume != TD_NULL &&
            dev->ops->resume(dev) != DEC_OK) {
            return DEC_ERR;
        }

        dev->state = DEC_DEV_STATE_RUN;
    }

    return DEC_OK;
}

STATIC td_void dec_dev_wait_over(const dec_dev_info *dev)
{
    td_u32 time_out = 0x400;
    dec_dev_vdh *vdh = TD_NULL;
    dec_dev_mdma *mdma = TD_NULL;

    vfmw_check(dev != TD_NULL);

    while (time_out-- > 0) {
        if (dev->type == DEC_DEV_TYPE_VDH) {
            vdh = (dec_dev_vdh *)dev->handle;
            if ((vdh == TD_NULL) || (vdh->reg_cfg_cnt == vdh->pxp_int_cnt)) {
                break;
            }
        } else {
            mdma = (dec_dev_mdma *)dev->handle;
            if ((mdma == TD_NULL) || (mdma->reg_cfg_cnt == mdma->int_cnt)) {
                break;
            }
        }

        os_msleep(1);
    }

    if (time_out == 0) {
        dprint(PRN_ERROR, "dec wait %d err\n", dev->dev_id);
    }
}

td_s32 dec_dev_suspend(td_void)
{
    dec_dev_info *dev = TD_NULL;
    td_u16 dev_id;

    for (dev_id = 0; dev_id < DEC_DEV_NUM; dev_id++) {
        if (dec_dev_get_dev(dev_id, &dev) != DEC_OK) {
            return DEC_ERR;
        }

        if (dev->state == DEC_DEV_STATE_NULL) {
            continue;
        }

        dec_dev_wait_over(dev);

        if (dev->ops->suspend != TD_NULL &&
            dev->ops->suspend(dev) != DEC_OK) {
            return DEC_ERR;
        }

        dev->state = DEC_DEV_STATE_SUSPEND;
    }

    return DEC_OK;
}

td_s32 dec_dev_config(td_u16 dev_id, td_void *cfg)
{
    vfmw_unused(cfg);
    vfmw_unused(dev_id);

    return DEC_ERR;
}

os_irqret dec_dev_isr_bsp0(td_s32 irq, td_void *dev_id)
{
    const td_s32 irq_handled = 1;

    vfmw_unused(irq);
    vfmw_unused(dev_id);

    dec_dev_bsp_int_process(0);

    return irq_handled;
}

VFMW_ACC_BASE os_irqret dec_dev_isr_pxp0(td_s32 irq, td_void *dev_id)
{
    const td_s32 irq_handled = 1;

    vfmw_unused(irq);
    vfmw_unused(dev_id);

    dec_dev_pxp_int_process(0);

    return irq_handled;
}

VFMW_ACC_BASE os_irqret dec_dev_isr_pxp1(td_s32 irq, td_void *dev_id)
{
    const td_s32 irq_handled = 1;

    vfmw_unused(irq);
    vfmw_unused(dev_id);

    dec_dev_pxp_int_process(1);

    return irq_handled;
}

os_irqret dec_dev_isr_mdma0(td_s32 irq, td_void *dev_id)
{
    const td_s32 irq_handled = 1;

    vfmw_unused(irq);
    vfmw_unused(dev_id);

    dec_dev_pxp_int_process(VDH_NUM);

    return irq_handled;
}

os_irqret dec_dev_isr_mdma1(td_s32 irq, td_void *dev_id)
{
    const td_s32 irq_handled = 1;

    vfmw_unused(irq);
    vfmw_unused(dev_id);

    dec_dev_pxp_int_process(VDH_NUM + 1);

    return irq_handled;
}

os_irqret dec_dev_isr_mmu0(td_s32 irq, td_void *dev_id)
{
    const td_s32 irq_handled = 1;

    vfmw_unused(irq);
    vfmw_unused(dev_id);

    dec_dev_mmu_int_process(0);

    return irq_handled;
}

os_irqret dec_dev_isr_mmu1(td_s32 irq, td_void *dev_id)
{
    const td_s32 irq_handled = 1;

    vfmw_unused(irq);
    vfmw_unused(dev_id);

    dec_dev_mmu_int_process(1);

    return irq_handled;
}

VFMW_ACC_BASE td_s32 dec_dev_acquire_dev(td_s32 is_sec,
    td_void *task, td_u16 *dev_id)
{
    return dec_hal_get_dev(is_sec, task, dev_id);
}

VFMW_ACC_BASE td_s32 dec_dev_acquire_msg(td_s32 chan_id, td_u16 dev_id, td_u16 *msg_id)
{
    dec_dev_info *dev = TD_NULL;
    dec_dev_vdh *vdh = TD_NULL;
    dec_dev_mdma *mdma = TD_NULL;

    if (dec_dev_get_dev(dev_id, &dev) != DEC_OK) {
        return DEC_ERR;
    }

    if (dev->handle == TD_NULL) {
        /* vdh not open */
        return DEC_ERR;
    }

    if (dev->type == DEC_DEV_TYPE_MDMA) {
        mdma = (dec_dev_mdma *)dev->handle;
        mdma->chan_id = chan_id;
        *msg_id = 0;
        return DEC_OK;
    }

    vdh = (dec_dev_vdh *)dev->handle;

    if (dec_dev_get_next_cfg_msg(dev, msg_id) != DEC_ERR) {
        vdh->vdh_msg_pool.vdh_msg[*msg_id].chan_id = chan_id;
        return DEC_OK;
    } else {
        return DEC_ERR;
    }
}

VFMW_ACC_BASE td_s32 dec_dev_acquire_reg(td_s32 chan_id, td_u16 dev_id, td_u16 *reg_id)
{
    dec_dev_info *dev = TD_NULL;
    dec_dev_vdh *vdh = TD_NULL;
    dec_dev_mdma *mdma = TD_NULL;

    if (dec_dev_get_dev(dev_id, &dev) != DEC_OK) {
        return DEC_ERR;
    }

    if (dev->handle == TD_NULL) {
        /* vdh not open */
        return DEC_ERR;
    }

    if (dec_hal_control(0, DHAL_CID_GET_VDH, TD_NULL, TD_NULL) != DEC_OK) {
        return DEC_ERR;
    }

    if (dev->type == DEC_DEV_TYPE_MDMA) {
        mdma = (dec_dev_mdma *)dev->handle;
        mdma->chan_id = chan_id;
        mdma->used = 1;
        *reg_id = 0;
        return DEC_OK;
    }

    vdh = (dec_dev_vdh *)dev->handle;

    if (dec_dev_get_next_cfg_reg(dev, reg_id) != DEC_ERR) {
        vdh->vdh_reg_pool.vdh_reg[*reg_id].chan_id = chan_id;
        return DEC_OK;
    } else {
        (td_void)dec_hal_control(0, DHAL_CID_PUT_VDH, TD_NULL, TD_NULL);
        return DEC_ERR;
    }
}

VFMW_ACC_BASE td_s32 dec_dev_acquire_cfg_id(td_u16 dev_id, td_u16 *cfg_id)
{
    dec_dev_info *dev = TD_NULL;

    vfmw_check_ret(cfg_id != TD_NULL, DEC_ERR);

    if (dec_dev_get_dev(dev_id, &dev) != DEC_OK) {
        return DEC_ERR;
    }

    *cfg_id = dev->cfg_frm_cnt++;

    return DEC_OK;
}

td_s32 dec_dev_release_msg(td_u16 dev_id, td_u16 msg_id)
{
    dec_dev_info *dev = TD_NULL;
    dec_dev_vdh *vdh = TD_NULL;

    if (dec_dev_get_dev(dev_id, &dev) != DEC_OK) {
        return DEC_ERR;
    }

    if (dev->handle == TD_NULL) {
        /* vdh not open */
        return DEC_ERR;
    }

    if (dev->type == DEC_DEV_TYPE_MDMA) {
        /* MDMA has only one Msg */
        return DEC_OK;
    }

    dec_dev_lock_dev(dev);

    vdh = (dec_dev_vdh *)dev->handle;
    vdh->vdh_msg_pool.vdh_msg[msg_id].used = 0;

    dec_dev_unlock_dev(dev);

    return DEC_OK;
}

td_s32 dec_dev_release_reg(td_u16 dev_id, td_u16 reg_id)
{
    dec_dev_info *dev = TD_NULL;
    dec_dev_vdh *vdh = TD_NULL;
    dec_dev_mdma *mdma = TD_NULL;

    if (dec_dev_get_dev(dev_id, &dev) != DEC_OK) {
        return DEC_ERR;
    }

    if (dev->handle == TD_NULL) {
        /* vdh not open */
        return DEC_ERR;
    }

    if (dev->type == DEC_DEV_TYPE_MDMA) {
        mdma = (dec_dev_mdma *)dev->handle;
        mdma->used = 0;
        mdma->chan_id = 0xffffffff;
        return DEC_OK;
    }

    dec_dev_lock_dev(dev);

    vdh = (dec_dev_vdh *)dev->handle;
    vdh->vdh_reg_pool.vdh_reg[reg_id].used = 0;
    vdh->vdh_reg_pool.vdh_reg[reg_id].msg_id = 0;
    vdh->vdh_reg_pool.vdh_reg[reg_id].chan_id = 0xffffffff;

    dec_dev_unlock_dev(dev);

    return DEC_OK;
}

VFMW_ACC_BASE td_s32 dec_dev_config_msg(td_u16 dev_id, td_u16 msg_id,
    dec_hal_ops *ops, td_void *task)
{
    dec_dev_info *dev = TD_NULL;
    td_s32 ret = DEC_ERR;

    vfmw_unused(msg_id);

    if (dec_dev_get_dev(dev_id, &dev) == DEC_OK) {
        ret = dev->ops->config_msg(dev, ops, task);
    }
    return ret;
}

VFMW_ACC_BASE td_s32 dec_dev_config_reg(td_u16 dev_id, td_u16 reg_id,
    dec_hal_ops *ops, td_void *task)
{
    dec_dev_info *dev = TD_NULL;

    vfmw_unused(reg_id);

    if (dec_dev_get_dev(dev_id, &dev) != DEC_OK) {
        dprint(PRN_ERROR, "%s return error!\n", __func__);
        return DEC_ERR;
    }
    dev->ops->config_reg(dev, ops, task);

    return DEC_OK;
}

STATIC VFMW_ACC_BASE td_void dev_dev_time_out_process(dec_chan *chan, td_u16 reg_id,
    dec_dev_info *dev, dec_chan_task *task)
{
    td_u32 cur_time_in_us;
    td_u32 delay_time;
    td_u32 vdh_time_out;

    vfmw_check(chan != TD_NULL);
    vfmw_check(dev != TD_NULL);
    vfmw_check(os_get_time_us != TD_NULL);

    cur_time_in_us = os_get_time_us();
    if (cur_time_in_us < dev->last_start_time) {
        delay_time = 0;
        dev->last_start_time = cur_time_in_us;
    } else {
        delay_time = (cur_time_in_us - dev->last_start_time) / 1000; /* 1000 :a number */
    }

#ifdef VCODEC_FPGA_SUPPORT
    vdh_time_out = VDH_TIME_OUT * 100; /* 100 times */
#else
    vdh_time_out = VDH_TIME_OUT;
#endif

#ifdef VFMW_VDH_DFX_SUPPORT
    if (dec_hal_control(dev->dev_id, DHAL_CID_IS_VDH_INT, &reg_id, TD_NULL) == DEC_OK) {
        dec_dev_isr_pxp0(0, &reg_id);
    }
#endif

    if (delay_time > vdh_time_out) {
        dprint(PRN_ERROR, "VDH delay_time(%d ms), start_time = %u, CurrTime = %u, MAX_TIME_OUT=%d ms\n",
               delay_time, dev->last_start_time, cur_time_in_us, vdh_time_out);
        dprint(PRN_ERROR, "Dec dps is %d, rev dps is %d\n", chan->dec_dps_num, chan->rcv_dps_num);
        vdec_record_chr_event(chan->chan_id, MT_VDEC_KEY_ERROR, MT_VDEC_VFMW_KEY_ERROR, MT_VDEC_VFMW_VDH_TIMEOUT);

        report_vdmerr(chan->chan_id);

        if (dev->ops->reset(dev) != DEC_OK) {
            dprint(PRN_ERROR, " %s %d reset VDH failed!\n", __func__, __LINE__);
            return;
        }

        dec_chan_release_task_seg(chan, task);
        dec_chan_update_pxp_task(chan);
        dec_chan_decrease_dec_task_num(chan);
        dec_chan_mark_display_state(chan, task, 0);
        dec_chan_clear_ref_info(chan, task);
        dec_chan_release_task_dps(chan, task);
        dec_dev_release_msg(dev->dev_id, task->msg_id);
        dec_dev_release_reg(dev->dev_id, reg_id);
    }
}

VFMW_ACC_BASE td_s32 dec_dev_time_out(td_u16 dev_id)
{
    td_s32 ret = DEC_ERR;
    dec_dev_info *dev = TD_NULL;
    dec_dev_vdh *vdh = TD_NULL;
    dec_chan *chan = TD_NULL;
    dec_chan_task *task = TD_NULL;
    td_u16 reg_id;
    td_s32 chan_id = 0;
    STATIC td_u32 run_time = 0;

    if (run_time % 10 != 0) { /* avoid run too frequency, run once when call 10times */
        run_time++;
        return DEC_OK;
    }
    run_time++;

    if (dec_dev_get_dev(dev_id, &dev) != DEC_OK) {
        dprint(PRN_ERROR, "%s GetDev return error!\n", __func__);
        return DEC_ERR;
    }

    /* Get Vdh */
    vdh = (dec_dev_vdh *)dev->handle;
    if (dev->handle == TD_NULL) {
        /* vdh not open */
        return DEC_OK;
    }

    /* Get Reg */
    reg_id = vdh->vdh_reg_pool.next_pxp_reg;

    /* Get Chan and task */
    if (dec_dev_get_chan_id(dev, reg_id, &chan_id) != DEC_OK) {
        return DEC_OK;
    }

    chan = dec_ctrl_get_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, DEC_ERR);

    if (dec_chan_check_chan_valid(chan) != DEC_OK) {
        dprint(PRN_ERROR, "%s Chan %d not valid\n", __func__, chan->chan_id);
        ret = DEC_ERR;
        goto exit;
    }

    if (chan->chan_task_pool.dec_task_num > 0) {
        task = &chan->chan_task_pool.task[chan->chan_task_pool.next_pxp_task];
    }

    if (task == TD_NULL) {
        ret = DEC_OK;
        goto exit;
    }

    if (task->state == DEC_TASK_DECODING) { /* if task is normal */
        dev_dev_time_out_process(chan, reg_id, dev, task);
    }

    ret = DEC_OK;

exit:
    return ret;
}

VFMW_ACC_BASE td_s32 dec_dev_start_reg(td_u16 dev_id, td_u16 reg_id)
{
    dec_dev_info *dev = TD_NULL;

    dec_dev_vdh *vdh = TD_NULL;
    dec_dev_mdma *mdma = TD_NULL;

    vfmw_check_ret(os_get_time_us != TD_NULL, DEC_ERR);
    if (dec_dev_get_dev(dev_id, &dev) != DEC_OK) {
        dprint(PRN_ERROR, "%s return error!\n", __func__);
        return DEC_ERR;
    }

#ifdef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
    os_flush_all_cache();
#endif
    dev->ops->start_reg(dev, reg_id);

    if (dev->type == DEC_DEV_TYPE_VDH) {
        vdh = (dec_dev_vdh *)dev->handle;
        vdh->reg_cfg_cnt++;
    } else {
        mdma = (dec_dev_mdma *)dev->handle;
        mdma->reg_cfg_cnt++;
    }

    dev->last_start_time = os_get_time_us();

    return DEC_OK;
}

td_s32 dec_dev_cancel_reg(td_u16 dev_id, td_u16 reg_id)
{
    dec_dev_info *dev = TD_NULL;
    td_s32 ret = DEC_ERR;

    if (dec_dev_get_dev(dev_id, &dev) == DEC_OK) {
        ret = dec_hal_cancel_dec(dev, reg_id);
    }
    return ret;
}

VFMW_ACC_BASE td_s32 dec_dev_calc_mem_param(td_u16 dev_id,
    dec_hal_ops *ops, td_void *cfg, td_void *task)
{
    td_s32 ret = DEC_ERR;
    dec_dev_info *dev = TD_NULL;

    if (dec_dev_get_dev(dev_id, &dev) == DEC_OK) {
        ret = dec_hal_calc_mem_param(dev, ops, cfg, task);
    }
    return ret;
}

td_s32 dec_dev_refresh_bin(td_void)
{
    td_u16 dev_id;
    dec_dev_info *dev = TD_NULL;

    for (dev_id = 0; dev_id < DEC_DEV_NUM; dev_id++) {
        if (dec_dev_get_dev(dev_id, &dev) != DEC_OK) {
            return DEC_ERR;
        }

        os_sema_down(dev->sema);
        dec_hal_control(dev_id, DHAL_CID_REFRESH_BIN, TD_NULL, TD_NULL);
        os_sema_up(dev->sema);
    }

    return DEC_OK;
}

td_s32 dec_dev_wait_hardware(td_void *chan, td_void *task)
{
    const td_u16 dev_id = 0;

    return dec_hal_control(dev_id, DHAL_CID_WAIT_DEC, chan, task);
}

td_void dec_dev_bypass_process(td_s32 dev_id)
{
    td_s32 chan_id = 0;
    td_u16 reg_id = 0;
    td_u32 keep_reg = 0;
    dec_dev_info *dev = TD_NULL;

    dec_dev_check_id(&dev_id);

    /* Get Dev */
    if (dec_dev_get_dev((td_u16)dev_id, &dev) != DEC_OK) {
        dprint(PRN_ERROR, "%s GetDev error\n", __func__);
        return;
    }

    if (dev->handle == TD_NULL) {
        dprint(PRN_ERROR, "%s dev->handle null\n", __func__);
        return;
    }

    /* Get Reg */
    if (dec_dev_get_cur_pxp_reg(dev, &reg_id) != DEC_OK) {
        dprint(PRN_ERROR, "%s dev %d type %d get_reg error\n", __func__, dev_id, dev->type);
        return;
    }

    /* Get Chan and task */
    if (dec_dev_get_chan_id(dev, reg_id, &chan_id) != DEC_OK) {
        dprint(PRN_ERROR, "%s dev %d get chan_id error\n", __func__, reg_id);
        goto out;
    }

    dec_dev_post_proc(dev, chan_id, &keep_reg);

out:
    if (keep_reg == 0) {
        dec_dev_update_pxp_reg(dev);
        dec_dev_release_reg((td_u16)dev_id, reg_id);
    }
}
