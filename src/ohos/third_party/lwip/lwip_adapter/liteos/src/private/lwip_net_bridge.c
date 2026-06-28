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

#include <stdint.h>
#include <stdbool.h>

#include "lwip/netifapi.h"
#include "netif/ethernet.h"
#include "lwip/tcp.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/udp.h"
#include "lwip/ip.h"
#include "lwip/ip4_frag.h"
#include "lwip/inet_chksum.h"
#include "lwip/etharp.h"
#include "lwip_private.h"

struct ip_map_node {
    struct ip_hdr ori_ip_hdr;
    uint16_t ori_port;
    uint16_t bridge_port;
    struct eth_addr src_eth_addr;
    struct ip_map_node *next;
};
#define IP_MAP_MAX_NUM 256

#define BRIDGE_LOCAL_PORT_RANGE_START 0xf001
#define BRIDGE_LOCAL_PORT_RANGE_END   0xffff

static int g_bridge_udp_port_index = 0;
static int g_bridge_tcp_port_index = 0;

static struct ip_map_node *g_ip_map_list_head = NULL;
static struct ip_map_node *g_ip_map_list_tail = NULL;
static sys_mutex_t g_list_mutex;
static struct netif *g_bridge_netif = NULL;

static int lwip_bridge_to_cat1(struct pbuf *p)
{
    uint16_t next_hdr_offset = SIZEOF_ETH_HDR;
    int ret;
    /* skip Ethernet header (min. size checked above) */
    if (pbuf_remove_header(p, next_hdr_offset)) {
        LWIP_DEBUG_LOG(("Can't move over header in packet"));
        return ERR_BUF;
    }
    LWIP_DEBUGF(IP_DEBUG, ("lwip_bridge_to_cat1\n"));
    ip4_debug_print(p);
    ret = lwip_netif_volte_linkout(NULL, p);
    pbuf_free(p);
    return ret;
}

static int lwip_get_tcp_port(struct pbuf *p, uint16_t header_offset, uint16_t *src_port, uint16_t *dest_port)
{
    struct tcp_hdr *tcphdr = (struct tcp_hdr *)((uint8_t *)p->payload + header_offset);
    /* Check that TCP header fits in payload */
    if (tcphdr == NULL || p->len < (TCP_HLEN + header_offset)) {
        /* drop short packets */
        LWIP_DEBUG_LOG(("tcp_input: short packet (%" U16_F " bytes) discarded\n", p->tot_len));
        return ERR_ARG;
    }
    /* sanity-check header length */
    uint8_t hdrlen_bytes = TCPH_HDRLEN_BYTES(tcphdr);
    if ((hdrlen_bytes < TCP_HLEN) || (hdrlen_bytes > p->tot_len)) {
        LWIP_DEBUG_LOG(("tcp_input: invalid header length (%" U16_F ")\n", (u16_t)hdrlen_bytes));
        return ERR_ARG;
    }
    if (src_port != NULL) {
        *src_port = lwip_ntohs(tcphdr->src);
    }
    if (dest_port != NULL) {
        *dest_port = lwip_ntohs(tcphdr->dest);
    }
    return ERR_OK;
}

static int lwip_get_udp_port(
    struct pbuf *p, uint16_t header_offset, uint16_t *src_port, uint16_t *dest_port)
{
    struct udp_hdr *udphdr = (struct udp_hdr *)((uint8_t *)p->payload + header_offset);
    if (p->len < (UDP_HLEN + header_offset) || udphdr == NULL) {
        /* drop short packets */
        LWIP_DEBUG_LOG(("udp_input: short UDP datagram (%" U16_F " bytes) discarded\n", p->tot_len));
        return ERR_ARG;
    }
    if (src_port != NULL) {
        *src_port = lwip_ntohs(udphdr->src);
    }
    if (dest_port != NULL) {
        *dest_port = lwip_ntohs(udphdr->dest);
    }
    return ERR_OK;
}

static void lwip_replace_ip_to_cat1(struct pbuf *p, struct ip_hdr *iphdr, uint16_t iphdr_hlen)
{
    struct netif *cat1_netif = cat1_get_netif();
    if (cat1_netif == NULL) {
        return;
    }
    IPH_CHKSUM_SET(iphdr, 0);
    ip4_addr_copy(iphdr->src, *netif_ip4_addr(cat1_netif)); /* ip replace and update checksum */
    IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, iphdr_hlen));
}

