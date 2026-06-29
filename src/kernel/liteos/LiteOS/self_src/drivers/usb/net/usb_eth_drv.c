/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025
 * Description : Usb Ethernet Driver
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <lwip/err.h>
#include <lwip/pbuf.h>
#include <lwip/dhcp.h>
#include <lwip/netifapi.h>
#include <lwip/etharp.h>

#include "usb_eth_drv.h"

static err_t eth_drv_send(struct netif *usb_netif, struct pbuf *p, const ip4_addr_t *ipaddr);
static err_t eth_drv_link_output(struct netif *usb_netif, struct pbuf *p);
static void eth_drv_recv(struct los_eth_driver *sc, int total_len);

static err_t eth_drv_lwip_init_fn(struct netif *netif_p)
{
    err_t ret;
    if (netif_p == NULL) {
        return ERR_ARG;
    }
    netif_p->output = etharp_output;
    netif_p->linkoutput = eth_drv_link_output;
    netif_p->mtu = 1500; /* 1500: Max Transmission Unit */

    netif_set_flags(netif_p, NETIF_FLAG_UP | NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP);

    ret = netifapi_netif_set_default(netif_p);
    if (ret) {
        PRINT_ERR("%s, %d, netif set default failed, ret:%d\n", __FUNCTION__, __LINE__, ret);
        return ERR_VAL;
    }

    ret = netifapi_netif_set_link_up(netif_p);
    if (ret) {
        PRINT_ERR("%s, %d, netif set link up failed, ret:%d\n", __FUNCTION__, __LINE__, ret);
        return ERR_VAL;
    }

    ret = netifapi_netif_set_up(netif_p);
    if (ret) {
        PRINT_ERR("%s, %d, netif set up failed, ret:%d\n", __FUNCTION__, __LINE__, ret);
        return ERR_VAL;
    }

    return ERR_OK;
}

/*
 * This function is called during system initialization to register a
 * network interface with the system.
 */
static void eth_drv_init(struct los_eth_driver *sc, const unsigned char *enaddr)
{
    struct netif *usb_netif = &sc->ac_if;
    struct eth_drv_sc *drv_sc = (struct eth_drv_sc *)sc->driver_context;
    err_t ret = 0;

    usb_netif->name[0] = 'u';
    usb_netif->name[1] = 'e';
    usb_netif->hwaddr_len = NETIF_MAX_HWADDR_LEN;
    usb_netif->input = tcpip_input;
    if (enaddr != 0) {
        (drv_sc->funs->start)(sc, NULL, 0); // ue_init
        (void)memcpy_s(usb_netif->hwaddr, NETIF_MAX_HWADDR_LEN, enaddr, NETIF_MAX_HWADDR_LEN);
    }

    ret = netifapi_netif_add(usb_netif, NULL, NULL, NULL, sc, eth_drv_lwip_init_fn, tcpip_input);
    if (ret) {
        PRINT_ERR("Fatal Error, unable to add USB network interface! ret:%d\n", ret);
        return;
    }
}

/*
 * This function is called from a hardware driver to indicate that an input
 * packet has arrived.  The routine will set up appropriate network resources
 * to hold the data and call back into the driver to retrieve the data.
 */
static void eth_drv_recv(struct los_eth_driver *sc, int total_len)
{
    struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
    struct netif *usb_netif = &sc->ac_if;
    struct eth_drv_sc *drv_sc = (struct eth_drv_sc *)sc->driver_context;
    struct pbuf *p;
    struct pbuf *q;
    int sg_len = 0;
    int tmp_total_len = total_len;

    if ((total_len > MAX_ETH_MSG) || (total_len < 0)) {
        tmp_total_len = MAX_ETH_MSG;
    }

    p = pbuf_alloc(PBUF_RAW, (tmp_total_len + ETH_PAD_SIZE), PBUF_RAM);
    if (p == NULL) {
        PRINTK("eth_drv_recv : pbuf_alloc failed\n");
        return;
    }

#if ETH_PAD_SIZE
    (void)pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    for (q = p; q != NULL && sg_len < MAX_ETH_DRV_SG; q = q->next) {
        sg_list[sg_len].buf = (uintptr_t)q->payload;
        sg_list[sg_len++].len = q->len;
    }

    (drv_sc->funs->recv)(sc, sg_list, sg_len);

#if ETH_PAD_SIZE
    (void)pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    if ((usb_netif != NULL) && (usb_netif->input != NULL)) {
        usb_netif->input(p, usb_netif);
    }
}

static err_t eth_drv_send(struct netif *usb_netif, struct pbuf *p, const ip4_addr_t *ipaddr)
{
    struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
    struct los_eth_driver *sc = (struct los_eth_driver *)usb_netif->state;
    struct eth_drv_sc *drv_sc = (struct eth_drv_sc*)sc->driver_context;
    int sg_len = 0;
    int ret = 0;
    struct pbuf *q;

#if ETH_PAD_SIZE
    int has_removed_header = 0;
#endif
#if DUMP_LWIP_DATA
    dprintf("%s %d in %s len: %d tol_len: %d \n", __func__, __LINE__, __FILE_NAME__, p->len, p->tot_len);
#endif
    do {
        ret = (drv_sc->funs->can_send)(sc);
    } while (ret == false);

    for (q = p; q != NULL && sg_len < MAX_ETH_DRV_SG; q = q->next) {
#if DUMP_LWIP_DATA
        int i;
        char *tmp;
        tmp = q->payload;
        for (i = 0; ((i < LWIP_DUMP_DATA_MAX_LENGTH) && (i < q->len)); i++) {
            dprintf("0x%02X  ", tmp[i]);
            if (i % LWIP_DUMP_DATA_ONE_LINE_LENGTH == (LWIP_DUMP_DATA_ONE_LINE_LENGTH - 1)) {
                dprintf("\r\n");
            }
        }
        dprintf("\r\n");
#endif
#if ETH_PAD_SIZE
    if (q == p) {
        (void)pbuf_header(q, -ETH_PAD_SIZE); /* drop the padding word */
        has_removed_header = 1;
    }
#endif
        sg_list[sg_len].buf = (uintptr_t)q->payload;
        sg_list[sg_len++].len = q->len;
#if ETH_PAD_SIZE
    if (has_removed_header == 1) {
        (void)pbuf_header(q, ETH_PAD_SIZE); /* reclaim the padding word */
        has_removed_header = 0;
    }
#endif
    }
    (drv_sc->funs->send)(sc, sg_list, sg_len, p->tot_len - ETH_PAD_SIZE, (uintptr_t)p);
    return ERR_OK;
}

static err_t eth_drv_link_output(struct netif *usb_netif, struct pbuf *p)
{
    return eth_drv_send(usb_netif, p, NULL);
}

const struct los_eth_funs eth_drv_funs_usb = {
    .init = eth_drv_init,
    .recv = eth_drv_recv,
    .send_complete = NULL,
};

const struct los_eth_funs *get_eth_drv_funs_usb(void)
{
    return &eth_drv_funs_usb;
}
