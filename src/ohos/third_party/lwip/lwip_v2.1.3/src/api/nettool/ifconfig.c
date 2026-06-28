/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2026-2026. All rights reserved.
 * Description: ifconfig API implementation
 * Create: 2026
 */
#include <stdio.h>
#include <stdint.h>
#include <securec.h>
#include "lwip/netifapi.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UNUSED
#define UNUSED(a) ((void)(a))
#endif

#define IFCONFIG_DEBUG
#define IFCONFIG_PRINT(fmt, args...) printf(fmt, ##args)

#define IFCONFIG_OK   0
#define IFCONFIG_FAIL (-1)

#define BYTE_CONVERT_UNIT 1024
#define PRINT_BUF_LEN   1024

struct ifconfig_option {
    char            iface[NETIF_NAMESIZE];
    unsigned int    option;
    ip_addr_t       ip_addr;
    ip_addr_t       netmask;
    ip_addr_t       gw;
    unsigned char   ethaddr[NETIF_MAX_HWADDR_LEN];
    uint16_t           mtu;
    uint8_t            result;
    /*
    * when using telnet, print to the telnet socket will result in system deadlock.
    * So we cahe the prinf data to a buf, and when the tcpip callback returns,
    * then print the data out to the telnet socket
    */
    sys_sem_t       cb_completed;
    char            cb_print_buf[PRINT_BUF_LEN];
    unsigned int    print_len;
};

static char* g_netif_name[] = {"volte", "bt pan", "sle", "usb"};

LWIP_STATIC void err_ifconfig_string_put(struct ifconfig_option *ifconfig_cmd, char *errcode)
{
    int ret;
    ret = snprintf_s(ifconfig_cmd->cb_print_buf + ifconfig_cmd->print_len,
        PRINT_BUF_LEN - ifconfig_cmd->print_len,
        ((PRINT_BUF_LEN - ifconfig_cmd->print_len) - 1),
        "ERR:%s\n",
        errcode);
    if ((ret > 0) && ((unsigned int)ret < (PRINT_BUF_LEN - ifconfig_cmd->print_len))) {
        ifconfig_cmd->print_len += (unsigned int)ret;
    }
}

LWIP_STATIC int print_netif_ip(struct netif *netif, char *print_buf, unsigned int buf_len)
{
    int ret;
    char *tmp = print_buf;

    if (buf_len <= 1) {
        return IFCONFIG_FAIL;
    }

#if LWIP_IPV4
    char ip_str[IPADDR_STRLEN_MAX];
    char netmask_str[IPADDR_STRLEN_MAX];
    char gateway_str[IPADDR_STRLEN_MAX];

    (void)ipaddr_ntoa_r(&netif->ip_addr, ip_str, IPADDR_STRLEN_MAX);
    (void)ipaddr_ntoa_r(&netif->netmask, netmask_str, IPADDR_STRLEN_MAX);
    (void)ipaddr_ntoa_r(&netif->gw, gateway_str, IPADDR_STRLEN_MAX);

    ret = snprintf_s(
        tmp, buf_len, (unsigned int)(buf_len - 1), "ip:%s netmask:%s gateway:%s\r\n", ip_str, netmask_str, gateway_str);
    if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
        return IFCONFIG_FAIL;
    }
    tmp += ret;
    buf_len -= (unsigned int)ret;
#endif

#if LWIP_IPV6
    char ip6_str[IPADDR_STRLEN_MAX];
    char *addr = NULL;
    int i;
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        /* only PREFERRED addresses are displyaed */
        if (!ip6_addr_isvalid(netif->ip6_addr_state[i])) {
            continue;
        }
        addr = ip6addr_ntoa_r((const ip6_addr_t *)&netif->ip6_addr[i], ip6_str, IPADDR_STRLEN_MAX);
        (void)addr;
        ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1), "\tip6: %s/64\r\n", ip6_str);
        if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
            return IFCONFIG_FAIL;
        }
        tmp += ret;
        buf_len -= (unsigned int)ret;
    }
#endif

    return (tmp - print_buf);
}

