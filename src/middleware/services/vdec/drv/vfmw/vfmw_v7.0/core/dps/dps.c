/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "dps.h"
#include "dps_ops.h"
#include "stm_intf.h"
#include "syn_intf.h"
#include "dec_intf.h"
#include "vfmw_pdt.h"
#include "vfmw_define.h"
#include "vdec_chr.h"

#define DPS_NAME_LEN  50

static dps_entry_info g_dps_entry = {
    .reserve1 = 0x1234abcd,
    .ref_cnt = 0,
    .state = DPS_MOD_NULL,
    .no_dps = 0,
    .ctx_size = sizeof(dps_entry_info),
    .reserve2 = 0x1234abcd,
};

/* inner func */
dps_entry_info *dps_get_entry(td_void)
{
    return &g_dps_entry;
}

dps_chan_info *dps_get_chan(td_s32 chan_id)
{
    if (chan_id >= 0 && chan_id < DPS_CHAN_NUM) {
        return g_dps_entry.chan[chan_id];
    } else {
        return TD_NULL;
    }
}

td_void dps_sema_down(td_void)
{
    os_sema_down(dps_get_entry()->sema);
}

td_void dps_sema_up(td_void)
{
    os_sema_up(dps_get_entry()->sema);
}

STATIC td_void dps_lock(td_ulong *lock_flag)
{
    os_spin_lock(dps_get_entry()->lock, lock_flag);
}

STATIC td_void dps_unlock(td_ulong *lock_flag)
{
    os_spin_unlock(dps_get_entry()->lock, lock_flag);
}

td_s32 dps_get_mem_need_size(vfmw_svr_mem_need *mem)
{
    td_u32 chan_size;
    td_s32 param_size;

    chan_size = (td_u32)sizeof(dps_chan_info);
    chan_size = vfmw_align_up(chan_size, VFMW_CTX_ALIGN);

    param_size = dps_get_max_param_size();

    vfmw_check_ret(mem != TD_NULL, DPS_ERR);

    mem->chan_cache_size += chan_size + (td_u32)param_size;

    return DPS_OK;
}

/* extern func */
td_s32 dps_open(td_void)
{
    dps_entry_info *entry = TD_NULL;

    entry = dps_get_entry();
    if (entry->state != DPS_MOD_NULL) {
        dprint(PRN_FATAL, "%s mod_state error \n", __func__);
        return DPS_ERR;
    }

    os_sema_init(&entry->sema);
    os_spin_lock_init(&entry->lock);

    entry->state = DPS_MOD_OPEN;
    vfmw_check_sec_func(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));

    return DPS_OK;
}

td_s32 dps_close(td_void)
{
    dps_entry_info *entry = TD_NULL;

    entry = dps_get_entry();
    if (entry->state == DPS_MOD_NULL ||
        entry->state == DPS_MOD_INIT) {
        dprint(PRN_FATAL, "%s mod_state error \n", __func__);
        return DPS_ERR;
    }

    entry->state = DPS_MOD_NULL;
    entry->ctx_size -= (td_u32)sizeof(dps_entry_info);
    vfmw_check_sec_func(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));

    os_sema_exit(entry->sema);
    os_spin_lock_exit(entry->lock);

    return DPS_OK;
}

td_s32 dps_init(td_void)
{
    td_s32 ret;
    dps_entry_info *entry = TD_NULL;

    dps_sema_down();

    entry = dps_get_entry();
    if (entry->state == DPS_MOD_NULL ||
        entry->state == DPS_MOD_INIT) {
        ret = DPS_ERR;
        dprint(PRN_FATAL, "%s mod_state error \n", __func__);
        goto out;
    }

    entry->ref_cnt++;
    entry->state = DPS_MOD_INIT;
    vfmw_check_sec_func(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));
    ret = DPS_OK;

out:

    dps_sema_up();

    return ret;
}

td_s32 dps_exit(td_void)
{
    td_s32 ret;
    dps_entry_info *entry = TD_NULL;

    dps_sema_down();

    entry = dps_get_entry();
    if (entry->state != DPS_MOD_INIT) {
        dprint(PRN_FATAL, "%s mod_state error \n", __func__);
        ret = DPS_ERR;
        goto out;
    }

    entry->ref_cnt--;
    entry->state = DPS_MOD_EXIT;
    vfmw_check_sec_func(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));
    ret = DPS_OK;