static void lwip_set_udp_checksum(
    struct pbuf *p, struct ip_hdr *iphdr, struct udp_hdr *udphdr, uint16_t iphdr_hlen, uint16_t next_hdr_offset)
{
    uint8_t *temp_payload = (uint8_t *)p->payload;
    uint32_t temp_len = p->len;
    uint32_t temp_total_len = p->tot_len;
    ip4_addr_t src_addr;
    ip4_addr_t dest_addr;

    udphdr->chksum = 0;
    pbuf_remove_header(p, next_hdr_offset + iphdr_hlen);
    ip4_addr_copy(src_addr, iphdr->src);
    ip4_addr_copy(dest_addr, iphdr->dest);
    uint16_t udpchksum = inet_chksum_pseudo(p, IP_PROTO_UDP, p->tot_len, &src_addr, &dest_addr);

    p->payload = (void *)temp_payload;
    p->len = temp_len;
    p->tot_len = temp_total_len;
    /* chksum zero must become 0xffff, as zero means 'no checksum' */
    if (udpchksum == 0x0000) {
        udpchksum = 0xffff;
    }
    udphdr->chksum = udpchksum;
}

static void lwip_set_tcp_checksum(
    struct pbuf *p, struct ip_hdr *iphdr, struct tcp_hdr *tcphdr, uint16_t iphdr_hlen, uint16_t next_hdr_offset)
{
    uint8_t *temp_payload = (uint8_t *)p->payload;
    uint32_t temp_len = p->len;
    uint32_t temp_total_len = p->tot_len;
    ip4_addr_t src_addr;
    ip4_addr_t dest_addr;

    tcphdr->chksum = 0;
    ip4_addr_copy(src_addr, iphdr->src);
    ip4_addr_copy(dest_addr, iphdr->dest);
    pbuf_remove_header(p, next_hdr_offset + iphdr_hlen);
    tcphdr->chksum = inet_chksum_pseudo(p, IP_PROTO_TCP, p->tot_len, &src_addr, &dest_addr);

    p->payload = (void *)temp_payload;
    p->len = temp_len;
    p->tot_len = temp_total_len;
}

static int lwip_replace_port_to_cat1(struct pbuf *p, struct ip_hdr *iphdr, uint16_t iphdr_hlen, uint16_t src_port)
{
    int ret;
    if (IPH_PROTO(iphdr) == IP_PROTO_TCP) {
        struct tcp_hdr *tcphdr = (struct tcp_hdr *)((uint8_t *)p->payload + SIZEOF_ETH_HDR + iphdr_hlen);
        tcphdr->src = lwip_htons(src_port);
        lwip_set_tcp_checksum(p, iphdr, tcphdr, iphdr_hlen, SIZEOF_ETH_HDR);
        LWIP_DEBUGF(TCP_DEBUG, ("bridge send tcpheader :\n"));
        tcp_debug_print(tcphdr);
        return ERR_OK;
    }
    if (IPH_PROTO(iphdr) == IP_PROTO_UDP) {
        struct udp_hdr *udphdr = (struct udp_hdr *)((uint8_t *)p->payload + SIZEOF_ETH_HDR + iphdr_hlen);
        if (p->len < (UDP_HLEN + SIZEOF_ETH_HDR + iphdr_hlen) || udphdr == NULL) {
            /* drop short packets */
            LWIP_DEBUG_LOG(("udp_input: short UDP datagram (%" U16_F " bytes) discarded\n", p->tot_len));
            return ERR_ARG;
        }
        udphdr->src = lwip_htons(src_port);
        lwip_set_udp_checksum(p, iphdr, udphdr, iphdr_hlen, SIZEOF_ETH_HDR);
        LWIP_DEBUGF(UDP_DEBUG, ("bridge send udpheader :\n"));
        udp_debug_print(udphdr);
        return ERR_OK;
    }
    return ERR_ARG;
}

