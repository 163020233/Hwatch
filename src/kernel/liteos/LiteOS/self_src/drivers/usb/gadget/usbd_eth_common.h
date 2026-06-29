/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LiteOS USB Ether Common HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2024-03-07
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
 * --------------------------------------------------------------------------- */

#ifndef _USBD_ETH_COMMON_H
#define _USBD_ETH_COMMON_H

#include "los_typedef.h"
#include "net/usb_eth_drv.h"
#include "gadget/f_eth.h"
#if defined(LOSCFG_DRIVERS_USB_ETHERNET_GADGET) || defined(LOSCFG_DRIVERS_USB_ETH_SER_GADGET)
#include "f_rndis.h"
#endif
#ifdef LOSCFG_DRIVERS_USB_ECM_GADGET
#include "f_ecm.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct net_driver_s
{
  struct los_eth_driver driver;
  uintptr_t buf[MAX_ETH_DRV_SG];
  uint32_t len[MAX_ETH_DRV_SG];
  uint32_t sg_len;
  uint32_t total_len;
  uint8_t *d_buf;
  uint32_t d_len;
  uint32_t d_flags;
  void *d_private;
  int (*d_ifup)(struct net_driver_s *driver);
  int (*d_ifdown)(struct net_driver_s *driver);
  int (*d_txavail)(struct net_driver_s *driver);
};

void netdev_register(struct net_driver_s *netdev);
void netdev_unregister(struct net_driver_s *netdev);
void link_status_check_func(bool connected);
void eth_rx(const uint8_t *buf, uint32_t buflen);
void eth_random_addr(uint8_t *addr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _USBD_ETH_COMMON_H */