out:

    dps_sema_up();

    return ret;
}

td_void *dps_alloc_mem(td_s32 chan_id, td_s32 size)
{
    td_s32 cnt;
    td_void *vir = TD_NULL;
    dps_chan_info *chan = TD_NULL;
    dps_entry_info *entry = TD_NULL;
    mem_record mem = { 0 };
#ifdef VFMW_CHECK_MEM_SUPPORT
    td_s32 ret;
    td_char name[DPS_NAME_LEN];
#else
    td_char *name = TD_NULL;
#endif

    entry = dps_get_entry();
    chan = dps_get_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, TD_NULL);

    for (cnt = 0; cnt < DPS_MEM_NUM; cnt++) {
        if (chan->mem.addr[cnt] == TD_NULL) {
            break;
        }
    }

    if (cnt >= DPS_MEM_NUM) {
        dprint(PRN_ERROR, "%s No mem Node\n", __func__);
        return TD_NULL;
    }
#ifdef VFMW_CHECK_MEM_SUPPORT
    ret = snprintf_s(name, DPS_NAME_LEN, DPS_NAME_LEN - 1, "dps_mem_%d", cnt);
    vfmw_check_ret(ret >= 0, TD_NULL);
#else
    name = "dps_mem";
#endif
    mem.mem_name = name;
    mem.length = vfmw_align_up((td_u32)size, VFMW_CTX_ALIGN);
    mem.is_cached = 0;
    mem.type = CACHE_MEM_TYPE;
    mem.unid = (td_u32)chan_id;
    vfmw_check_ret(os_malloc != TD_NULL, TD_NULL);
    os_malloc(&mem); /* "DPS" */

    vir = (td_void *)mem.vir_addr;
    vfmw_check_ret(vir != TD_NULL, TD_NULL);

    vfmw_check_sec_func(memset_s(vir, (td_u32)size, 0, (td_u32)size));
    chan->mem.addr[cnt] = vir;
    chan->mem.len[cnt] = size;
    entry->ctx_size += (td_u32)size;

    return vir;
}

STATIC td_s32 dps_free_mem(td_s32 chan_id)
{
    td_s32 cnt;
    td_void *vir = TD_NULL;
    dps_chan_info *chan = TD_NULL;
    dps_entry_info *entry = TD_NULL;
    mem_record mem = { 0 };

    entry = dps_get_entry();
    chan = dps_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "%s chan = TD_NULL\n", __func__);
        return DPS_ERR;
    }

    for (cnt = 0; cnt < DPS_MEM_NUM; cnt++) {
        vir = chan->mem.addr[cnt];
        if (vir != TD_NULL) {
            mem.vir_addr = vir;
            mem.is_cached = 0;
            os_free(&mem);
            entry->ctx_size -= (td_u32)chan->mem.len[cnt];
        }
    }

    vfmw_check_sec_func(memset_s(&chan->mem, sizeof(dps_mem), 0, sizeof(dps_mem)));

    return DPS_OK;
}