static int lwip_check_ip_map_for_send(struct ip_map_node *cur, struct pbuf *p)
{
    uint16_t next_hdr_offset = SIZEOF_ETH_HDR;
    int ret;
    /* identify the IP header */
    struct ip_hdr *iphdr = (struct ip_hdr *)((uint8_t *)p->payload + next_hdr_offset);
    if ((IPH_PROTO(iphdr) == IPH_PROTO(&cur->ori_ip_hdr)) && ip4_addr_cmp(&cur->ori_ip_hdr.src, &iphdr->src) &&
        ip4_addr_cmp(&cur->ori_ip_hdr.dest, &iphdr->dest)) {
        if (IPH_PROTO(iphdr) == IP_PROTO_ICMP) {
            return ERR_OK;
        }
        uint16_t src_port = 0;
        if (IPH_PROTO(iphdr) == IP_PROTO_TCP) {
            ret = lwip_get_tcp_port(p, IPH_HL_BYTES(iphdr) + next_hdr_offset, &src_port, NULL);
            if (ret != ERR_OK) {
                return ret;
            }
            if (src_port == cur->ori_port) { /* map already set */
                return ERR_OK;
            }
        }
        if (IPH_PROTO(iphdr) == IP_PROTO_UDP) {
            ret = lwip_get_udp_port(p, IPH_HL_BYTES(iphdr) + next_hdr_offset, &src_port, NULL);
            if (ret != ERR_OK) {
                return ret;
            }
            if (src_port == cur->ori_port) { /* map already set */
                return ERR_OK;
            }
        }
    }
    return 1; /* continue search */
}

static struct ip_map_node *lwip_init_ip_map_node(struct pbuf *p, uint16_t *src_port)
{
    uint16_t next_hdr_offset = SIZEOF_ETH_HDR;
    struct eth_hdr *ethhdr = (struct eth_hdr *)(p->payload);
    struct ip_hdr *iphdr = (struct ip_hdr *)((uint8_t *)p->payload + next_hdr_offset);
    int ret;
    struct ip_map_node *cur = calloc(1, sizeof(struct ip_map_node));
    if (cur == NULL) {
        return NULL;
    }
    (void)memcpy_s(&cur->src_eth_addr, sizeof(struct eth_addr), &ethhdr->src, sizeof(struct eth_addr));
    (void)memcpy_s(&cur->ori_ip_hdr, sizeof(struct ip_hdr), iphdr, sizeof(struct ip_hdr));
    if (IPH_PROTO(iphdr) == IP_PROTO_TCP) {
        ret = lwip_get_tcp_port(p, IPH_HL_BYTES(iphdr) + next_hdr_offset, &cur->ori_port, NULL);
        if (ret != ERR_OK) {
            goto ERROR;
        }
        if (g_bridge_tcp_port_index >= (BRIDGE_LOCAL_PORT_RANGE_END - BRIDGE_LOCAL_PORT_RANGE_START)) {
            g_bridge_tcp_port_index = 0;
        }
        cur->bridge_port = BRIDGE_LOCAL_PORT_RANGE_START + g_bridge_tcp_port_index;
        *src_port = cur->bridge_port;
        g_bridge_tcp_port_index++;
    }
    if (IPH_PROTO(iphdr) == IP_PROTO_UDP) {
        ret = lwip_get_udp_port(p, IPH_HL_BYTES(iphdr) + next_hdr_offset, &cur->ori_port, NULL);
        if (ret != ERR_OK) {
            goto ERROR;
        }
        if (g_bridge_udp_port_index >= (BRIDGE_LOCAL_PORT_RANGE_END - BRIDGE_LOCAL_PORT_RANGE_START)) {
            g_bridge_udp_port_index = 0;
        }
        cur->bridge_port = BRIDGE_LOCAL_PORT_RANGE_START + g_bridge_udp_port_index;
        *src_port = cur->bridge_port;
        g_bridge_udp_port_index++;
    }
    return cur;
ERROR:
    free(cur);
    return NULL;
}

