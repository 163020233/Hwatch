/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "dec_post.h"
#include "dec_chn.h"
#include "dec_dev.h"
#include "fsp.h"
#include "dbg.h"
#include "event.h"
#include "vfmw_define.h"
#include "vdec_chr.h"

STATIC td_u32 dec_post_calc_err_ratio(const dec_repair_param *repair_param, const dec_report_info *dec_report)
{
    td_s32 i;
    td_u32 err_ratio;
    td_u32 err_mbs;
    td_u32 total_mbs = 0;

    vfmw_check_ret(repair_param != TD_NULL, DEC_ERR);

    err_mbs = 0;
    if (repair_param->image_width * repair_param->image_height < VFMW_MAX_SINT32) {
        total_mbs = (td_u32)(repair_param->image_width * repair_param->image_height);
    } else {
        dprint(PRN_ERROR, "overflow error.\n");
    }

    if (dec_report->is_read_from_reg == TD_TRUE) {
        err_mbs = total_mbs - dec_report->correct_cnt;
    } else {
        for (i = 0; i < repair_param[0].valid_group_num; i++) {
            err_mbs += (repair_param[0].mb_group[i].end_mbn - repair_param[0].mb_group[i].start_mbn + 1);
        }

        for (i = 0; i < repair_param[1].valid_group_num; i++) {
            err_mbs += (repair_param[1].mb_group[i].end_mbn - repair_param[1].mb_group[i].start_mbn + 1);
        }
    }

    if (total_mbs == 0) {
        err_ratio = 0;
    } else {
        err_ratio = vfmw_min(100, (td_u32)(((err_mbs * 100) / total_mbs))); /* 100 :a number 100 :a number */
    }

    if (err_mbs > 2 && err_ratio == 0) { /* 2 :a number */
        err_ratio = 1;
    }

    if (err_ratio == 0 && repair_param->err_flag == 1) {
        err_ratio = 1;
    }

    return err_ratio;
}

td_s32 dec_post_make_report(td_void *chan_arg, td_void *task_arg)
{
    dec_dev_info *dev = TD_NULL;
    dec_chan *chan = TD_NULL;
    dec_dev_vdh *vdh = TD_NULL;
    dec_chan_task *task = (dec_chan_task *)task_arg;
    dec_repair_param *rep_param = TD_NULL;
    td_u32 err_ratio = 0;
    td_s32 ret;

    vfmw_check_ret(task_arg != TD_NULL, DEC_ERR);
    vfmw_check_ret(task != TD_NULL, DEC_ERR);
    vfmw_check_ret(task->dps != TD_NULL, DEC_ERR);

    chan = (dec_chan *)chan_arg;
    ret = dec_dev_get_dev(task->dev_id, &dev);
    vfmw_check_ret(ret == DEC_OK, DEC_ERR);
    dec_hal_read_dec_reg(dev, task->reg_id);
    dec_hal_read_frm_msg(dev, chan_arg, task_arg);

    if (task->dps->sed_state == SED_TRY) {
        vdh = (dec_dev_vdh *)dev->handle;
        task->dps->sed_ctb_num = (td_s32)dev->back_up.ctb_num;
        if (dev->back_up.err_flag != 0) {
            task->dps->sed_ctb_num = 0;
        }
        return DEC_OK;
    }

    if (dev->back_up.rd_slc_msg == 1 &&
        task->dps->is_frm_cpy == 0 &&
        chan->dec_ops->get_rep_param != TD_NULL) {
        rep_param = task->rep_param;
        rep_param[0].valid_group_num = 0;
        rep_param[1].valid_group_num = 0;
        vdh = (dec_dev_vdh *)dev->handle;

        dec_hal_read_slc_msg(dev, task->reg_id, &task->slice_list);
        rep_param->err_flag = dev->back_up.err_flag;
        chan->dec_ops->get_rep_param(rep_param, task, &vdh->dec_report);
        err_ratio = dec_post_calc_err_ratio(rep_param, &vdh->dec_report);
    }

    if (err_ratio != 0) {
        dprint(PRN_ERROR, "Chan %d Pic %d err_ratio %d\n", chan->chan_id, task->dps->unid, err_ratio);
        report_err_frame(chan->chan_id, (td_s8)task->dps->pic_coding_type, (td_s8)err_ratio);
        chan->err_frame_num++;
    }

    task->err_ratio = (td_u16)err_ratio;

    return DEC_OK;
}

td_u32 dec_post_record_reg_info(td_void *chan_arg, td_void *task_arg)
{
    dec_dev_info *dev = TD_NULL;
    dec_chan *chan = (dec_chan *)chan_arg;
    dec_chan_task *task = (dec_chan_task *)task_arg;

    vfmw_check_ret(task_arg != TD_NULL, DEC_ERR);
    vfmw_unused(chan);

    if (dec_dev_get_dev(task->dev_id, &dev) != DEC_OK) {
        dprint(PRN_FATAL, "dec_dev_get_dev return error! dev_id %d\n", task->dev_id);

        return DEC_ERR;
    }

    return DEC_OK;
}