td_s32 dps_create(td_s32 chan_id)
{
    td_s32 ret;
    td_s32 size;
    td_void *vir = TD_NULL;
    dps_chan_info *chan = TD_NULL;
    dps_entry_info *entry = TD_NULL;
    mem_record mem = { 0 };

    size = (td_s32)sizeof(dps_chan_info);
    mem.mem_name = "dps_chan";
    mem.length = vfmw_align_up((td_u32)size, VFMW_CTX_ALIGN);
    mem.is_cached = 0;
    mem.type = CACHE_MEM_TYPE;
    mem.unid = (td_u32)chan_id;
    vfmw_check_ret(os_malloc != TD_NULL, DPS_ERR);
    os_malloc(&mem); /* "DPS_Inst" */
    vir = (td_void *)mem.vir_addr;
    vfmw_check_ret(vir != TD_NULL, DPS_ERR);

    vfmw_check_sec_func(memset_s(vir, (td_u32)size, 0, (td_u32)size));

    dps_sema_down();

    entry = dps_get_entry();
    if (entry->state != DPS_MOD_INIT) {
        ret = DPS_ERR;
        dprint(PRN_FATAL, "%s mod_state error \n", __func__);
        goto out;
    }

    if (chan_id < 0 ||
        chan_id >= DPS_CHAN_NUM ||
        entry->chan[chan_id] != TD_NULL) {
        ret = DPS_ERR;
        dprint(PRN_ERROR, "%s chan_id error \n", __func__);
        goto out;
    }

    chan = (dps_chan_info *)vir;
    entry->chan[chan_id] = chan;
    entry->ctx_size += (td_u32)size;

    vfmw_check_ret(os_get_unid != TD_NULL, DEC_ERR);
    chan->unid = (td_u32)os_get_unid();
    chan->chan_id = chan_id;
    chan->handle = chan;
    chan->state = DPS_CHAN_OPEN;
    ret = DPS_OK;

out:

    dps_sema_up();

    if (ret != DPS_OK) {
        os_free(&mem);
    }
    vdec_record_chr_event(chan_id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_CREATE_DPS_OK);

    return ret;
}

td_s32 dps_destroy(td_s32 chan_id)
{
    td_s32 ret;
    dps_entry_info *entry = TD_NULL;
    mem_record mem = { 0 };

    dps_sema_down();

    entry = dps_get_entry();
    if (entry->state != DPS_MOD_INIT) {
        dprint(PRN_FATAL, "%s mod_state error \n", __func__);
        ret = DPS_ERR;
        goto out;
    }

    if (chan_id < 0 ||
        chan_id >= DPS_CHAN_NUM ||
        entry->chan[chan_id] == TD_NULL) {
        dprint(PRN_FATAL, "%s chan_id error \n", __func__);
        ret = DPS_ERR;
        goto out;
    }

    ret = dps_free_mem(chan_id);
    if (ret != DPS_OK) {
        dprint(PRN_FATAL, "%s dps_free_mem error\n", __func__);
    }

    mem.is_cached = 0;
    mem.vir_addr = (td_u8 *)(entry->chan[chan_id]);
    os_free(&mem);

    entry->chan[chan_id] = TD_NULL;
    entry->ctx_size -= (td_u32)sizeof(dps_chan_info);
    ret = DPS_OK;

out:

    dps_sema_up();

    return ret;
}

td_s32 dps_reset(td_s32 chan_id)
{
    td_s32 ret;
    td_s32 cnt;
    dps_chan_info *chan = TD_NULL;
    dps_entry_info *entry = TD_NULL;

    dps_sema_down();

    entry = dps_get_entry();
    if (entry->state != DPS_MOD_INIT) {
        dprint(PRN_FATAL, "%s mod_state error \n", __func__);
        ret = DPS_ERR;
        goto out;
    }

    chan = dps_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_FATAL, "%s chan = TD_NULL\n", __func__);
        ret = DPS_ERR;
        goto out;
    }

    chan->tail = 0;
    chan->head = 0;
    chan->make_num = 0;
    chan->recv_try = 0;
    chan->recv_num = 0;
    chan->rels_num = 0;

    for (cnt = 0; cnt < MAX_DPS_NUM; cnt++) {
        chan->dps[cnt].state = DPS_IDLE;
        chan->dps[cnt].slice_num = 0;
        chan->dps[cnt].slice_head = TD_NULL;
        chan->dps[cnt].slice_tail = TD_NULL;
    }

    ret = DPS_OK;

out:

    dps_sema_up();
    vdec_record_chr_event(chan_id, MT_VDEC_RESET, MT_VDEC_VFMW_RESET, MT_VFMW_DPS_RESET_OK);

    return ret;
}

