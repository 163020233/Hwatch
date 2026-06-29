/*
 * Copyright (c) 2019-2020 Huawei Technologies Co., Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdbool.h>
#include "lwip/netifapi.h"
#include "lwip/netif.h"
#include "lwip/etharp.h"
#include "lwip_public.h"
#include "lwip_private.h"

#include "sle_errcode.h"
#include "sle_common.h"
#include "sle_chba_manager.h"
#include "sle_device_discovery.h"
#include "securec.h"
#include "bs_device_mgr.h"
#include "ssf_init.h"
#include "ssf_config_stru.h"
#include "ssf_config.h"
#include "ssf_connection.h"
#include "sle_connection_manager.h"

#define SLE_CHBA_NETIF_NAME "sle"
#define MS_300                           300000
#define SLE_TRANSMITTER_ADDR        {0x12, 0x12, 0x12, 0x12, 0x12, 0x12}
#define SLE_RECEIVER_ADDR           {0xEE, 0xEF, 0x33, 0x9B, 0x8D, 0xBB}
#define SLE_RECEIVER_DEVICE_NAME    "sle_chba_video_central"
#define SLE_TRANSMITTER_DEVICE_NAME    "SLETransmiter"

#define INDEX_0 0
#define INDEX_4 4
#define INDEX_5 5

static struct netif g_sle_chba_localdev = {0};
static uint32_t g_rx_dropped_cnt = 0;
static uint8_t g_sle_trans_mode = SLE_NONE_MODE;
/* 扫描到的预期对端地址 */
static sle_addr_t g_remote_addr = {0};
static ip4_addr_t g_local_ip = {0};
static ip4_addr_t g_local_gw = {0};
static ip4_addr_t g_netmask = {0};

static uint8_t g_local_mac_addr[SLE_ADDR_LEN] = {0}; /* 6 : mac addr length */
static uint8_t g_remote_mac_addr[SLE_ADDR_LEN] = {0}; /* 6 : mac addr length */
static uint8_t g_is_mac_user_set = 0;
errcode_t sle_get_conn_id_by_dev_id(uint16_t dev_id, uint16_t *conn_id);

static errcode_t sle_chba_netdev_input_cb(uint8_t *data, uint16_t len)
{
    errcode_t rc = ERRCODE_SUCC;
    struct pbuf *p;
    struct netif *dev = sle_chba_netdev_get();

    /*
     * The packet has been retrieved from the transmission
     * medium. Build an skb around it, so upper layers can handle it
     */
#if defined(ETH_PAD_SIZE) && ETH_PAD_SIZE
    p = pbuf_alloc(PBUF_RAW, len + ETH_PAD_SIZE, PBUF_RAM);
#else
    p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);
#endif
    if (p == NULL) {
        printf("chba rx: low on mem - packet dropped\n");
        g_rx_dropped_cnt++;
        return ERRCODE_MALLOC;
    }
    /*
     * 在以太网报头之前添加的字节数，以确保该报头之后的有效负载对齐。
     * 由于报头长度为14字节，如果没有此填充，IP报头中的地址将不会在32位边界上对齐。
     * 因此，将其设置为2可以加快32位平台的速度。
     */
#if defined(ETH_PAD_SIZE) && ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE);
#endif
    rc = pbuf_take(p, data, len);
    if (rc != ERRCODE_SUCC) {
        printf("pbuf_take fail(%d)!!!\r\n", rc);
        g_rx_dropped_cnt++;
        pbuf_free(p);
        return ERRCODE_MEMCPY;
    }
#if defined(ETH_PAD_SIZE) && ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE);
#endif
#if defined(SUPPORT_SLE_BRIDGE)
    if (lwip_net_bridge_netif_input(p, &g_sle_chba_localdev)) {
        return ERRCODE_SUCC;
    }
#endif
    /* full packet send to tcpip_thread to process */
    if (dev->input != NULL) {
        dev->input(p, dev);
    }
    return ERRCODE_SUCC;
}

