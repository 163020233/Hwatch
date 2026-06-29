/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "usrdata.h"
#include "vfmw_define.h"

#ifdef VFMW_CHAN_NUM
#define USD_CHAN_NUM VFMW_CHAN_NUM
#else
#define USD_CHAN_NUM 1
#endif

#if defined(CFG_MAX_USRDAT_BLK_NUM)
#define MAX_USRDAT_BLK_NUM CFG_MAX_USRDAT_BLK_NUM
#else
#define MAX_USRDAT_BLK_NUM (48 * 4)
#endif

typedef struct {
    td_u32 valid_num;
    vfmw_usrdat *usrdata[MAX_USRDAT_BLK_NUM];
} usd_chan;

typedef struct {
    os_sema sema;
    td_u32 ctx_size;
    usd_chan *chan[USD_CHAN_NUM];
} usd_entry;

static usd_entry g_usd_entry;

usd_entry *usd_get_entry(td_void)
{
    return &g_usd_entry;
}
td_void usd_lock(td_void)
{
    os_sema_down(usd_get_entry()->sema);
}

td_void usd_unlock(td_void)
{
    os_sema_up(usd_get_entry()->sema);
}

usd_chan *usd_get_chan(td_s32 chan_id)
{
    if (chan_id < 0 || chan_id >= USD_CHAN_NUM) {
        return TD_NULL;
    }

    return usd_get_entry()->chan[chan_id];
}

td_s32 usd_get_mem_need_size(vfmw_svr_mem_need *mem)
{
    td_u32 size;

    vfmw_check_ret(mem != TD_NULL, USD_ERR);

    size = vfmw_align_up(sizeof(vfmw_usrdat), 4) * 16; /* 4 :a number 16 :a number */
    size = vfmw_align_up(size, VFMW_CTX_ALIGN);
    size = size * ((MAX_USRDAT_BLK_NUM + 15) / 16); /* 15 :a number 16 :a number */
    mem->chan_cache_size += (td_s32)size;
    mem->chan_cache_size += vfmw_align_up(sizeof(usd_chan), VFMW_CTX_ALIGN);

    return USD_OK;
}

td_s32 usd_open(td_void)
{
    usd_entry *entry = TD_NULL;

    entry = usd_get_entry();
    os_sema_init(&entry->sema);

    vfmw_check_sec_func(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));

    return USD_OK;
}

td_s32 usd_close(td_void)
{
    usd_entry *entry = TD_NULL;

    entry = usd_get_entry();

    vfmw_check_sec_func(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));

    os_sema_exit(entry->sema);

    return USD_OK;
}

td_s32 usd_create(td_s32 chan_id)
{
    td_s32 ret = USD_ERR;
    td_u32 size;
    td_u8 *vir = TD_NULL;
    usd_chan *chan = TD_NULL;
    td_s32 i;
    mem_record mem = { 0 };

    size = sizeof(usd_chan);

    mem.mem_name = "usd_chan";
    mem.length = vfmw_align_up(size, VFMW_CTX_ALIGN);
    mem.is_cached = 0;
    mem.type = CACHE_MEM_TYPE;
    mem.unid = (td_u32)chan_id;
    os_malloc(&mem); /* USD_Inst */
    vir = mem.vir_addr;
    if (vir == TD_NULL) {
        dprint(PRN_ERROR, "UserData AllocMem(%d) Error\n", size);
        return USD_ERR;
    }

    vfmw_check_sec_func(memset_s(vir, size, 0, size));

    usd_lock();

    chan = usd_get_chan(chan_id);
    if (chan == TD_NULL) {
        chan = (usd_chan *)vir;
        for (i = 0; i < MAX_USRDAT_BLK_NUM; i++) {
            chan->valid_num = 0;
            chan->usrdata[i] = TD_NULL;
        }

        usd_get_entry()->chan[chan_id] = chan;
        usd_get_entry()->ctx_size += size;

        ret = USD_OK;
    }

    usd_unlock();

    return ret;
}

td_s32 usd_destroy(td_s32 chan_id)
{
    td_s32 i;
    td_void *vir = TD_NULL;
    usd_chan *chan = TD_NULL;
    mem_record mem = { 0 };

    usd_lock();

    chan = usd_get_chan(chan_id);
    if (chan == TD_NULL) {
        usd_unlock();
        return USD_ERR;
    }

    for (i = 0; i < MAX_USRDAT_BLK_NUM; i++) {
        if (chan->usrdata[i] != TD_NULL && chan->usrdata[i]->is_mem_start == 1) {
            if (vir != TD_NULL) {
                mem.vir_addr = (td_u8 *)(vir);
                mem.is_cached = 0;
                os_free(&mem);
            }
            vir = chan->usrdata[i];
        }
    }

    if (vir != TD_NULL) {
        mem.vir_addr = (td_u8 *)(vir);
        mem.is_cached = 0;
        os_free(&mem);
    }

    mem.vir_addr = (td_u8 *)(chan);
    mem.is_cached = 0;
    os_free(&mem);

    usd_get_entry()->chan[chan_id] = TD_NULL;
    usd_get_entry()->ctx_size -= sizeof(usd_chan);
    usd_unlock();

    return USD_OK;
}