td_s32 dps_config(td_s32 chan_id, const td_void *args)
{
    td_s32 ret = DPS_ERR;
    dps_cfg_info *cfg = TD_NULL;
    dps_chan_info *chan = TD_NULL;
    dps_ops *ops = TD_NULL;
    td_s32 max_slc_num;
    td_s32 dps_num = 0;

    if (args == TD_NULL) {
        dprint(PRN_FATAL, "%s args = TD_NULL\n", __func__);
        return DPS_ERR;
    }

    dps_sema_down();

    cfg = (dps_cfg_info *)args;
    chan = dps_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_FATAL, "%s chan = TD_NULL\n", __func__);
        goto out;
    }

    ops = dps_get_ops(cfg->vid_std);
    if (ops == TD_NULL) {
        dprint(PRN_FATAL, "%s std %d fail\n", __func__, cfg->vid_std);
        goto out;
    }

    max_slc_num = cfg->max_slc_num;
    if (max_slc_num == 0 && ops->get_max_slice_num) {
        max_slc_num = ops->get_max_slice_num();
    }

#ifdef VFMW_LOCAL_SUPPORT
    if (chan->cfg.vid_std != cfg->vid_std || chan->cfg.max_slc_num != max_slc_num) {
        ret = dps_free_mem(chan_id);
        if (ret == DPS_ERR) {
            dprint(PRN_FATAL, "%s dps_free_mem err\n", __func__);
        }
        vfmw_check_sec_func(memset_s(chan->dps, sizeof(chan->dps), 0, sizeof(chan->dps)));
    }
#endif

    chan->ops = ops;
    chan->cfg.vid_std = cfg->vid_std;
    chan->cfg.max_slc_num = max_slc_num;
    chan->cfg.alloc_type = cfg->alloc_type;
    chan->cfg.adpt_type = cfg->adpt_type;

    pdt_get_dps_num(cfg->vid_std, &dps_num);
    chan->cfg.max_num = dps_num;

    ret = DPS_OK;

out:

    dps_sema_up();

    return ret;
}

STATIC td_bool dps_is_resize(td_s32 chan_id)
{
    td_u32 i;
    dps_chan_info *chan = dps_get_chan(chan_id);

    vfmw_check_ret(chan != TD_NULL, TD_FALSE);

    for (i = 0; i < MAX_DPS_NUM; i++) {
        if (chan->dps[i].frm_width == 0 ||
            chan->dps[i].frm_height == 0 ||
            chan->dps[i].bit_depth == 0) {
            continue;
        }

        if (chan->dps[i].frm_width  != chan->dps[0].frm_width ||
            chan->dps[i].frm_height != chan->dps[0].frm_height ||
            chan->dps[i].bit_depth  != chan->dps[0].bit_depth) {
            return TD_TRUE;
        }
    }

    return TD_FALSE;
}

VFMW_ACC_FRAME STATIC td_void dps_init_node(dps_chan_info *chan, td_s32 id)
{
    dps_info *dps = &chan->dps[id];

    if (dps->sed_state == SED_IDLE) {
        dps->slice_num = 0;
        dps->slice_head = TD_NULL;
        dps->slice_tail = TD_NULL;
    }

    dps->dps_id = id;
    dps->state = DPS_MAKE;
    dps->ldy_mode = LDY_IDLE;
    dps->make_num = 0;
    dps->is_frm_cpy = 0;
    dps->mtdt_size = 0;
    dps->bit_depth = 8; /* 8 is bit depth */
    dps->pic_struct = 0;
    dps->ctb_size = 16; /* 16 is ctb size */
    dps->ops = chan->ops;
}

VFMW_ACC_FRAME td_s32 dps_get_node(td_s32 chan_id, dps_info **dps)
{
    td_s32 ret = DPS_ERR;
    td_s32 id;
    td_ulong lock_flag;
    dps_chan_info *chan = TD_NULL;
    td_bool resize, usable_dps;

    dps_lock(&lock_flag);

    chan = dps_get_chan(chan_id);
    if (chan == TD_NULL || chan->ops == TD_NULL) {
        dprint(PRN_ERROR, "%s chan = TD_NULL or chan->ops == TD_NULL\n", __func__);
        goto out;
    }

    if (dps_get_entry()->ser_mode != 0) {
        for (id = 0; id < MAX_DPS_NUM; id++) {
            if (chan->dps[id].state != DPS_IDLE) {
                goto out;
            }
        }
    }

    if (dps_get_entry()->dps_num > 0 &&
        dps_get_entry()->dps_num <= (td_s32)chan->make_num) {
        goto out;
    }

    id = chan->tail;

    resize = dps_is_resize(chan_id);
    /* when ext buf: none dps is decoding,and none dps is waiting */
    usable_dps = chan->make_num > chan->recv_num || chan->recv_num > chan->rels_num;
    if (chan->cfg.alloc_type == VFMW_FRM_ALLOC_OUTER && resize && usable_dps) {
        goto out;
    }

    if (chan->dps[id].state == DPS_IDLE) {
        dps_init_node(chan, id);

        if (dps == TD_NULL) {
            dprint(PRN_ERROR, "%s dps is null\n", __func__);
            dps_unlock(&lock_flag);
            return DPS_ERR;
        }
        (*dps) = &chan->dps[id];
        ret = DPS_OK;
    }

out:

    dps_unlock(&lock_flag);

    return ret;
}

