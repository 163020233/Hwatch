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

#include "lwip_hooks.h"
#include "lwip_public.h"
#include "lwip_private.h"
#include "lwip/dns.h"
#include "errcode.h"
#include "ril_interface.h"
#include "cat1_net_interface.h"
#ifdef CONFIG_NV_FEATURE_SUPPORT
#include "nv.h"
#endif

#define RETRY_TIMES  5
#define IP_ADDR_LEN 16
typedef struct {
    uint8_t dns_tc[IP_ADDR_LEN];
    uint8_t dns_ali[IP_ADDR_LEN];
    uint8_t dns_cnn[IP_ADDR_LEN];
} net_dns_addr_t;

static struct netif *g_volte_netif = NULL;
static net_connection_callback g_volte_callback = NULL;
static sys_sem_t g_ip_fetch_sem = {0};
static uint8_t g_cat1_net_state = CONNECTION_UNKNOWN;

static err_t lwip_netif_output_ip4(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr)
{
    if (netif == NULL || p == NULL || ipaddr == NULL) {
        return ERR_MEM;
    }
    return netif->linkoutput(netif, p);
}

err_t lwip_netif_volte_linkout(struct netif *netif, struct pbuf *p)
{
    int ret;
    (void)netif;
    if (p == NULL || p->payload == NULL) {
        return ERR_ARG;
    }
    uint8_t *payload = NULL;
    struct pbuf *p_clone = NULL;
    if (p->tot_len > p->len && p->next != NULL) {
        p_clone = pbuf_clone(PBUF_RAW, PBUF_RAM, p); /* packet queue copy to one packet */
        if (p_clone == NULL) {
            return ERR_MEM;
        }
        payload = p_clone->payload;
    } else {
        payload = p->payload;
    }

    ret = cat1_network_service_write_data((uint8_t *)payload, p->tot_len);
    if (ret != ERRCODE_CAT1_SUCCESS) {
        LWIP_DEBUG_LOG(("cat1 network write fail,ret=0x%x\n", ret));
        ret = ERR_CONN;
        goto END;
    }
    LWIP_DEBUGF(TCP_DEBUG, ("cat1 write len=%d\n", p->tot_len));
    ret = ERR_OK;
END:
    if (p_clone != NULL) {
        pbuf_free(p_clone);
    }
    return ret;
}

#if LWIP_IPV6
static err_t lwip_netif_output_ip6(struct netif *netif, struct pbuf *p, const ip6_addr_t *ipaddr)
{
    if (netif == NULL || p == NULL || ipaddr == NULL) {
        return ERR_MEM;
    }

    return netif->linkoutput(netif, p);
}
#endif

static err_t cat1_driver_init_fn(struct netif *netif_p)
{
    if (netif_p == NULL) {
        return ERR_MEM;
    }
    netif_state *state = (netif_state *)netif_p->state;
    if (state == NULL) {
        return ERR_ARG;
    }
#if LWIP_NETIF_HOSTNAME
    /* Initialise interface hostname */
    netif_p->hostname = state->hostname;
#endif /* LWIP_NETIF_HOSTNAME */
    netif_p->hwaddr_len = 0;
#if LWIP_IPV4
    netif_p->output = lwip_netif_output_ip4;
#endif
#if LWIP_IPV6
    netif_p->output_ip6 = lwip_netif_output_ip6;
    netif_p->mtu6 = 1500; /* 1500: max transmit unit */
#endif
    (void)sprintf_s(netif_p->name, sizeof(netif_p->name), "%d", LWIP_NETIF_VOLTE);
    netif_p->linkoutput = lwip_netif_volte_linkout;
    netif_p->flags |= (NETIF_FLAG_BROADCAST);
    netif_p->mtu = 1500; /* 1500: max transmit unit */
    netifapi_netif_set_default(netif_p);
    return ERR_OK;
}

static void set_ip6_inner(struct netif *netif_p, s8_t addr_idx, const ip6_addr_t *ip_addr)
{
    const ip6_addr_t *old_ip6 = netif_ip6_addr(netif_p, addr_idx);
    if (ip6_addr_cmp(old_ip6, ip_addr)) {
        return;
    }

    netif_ip6_addr_set(netif_p, addr_idx, ip_addr);
    LWIP_DEBUG_LOG(("Set ipv6\r\n"));
}