static int lwip_add_ip_map_list(struct pbuf *p, int map_count, uint16_t *src_port)
{
    struct ip_map_node *head = g_ip_map_list_head;
    struct ip_map_node *cur = lwip_init_ip_map_node(p, src_port);
    if (cur == NULL) {
        return ERR_MEM;
    }
    if (map_count >= IP_MAP_MAX_NUM) { /* list count over the maximum, replace from the list head */
        if (head->next == NULL) {
            free(cur);
            return ERR_MEM;
        }
        g_ip_map_list_head = head->next;
        free(head);
        g_ip_map_list_tail->next = cur;
        g_ip_map_list_tail = cur;
        return ERR_OK;
    }
    if (head == NULL) {
        g_ip_map_list_head = cur;
        g_ip_map_list_tail = cur;
        return ERR_OK;
    }
    g_ip_map_list_tail->next = cur;
    g_ip_map_list_tail = cur;
    return ERR_OK;
}

static int lwip_update_ip_map_for_send(struct pbuf *p, uint16_t *src_port)
{
    struct ip_map_node *cur = g_ip_map_list_head;
    int map_count = 0;
    int ret;
    while (cur != NULL) { /* search exit ip map */
        ret = lwip_check_ip_map_for_send(cur, p);
        if (ret == ERR_OK && src_port == NULL) {
            return ERR_OK; /* ping */
        }
        if (ret == ERR_OK) {
            *src_port = cur->bridge_port;
            break;
        }
        if (ret < 0) {
            return ret;
        }
        cur = cur->next;
        map_count++;
    }
    if (cur != NULL) {
        return ERR_OK; /* no need update map */
    }
    return lwip_add_ip_map_list(p, map_count, src_port);
}

static int lwip_bridge_ip_packet_to_cat1(struct pbuf *p, struct ip_hdr *iphdr, uint16_t iphdr_hlen)
{
    int ret;
    uint16_t src_port;
#if LWIP_ICMP
    if (IPH_PROTO(iphdr) == IP_PROTO_ICMP) { /* ICMP packet */
        sys_mutex_lock(&g_list_mutex);
        ret = lwip_update_ip_map_for_send(p, NULL);
        sys_mutex_unlock(&g_list_mutex);
        if (ret != ERR_OK) {
            return ret;
        }
        lwip_replace_ip_to_cat1(p, iphdr, iphdr_hlen);
        return ERR_OK;
    }
#endif
    if (IPH_PROTO(iphdr) != IP_PROTO_UDP && IPH_PROTO(iphdr) != IP_PROTO_TCP) {
        return ERR_ARG;
    }
    sys_mutex_lock(&g_list_mutex);
    ret = lwip_update_ip_map_for_send(p, &src_port);
    sys_mutex_unlock(&g_list_mutex);
    if (ret != ERR_OK) {
        return ret;
    }
    lwip_replace_ip_to_cat1(p, iphdr, iphdr_hlen);
    return lwip_replace_port_to_cat1(p, iphdr, iphdr_hlen, src_port);
}

static bool lwip_net_netif_ip4_input(struct pbuf *p, struct netif *netif_p)
{
    bool ret = true;
    struct ip_hdr *iphdr = NULL;
    ip4_addr_t current_iphdr_dest;
    uint16_t iphdr_hlen;
    uint16_t iphdr_len;
    uint16_t next_hdr_offset = SIZEOF_ETH_HDR;

    /* identify the IP header */
    iphdr = (struct ip_hdr *)((uint8_t *)p->payload + next_hdr_offset);
    if (IPH_V(iphdr) != 4) { /* 4: ip version */
        LWIP_DEBUG_LOG(("IP packet dropped due to bad version number %" U16_F "\n", (u16_t)IPH_V(iphdr)));
        return false;
    }
    /* obtain IP header length in bytes */
    iphdr_hlen = IPH_HL_BYTES(iphdr);
    /* obtain ip length in bytes */
    iphdr_len = lwip_ntohs(IPH_LEN(iphdr));
    /* header length exceeds first pbuf length, or ip length exceeds total pbuf length? */
    if ((iphdr_hlen > p->len) || (iphdr_len > p->tot_len) || (iphdr_hlen < IP_HLEN)) {
        LWIP_DEBUG_LOG(("ip4_input: short IP header (%"U16_F" bytes) received, IP packet dropped\n", iphdr_hlen));
        return false;
    }

    ip4_addr_copy(current_iphdr_dest, iphdr->dest);
    if (ip4_addr_cmp(netif_ip4_addr(netif_p), &current_iphdr_dest)) { /* compare dest ip with netif ip */
        return false;
    }
    if (lwip_bridge_ip_packet_to_cat1(p, iphdr, iphdr_hlen) != ERR_OK) {
        return false;
    }

    if (lwip_bridge_to_cat1(p) != ERR_OK) {
        LWIP_DEBUG_LOG(("cat1 send packet fail\n"));
    }
    return ret;
}