err_t sle_chba_send_pkt(struct netif *dev, struct pbuf *pbuf)
{
    (void)(dev);
    err_t ret;

#if ETH_PAD_SIZE
    (void)pbuf_header(pbuf, -ETH_PAD_SIZE); /* drop the padding word */
#endif
    uint8_t *payload = NULL;
    struct pbuf *p_clone = NULL;
    if (pbuf->tot_len > pbuf->len && pbuf->next != NULL) {
        p_clone = pbuf_clone(PBUF_RAW, PBUF_RAM, pbuf); /* packet queue copy to one packet */
        if (p_clone == NULL) {
            return ERR_MEM;
        }
        payload = p_clone->payload;
    } else {
        payload = pbuf->payload;
    }
    ret = sle_chba_netdev_driver_send(payload, pbuf->tot_len);
    if (p_clone != NULL) {
        pbuf_free(p_clone);
    }
#if ETH_PAD_SIZE
    (void)pbuf_header(pbuf, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
    return ret;
}

struct netif *sle_chba_netdev_get(void)
{
    return &g_sle_chba_localdev;
}

uint8_t *sle_get_remote_addr(void)
{
    return g_remote_addr.addr;
}

static errcode_t sle_chba_netdev_wake_queue_cb(void)
{
    return ERRCODE_SUCC;
}

static errcode_t sle_chba_netdev_stop_queue_cb(void)
{
    return ERRCODE_SUCC;
}

static errcode_t sle_chba_netdev_set_link_up_cb(void)
{
    netifapi_netif_set_link_up(&g_sle_chba_localdev);
    printf("%s\n", __FUNCTION__);
    return ERRCODE_SUCC;
}

static errcode_t sle_chba_netdev_set_link_down_cb(void)
{
    netifapi_netif_set_link_down(&g_sle_chba_localdev);
    printf("%s\n", __FUNCTION__);
    return ERRCODE_SUCC;
}

static errcode_t lwip_sle_chba_register_callbacks(void)
{
    sle_chba_netdev_callbacks_t cbk = { .stop_queue_cb = sle_chba_netdev_stop_queue_cb,
                                        .wake_queue_cb = sle_chba_netdev_wake_queue_cb,
                                        .set_link_up_cb = sle_chba_netdev_set_link_up_cb,
                                        .set_link_down_cb = sle_chba_netdev_set_link_down_cb,
                                        .netdev_input_cb = sle_chba_netdev_input_cb,
                                        };
    return sle_chba_netdev_register_callbacks(&cbk);
}

static err_t sle_chba_init_fn(struct netif *netif_p)
{
    if (netif_p == NULL) {
        return ERR_ARG;
    }
    netif_p->output = etharp_output;
    netif_p->linkoutput = sle_chba_send_pkt;

    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif_p->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;
    (void)sprintf_s(netif_p->name, sizeof(netif_p->name), "%d", LWIP_NETIF_SLE);

    if (netif_default == NULL) {
        netif_set_default(netif_p);
    }

    netifapi_netif_set_up(netif_p);
    netifapi_netif_set_link_down(netif_p);
    /* maximum transfer unit */
    netif_p->mtu = IP_FRAG_MAX_MTU;
    return ERR_OK;
}

static errcode_t chba_adapter_netdev_init(void)
{
    sle_addr_t addr = { 0 };
    errcode_t ret = sle_get_local_addr(&addr);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    (void)memset_s(&g_sle_chba_localdev, sizeof(struct netif), 0, sizeof(struct netif));
    g_sle_chba_localdev.hwaddr_len       = SLE_ADDR_LEN;
    (void)memcpy_s(g_sle_chba_localdev.hwaddr, SLE_ADDR_LEN, addr.addr, SLE_ADDR_LEN);
    g_sle_chba_localdev.hwaddr[0] = g_sle_chba_localdev.hwaddr[0] & 0xFE;

    errcode_t rc = netifapi_netif_add(&g_sle_chba_localdev, &g_local_ip, &g_netmask, &g_local_gw, NULL, sle_chba_init_fn, tcpip_input);
    if (rc != ERRCODE_SUCC) {
        printf("%s: create netif fail err(%d)!", __func__, rc);
        return ERRCODE_FAIL;
    }

    lwip_sle_chba_register_callbacks();
#if defined(SUPPORT_SLE_BRIDGE)
    lwip_set_bridge_netif(&g_sle_chba_localdev);
#endif
    return ERRCODE_SUCC;
}

errcode_t chba_adapter_netdev_deinit(void)
{
    netifapi_netif_remove(&g_sle_chba_localdev);
#if defined(SUPPORT_SLE_BRIDGE)
    lwip_set_bridge_netif(NULL);
#endif
    g_sle_chba_localdev.linkoutput         = NULL;
    return ERRCODE_SUCC;
}

static void sle_trans_discovery_state_cbk(uint8_t type, uint8_t state)
{
    printf("discovery state, type:0x%02x, state:0x%02x.\r\n", type, state);
    if (type == SLE_DISCOVERY_CENTRAL && state == BS_SLE_COMMON_STATE_DISABLE) {
        usleep(MS_300);
        bs_sle_start_pair(&g_remote_addr);
    }
}

static void sle_trans_discovery_result_info_cbk(const sle_addr_t *addr,
    bs_sle_discovery_result_t *discovery_result, uint8_t *data, uint16_t data_len)
{
    (void)data;
    (void)data_len;
    uint8_t transmiiter_addr[SLE_ADDR_LEN] = SLE_TRANSMITTER_ADDR;
    if (g_is_mac_user_set) {
        memcpy_s(transmiiter_addr, SLE_ADDR_LEN, g_remote_mac_addr, SLE_ADDR_LEN);
    }
    printf(
        "find device, addr:%02x:**:**:**:%02x:%02x\r\n", addr->addr[INDEX_0], addr->addr[INDEX_4], addr->addr[INDEX_5]);
    if (g_sle_trans_mode == SLE_RECEIVER_MODE && !memcmp(transmiiter_addr, addr->addr, (sizeof(transmiiter_addr) / sizeof(transmiiter_addr[0])))) {
        printf("find transmitter device.\r\n");
        memcpy_s(&g_remote_addr, sizeof(sle_addr_t), addr, sizeof(sle_addr_t));
        bs_sle_stop_discovery(SLE_DISCOVERY_CENTRAL);
    }
}

static void sle_trans_connect_state_changed_cbk(bs_sle_connection_result_t *result)
{
    errcode_sle_t ret = ERRCODE_SLE_SUCCESS;
    printf("connect state cbk device_id:0x%02x, conn_state:0x%x, encrypt_state:0x%x, reason:0x%x, \
        role:0x%x\r\n", result->dev_id, result->conn_state, result->encrypt_state, result->reason, result->local_role);
    if (result->conn_state == SLE_CONNECT_CONNECTED && result->encrypt_state == SLE_ENCRYPT_SUCCESS) {
        uint16_t conn_id = 0;
        sle_get_conn_id_by_dev_id(result->dev_id, &conn_id);
        ret = sle_chba_netdev_add_link(conn_id, (const sle_addr_t *)&result->addr);
        if (g_sle_trans_mode == SLE_TRANSMITTER_MODE) {
            bs_sle_stop_discovery(SLE_DISCOVERY_PERIPHERAL);
        }
        printf("chba add link, ret: %x\r\n", ret);
    } else if (result->conn_state == SLE_CONNECT_DISCONNECTED) {
        printf("SLE disconnect. reason=0x%x\n", result->reason);
        sle_chba_netdev_del_link(result->dev_id, (const sle_addr_t *)&result->addr);
        if (result->reason == SLE_DISCONNECT_BY_MIC_ERROR) {
            sle_remove_paired_remote_device((const sle_addr_t *)&result->addr);
        }
        if (result->reason != SLE_DISCONNECT_BY_LOCAL_HOST) {
            if (g_sle_trans_mode == SLE_RECEIVER_MODE) {
                bs_sle_start_discovery(SLE_DISCOVERY_CENTRAL);
            } else {
                bs_sle_start_discovery(SLE_DISCOVERY_PERIPHERAL);
            }
        }
    }
}

static void sle_trans_pair_complete_cbk(uint16_t device_id, const sle_addr_t *addr,
    const bs_sle_pair_info_t *pair_info)
{
    printf("pair complete device_id:%02x, addr:%02x:**:**:**:%02x:%02x\r\n",
        device_id, addr->addr[INDEX_0], addr->addr[INDEX_4], addr->addr[INDEX_5]);
}

static errcode_t sle_status_cbk_register(void)
{
    bs_sle_callbacks_t cbk = {
        .pair_request_cb = NULL,
        .connect_state_cb = sle_trans_connect_state_changed_cbk,
        .member_connect_state_cb = NULL,
        .pair_complete_cb = sle_trans_pair_complete_cbk,
        .pair_remove_cb = NULL,
        .discovery_result_cb = sle_trans_discovery_result_info_cbk,
        .discovery_state_cb = sle_trans_discovery_state_cbk,
        .auto_conn_state_cb = NULL,
    };

    return bs_sle_register_callbacks(0, &cbk);
}

static void sle_enable_status_cbk(uint8_t status)
{
    printf("enable_cbk status = %x.\n", status);
    errcode_t ret = ERRCODE_SLE_SUCCESS;
    if (status != SLE_STATE_ENABLED) {
        return;
    }
    ret |= sle_status_cbk_register();
    ret |= sle_chba_netdev_create(CHBA_ROLE_STA, 0);
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("sle_chba_netdev_create fail 0x%x.\n", ret);
        return;
    }
    ret |= chba_adapter_netdev_init();
    if (ret != ERRCODE_SLE_SUCCESS) {
        return;
    }
    if (g_sle_trans_mode == SLE_RECEIVER_MODE) {
        bs_sle_start_discovery(SLE_DISCOVERY_CENTRAL);
    } else {
        bs_sle_start_discovery(SLE_DISCOVERY_PERIPHERAL);
    }
}