LWIP_STATIC int print_netif_hw(struct netif *netif, char *print_buf, unsigned int buf_len)
{
    int ret, i;
    char *tmp = print_buf;
    if (buf_len <= 1 || netif->hwaddr_len > NETIF_MAX_HWADDR_LEN) {
        return IFCONFIG_FAIL;
    }
    ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1), "%s", "\tHWaddr ");
    if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
        return IFCONFIG_FAIL;
    }
    tmp += ret;
    unsigned int tmp_len = buf_len - (unsigned int)ret;

    for (i = 0; i < netif->hwaddr_len - 1; i++) {
        ret = snprintf_s(tmp, tmp_len, (tmp_len - 1), "%02x:", netif->hwaddr[i]);
        if ((ret <= 0) || ((unsigned int)ret >= tmp_len)) {
            return IFCONFIG_FAIL;
        }
        tmp += ret;
        tmp_len -= (unsigned int)ret;
    }

    ret = snprintf_s(tmp, tmp_len, (unsigned int)(tmp_len - 1), "%02x", netif->hwaddr[i]);
    if ((ret <= 0) || ((unsigned int)ret >= tmp_len)) {
        return IFCONFIG_FAIL;
    }
    tmp += ret;

    return (tmp - print_buf);
}

LWIP_STATIC int print_netif_link(struct netif *netif, char *print_buf, unsigned int buf_len)
{
    int ret;
    char *tmp = print_buf;
    if (buf_len <= 1) {
        return IFCONFIG_FAIL;
    }

    ret = snprintf_s(tmp,
        buf_len,
        (unsigned int)(buf_len - 1),
        " MTU:%hu %s %s %s\r\n",
        netif->mtu,
        ((netif->flags & NETIF_FLAG_UP) != 0) ? "Running" : "Stop",
        "Default",
        ((netif->flags & NETIF_FLAG_LINK_UP) != 0) ? "Link UP" : "Link Down");
    if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
        return IFCONFIG_FAIL;
    }
    tmp += ret;

    return (tmp - print_buf);
}

#if MIB2_STATS
LWIP_STATIC int print_netif_mib(struct netif *netif, char *print_buf, unsigned int buf_len)
{
    int ret;
    char *tmp = print_buf;

    if (buf_len <= 1) {
        return IFCONFIG_FAIL;
    }

    uint64_t rx_cnt = netif->mib2_counters.ifinucastpkts + netif->mib2_counters.ifinnucastpkts;
    uint64_t tx_cnt = netif->mib2_counters.ifoutucastpkts + netif->mib2_counters.ifoutnucastpkts;
    ret = snprintf_s(tmp,
        buf_len,
        (unsigned int)(buf_len - 1),
        "\tRX packets:%u errors:%u dropped:%u overruns:%u\r\n\tTX packets:%u errors:%u dropped:%u\r\n"
        "\tRX bytes:%u TX bytes:%u\r\n",
        rx_cnt,
        netif->mib2_counters.ifinerrors,
        netif->mib2_counters.ifindiscards,
        netif->mib2_counters.ifinoverruns,
        tx_cnt,
        netif->mib2_counters.ifouterrors,
        netif->mib2_counters.ifoutdiscards,
        netif->mib2_counters.ifinoctets,
        netif->mib2_counters.ifoutoctets);
    if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
        return IFCONFIG_FAIL;
    }
    tmp += ret;
    return (tmp - print_buf);
}
#endif

LWIP_STATIC int print_netif(struct netif *netif, char *print_buf, unsigned int buf_len)
{
    int ret;
    char *tmp = print_buf;

    if (buf_len <= 1) {
        return -1;
    }

    unsigned int name_index = netif->name[0] - '0';
    if (name_index >= sizeof(g_netif_name) / sizeof(g_netif_name[0])) {
        goto out;
    }
    ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1), "%s%u\t", g_netif_name[name_index], netif->num);
    if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
        goto out;
    }
    tmp += ret;
    unsigned int tmp_len = buf_len - (unsigned int)ret;

    ret = print_netif_ip(netif, tmp, tmp_len);
    if ((ret <= 0) || ((unsigned int)ret >= tmp_len)) {
        goto out;
    }
    tmp += ret;
    tmp_len -= (unsigned int)ret;

    ret = print_netif_hw(netif, tmp, tmp_len);
    if ((ret <= 0) || ((unsigned int)ret >= tmp_len)) {
        goto out;
    }
    tmp += ret;
    tmp_len -= (unsigned int)ret;

    ret = print_netif_link(netif, tmp, tmp_len);
    if ((ret <= 0) || ((unsigned int)ret >= tmp_len)) {
        goto out;
    }
    tmp += ret;
    tmp_len -= (unsigned int)ret;

