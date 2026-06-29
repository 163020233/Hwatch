/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: diag channel
 * This file should be changed only infrequently and with great care.
 */

#include "diag_channel_item.h"
#include "securec.h"
#include "diag_mem.h"
#include "diag_adapt_layer.h"
#include "errcode.h"

typedef struct {
    diag_channel_item_t item[DIAG_SUPPORT_CHANNEL_CNT];
} diag_channel_ctrl_t;

STATIC diag_channel_ctrl_t g_diag_channel_ctrl = { 0 };

STATIC diag_channel_ctrl_t *diag_get_channel_ctrl(void)
{
    return &g_diag_channel_ctrl;
}

diag_channel_item_t *diag_chan_idx_2_item(diag_channel_id_t id)
{
    diag_channel_ctrl_t *chan_ctrl = diag_get_channel_ctrl();
    if (id >= DIAG_SUPPORT_CHANNEL_CNT) {
        return NULL;
    }
    return &chan_ctrl->item[id];
}

errcode_t uapi_diag_channel_set_notify_hook(diag_channel_id_t id, diag_channel_notify_hook hook)
{
    diag_channel_item_t *item = diag_chan_idx_2_item(id);
    if (item == NULL) {
        return ERRCODE_FAIL;
    }

    item->notify_hook = hook;
    return ERRCODE_SUCC;
}

errcode_t uapi_diag_channel_set_tx_hook(diag_channel_id_t id, diag_channel_tx_hook hook)
{
    diag_channel_item_t *item = diag_chan_idx_2_item(id);
    if (item == NULL) {
        return ERRCODE_FAIL;
    }

    item->tx_hook = hook;
    return ERRCODE_SUCC;
}

errcode_t uapi_diag_channel_set_connect_hso_addr(diag_channel_id_t id, uint8_t hso_addr)
{
    diag_channel_item_t *item = diag_chan_idx_2_item(id);
    if (item == NULL) {
        return ERRCODE_FAIL;
    }
    item->hso_addr = hso_addr;
    return ERRCODE_SUCC;
}

diag_channel_item_t *diag_dst_2_chan(uint8_t addr)
{
    diag_channel_id_t channel_id = diag_adapt_addr_2_channel_id(addr);
    diag_channel_item_t *item = diag_chan_idx_2_item(channel_id);
    return item;
}

errcode_t uapi_diag_channel_init(diag_channel_id_t id, uint32_t attribute)
{
    diag_channel_item_t *item = diag_chan_idx_2_item(id);
    if (item == NULL || item->init == true) {
        return ERRCODE_FAIL;
    }

    if ((attribute & DIAG_CHANNEL_ATTR_RX_BUF_MASK) == DIAG_CHANNEL_ATTR_NEED_FIXED_RX_BUF) {
        item->rx_buf_pos = 0;
        item->rx_buf_is_using = false;
        item->rx_buf_need_free = false;
    } else if ((attribute & DIAG_CHANNEL_ATTR_RX_BUF_MASK) == DIAG_CHANNEL_ATTR_NEED_RX_BUF) {
        item->rx_buf_len = CONFIG_DIAG_RX_BUF_SIZE;
        item->rx_buf_pos = 0;
        item->rx_buf_is_using = false;
        item->rx_buf_need_free = true;
        /* 按默认大小分配rx_buf */
        item->rx_buf = dfx_malloc(0, item->rx_buf_len);
        if (item->rx_buf == NULL) {
            return ERRCODE_MALLOC;
        }
    }

    item->id = (uint8_t)id;
    item->mfs = DIAG_SER_DATA_MFS_DEFAULT;
    item->max_serv_pkg_len = DIAG_SER_MAX_DATA_LEN_DEFAULT;
    item->init = true;
    return ERRCODE_SUCC;
}

errcode_t uapi_diag_channel_set_rx_buf(diag_channel_id_t id, uint8_t *buf, uint32_t buf_len)
{
    diag_channel_item_t *item = diag_chan_idx_2_item(id);
    if (item == NULL || item->init == false) {
        return ERRCODE_FAIL;
    }

    if (item->rx_buf_need_free == false) {
        item->rx_buf = buf;
        item->rx_buf_len = buf_len;
    } else if (item->rx_buf != NULL && item->rx_buf_len != buf_len) {
        /* 通过这个接口，修改rx_buf大小，重新分配rx_buf */
        dfx_free(0, item->rx_buf);
        item->rx_buf_len = buf_len;
        item->rx_buf = dfx_malloc(0, item->rx_buf_len);
        if (item->rx_buf == NULL) {
            return ERRCODE_MALLOC;
        }
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_diag_channel_deinit(diag_channel_id_t id)
{
    diag_channel_item_t *item = diag_chan_idx_2_item(id);
    if (item == NULL || item->init == false) {
        return ERRCODE_FAIL;
    }

    if (item->rx_buf != NULL && item->rx_buf_need_free) {
        dfx_free(0, item->rx_buf);
    }
    memset_s(item, sizeof(diag_channel_item_t), 0, sizeof(diag_channel_item_t));
    return ERRCODE_SUCC;
}

errcode_t uapi_diag_channel_set_mfs(diag_channel_id_t id, uint16_t mfs, uint32_t max_serv_pkg_len)
{
    diag_channel_item_t *item = diag_chan_idx_2_item(id);
    if (item == NULL) {
        return ERRCODE_FAIL;
    }

    item->mfs = mfs;
    item->max_serv_pkg_len = max_serv_pkg_len;
    return ERRCODE_SUCC;
}