static bool lwip_net_bridge_arp_reply(struct pbuf *p, struct netif *netif)
{
    struct etharp_hdr *hdr;
    /* these are aligned properly, whereas the ARP header fields might not be */
    ip4_addr_t sipaddr, dipaddr;
    uint8_t for_us;

    hdr = (struct etharp_hdr *)((uint8_t *)p->payload + SIZEOF_ETH_HDR);
    /* RFC 826 "Packet Reception": */
    if ((hdr->hwlen != ETH_HWADDR_LEN) ||
        (hdr->protolen != sizeof(ip4_addr_t)) || (hdr->proto != PP_HTONS(ETHTYPE_IP))) {
        LWIP_DEBUG_LOG(("etharp_input: packet dropped, wrong hw type, hwlen, proto, protolen or ethernet type (%" U16_F
               "/%" U16_F "/%" U16_F "/%" U16_F ")\n",
            hdr->hwtype,
            (u16_t)hdr->hwlen,
            hdr->proto,
            (u16_t)hdr->protolen));
        return false;
    }
    IPADDR_WORDALIGNED_COPY_TO_IP4_ADDR_T(&sipaddr, &hdr->sipaddr);
    IPADDR_WORDALIGNED_COPY_TO_IP4_ADDR_T(&dipaddr, &hdr->dipaddr);

    /* ARP packet directed to us? */
    for_us = (u8_t)ip4_addr_cmp(&dipaddr, netif_ip4_addr(netif));
    if (for_us) {
        return false;
    }
    switch (hdr->opcode) {
        /* ARP request? */
        case PP_HTONS(ARP_REQUEST): { /* reply arp , mac addr : netif hwaddr */
            etharp_update_arp_map(
                netif, &sipaddr, &(hdr->shwaddr), 1);
            etharp_reply(netif, &dipaddr, &hdr->shwaddr, &sipaddr);
            pbuf_free(p);
            return true;
        }
        default: {
            LWIP_DEBUG_LOG(("arp default, opcode=0x%x\n", lwip_ntohs(hdr->opcode)));
            break;
        }
    }
    return false;
}

bool lwip_net_bridge_netif_input(struct pbuf *p, struct netif *netif)
{
    uint16_t eth_type;
    struct netif *cat1_netif = cat1_get_netif();
    struct eth_hdr *ethhdr;

    if (netif == NULL || !netif_is_up(netif) || !netif_is_link_up(netif) ||
        ip4_addr_isany_val(*netif_ip4_addr(netif))) {
        return false;
    }

    if (cat1_netif == NULL || !netif_is_up(cat1_netif) || !netif_is_link_up(cat1_netif) ||
        ip4_addr_isany_val(*netif_ip4_addr(cat1_netif))) {
        return false;
    }

    if (p == NULL || p->len <= SIZEOF_ETH_HDR) {
        return false;
    }
    /* points to packet payload, which starts with an Ethernet header */
    ethhdr = (struct eth_hdr *)p->payload;
    eth_type = ethhdr->type;
    /* this might be a multicast or broadcast packet */

    switch (eth_type) {
        /* IP packet? */
        case PP_HTONS(ETHTYPE_IP): {
            /* skip Ethernet header (min. size checked above) */
            return lwip_net_netif_ip4_input(p, netif);
        }
        case PP_HTONS(ETHTYPE_ARP): {
            return lwip_net_bridge_arp_reply(p, netif);
        }
        default: {
            LWIP_DEBUG_LOG(("wrong ethernet type=0x%x\n", lwip_ntohs(ethhdr->type)));
            break;
        }
    }
    return false;
}