td_s32 usd_reset(td_s32 chan_id)
{
    td_s32 i;
    usd_chan *chan = TD_NULL;

    chan = usd_get_entry()->chan[chan_id];
    if (chan != TD_NULL) {
        for (i = 0; i < MAX_USRDAT_BLK_NUM; i++) {
            if (chan->usrdata[i] != TD_NULL) {
                chan->usrdata[i]->from = VFMW_USD_INVALID;
                chan->usrdata[i]->dnr_used_flag = 0;
            }
        }
    }

    return USD_OK;
}

td_void usd_init_node(vfmw_usrdat *usrdata)
{
    vfmw_check(usrdata != TD_NULL);

    usrdata->dnr_used_flag = 0;
    usrdata->pts = 0;
    usrdata->data_size = 0;
    usrdata->seq_cnt = usrdata->seq_img_cnt = 0;
    usrdata->is_registered = 0;
    usrdata->itu_t_t35_country_code = 0;
    usrdata->itu_t_t35_country_code_extension_byte = 0;
    usrdata->itu_t_t35_provider_code = 0;
}

td_void usd_get_usrdata(usd_chan *chan, td_s32 blk_num, td_s32 blk_size,
    const td_u8 *vir, const mem_record *mem)
{
    td_s32 idx, i;

    vfmw_check(chan != TD_NULL);
    vfmw_check(vir != TD_NULL);
    vfmw_check(mem != TD_NULL);

    idx = (td_s32)chan->valid_num;
    for (i = 0; i < blk_num; i++, idx++) {
        if (idx >= MAX_USRDAT_BLK_NUM) {
            break;
        }
        chan->usrdata[idx] = (vfmw_usrdat *)(vir + i * blk_size);
        chan->usrdata[idx]->is_mem_start = 0;
        chan->usrdata[idx]->from = VFMW_USD_INVALID;
        chan->usrdata[idx]->dnr_used_flag = VFMW_USD_INVALID;
        chan->usrdata[idx]->dma_buf = mem->dma_buf;
        chan->usrdata[idx]->offset = (td_u64)(i * blk_size);
    }
}

vfmw_usrdat *usd_get(td_s32 chan_id)
{
    td_u32 i, blk_num, blk_size;
    td_u8 *vir = TD_NULL;
    usd_chan *chan = usd_get_entry()->chan[chan_id];
    vfmw_usrdat *usrdata = TD_NULL;
    mem_record mem = { 0 };

    vfmw_check_ret(chan != TD_NULL, TD_NULL);

    for (i = 0; i < MAX_USRDAT_BLK_NUM; i++) {
        if (chan->usrdata[i] != TD_NULL &&
            chan->usrdata[i]->from == VFMW_USD_INVALID &&
            chan->usrdata[i]->dnr_used_flag == 0) {
            usrdata = chan->usrdata[i];
            usd_init_node(usrdata);
            return usrdata;
        }
    }

    vfmw_check_ret(chan->valid_num < MAX_USRDAT_BLK_NUM, TD_NULL);

    blk_num = 16; /* 16 is blk num */
    blk_size = vfmw_align_up(sizeof(vfmw_usrdat), 4); /* 4 :a number */
    if (blk_num > MAX_USRDAT_BLK_NUM - chan->valid_num) {
        blk_num = MAX_USRDAT_BLK_NUM - chan->valid_num;
    }

    mem.mem_name = "usd_blk";
    mem.length = vfmw_align_up(blk_num * blk_size, VFMW_CTX_ALIGN);
    mem.is_cached = 0;
    mem.type = CACHE_MEM_TYPE;
    mem.unid = (td_u32)chan_id;
    os_malloc(&mem); /* USD_BLK */
    vir = mem.vir_addr;
    vfmw_check_ret(vir != TD_NULL, TD_NULL);
    vfmw_check_sec_func(memset_s(vir, blk_num * blk_size, 0, blk_num * blk_size));

    usd_get_usrdata(chan, blk_num, blk_size, vir, &mem);

    usrdata = chan->usrdata[chan->valid_num];
    usrdata->is_mem_start = 1;

    usd_init_node(usrdata);

    chan->valid_num += blk_num;

    return usrdata;
}

td_void usd_release(td_s32 chan_id, td_void *usrdata)
{
    vfmw_unused(chan_id);

    if (usrdata != TD_NULL) {
        ((vfmw_usrdat *)usrdata)->from = VFMW_USD_INVALID;
    }

    return;
}
