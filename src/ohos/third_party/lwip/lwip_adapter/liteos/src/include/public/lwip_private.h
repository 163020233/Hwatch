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

#ifndef LWIP_PRIVATE_H_
#define LWIP_PRIVATE_H_

#include <string.h>
#include "arch/sys_arch.h"
#include "lwip/netifapi.h"
#include "lwip/err.h"
#include "lwip/api.h"
#include "lwip_public.h"
#include "lwip/ip4_addr.h"

typedef struct lwip_status_change_callback {
    /** Pointer to next function in the list */
    struct lwip_status_change_callback *next;

    /** Function to call back */
    void (*callback)(void*);

    /** Context to pass back in callback function */
    void* callback_context;
} lwip_status_change_callback_s;

typedef struct netif_state {
    /** List of user callbacks for this netif */
    lwip_status_change_callback_s *status_change_callbacks;

    /** List of user callbacks for this netif */
    lwip_status_change_callback_s *status_link_callbacks;

#if LWIP_NETIF_HOSTNAME
    /** Hostname, or NULL if not set */
    char *hostname;
#endif

    /** Whether to start the stack and bring the interface up when created */
    uint8_t start_stack_init;

    uint8_t type;
} netif_state;

void bt_pan_open(void);

void bt_pan_close(void);

void bt_pan_register_net_callback(net_connection_callback callback);

void bt_pan_unregister_net_callback(void);

void cat1_register_net_callback(net_connection_callback callback);

void cat1_unregister_net_callback(void);

void cat1_open_inner(void);

void cat1_close_inner(void);

uint8_t cat1_get_net_state(void);

uint8_t pan_get_net_state(void);

struct netif *cat1_get_netif(void);

struct netif *sle_chba_netdev_get(void);

err_t lwip_netif_volte_linkout(struct netif *netif, struct pbuf *p);

bool lwip_net_bridge_sle_input(struct pbuf *p);

bool lwip_net_bridge_cat1(struct pbuf *p);

err_t sle_chba_send_pkt(struct netif *dev, struct pbuf *pbuf);

uint8_t *sle_get_remote_addr(void);

void lwip_set_bridge_netif(struct netif *netif_p);

void lwip_net_bridge_init(void);

bool lwip_net_bridge_netif_input(struct pbuf *p, struct netif *netif);

#define LWIP_ADAPTER_DEBUG
#ifdef LWIP_ADAPTER_DEBUG
#define LWIP_DEBUG_LOG(message) LWIP_PLATFORM_DIAG(message)
#else
#define LWIP_DEBUG_LOG(message)
#endif

#endif /* LWIP_PRIVATE_H_ */