static void set_ip4_inner(struct netif *netif_p, ip4_addr_t *ip_addr)
{
    const ip4_addr_t *old_ip4 = netif_ip4_addr(netif_p);
    if (ip4_addr_cmp(old_ip4, ip_addr)) {
        return;
    }

    ip_addr_t gw;
    ip_addr_t netmask;
    IP_ADDR4(&netmask, 255, 255, 255, 0);
    IP_ADDR4(&gw, 255, 255, 255, 255);

    netifapi_netif_set_addr(netif_p, ip_addr, ip_2_ip4(&netmask), ip_2_ip4(&gw));
    LWIP_DEBUG_LOG(("Set ipv4 OK\r\n"));
}

void lwip_cat1_set_ip(void)
{
    uint32_t retryTimes = 0;
    struct netif *netif_p = g_volte_netif;
    if (netif_p == NULL) {
        LWIP_DEBUG_LOG(("netif is null\r\n"));
        return;
    }
    ril_ip_addr_info ip_addr_info = {0};
    sys_sem_new(&g_ip_fetch_sem, 0);
#if defined(SUPPORT_USB_BRIDGE) || defined(SUPPORT_SLE_BRIDGE)
    lwip_net_bridge_init();
#endif
    while (true) {
        netif_p = g_volte_netif;
        if (netif_p == NULL || !netif_is_up(netif_p)) {
            goto WAITING;
        }
        if (ril_get_ue_addr(&ip_addr_info) != 0 || ip_addr_info.ip_num == 0) {
            sleep(1);
            continue;
        }

#if LWIP_IPV4 && LWIP_IPV6
        for (int i = 0; i < ip_addr_info.ip_num; i++) {
            if (ip_addr_info.ip_addr[i].type == IPADDR_TYPE_V6) {
                set_ip6_inner(netif_p, 0, ip_2_ip6(&ip_addr_info.ip_addr[i]));
            } else if (ip_addr_info.ip_addr[i].type == IPADDR_TYPE_V4) {
                set_ip4_inner(netif_p, ip_2_ip4(&ip_addr_info.ip_addr[i]));
            } else {
                LWIP_DEBUG_LOG(("Set ip fail\r\n"));
            }
        }

        if (ip_addr_info.ip_num == 1 && retryTimes < RETRY_TIMES) { // 1: only ipv4 or ipv6
            sleep(1);
            retryTimes++;
            continue;
        }

    WAITING:
        sys_arch_sem_wait(&g_ip_fetch_sem, 0);
        retryTimes = 0;
        LWIP_DEBUG_LOG(("lwip now check cat1 ip addr...\r\n"));
#else
        (void)netif_p;
        sys_sem_free(&g_ip_fetch_sem);
        LWIP_DEBUG_LOG(("set ip fail, lwip not support ipv4 or ipv6, check config\r\n"));
        break;
#endif
    }
}

static struct netif *lwip_netif_4g_create(char *hostname)
{
    /** The netif structure that this is looking after. */
    struct netif *netif_p = NULL;
    netif_state *state = NULL;
    state = malloc(sizeof(netif_state));
    if (state == NULL) {
        return NULL;
    }

#if LWIP_NETIF_HOSTNAME
    state->hostname = hostname;
#else
    unused(hostname);
#endif
    state->start_stack_init = true;

    netif_p = (struct netif *)malloc(sizeof(struct netif));
    if (netif_p == NULL) {
        free(state);
        return NULL;
    }
    memset_s(netif_p, sizeof(struct netif), 0, sizeof(struct netif));
    netifapi_netif_add(netif_p,
#if LWIP_IPV4
        NULL,
        NULL,
        NULL,
#endif
        (void *)state,
        cat1_driver_init_fn,
        tcpip_input);

#if LWIP_IPV6
    IP_ADDR6_HOST(netif_p->ip6_addr, 0, 0, 0, 0x00000001UL);
    netif_p->ip6_addr_state[0] = IP6_ADDR_VALID;
#endif /* LWIP_IPV6 */
    return (netif_p);
}

static void cat1_netif_set_up(struct netif *netif_p)
{
    netifapi_netif_set_link_up(netif_p);
    netifapi_netif_set_up(netif_p);
    netifapi_netif_set_default(netif_p);
}

static int cat1_netif_open(void)
{
    struct netif *netif_p = NULL;
    if (g_volte_netif != NULL) {
        cat1_netif_set_up(g_volte_netif);
        return 0;
    }
    netif_p = lwip_netif_4g_create("4G cat1");
    if (netif_p == NULL) {
        LWIP_DEBUG_LOG(("netif for 4g create fail\n"));
        return -1;
    }
    g_volte_netif = netif_p;
    return 0;
}