void sle_chba_set_mac_addr(uint8_t *local_addr, uint8_t local_size, uint8_t *remote_addr, uint8_t remote_size)
{
    if (local_size != SLE_ADDR_LEN || remote_size != SLE_ADDR_LEN) {
        printf("mac addr length error\n");
        return;
    }
    if (memcpy_s(g_local_mac_addr, sizeof(g_local_mac_addr), local_addr, local_size) != EOK) {
        return;
    }
    if (memcpy_s(g_remote_mac_addr, sizeof(g_remote_mac_addr), remote_addr, remote_size) != EOK) {
        return;
    }
    printf("input local addr:%02x:**:**:**:%02x:%02x,remote addr:%02x:**:**:**:%02x:%02x\r\n",
        g_local_mac_addr[INDEX_0], g_local_mac_addr[INDEX_4], g_local_mac_addr[INDEX_5],
        g_remote_mac_addr[INDEX_0], g_remote_mac_addr[INDEX_4], g_remote_mac_addr[INDEX_5]);
    g_is_mac_user_set = 1;
}

errcode_t sle_chba_init(uint8_t trans_mode, ip4_addr_t local_ip, ip4_addr_t gw, ip4_addr_t netmask)
{
    errcode_t ret = ERRCODE_SUCC;
    char *dev_name = NULL;
    uint8_t role;
    uint8_t *local_addr = NULL;
    uint8_t max_size = 8;
    uint8_t receive_addr[SLE_ADDR_LEN] = SLE_RECEIVER_ADDR;
    uint8_t transmitter_addr[SLE_ADDR_LEN] = SLE_TRANSMITTER_ADDR;
    if (trans_mode >= SLE_NONE_MODE) {
        return ERRCODE_SLE_PARAM_ERR;
    }
    g_sle_trans_mode = trans_mode;
    (void)memcpy_s(&g_local_ip, sizeof(ip4_addr_t), &local_ip, sizeof(ip4_addr_t));
    (void)memcpy_s(&g_local_gw, sizeof(ip4_addr_t), &gw, sizeof(ip4_addr_t));
    (void)memcpy_s(&g_netmask, sizeof(ip4_addr_t), &netmask, sizeof(ip4_addr_t));

    sle_service_init();
    if (trans_mode == SLE_TRANSMITTER_MODE) {
        dev_name = SLE_TRANSMITTER_DEVICE_NAME;
        local_addr = transmitter_addr;
        role = SLE_CONFIG_ROLE_PERIPHERAL;
    } else {
        dev_name = SLE_RECEIVER_DEVICE_NAME;
        local_addr = receive_addr;
        role = SLE_CONFIG_ROLE_CENTRAL;
    }

    if (g_is_mac_user_set) {
        local_addr = g_local_mac_addr;
    }
    bs_sle_stack_state_callbacks_t cbk = {
        .stack_state_cbk = sle_enable_status_cbk,
    };
    bs_sle_register_stack_state_callbacks(0, &cbk);

    if (trans_mode == SLE_RECEIVER_MODE) {
        uint32_t appearance = SLE_CONFIG_ACTION_CAMERA;
        ret |= bs_config(SLE_CONFIG_ADV_APPEARANCE, &appearance, sizeof(uint32_t));
        ret |= bs_config(SLE_CONFIG_SCAN_MAX_CACHE_RESULT, &max_size, sizeof(uint8_t));
    }
    ret |= bs_config(SLE_CONFIG_COMMON_ROLE, &role, sizeof(uint8_t));
    ret |= bs_config(SLE_CONFIG_ADV_SHORT_NAME, dev_name, sizeof(dev_name));
    ret |= bs_config(SLE_CONFIG_COMMON_LOCAL_ADDR, local_addr, SLE_ADDR_LEN);
    ret |= bs_sle_enable();

    return ret;
}

errcode_t sle_chba_deinit(void)
{
    sle_chba_netdev_destroy();
    g_sle_trans_mode = SLE_NONE_MODE;
    errcode_t ret = bs_sle_disable();
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("disable sle fail!\n");
    }
    return ret;
}