#if MIB2_STATS
    ret = print_netif_mib(netif, tmp, tmp_len);
    if ((ret <= 0) || ((unsigned int)ret >= tmp_len)) {
        goto out;
    }
    tmp += ret;
    tmp_len -= (unsigned int)ret;
#endif

out:
    return (int)(tmp - print_buf);
}

LWIP_STATIC void lwip_ifconfig_show_internal(void *arg)
{
    struct netif *netif = NULL;
    struct ifconfig_option *ifconfig_cmd = (struct ifconfig_option *)arg;
    int ret;

    if (netif_list == NULL) {
        err_ifconfig_string_put(ifconfig_cmd, "206 net device not ready");
        sys_sem_signal(&ifconfig_cmd->cb_completed);
        return;
    }

    if (ifconfig_cmd->iface[0] == '\0') {
        /* display all netif */
        for (netif = netif_list; netif != NULL; netif = netif->next) {
            ret = print_netif(
                netif, ifconfig_cmd->cb_print_buf + ifconfig_cmd->print_len, PRINT_BUF_LEN - ifconfig_cmd->print_len);
            if (ret == -1) {
                sys_sem_signal(&ifconfig_cmd->cb_completed);
                return;
            }
            ifconfig_cmd->print_len += (unsigned int)ret;
        }
    } else {
        netif = netif_find(ifconfig_cmd->iface);
        if (netif == NULL) {
            err_ifconfig_string_put(ifconfig_cmd, "205 net device not found");
            sys_sem_signal(&ifconfig_cmd->cb_completed);
            IFCONFIG_PRINT("205 net device not found\n");
            return;
        }

        ret = print_netif(
            netif, ifconfig_cmd->cb_print_buf + ifconfig_cmd->print_len, PRINT_BUF_LEN - ifconfig_cmd->print_len);
        if (ret == -1) {
            sys_sem_signal(&ifconfig_cmd->cb_completed);
            return;
        }
        ifconfig_cmd->print_len += (unsigned int)ret;
    }
    sys_sem_signal(&ifconfig_cmd->cb_completed);
}

void lwip_printsize(size_t size)
{
    static const char *sizes[] = {"", "K", "M", "G"};
    size_t divis = 0;
    size_t rem = 0;
    size_t len = size;
    while ((len >= BYTE_CONVERT_UNIT) && (divis < ((sizeof(sizes) / sizeof(char *)) - 1))) {
        rem = (len % BYTE_CONVERT_UNIT);
        divis++;
        len /= BYTE_CONVERT_UNIT;
    }
    /* 1024.0 : byte in float unit */
    IFCONFIG_PRINT("(%.1f %sB) \r\n", (float)len + (float)rem / 1024.0, sizes[divis]);
}

LWIP_STATIC void lwip_ifconfig_usage(void)
{
    IFCONFIG_PRINT("Usage:\r\nifconfig [-a] \r\n");
}

