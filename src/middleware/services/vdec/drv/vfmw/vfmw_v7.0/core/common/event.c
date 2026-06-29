/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "event.h"
#include "vdec_chr.h"

#define EVENT_DEFAULT_MAP  (EVNT_ALLOC_MEM |  \
            EVNT_RELEASE_MEM | EVNT_RELEASE_BURST_MEM | EVNT_CANCEL_ALLOC | EVNT_SET_ADDRESS_IN)

typedef struct {
    td_u64 event_map;
} event_chan;

typedef struct {
    event_chan chan[VFMW_CHAN_NUM];
    fun_vfmw_event_report report;
} event_entry;

static event_entry g_event_enty = {{{0x0}}, TD_NULL};

static event_entry *event_get_entry(td_void)
{
    return &g_event_enty;
}

STATIC event_chan *event_get_chan(td_s32 chan_id)
{
    if (chan_id < 0 || chan_id >= VFMW_CHAN_NUM) {
        return TD_NULL;
    }
    return &g_event_enty.chan[chan_id];
}

td_s32 event_set_map(td_s32 chan_id, td_u64 event_map)
{
    event_chan *chan = TD_NULL;

    chan = event_get_chan(chan_id);
    if (chan == TD_NULL) {
        return EXT_FAILURE;
    }

    chan->event_map = event_map;
    vdec_record_chr_event(chan_id, MT_VDEC_REGISTER_EVENT, MT_VDEC_REGISTER_EVENT_OK, MT_VFMW_REGISTER_EVENT_OK);

    return EXT_SUCCESS;
}

td_void event_set_callback(fun_vfmw_event_report report)
{
    event_entry *entry = event_get_entry();

    entry->report = report;

    return;
}

td_void event_report(const td_s32 inst_id, td_s32 type, td_void *args, td_s32 len)
{
    event_chan *chan = TD_NULL;
    event_entry *entry = event_get_entry();
    td_u64 event_map = EVENT_DEFAULT_MAP;

    chan = event_get_chan(inst_id);
    if (chan == TD_NULL) {
        return;
    }

    event_map |= chan->event_map;
    if ((event_map & ((td_u32)type)) != 0 && entry->report != TD_NULL) {
        (td_void)(entry->report)(inst_id, type, args, len);
    }

    return;
}

td_bool event_check_typical_resolution(td_u32 width)
{
    if (width == 3840 || /* 3840 is width */
        width == 4096 || /* 4096 is width */
        width == 2160 || /* 2160 is width */
        width == 7680 || /* 7680 is width */
        width == 8192 || /* 8192 is width */
        width == 32 || /* 32 is width */
        width == 64 || /* 64 is width */
        width == 2304 || /* 2304 is width */
        width == 2560) { /* 2560 is width */
        return 1;
    }

    return 0;
}