static void cat1_netif_state_callback(uint8_t net_state)
{
    LWIP_DEBUG_LOG(("cat1 net_state=%u\n", net_state));
    struct netif *netif_p = g_volte_netif;
    if (netif_p == NULL) {
        return;
    }

    if (net_state == CAT1_NET_STATE_ON) {
        netifapi_netif_set_link_up(netif_p);
        netifapi_netif_set_up(netif_p);
        netifapi_netif_set_default(netif_p);
        sys_sem_signal(&g_ip_fetch_sem);
    } else {
        netifapi_netif_set_link_down(netif_p);
        netifapi_netif_set_down(netif_p);
#if LWIP_IPV4
        ip_addr_set_zero_ip4(&netif_p->ip_addr);
        ip_addr_set_zero_ip4(&netif_p->netmask);
        ip_addr_set_zero_ip4(&netif_p->gw);
#endif
    }
}

static void cat1_ethernet_data_callback(uint8_t *data, uint16_t length)
{
    struct netif *netif_p = g_volte_netif;
    struct pbuf *p = NULL;
    err_t rc;
    LWIP_DEBUGF(TCP_DEBUG, ("cat1 input len=%d\n", length));
    if (netif_p == NULL || data == NULL) {
        return;
    }
    p = pbuf_alloc(PBUF_RAW, (length + SIZEOF_ETH_HDR), PBUF_RAM);
    if (p == NULL) {
        return;
    }
#if defined(ETH_PAD_SIZE) && ETH_PAD_SIZE
    pbuf_header(p, -SIZEOF_ETH_HDR);
#endif
    rc = pbuf_take(p, data, length);
    if (rc != ERRCODE_SUCC) {
        printf("pbuf_take fail(%d)!!!\r\n", rc);
        pbuf_free(p);
        return;
    }
#if defined(SUPPORT_SLE_BRIDGE) || defined(SUPPORT_USB_BRIDGE)
    if (lwip_net_bridge_cat1(p)) {
        return;
    }
#endif
    netif_p->input(p, netif_p);
}

static void cat1_netif_call_state_callback(uint8_t call_state)
{
    LWIP_DEBUG_LOG(("cat1 call_state=%u\n", call_state));
    struct netif *netif_p = g_volte_netif;
    if (netif_p == NULL) {
        return;
    }
    if (call_state == CAT1_CALL_STATE_ON && cat1_network_service_net_state_get() == CAT1_NET_STATE_ON) {
        netifapi_netif_set_link_down(netif_p);
        netifapi_netif_set_down(netif_p);
#if LWIP_IPV4
        ip_addr_set_zero_ip4(&netif_p->ip_addr);
        ip_addr_set_zero_ip4(&netif_p->netmask);
        ip_addr_set_zero_ip4(&netif_p->gw);
#endif
    }
    if (call_state == CAT1_CALL_STATE_OFF && cat1_network_service_net_state_get() == CAT1_NET_STATE_ON) {
        netifapi_netif_set_link_up(netif_p);
        netifapi_netif_set_up(netif_p);
        sys_sem_signal(&g_ip_fetch_sem);
    }
}

static cat1_net_callbacks_t g_cat1_net_callback = {
    cat1_netif_state_callback, cat1_ethernet_data_callback, cat1_netif_call_state_callback};

void cat1_close_inner(void)
{
    struct netif *netif_p = g_volte_netif;
    if (netif_p == NULL) {
        return;
    }
    cat1_net_deregister_callbacks();

    netifapi_netif_set_link_down(netif_p);
    netifapi_netif_set_down(netif_p);
#if LWIP_IPV4
    ip_addr_set_zero_ip4(&netif_p->ip_addr);
    ip_addr_set_zero_ip4(&netif_p->netmask);
    ip_addr_set_zero_ip4(&netif_p->gw);
#endif
    g_cat1_net_state = CONNECTION_DISCONNECTED;
}