static bool is_route_to_netif(struct pbuf *p, struct ip_hdr *iphdr, uint16_t *dest_port)
{
    int ret;
    if (IPH_PROTO(iphdr) == IP_PROTO_UDP) {
        ret = lwip_get_udp_port(p, IPH_HL_BYTES(iphdr), NULL, dest_port);
        if (ret != ERR_OK) {
            return false;
        }
        if (*dest_port <= UDP_LOCAL_PORT_RANGE_END) {
            return false;
        } else if (*dest_port >= BRIDGE_LOCAL_PORT_RANGE_START && *dest_port <= BRIDGE_LOCAL_PORT_RANGE_END) {
            return true;
        }
    }
    if (IPH_PROTO(iphdr) == IP_PROTO_TCP) {
        ret = lwip_get_tcp_port(p, IPH_HL_BYTES(iphdr), NULL, dest_port);
        if (ret != ERR_OK) {
            return false;
        }
        if (*dest_port <= TCP_LOCAL_PORT_RANGE_END) {
            return false;
        } else if (*dest_port >= BRIDGE_LOCAL_PORT_RANGE_START && *dest_port <= BRIDGE_LOCAL_PORT_RANGE_END) {
            return true;
        }
    }
    return true;
}

static void lwip_replace_ip_to_netif(struct ip_hdr *iphdr, uint16_t iphdr_hlen, ip4_addr_t *dest_ip)
{
    IPH_CHKSUM_SET(iphdr, 0);
    ip4_addr_copy(iphdr->dest, *dest_ip); /* ip replace and update checksum */
    IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, iphdr_hlen));
}

static int lwip_replace_port_to_netif(struct pbuf *p, struct ip_hdr *iphdr, uint16_t iphdr_hlen, uint16_t dest_port)
{
    if (IPH_PROTO(iphdr) == IP_PROTO_TCP) {
        struct tcp_hdr *tcphdr = (struct tcp_hdr *)((uint8_t *)p->payload + iphdr_hlen);
        tcphdr->dest = lwip_htons(dest_port);
        lwip_set_tcp_checksum(p, iphdr, tcphdr, iphdr_hlen, 0);
        return ERR_OK;
    }
    if (IPH_PROTO(iphdr) == IP_PROTO_UDP) {
        struct udp_hdr *udphdr = (struct udp_hdr *)((uint8_t *)p->payload + iphdr_hlen);
        udphdr->dest = lwip_htons(dest_port);
        lwip_set_udp_checksum(p, iphdr, udphdr, iphdr_hlen, 0);
        return ERR_OK;
    }
    return ERR_OK;
}

static void lwip_remove_node_from_list(struct ip_map_node *last, struct ip_map_node *cur)
{
    if (last == cur) {
        g_ip_map_list_head = cur->next;
    } else {
        last->next = cur->next;
    }
    if (cur == g_ip_map_list_tail) {
        if (last != cur) {
            g_ip_map_list_tail = last;
        } else {
            g_ip_map_list_tail = NULL;
        }
    }
    free(cur);
}

static int lwip_search_ip_map_for_recv(struct ip_hdr *input_iphdr, ip4_addr_t *current_ip_dest, uint16_t *dest_port, struct eth_addr *dest_eth)
{
    struct ip_map_node *cur = g_ip_map_list_head;
    struct ip_map_node *last = cur;

    while (cur != NULL) { /* search exit ip map */
        if ((IPH_PROTO(&cur->ori_ip_hdr) == IPH_PROTO(input_iphdr)) &&
            (ip4_addr_cmp(&cur->ori_ip_hdr.dest, &input_iphdr->src))) {
#if LWIP_ICMP
            if (IPH_PROTO(input_iphdr) == IP_PROTO_ICMP) {
                ip4_addr_copy(*current_ip_dest, cur->ori_ip_hdr.src);
                (void)memcpy_s(dest_eth, sizeof(struct eth_addr), &cur->src_eth_addr, sizeof(struct eth_addr));
                lwip_remove_node_from_list(last, cur);
                return ERR_OK;
            }
#endif
            if (IPH_PROTO(input_iphdr) == IP_PROTO_UDP || IPH_PROTO(input_iphdr) == IP_PROTO_TCP) {
                if (*dest_port == cur->bridge_port) {
                    ip4_addr_copy(*current_ip_dest, cur->ori_ip_hdr.src);
                    (void)memcpy_s(dest_eth, sizeof(struct eth_addr), &cur->src_eth_addr, sizeof(struct eth_addr));
                    *dest_port = cur->ori_port;
                    return ERR_OK;
                }
            }
        }
        last = cur;
        cur = cur->next;
    }
    return ERR_ARG;
}