VFMW_ACC_FRAME td_s32 dps_reset_node(td_s32 chan_id, dps_info *dps)
{
    vfmw_unused(chan_id);

    if (dps == TD_NULL) {
        dprint(PRN_ERROR, "%s dps = TD_NULL\n", __func__);
        return DPS_ERR;
    }

    dps->slice_num = 0;
    dps->slice_head = TD_NULL;
    dps->slice_tail = TD_NULL;

    return DPS_OK;
}

VFMW_ACC_FRAME td_s32 dps_insert(td_s32 chan_id, dps_info *dps)
{
    td_s32 ret = DPS_ERR;
    td_ulong lock_flag;
    dps_chan_info *chan = TD_NULL;

    if (dps == TD_NULL) {
        dprint(PRN_ERROR, "%s dps = TD_NULL\n", __func__);
        return DPS_ERR;
    }
    vfmw_check_ret(os_get_time_us != TD_NULL, DPS_ERR);

    dps_lock(&lock_flag);

    if (dps->ops == TD_NULL ||
        dps->state != DPS_MAKE ||
        dps->pic_param == TD_NULL) {
        dprint(PRN_ERROR, "%s state error\n", __func__);
        goto out;
    }

    chan = dps_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "%s chan = TD_NULL\n", __func__);
        goto out;
    }

    if (chan->dps + chan->tail == dps) {
        if (dps->sed_state != SED_TRY) {
            chan->tail += 1;
            chan->tail %= chan->cfg.max_num;
        } else {
            chan->sed_try++;
        }

        if (dcheck(PRN_TIME)) {
            td_s32 cid = chan_id;
            td_u32 fid = chan->make_num;
            dbg_add_time(cid, fid, TPOS_QUE_DPS, os_get_time_us());
        }

        dps->state = DPS_WAIT;
        dps->unid = (td_s32)(chan->make_num++);
        ret = DPS_OK;
    }

out:
    dps_unlock(&lock_flag);

    dec_wakeup(chan_id);

    if (ret != DPS_OK) {
        dprint(PRN_ERROR, "dps_insert Fail\n");
    }

    return ret;
}

VFMW_ACC_FRAME td_s32 dps_receive(td_s32 chan_id, dps_info **dps)
{
    td_s32 ret = DPS_ERR;
    td_s32 id;
    td_ulong lock_flag;
    dps_entry_info *entry = TD_NULL;
    dps_chan_info *chan = TD_NULL;
    vfmw_check_ret(os_get_time_us != TD_NULL, DPS_ERR);

    dps_lock(&lock_flag);

    chan = dps_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "%s chan = TD_NULL\n", __func__);
        goto out;
    }

    chan->recv_try++;
    entry = dps_get_entry();
    vfmw_check_goto(entry->no_dps == 0, out);
    id = chan->head;
    if (chan->dps[id].state == DPS_WAIT) {
        if (chan->dps[id].sed_state != SED_TRY) {
            chan->head += 1;
            chan->head %= chan->cfg.max_num;
        }

        chan->dps[id].state = DPS_DEC;
        chan->recv_num++;
        if (dps == TD_NULL) {
            dprint(PRN_ERROR, "%s dps is null\n", __func__);
            dps_unlock(&lock_flag);
            return DPS_ERR;
        }

        if (dcheck(PRN_TIME)) {
            td_s32 cid = chan_id;
            td_u32 fid = (td_u32)chan->dps[id].unid;
            dbg_add_time(cid, fid, TPOS_DEQ_DPS, os_get_time_us());
        }

        *dps = &(chan->dps[id]);
        ret = DPS_OK;
    }