/* To support "ifconfig -a" command
 *   RX packets:XXXX errors:X dropped:X overruns:X bytes:XXXX (Human readable format)
 *   TX packets:XXXX errors:X dropped:X overruns:X bytes:XXXX (Human readable format)
 *
 *   Below is assumed for 'overrun' stat.
 *   Linux Kernel:
 *          RX: FIFO overrun
 *              Data structure: net_device->stats->rx_fifo_errors
 *              Flag which is marked when FIFO overrun: ENRSR_FO
 *
 *          Function: ei_receive->ENRSR_FO
 *
 *          TX: A "FIFO underrun" occurred during transmit.
 *              Data structure: net_device->stats->tx_fifo_errors
 *              Flag which is marked when FIFO underrun: ENTSR_FU
 *
 *          Function: ei_tx_intr->ENTSR_FU
 *
 *  LWIP:
 *      So in our case,
 *      while receiving a packet RX case, if the buffer is full (trypost - it is sys_mbox_trypost)
 *      the error will be returned, we can consider that an overflow has happend.
 *      So this can be RX overrun.
 *
 *      But while transmitting a packet TX case, underrun cannot happen because it block on the
 *      message Q if it is full (NOT trypost - it is sys_mbox_post). So TX overrun is always 0.
 */
LWIP_STATIC uint32_t lwip_ifconfig_all(uint8_t argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
#if LWIP_STATS
    uint32_t stat_err_cnt;
    uint32_t stat_drop_cnt;
    uint32_t stat_rx_or_tx_cnt;
    uint32_t stat_rx_or_tx_bytes;

    stat_rx_or_tx_cnt = lwip_stats.ip.recv;
    stat_err_cnt = (uint32_t)(lwip_stats.ip.ip_rx_err + lwip_stats.ip.lenerr + lwip_stats.ip.chkerr +
        lwip_stats.ip.opterr + lwip_stats.ip.proterr);
    stat_drop_cnt = (uint32_t)(lwip_stats.ip.drop + lwip_stats.link.link_rx_drop);
    stat_rx_or_tx_bytes = lwip_stats.ip.ip_rx_bytes;

    IFCONFIG_PRINT("%18s:%u\t errors:%u\t ip dropped:%u\t link dropped:%u\t overrun:%hu\t bytes:%u ", "RX packets",
        stat_rx_or_tx_cnt, stat_err_cnt, stat_drop_cnt, lwip_stats.link.link_rx_drop, lwip_stats.ip.link_rx_overrun,
        stat_rx_or_tx_bytes);

    /* Print in Human readable format of the incoming bytes */
    lwip_printsize(lwip_stats.ip.ip_rx_bytes);
#if IP6_STATS
    stat_rx_or_tx_cnt = lwip_stats.ip6.recv;
    stat_err_cnt = (uint32_t)(lwip_stats.ip6.ip_rx_err + lwip_stats.ip6.lenerr + lwip_stats.ip6.chkerr +
        lwip_stats.ip6.opterr + lwip_stats.ip6.proterr);
    stat_drop_cnt = lwip_stats.ip6.drop;
    stat_rx_or_tx_bytes = lwip_stats.ip6.ip_rx_bytes;

    IFCONFIG_PRINT("%18s:%u\t errors:%u\t dropped:%u\t overrun:%hu\t bytes:%u ", "RX packets(ip6)", stat_rx_or_tx_cnt,
        stat_err_cnt, stat_drop_cnt, lwip_stats.ip.link_rx_overrun, stat_rx_or_tx_bytes);

    /* Print in Human readable format of the incoming bytes */
    lwip_printsize(lwip_stats.ip6.ip_rx_bytes);
#endif
    stat_rx_or_tx_cnt = (uint32_t)(lwip_stats.ip.fw + lwip_stats.ip.xmit);
    stat_err_cnt = (uint32_t)(lwip_stats.ip.rterr + lwip_stats.ip.ip_tx_err);
    /* IP layer drop stat param is not maintained, failure at IP is considered in 'errors' stat */
    stat_drop_cnt = lwip_stats.link.link_tx_drop;
    stat_rx_or_tx_bytes = lwip_stats.ip.ip_tx_bytes;

    IFCONFIG_PRINT("%18s:%u\t errors:%u\t link dropped:%u\t overrun:0\t bytes:%u", "TX packets", stat_rx_or_tx_cnt,
        stat_err_cnt, stat_drop_cnt, stat_rx_or_tx_bytes);

    /* Print in Human readable format of the outgoing bytes */
    lwip_printsize(lwip_stats.ip.ip_tx_bytes);

#if IP6_STATS
    stat_rx_or_tx_cnt = (uint32_t)(lwip_stats.ip6.fw + lwip_stats.ip6.xmit);
    stat_err_cnt = (uint32_t)(lwip_stats.ip6.rterr + lwip_stats.ip6.ip_tx_err);
    stat_rx_or_tx_bytes = lwip_stats.ip6.ip_tx_bytes;

    IFCONFIG_PRINT("%18s:%u\t errors:%u\t overrun:0\t bytes:%u", "TX packets(ip6)", stat_rx_or_tx_cnt, stat_err_cnt,
        stat_rx_or_tx_bytes);

    /* Print in Human readable format of the outgoing bytes */
    lwip_printsize(lwip_stats.ip6.ip_tx_bytes);
#endif /* IP6_STATS */
#endif /* LWIP_STATS */
    return IFCONFIG_OK;
}