static int lwip_bridge_ip_packet_to_netif(
    struct pbuf *p, struct ip_hdr *iphdr, uint16_t iphdr_hlen, ip4_addr_t *current_ip_dest, struct eth_addr *dest_addr)
{
    uint16_t dest_port;

    if (!is_route_to_netif(p, iphdr, &dest_port)) {
        return ERR_ARG;
    }
    sys_mutex_lock(&g_list_mutex);
    if (lwip_search_ip_map_for_recv(iphdr, current_ip_dest, &dest_port, dest_addr) != ERR_OK) {
        sys_mutex_unlock(&g_list_mutex);
        return ERR_ARG;
    }
    sys_mutex_unlock(&g_list_mutex);

    lwip_replace_ip_to_netif(iphdr, iphdr_hlen, current_ip_dest);
    return lwip_replace_port_to_netif(p, iphdr, iphdr_hlen, dest_port);
}

static int lwip_bridge_to_netif(struct pbuf *p, struct eth_addr *src, struct eth_addr *dst, struct netif *netif)
{
    if (netif == NULL) {
        return ERR_ARG;
    }
    LWIP_DEBUGF(IP_DEBUG, ("lwip_bridge_to_sle\n"));
    ip4_debug_print(p);
    int ret = ethernet_output(netif, p, src, dst, ETHTYPE_IP);
    pbuf_free(p);
    return ret;
}

static bool lwip_bridge_verify_ip_checksum(struct ip_hdr *iphdr, uint16_t iphdr_hlen)
{
    if (inet_chksum(iphdr, iphdr_hlen) != 0) {
        LWIP_DEBUG_LOG(("bridge: IP header checksum verification failed\n"));
        return false;
    }
    return true;
}

static bool lwip_bridge_verify_tcp_checksum(struct pbuf *p, struct ip_hdr *iphdr, uint16_t iphdr_hlen)
{
    struct tcp_hdr *tcphdr = (struct tcp_hdr *)((uint8_t *)p->payload + iphdr_hlen);
    uint8_t hdrlen_bytes = TCPH_HDRLEN_BYTES(tcphdr);
    uint16_t tcp_len = lwip_ntohs(IPH_LEN(iphdr)) - iphdr_hlen;

    if (hdrlen_bytes < TCP_HLEN) {
        LWIP_DEBUG_LOG(("bridge: invalid TCP header length\n"));
        return false;
    }
    if (tcp_len < hdrlen_bytes) {
        LWIP_DEBUG_LOG(("bridge: TCP length too short for header\n"));
        return false;
    }

    uint8_t *temp_payload = (uint8_t *)p->payload;
    uint32_t temp_len = p->len;
    uint32_t temp_total_len = p->tot_len;
    ip4_addr_t src_addr;
    ip4_addr_t dest_addr;

    ip4_addr_copy(src_addr, iphdr->src);
    ip4_addr_copy(dest_addr, iphdr->dest);
    pbuf_remove_header(p, iphdr_hlen);
    uint16_t chksum = inet_chksum_pseudo(p, IP_PROTO_TCP, tcp_len, &src_addr, &dest_addr);

    p->payload = (void *)temp_payload;
    p->len = temp_len;
    p->tot_len = temp_total_len;

    if (chksum != 0) {
        LWIP_DEBUG_LOG(("bridge: TCP checksum verification failed\n"));
        return false;
    }
    return true;
}