static void cat1_set_dns_server(void)
{
#ifdef CONFIG_NV_FEATURE_SUPPORT
    net_dns_addr_t dns_addr;
    uint16_t len;
    errcode_t ret = uapi_nv_read(NV_ID_NET_DNS_ADDR, sizeof(net_dns_addr_t), &len, (uint8_t *)&dns_addr);
    if (ret != ERRCODE_SUCC) {
        LWIP_DEBUG_LOG(("read dns server fail\n"));
        return;
    }
    /* initialize default DNS server address */
    ip_addr_t dns_server = {0};
    ip4_addr_set_u32(ip_2_ip4(&dns_server), ipaddr_addr((const char *)(dns_addr.dns_tc))); /* tencent dns */
    dns_setserver(0, &dns_server);
    ip4_addr_set_u32(ip_2_ip4(&dns_server), ipaddr_addr((const char *)dns_addr.dns_ali)); /* ali dns */
    dns_setserver(1, &dns_server); /* 1: dns index */
    ip4_addr_set_u32(ip_2_ip4(&dns_server), ipaddr_addr((const char *)dns_addr.dns_cnn)); /* CNNIC dns */
    dns_setserver(2, &dns_server); /* 2: dns index */
#else
    LWIP_DEBUG_LOG(("set cat1 dns server fail\n"));
#endif
}

void cat1_open_inner(void)
{
    int ret = cat1_netif_open();
    if (ret != 0) {
        lwip_cat1_close();
        return;
    }

    cat1_set_dns_server();

    ret = cat1_net_register_callbacks(&g_cat1_net_callback);
    if (ret != ERRCODE_CAT1_SUCCESS) {
        LWIP_DEBUG_LOG(("cat1 register fail, ret=0x%x\n", ret));
        lwip_cat1_close();
    } else {
        LWIP_DEBUG_LOG(("cat1_net_register_callbacks ok\n"));
    }

    if (cat1_network_service_net_state_get() == CAT1_NET_STATE_ON) {
        sys_sem_signal(&g_ip_fetch_sem);
    }
}

uint8_t cat1_get_net_state(void)
{
    return g_cat1_net_state;
}

struct netif *cat1_get_netif(void)
{
    return g_volte_netif;
}

#if LWIP_NETIF_EXT_STATUS_CALLBACK
static netif_ext_callback_t g_netif_ext_callback;

static void cat1_net_callback(struct netif *netif, netif_nsc_reason_t reason, const netif_ext_callback_args_t *args)
{
    if (netif != g_volte_netif) {
        return;
    }

    if ((reason & LWIP_NSC_IPV4_ADDRESS_CHANGED || reason & LWIP_NSC_IPV6_SET) &&
        g_cat1_net_state != CONNECTION_CONNECTED) {
        g_cat1_net_state = CONNECTION_CONNECTED;
        if (g_volte_callback != NULL) {
            LWIP_DEBUG_LOG(("cat1 CONNECTION_CONNECTED\n"));
            g_volte_callback(LWIP_NETIF_VOLTE, CONNECTION_CONNECTED);
        }
        return;
    }

    if ((reason & LWIP_NSC_STATUS_CHANGED) && (args->status_changed.state == 0) &&
        g_cat1_net_state != CONNECTION_DISCONNECTED) {
        g_cat1_net_state = CONNECTION_DISCONNECTED;
        if (g_volte_callback != NULL) {
            LWIP_DEBUG_LOG(("cat1 CONNECTION_DISCONNECTED\n"));
            g_volte_callback(LWIP_NETIF_VOLTE, CONNECTION_DISCONNECTED);
        }
    }
}

void cat1_register_net_callback(net_connection_callback callback)
{
    if (g_volte_callback != NULL) {
        return;
    }
    g_volte_callback = callback;
    netif_add_ext_callback(&g_netif_ext_callback, cat1_net_callback);
}

void cat1_unregister_net_callback(void)
{
    g_volte_callback = NULL;
    netif_remove_ext_callback(&g_netif_ext_callback);
}
#else
void cat1_register_net_callback(net_connection_callback callback)
{
    (void)callback;
}

void cat1_unregister_net_callback(void)
{}
#endif

/* Based on ip4_route, but use src as opposed to dest. */
struct netif *sys_hooks_ip4_route_src(const ip4_addr_t *dest, const ip4_addr_t *src)
{
    (void)(dest);
    struct netif *net_if = NULL;
    if (src == NULL) {
        return NULL;
    }

    for (net_if = netif_list; net_if != NULL; net_if = net_if->next) {
        if (netif_is_link_up(net_if) && netif_is_up(net_if) && !ip4_addr_isany_val(*netif_ip4_addr(net_if))) {
            if (ip4_addr_netcmp(src, netif_ip4_addr(net_if), netif_ip4_netmask(net_if))) {
                return net_if;
            }
            if (ip4_addr_cmp(src, netif_ip4_gw(net_if)) && ((net_if->flags & NETIF_FLAG_BROADCAST) == 0)) {
                return net_if;
            }
        }
    }

    return NULL;
}