out:

    dps_unlock(&lock_flag);

    return ret;
}

VFMW_ACC_FRAME td_s32 dps_release(td_s32 chan_id, const dps_info *dps)
{
    td_s32 ret = DPS_ERR;
    td_s32 cnt;
    td_ulong lock_flag;
    dps_chan_info *chan = TD_NULL;

    if (dps == TD_NULL) {
        dprint(PRN_ERROR, "%s dps = TD_NULL\n", __func__);
        return DPS_ERR;
    }

    syn_wake_up();

    dps_lock(&lock_flag);

    if (dps->ops == TD_NULL ||
        dps->state == DPS_IDLE) {
        dprint(PRN_ERROR, "%s state error\n", __func__);
        goto out;
    }

    chan = dps_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "%s chan = TD_NULL\n", __func__);
        goto out;
    }

    for (cnt = 0; cnt < chan->cfg.max_num; cnt++) {
        if (&(chan->dps[cnt]) == dps) {
            if (chan->dps[cnt].sed_state == SED_SUCC) {
                chan->sed_succ++;
            }

            chan->dps[cnt].state = DPS_IDLE;
            chan->rels_num++;
            ret = DPS_OK;
            break;
        }
    }

out:

    dps_unlock(&lock_flag);

    if (ret != DPS_OK) {
        dprint(PRN_ERROR, "DPS chan %d dps_release Fail\n", chan_id);
    }

    return ret;
}

td_s32 dps_have_idle(td_s32 chan_id)
{
    td_s32 ret = DPS_ERR;
    td_s32 cnt;
    td_ulong lock_flag;
    dps_chan_info *chan = TD_NULL;

    dps_lock(&lock_flag);

    chan = dps_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "%s chan = TD_NULL\n", __func__);
        goto out;
    }

    for (cnt = 0; cnt < chan->cfg.max_num; cnt++) {
        if (chan->dps[cnt].state == DPS_IDLE) {
            ret = DPS_OK;
            break;
        }
    }

out:

    dps_unlock(&lock_flag);

    return ret;
}

STATIC td_bool dps_have_make(td_s32 chan_id)
{
    td_s32 cnt;
    dps_chan_info *chan = TD_NULL;

    chan = dps_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "%s chan = TD_NULL\n", __func__);
        return TD_FALSE;
    }

    for (cnt = 0; cnt < chan->cfg.max_num; cnt++) {
        if (chan->dps[cnt].state == DPS_MAKE) {
            return TD_TRUE;
        }
    }

    return TD_FALSE;
}

td_s32 dps_have_wait(td_s32 chan_id)
{
    td_s32 ret = DPS_ERR;
    td_s32 cnt;
    td_ulong lock_flag;
    dps_chan_info *chan = TD_NULL;

    dps_lock(&lock_flag);

    chan = dps_get_chan(chan_id);
    if (chan == TD_NULL) {
        goto out;
    }

    for (cnt = 0; cnt < chan->cfg.max_num; cnt++) {
        if (chan->dps[cnt].state == DPS_WAIT) {
            ret = DPS_OK;
            break;
        }
    }

out:

    dps_unlock(&lock_flag);

    return ret;
}

VFMW_ACC_FRAME td_s32 dps_set_pic_param(td_s32 chan_id, td_void *ctx, dps_info *dps)
{
    td_s32 ret;
    dps_ops *ops = TD_NULL;
    dps_chan_info *chan = TD_NULL;

    if (ctx == TD_NULL ||
        dps == TD_NULL ||
        dps->ops == TD_NULL ||
        dps->ops->make_dps_pic == TD_NULL) {
        dprint(PRN_ERROR, "%s args = TD_NULL\n", __func__);
        return DPS_ERR;
    }

    chan = dps_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "%s chan = TD_NULL\n", __func__);
        return DPS_ERR;
    }

    ops = dps->ops;
    if (dps->pic_param == TD_NULL) {
        dps->pic_param = dps_alloc_mem(chan_id, ops->get_pic_param_size());
        if (dps->pic_param == TD_NULL) {
            dprint(PRN_ERROR, "%s alloc_mem fail\n", __func__);
            return DPS_ERR;
        }
    }

    ret = ops->make_dps_pic(ctx, dps->pic_param);

    return ret;
}