LWIP_STATIC uint32_t lwip_ifconfig_callback(tcpip_callback_fn function, struct ifconfig_option *ifconfig_cmd)
{
    if (tcpip_callback(function, ifconfig_cmd) != ERR_OK) {
        sys_sem_free(&ifconfig_cmd->cb_completed);
        IFCONFIG_PRINT("208 service failure\n");
        return (uint32_t)(IFCONFIG_FAIL);
    }
    (void)sys_arch_sem_wait(&ifconfig_cmd->cb_completed, 0);
    sys_sem_free(&ifconfig_cmd->cb_completed);

    ifconfig_cmd->cb_print_buf[PRINT_BUF_LEN - 1] = '\0';
    if (ifconfig_cmd->result != IFCONFIG_OK) {
        return (uint32_t)(IFCONFIG_FAIL);
    }
    return IFCONFIG_OK;
}

LWIP_STATIC uint32_t lwip_ifconfig_print_all(struct ifconfig_option *ifconfig_cmd)
{
    uint32_t ret = lwip_ifconfig_callback(lwip_ifconfig_show_internal, ifconfig_cmd);
    if (ret != 0) {
        return ret;
    }

#ifdef IFCONFIG_DEBUG
    IFCONFIG_PRINT("%s\n", ifconfig_cmd->cb_print_buf);
#endif
    return IFCONFIG_OK;
}

LWIP_STATIC uint32_t lwip_ifconfig_basic(uint8_t argc, char *argv[], int *idx, struct ifconfig_option *ifconfig_cmd)
{
    UNUSED(argc);
    if (argc == 0) {
        return lwip_ifconfig_print_all(ifconfig_cmd);
    }

    /* not a basic process, continue next process */
    return (uint32_t)(IFCONFIG_FAIL);
}

uint32_t CmdIfconfig(uint8_t argc, char *argv[])
{
    int i;
    uint32_t ret;
    if (argv == NULL) {
        lwip_ifconfig_usage();
        return (uint32_t)IFCONFIG_FAIL;
    }

    if (argc != 0 && strcmp("-a", argv[0]) == 0) {
        return lwip_ifconfig_all(argc, argv);
    }

    struct ifconfig_option *ifconfig_cmd = mem_malloc(sizeof(struct ifconfig_option));
    if (ifconfig_cmd == NULL) {
        IFCONFIG_PRINT("203 mem malloc failed\n");
        return (uint32_t)IFCONFIG_FAIL;
    }
    (void)memset_s(ifconfig_cmd, sizeof(struct ifconfig_option), 0, sizeof(struct ifconfig_option));
    if (sys_sem_new(&ifconfig_cmd->cb_completed, 0) != ERR_OK) {
        IFCONFIG_PRINT("204 sem create failed\n");
        ret = (uint32_t)IFCONFIG_FAIL;
        goto ifconfig_free_and_return;
    }

    i = 0;
    ret = lwip_ifconfig_basic(argc, argv, &i, ifconfig_cmd);
    if (ret != (uint32_t)(IFCONFIG_OK)) {
        goto ifconfig_free_and_return;
    }

ifconfig_free_and_return:
    mem_free(ifconfig_cmd);
    return ret;
}

#ifdef __cplusplus
}
#endif