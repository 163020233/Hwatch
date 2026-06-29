/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LiteOS USB Ether Common
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

#include "usbd_eth_common.h"
#include "gadget/rndis_std.h"
#include "gadget/composite_adapter.h"

static struct usb_eth_operations g_eth_ops;
typedef void (*eth_send_func)(const uint8_t *buf, uint32_t buflen);
typedef void (*eth_connect_func)(bool connect);
eth_send_func g_eth_send_func;
eth_connect_func g_eth_connect_func;
uint8_t g_packet_hdr_len = 0;

void link_status_check_func(bool connected)
{
  if (g_eth_ops.eth_link_change != NULL)
    {
      g_eth_ops.eth_link_change(connected);
    }
}

void eth_rx(const uint8_t *buf, uint32_t buflen)
{
  if (g_eth_ops.eth_rx != NULL)
    {
      g_eth_ops.eth_rx(buf, buflen);
    }
}

void set_ethernet_config(void)
{
  device_type type = dev_type_get();
  if (type == DEV_ETHERNET || type == DEV_SER_ETH)
    {
#if defined(LOSCFG_DRIVERS_USB_ETHERNET_GADGET) || defined(LOSCFG_DRIVERS_USB_ETH_SER_GADGET)
      g_eth_send_func = rndis_send_data;
      g_eth_connect_func = rndis_change_connect_status;
      g_packet_hdr_len = sizeof(struct rndis_packet_msg);
#endif
    }
  else
    {
#ifdef LOSCFG_DRIVERS_USB_ECM_GADGET
      g_eth_send_func = ecm_send_data;
      g_eth_connect_func = ecm_change_connect_status;
      g_packet_hdr_len = 0;
#endif
    }
}

void netdev_register(struct net_driver_s *netdev)
{
  if (g_eth_ops.eth_register != NULL)
    {
      g_eth_ops.eth_register();
    }

  set_ethernet_config();
}

void netdev_unregister(struct net_driver_s *netdev)
{
    if (g_eth_ops.eth_unregister != NULL)
    {
      g_eth_ops.eth_unregister();
    }
    (void)memset_s(&g_eth_ops, sizeof(struct usb_eth_operations), 0, sizeof(struct usb_eth_operations));
}

static bool is_ops_init(struct usb_eth_operations *ops)
{
  if (ops->eth_rx != NULL || ops->eth_register != NULL ||
      ops->eth_unregister != NULL || ops->eth_link_change != NULL)
    {
      return true;
    }
  return false;
}

int feth_init_ops(struct usb_eth_operations *ops)
{
  uint32_t len = sizeof(struct usb_eth_operations);
  if (ops == NULL || is_ops_init(ops) == false)
    {
      usb_err("ops is invalid.\n");
      return -1;
    }

  if (is_ops_init(&g_eth_ops))
    {
      usb_err("ops has been initialized.\n");
      return -1;
    }

  (void)memcpy_s(&g_eth_ops, len, ops, len);

  return LOS_OK;
}

void feth_send_data(const uint8_t *buf, uint32_t buflen)
{
  if (g_eth_send_func != NULL)
    {
      g_eth_send_func(buf, buflen);
    }
}

void feth_change_connect_status(bool connect)
{
  if (g_eth_connect_func != NULL)
    {
      g_eth_connect_func(connect);
    }
}

void eth_random_addr(uint8_t *addr)
{
  uint32_t rand_val;
  uint32_t nowclocks;

  LOS_Msleep(200); /* delay 200 ms */
  nowclocks = LOS_CurrNanosec() & 0xffffffff;  /* Get low 32-bit value */
  srand(nowclocks);
  rand_val = (uint32_t)rand();

  /* A random value of U32 can be assigned to four members. */

  addr[0]  = rand_val & 0xff;
  addr[1]  = (rand_val >> 8) & 0xff;
  addr[2]  = (rand_val >> 16) & 0xff;
  addr[3]  = (rand_val >> 24) & 0xff;

  LOS_Msleep(200); /* delay 200 ms */
  nowclocks = LOS_CurrNanosec() & 0xffffffff;  /* Get low 32-bit value */
  srand(nowclocks);
  rand_val = (uint32_t)rand();
  addr[4]  = rand_val & 0xff;
  addr[5]  = (rand_val >> 8) & 0xff;
  addr[0] &= 0xfe;  /* Clear multicast bit */
  addr[0] |= 0x02;  /* Set local assignment bit (IEEE802) */
}