VFMW_ACC_SLICE STATIC td_s32 dps_prepare_pic_slice_mem(td_s32 chan_id, dps_info *dps)
{
    td_s32 pic_size, slc_size, slc_num, mem_size, cnt;
    const td_s32 blk_size = 8192; /* 8192 is block size */
    td_u8 *vir = TD_NULL;

    pic_size = 0;
    slc_num = dps->ops->get_max_slice_num();
    slc_size = dps->ops->get_slice_param_size();
    vfmw_check_ret((td_s64)slc_size * slc_num < VFMW_MAX_SINT32, DPS_ERR);

    /* pic + slc to save virmem */
    if (dps->slice_num == 0 && dps->pic_param == TD_NULL) {
        pic_size = dps->ops->get_pic_param_size();
    }

    if (pic_size + slc_size * slc_num > blk_size) {
        slc_num = dps->slice_num == 0 ? 4 : slc_num / 8; /* 4 is slc num, 8 is times */
    }

    mem_size = pic_size + slc_size * slc_num;
    mem_size = (td_s32)vfmw_align_up((td_u32)mem_size, (td_u32)blk_size);
    slc_num = (mem_size - pic_size) / slc_size;
    if (dps->slice_num + slc_num >= DPS_SLC_NUM) {
        slc_num = DPS_SLC_NUM - dps->slice_num;
    }

    mem_size = pic_size + slc_size * slc_num;
    vir = (td_u8 *)dps_alloc_mem(chan_id, mem_size);
    if (vir == TD_NULL) {
        dprint(PRN_ERROR, "%s alloc_mem fail\n", __func__);
        return DPS_ERR;
    }

    if (pic_size > 0) {
        dps->pic_param = (td_void *)vir;
        vir += pic_size;
    }

    for (cnt = 0; cnt < slc_num; cnt++) {
        dps->slice_list[dps->slice_num + cnt] = vir + slc_size * cnt;
    }

    return DPS_OK;
}

VFMW_ACC_SLICE td_s32 dps_set_slice_param(td_s32 chan_id, td_void *ctx, dps_info *dps)
{
    td_void *vir = TD_NULL;
    dps_chan_info *chan = TD_NULL;

    if (ctx == TD_NULL ||
        dps == TD_NULL ||
        dps->ops == TD_NULL ||
        dps->ops->make_dps_slice == TD_NULL) {
        return DPS_ERR;
    }

    chan = dps_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "%s chan = TD_NULL\n", __func__);
        return DPS_ERR;
    }

    if (dps->slice_num >= DPS_SLC_NUM) {
        dprint(PRN_ERROR, "slice_num is over %d\n", DPS_SLC_NUM);
        return DPS_ERR;
    }

    if (dps->slice_list[dps->slice_num] == TD_NULL) {
        if (dps_prepare_pic_slice_mem(chan_id, dps) != DPS_OK) {
            dprint(PRN_ERROR, "prepare pic slice memory fail!\n");
            return DPS_ERR;
        }
    }

    vir = dps->slice_list[dps->slice_num];
    dps->ops->make_dps_slice(ctx, vir, dps->slice_tail);

    dps->slice_num++;
    if (dps->slice_head == TD_NULL) {
        dps->slice_head = vir;
    }
    dps->slice_tail = vir;

    return DPS_OK;
}

/* if sedonly try fail then delete half slice */
td_s32 dps_delete_slice(td_s32 chan_id, td_void *ctx, dps_info *dps)
{
    vfmw_unused(chan_id);
    vfmw_unused(ctx);

    if (dps == TD_NULL) {
        return DPS_ERR;
    }

    if (dps->slice_num <= 0 ||
        dps->slice_num > DPS_SLC_NUM) {
        return DPS_ERR;
    }

    dps->slice_num--;
    if (dps->slice_num == 0) {
        dps->slice_head = TD_NULL;
        dps->slice_tail = TD_NULL;
    } else {
        dps->slice_tail = dps->slice_list[dps->slice_num - 1];
    }

    return DPS_OK;
}

