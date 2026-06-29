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

#ifndef INCLUDE_LWIP_PUBLIC_H
#define INCLUDE_LWIP_PUBLIC_H

#include <stdint.h>
#ifdef CHBA_SUPPORT
#include "errcode.h"
#include "lwip/ip_addr.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_IPADDR_LEN   46

typedef enum {
    /* 4g Cat1 netif */
    LWIP_NETIF_VOLTE = 0,
    /* bluetooth BT PAN netif */
    LWIP_NETIF_BT_PAN = 1,
    /* bluetooth SLE netif */
    LWIP_NETIF_SLE = 2,
    /* usb netif */
    LWIP_NETIF_USB = 3,
    /* NULL netif */
    LWIP_NETIF_NONE
} lwip_netif_type;

typedef enum {
    CONNECTION_CONNECTING = 0,
    CONNECTION_CONNECTED = 1,

    CONNECTION_DISCONNECTING = 2,
    CONNECTION_DISCONNECTED = 3,

    CONNECTION_UNKNOWN = 100
} lwip_net_connection_state;

typedef enum {
    SLE_RECEIVER_MODE = 0,
    SLE_TRANSMITTER_MODE = 1,
    SLE_NONE_MODE
} sle_trans_mode;

/**
 * @brief Defines the callback function net connection status change.
 *
 * @param netif_type net if type, refer to #lwip_netif_type.
 * @param net_connect_state net connection status, refer to #lwip_net_connection_state.
 */
typedef void (*net_connection_callback)(const uint8_t netif_type, const uint8_t net_connect_state);

/**
 * @brief This function register network connection status listener.
 * @param[in] conn_listener             net_connection_callback.
 * @return 0 if success; non-0 otherwise.
 */
int lwip_register_connect_listener(net_connection_callback conn_listener);

/**
 * @brief Defines the callback unregister function for listening net connection status change.
 */
void lwip_unregister_connect_listener(net_connection_callback conn_listener);

/**
 * @brief This function get current network connection status.
 * @param[in] netif_type             netif type, refer to #lwip_netif_type.
 * @param[in, out] net_connect_state      network connection statue, refer to #lwip_net_connection_state.
 * @return 0 if success; non-0 otherwise.
 */
int lwip_get_connect_status(uint8_t netif_type, uint8_t *net_connect_state);

struct netif *netif_find_num(const uint8_t num);

/**
 * @brief init and open 4G cat1 net interface.
 */
void lwip_cat1_open(void);

/**
 * @brief close 4G cat1 net interface.
 */
void lwip_cat1_close(void);

/**
 * @brief set cat1 netif ip addr, and wait.
 * Attention: this function will block the caller, make sure no task after that.
 */
void lwip_cat1_set_ip(void);

/**
 * @brief switch netif to 4G cat1.
 */
void lwip_netif_switch_volte(void);

/**
 * @brief switch netif to bluetooth pan.
 */
void lwip_netif_switch_bt_pan(void);

/**
 * @brief init and open bt pan net interface.
 */
void lwip_open_bt_pan(void);

/**
 * @brief close bt pan net interface.
 */
void lwip_close_bt_pan(void);

#ifdef CHBA_SUPPORT
errcode_t sle_chba_init(uint8_t trans_mode, ip4_addr_t local_ip, ip4_addr_t gw, ip4_addr_t netmask);

errcode_t sle_chba_deinit(void);

void sle_chba_set_mac_addr(uint8_t *local_addr, uint8_t local_size, uint8_t *remote_addr, uint8_t remote_size);
#endif

/**
 * @brief This function get current network ip address.
 * @param[in] type             netif type, refer to #lwip_netif_type.
 * @param[out] ip_addr      network ip addr. The type of ip_addr[0] is ipv4. The type of ip_addr[1] is ipv6.
 * @param[in] ip_addr_len      the length of ip_addr. It must be 2.
 * @return 0 if success; non-0 otherwise.
 */
int lwip_get_ip_addr(lwip_netif_type type, char ip_addr[][MAX_IPADDR_LEN], uint8_t ip_addr_len);

#ifdef __cplusplus
}
#endif

#endif