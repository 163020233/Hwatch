/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LiteOS USB Driver ECM Head file.
 * Author: Huawei LiteOS Team
 * Create: 2024-03-28
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

#ifndef _F_ECM_H
#define _F_ECM_H

#include <los_event.h>
#include <los_atomic.h>
#include "usbd_eth_common.h"
#include "gadget/usbd_cdc.h"
#include "gadget/composite_pri.h"
#include "implementation/_thread_ref.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef USB3_DEVICE_CONTROLLER
#define ECM_MAX_PACKET_SIZE 0x400
#else
#define ECM_MAX_PACKET_SIZE 0x200
#endif

#define DEVICE_VERSION          0x0100

enum ecm_ep_index
{
  ECM_INTERRUPT_EP_IDX = 0,
  ECM_BULKIN_EP_IDX,
  ECM_BULKOUT_EP_IDX
};

#define ECM_SET_INTERRUPT_EP_NUM(desc) ((desc)->epno[ECM_INTERRUPT_EP_IDX])
#define ECM_SET_BULKIN_EP_NUM(desc)    ((desc)->epno[ECM_BULKIN_EP_IDX])
#define ECM_SET_BULKOUT_EP_NUM(desc)   ((desc)->epno[ECM_BULKOUT_EP_IDX])

#define ECM_CONFIG_NUM              1
#define ECM_IF_NUM_PER_PORT         2 /* Interfaces number per ECM port */
#define ECM_CONFIG_ID               1 /* The only supported configuration ID */
#define ECM_STR_ID_NUM              4
#define ECM_TOTAL_EP_NUM            3

/* Number of interfaces in the configuration */

#define ECM_INTERFACE_NUM           ECM_IF_NUM_PER_PORT

/* Maximum segment size supported by an Ethernet device */
#define ECM_NET_PKTSIZE             1514
#define ECM_NET_OUT_PKTSIZE         1600

#define ECM_SET_PACKET_FILTER       0x43

#define CDC_NOTIFY_NETWORK_CONNECTION  0x0
#define CDC_NOTIFY_SPEED_CHANGE        0x2a
#define CDC_NOTIFY_SIZE                0x8
#define CDC_NET_SPEED_DATA_LENGTH      0x8
#define CDC_NET_UP_SPEED               0x800000   // 1M/s
#define CDC_NET_DOWN_SPEED             0x800000

#define ECM_MAX_TX_TIMEOUT             1000

#define ECM_TX_COMPLETE_EVENT          1
#define ECM_UNBIND_EVENT               2

#define ECM_MAC_ADDR_LEN               6
#define ECM_DESC_MAC_INDEX             2
#define ECM_DESC_GET_INDEX1(i)         (ECM_DESC_MAC_INDEX + ((i) * 4))
#define ECM_DESC_GET_INDEX2(i)         (ECM_DESC_MAC_INDEX + ((i) * 4) + 2)
#define DIGIT_TO_CHARACTER(num)        (((num) <= 9) ? ((num) + '0') : ((num) - 10 + 'a'))
#define GET_HEX_LEFT_VALUE(num)        (((num) & 0xf0) >> 4)
#define GET_HEX_RIGHT_VALUE(num)       ((num) & 0xf)

enum ecm_status_e
{
  ECM_STATUS_INIT = 0,
  ECM_STATUS_IDLE,
  ECM_STATUS_BUSY,
};

enum ecm_work_event_e
{
  ECM_RX_EVENT_UNLOAD  = 1,
  ECM_RX_EVENT_RECEIVE = 1 << 1,
  ECM_RX_EVENT_EXIT    = 1 << 2,
};

struct ecm_hs_function_descriptor
{
  struct usb_iaddesc_s ifcad;
  struct usb_ifdesc_s ifcd; /* ecm control interface descriptor */
  struct cdc_header_func_desc cdc_desc;
  struct cdc_union_func_desc cdc_union_desc;
  struct cdc_ecm_func_desc cdc_ecm_desc;
  struct usb_epdesc_s nepd;
#ifdef USB3_DEVICE_CONTROLLER
  struct usb_ss_comp_epdesc_s ncompd;
#endif
  struct usb_ifdesc_s ifod;
  struct usb_epdesc_s iepd;
#ifdef USB3_DEVICE_CONTROLLER
  struct usb_ss_comp_epdesc_s icompd;
#endif
  struct usb_epdesc_s oepd;
#ifdef USB3_DEVICE_CONTROLLER
  struct usb_ss_comp_epdesc_s ocompd;
#endif
} __attribute__ ((packed));

void ecm_send_data(const uint8_t *buf, uint32_t buflen);
void ecm_change_connect_status(bool connect);
void ecm_init_macaddr_desc(char *macaddr_desc);
void usbdev_ecm_initialize_dev(struct composite_devdesc_s *dev, int ifnobase, int minor);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _F_ECM_H */