VFMW_ACC_FRAME td_s32 dps_check_valid(td_s32 chan_id, const dps_info *dps)
{
    vfmw_unused(chan_id);

    if (dps == TD_NULL || dps->ops == TD_NULL) {
        dprint(PRN_ERROR, "DPS is TD_NULL!\n");
        return DPS_ERR;
    }

    if (dps->pic_param == TD_NULL) {
        dprint(PRN_ERROR, "pic_param of DPS is TD_NULL!\n");
        return DPS_ERR;
    }

    if (dps->slice_head == TD_NULL &&
        dps->is_frm_cpy == 0 &&
        dps->ops->make_dps_slice) {
        dprint(PRN_ERROR, "head_slice of DPS is TD_NULL!\n");
        return DPS_ERR;
    }

    if (dps->slice_tail == TD_NULL &&
        dps->is_frm_cpy == 0 &&
        dps->ops->make_dps_slice) {
        dprint(PRN_ERROR, "tail_slice of DPS is TD_NULL!\n");
        return DPS_ERR;
    }

    return DPS_OK;
}

VFMW_ACC_BASE td_s32 dps_release_seg(td_s32 chan_id, const dps_info *dps)
{
    dps_chan_info *chan = TD_NULL;

    if (dps == TD_NULL ||
        dps->ops == TD_NULL) {
        return DPS_ERR;
    }

    chan = dps_get_chan(chan_id);
    if (chan == TD_NULL) {
        return DPS_ERR;
    }

    stm_wake_up(TD_NULL);

    dps->ops->release_seg(chan_id, dps);

    return DPS_OK;
}

VFMW_ACC_BASE td_s32 dps_get_fsp_logic_id(td_s32 chan_id, const dps_info *dps)
{
    vfmw_unused(chan_id);

    if (dps == TD_NULL ||
        dps->ops == TD_NULL ||
        dps->pic_param == TD_NULL) {
        return -1;
    }

    return dps->logic_id;
}

td_s32 dps_get_pic_struct(td_s32 chan_id, const dps_info *dps)
{
    vfmw_unused(chan_id);

    if (dps == TD_NULL ||
        dps->ops == TD_NULL ||
        dps->pic_param == TD_NULL) {
        return 0;
    }

    return dps->pic_struct;
}

VFMW_ACC_FRAME td_s32 dps_get_ref_info(td_s32 chan_id, dps_info *dps, dps_ref_info *dps_ref)
{
    vfmw_unused(chan_id);

    if (dps == TD_NULL ||
        dps->ops == TD_NULL ||
        dps->pic_param == TD_NULL) {
        return DPS_ERR;
    }

    dps->ops->get_ref_info(dps->pic_param, dps_ref);

    return DPS_OK;
}

td_s32 dps_get_unfinish_dps_num(td_s32 chan_id)
{
    td_s32 dps_num = -1;
    td_u32 i;
    dps_chan_info *chan = dps_get_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, dps_num);
    dps_num = 0;
    for (i = 0; i < MAX_DPS_NUM; i++) {
        dps_num += (chan->dps[i].state == DPS_WAIT || chan->dps[i].state == DPS_DEC) ? 1 : 0;
    }
    return dps_num;
}

td_s32 dps_get_info(td_s32 chan_id, const td_void *args)
{
    td_s32 ret = DPS_ERR;
    td_ulong lock_flag;
    dps_chan_info *chan = TD_NULL;
    dps_fault_check_info *info = (dps_fault_check_info *)args;
    vfmw_check_ret(info != TD_NULL, ret);

    dps_lock(&lock_flag);

    chan = dps_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "%s chan = TD_NULL\n", __func__);
        ret = DPS_ERR;
        goto out;
    }

    info->has_node_make = dps_have_make(chan_id);

    ret = DPS_OK;

out:
    dps_unlock(&lock_flag);

    return ret;
}