static bool lwip_bridge_verify_udp_checksum(struct pbuf *p, struct ip_hdr *iphdr, uint16_t iphdr_hlen)
{
    struct udp_hdr *udphdr = (struct udp_hdr *)((uint8_t *)p->payload + iphdr_hlen);

    /* UDP checksum of 0 means no checksum in IPv4, which is valid */
    if (udphdr->chksum == 0) {
        return true;
    }

    uint16_t udp_len = lwip_ntohs(udphdr->len);
    if (udp_len < UDP_HLEN) {
        LWIP_DEBUG_LOG(("bridge: invalid UDP length\n"));
        return false;
    }

    uint8_t *temp_payload = (uint8_t *)p->payload;
    uint32_t temp_len = p->len;
    uint32_t temp_total_len = p->tot_len;
    ip4_addr_t src_addr;
    ip4_addr_t dest_addr;

    ip4_addr_copy(src_addr, iphdr->src);
    ip4_addr_copy(dest_addr, iphdr->dest);
    pbuf_remove_header(p, iphdr_hlen);
    uint16_t chksum = inet_chksum_pseudo(p, IP_PROTO_UDP, udp_len, &src_addr, &dest_addr);

    p->payload = (void *)temp_payload;
    p->len = temp_len;
    p->tot_len = temp_total_len;

    if (chksum != 0) {
        LWIP_DEBUG_LOG(("bridge: UDP checksum verification failed\n"));
        return false;
    }
    return true;
}

static bool lwip_bridge_verify_cat1_checksum(struct pbuf *p, struct ip_hdr *iphdr, uint16_t iphdr_hlen)
{
    if (!lwip_bridge_verify_ip_checksum(iphdr, iphdr_hlen)) {
        return false;
    }

    if (IPH_PROTO(iphdr) == IP_PROTO_TCP) {
        return lwip_bridge_verify_tcp_checksum(p, iphdr, iphdr_hlen);
    }

    if (IPH_PROTO(iphdr) == IP_PROTO_UDP) {
        return lwip_bridge_verify_udp_checksum(p, iphdr, iphdr_hlen);
    }

    /* ICMP and other protocols: IP header checksum only */
    return true;
}

static bool lwip_net_bridge_cat1_input(struct pbuf *p, struct netif *to_netif)
{
    struct ip_hdr *iphdr = NULL;
    uint16_t iphdr_len;
    uint16_t iphdr_hlen;
    uint16_t dest_port = 0;
    ip4_addr_t current_ip_dest;
    struct eth_addr dest_addr;

    if (to_netif == NULL || p == NULL) {
        return false;
    }
    if (!netif_is_up(to_netif) || !netif_is_link_up(to_netif) || ip4_addr_isany_val(*netif_ip4_addr(to_netif))) {
        return false;
    }
    /* identify the IP header */
    iphdr = (struct ip_hdr *)((uint8_t *)p->payload);
    if (IPH_V(iphdr) != 4) { /* 4: ip version */
        LWIP_DEBUG_LOG(("IP packet dropped due to bad version number %" U16_F "\n", (u16_t)IPH_V(iphdr)));
        return false;
    }
    /* obtain IP header length in bytes */
    iphdr_hlen = IPH_HL_BYTES(iphdr);
    /* obtain ip length in bytes */
    iphdr_len = lwip_ntohs(IPH_LEN(iphdr));
    /* header length exceeds first pbuf length, or ip length exceeds total pbuf length? */
    if ((iphdr_hlen > p->len) || (iphdr_len > p->tot_len) || (iphdr_hlen < IP_HLEN)) {
        LWIP_DEBUG_LOG(("ip4_input: short IP header (%"U16_F" bytes) received, IP packet dropped\n", iphdr_hlen));
        return false;
    }

    if (!lwip_bridge_verify_cat1_checksum(p, iphdr, iphdr_hlen)) {
        LWIP_DEBUG_LOG(("bridge: checksum verification failed, packet dropped\n"));
        return false;
    }

    if (lwip_bridge_ip_packet_to_netif(p, iphdr, iphdr_hlen, &current_ip_dest, &dest_addr) != ERR_OK) {
        return false;
    }

    if (lwip_bridge_to_netif(p, (struct eth_addr *)(to_netif->hwaddr), &dest_addr, to_netif) != ERR_OK) {
        LWIP_DEBUG_LOG(("sle send packet fail\n"));
    }
    return true;
}

void lwip_set_bridge_netif(struct netif *netif_p)
{
    g_bridge_netif = netif_p;
}

bool lwip_net_bridge_cat1(struct pbuf *p)
{
    return lwip_net_bridge_cat1_input(p, g_bridge_netif);
}

void lwip_net_bridge_init(void)
{
    if (sys_mutex_new(&g_list_mutex) != ERR_OK) {
        LWIP_DEBUG_LOG(("mutex create fail\n"));
    }
}