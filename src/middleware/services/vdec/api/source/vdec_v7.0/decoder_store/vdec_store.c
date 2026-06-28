/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#include "vdec_store.h"
#include "mpi_vdec_sdk.h"
#include "vdec_hal.h"
#include "vdec_decoder_hw.h"
#ifdef EXT_VDEC_MJPEG_SUPPORT
#include "vdec_decoder_mjpeg.h"
#endif

typedef enum {
    VDEC_DECODER_HW = 0,
    VDEC_DECODER_MJPEG,
    VDEC_DECODER_MAX
} vdec_decoder_index;

typedef struct {
    td_bool inited;
    vdec_decoder* decoder_slot[VDEC_DECODER_MAX];
} vdec_store_entry;

static vdec_store_entry g_store_entry = {
    .inited = TD_FALSE,
    .decoder_slot = {TD_NULL, TD_NULL},
};

STATIC vdec_store_entry* vdec_store_get_entry(td_void)
{
    return &g_store_entry;
}

STATIC vdec_decoder* vdec_store_get_decoder(vdec_decoder_index idx)
{
    vdec_store_entry *store = vdec_store_get_entry();

    return store->decoder_slot[idx];
}

STATIC td_s32 vdec_store_create_chan(td_s32 *chan_id)
{
    return vdec_hal_create(chan_id);
}

STATIC td_s32 vdec_store_destroy_chan(td_s32 chan_id)
{
    return vdec_hal_destroy(chan_id);
}

STATIC td_bool vdec_store_is_decoder_valid(const vdec_decoder *decoder)
{
    td_bool ret;
    td_bool result;

    const vdec_decoder_intf *intf = &(decoder->intf);

    result = intf->create != TD_NULL && intf->destroy != TD_NULL &&
        intf->start != TD_NULL && intf->stop != TD_NULL &&
        intf->reset != TD_NULL && intf->flush != TD_NULL &&
        intf->decode != TD_NULL && intf->get_attr != TD_NULL &&
        intf->set_attr != TD_NULL && intf->register_event != TD_NULL &&
        intf->listen_event != TD_NULL && intf->queue_stream != TD_NULL &&
        intf->dequeue_stream != TD_NULL && intf->acq_frame != TD_NULL &&
        intf->rls_frame != TD_NULL && intf->get_param != TD_NULL &&
        intf->set_param != TD_NULL && intf->get_feature != TD_NULL &&
        intf->get_status != TD_NULL && intf->std_support != TD_NULL &&
        intf->get_cap != TD_NULL && intf->send_eos != TD_NULL;

    ret = result ? TD_TRUE : TD_FALSE;

    return ret;
}

STATIC td_s32 vdec_store_init(vdec_store_entry *store)
{
    if (store->inited == TD_TRUE) {
        return EXT_SUCCESS;
    }

    store->decoder_slot[VDEC_DECODER_HW] = vdec_load_decoder_hardware();
    if (vdec_store_is_decoder_valid(store->decoder_slot[VDEC_DECODER_HW]) != TD_TRUE) {
        vdec_print_alert("store load hardware decoder failed!\n");
        return EXT_FAILURE;
    }

#ifdef EXT_VDEC_MJPEG_SUPPORT
    store->decoder_slot[VDEC_DECODER_MJPEG] = vdec_load_decoder_mjpeg();
    if (vdec_store_is_decoder_valid(store->decoder_slot[VDEC_DECODER_MJPEG]) != TD_TRUE) {
        vdec_print_alert("store load mjpeg decoder failed!\n");
        return EXT_FAILURE;
    }
#endif
    store->inited = TD_TRUE;

    return EXT_SUCCESS;
}

td_s32 vdec_store_open(td_s32* chan_id)
{
    td_s32 ret;
    vdec_store_entry *store = vdec_store_get_entry();

    ret = vdec_store_init(store);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("store init fail!\n");
        return EXT_FAILURE;
    }

    ret = vdec_store_create_chan(chan_id);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("store create chan fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}


td_s32 vdec_store_close(td_s32 chan_id)
{
    td_s32 ret;

    ret = vdec_store_destroy_chan(chan_id);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("store destroy chan fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_store_register_decoder(const td_char *lib, td_u32 len)
{
    vdec_mpi_unused(lib);
    vdec_mpi_unused(len);
    return 0;
}

td_s32 vdec_store_unregister_decoder(const td_char *lib, td_u32 len)
{
    vdec_mpi_unused(lib);
    vdec_mpi_unused(len);
    return 0;
}

td_s32 vdec_store_query_decoder(ext_vdec_standard std_type, vdec_decoder **decoder)
{
    td_s32 ret;
    td_s32 idx;
    vdec_decoder *tmp_decoder = TD_NULL;
    vdec_store_entry *store = vdec_store_get_entry();
    ext_vdec_opt_is_std_support support = {0};

    vdec_check_ret(decoder != TD_NULL);
    (*decoder) = TD_NULL;

    if (store->inited == TD_FALSE) {
        vdec_print_err("store is not inited!\n");
        return EXT_FAILURE;
    }

    for (idx = VDEC_DECODER_HW; idx < VDEC_DECODER_MAX; idx++) {
        tmp_decoder = vdec_store_get_decoder(idx);
        if (tmp_decoder != TD_NULL) {
            support.user_dec = (idx == VDEC_DECODER_MJPEG) ? TD_TRUE : TD_FALSE;
            support.standard = std_type;
            ret = tmp_decoder->intf.std_support(&support);
            if (ret == EXT_SUCCESS && support.support == TD_TRUE) {
                break;
            }
        }
    }

    if (idx >= VDEC_DECODER_MAX) {
        vdec_print_alert("query decoder for std %s failed!\n", vdec_show_std(std_type));
        return EXT_FAILURE;
    }

    vdec_print_info("query decoder %s for std %s success!\n", tmp_decoder->name, vdec_show_std(std_type));

    (*decoder) = tmp_decoder;

    return EXT_SUCCESS;
}

td_void vdec_store_get_default_decoder(vdec_decoder **decoder)
{
    if (decoder == TD_NULL) {
        return;
    }

    (*decoder) = vdec_store_get_decoder(VDEC_DECODER_HW);

    return;
}

td_s32 vdec_store_get_cap(ext_vdec_capability *cap)
{
    td_s32 ret;
    td_u32 cnt;
    vdec_decoder *decoder = TD_NULL;
    vdec_store_entry *store = vdec_store_get_entry();

    ret = vdec_store_init(store);
    if (ret != EXT_SUCCESS) {
        vdec_print_err("store is not inited!\n");
        return EXT_FAILURE;
    }

    ret = vdec_hal_open();
    if (ret != EXT_SUCCESS) {
        vdec_print_err("hal open err!\n");
        return EXT_FAILURE;
    }

    for (cnt = 0; cnt < VDEC_DECODER_MAX; cnt++) {
        decoder = vdec_store_get_decoder(cnt);
        if (decoder != TD_NULL && vdec_store_is_decoder_valid(decoder) == TD_TRUE) {
            ret = decoder->intf.get_cap(cap);
            if (ret == EXT_SUCCESS) {
                break;
            }
        }
    }

    ret = vdec_hal_close();
    if (ret != EXT_SUCCESS) {
        vdec_print_err("hal close err!\n");
        return EXT_FAILURE;
    }

    if (cnt >= VDEC_DECODER_MAX) {
        vdec_print_err("%s err!\n", __func__);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}
