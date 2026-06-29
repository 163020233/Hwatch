/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2016-2022. All rights reserved.
 * Description: LiteOS USB Driver RNDIS Protocol
 * Author: Huawei LiteOS Team
 * Create: 2016-07-13
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

#include "gadget/f_rndis.h"
#include "gadget/rndis.h"
#include "controller/usb_device/dwc_otg_pcd.h"
#include "controller/usb_device/dwc_otg_pcd_intr.h"
#include "net/usb_eth_drv.h"
#include "los_mux.h"

static struct net_driver_s *g_netdev;

static Atomic g_rndis_status = RNDIS_STATUS_INIT;

/* Network connection event */

static EVENT_CB_S g_link_status_event;

/* RNDIS_MAX_BUFFER_COUNT transfer complete event */

static EVENT_CB_S g_tx_complete_event;

/* Network connection status processing task */

static uint32_t g_task_id;

/* Workqueue for TX and RX */

struct workqueue_struct *g_workqueue;

/* Tx transmission flag */

static Atomic g_tx_transferring;

/* Existing buffer count waiting to be transferred */

static Atomic g_buf_count;

/* Rndis net lock mutex */

static uint32_t g_rndis_mtx = INVALID_MUX;

/* Rndis spin lock */

static SPIN_LOCK_INIT(g_rndis_spinlock);

static const char g_rndis_string_id[RNDIS_STRING_ID_LEN] =
{
  RNDIS_STRING_ID_LEN, USB_DESC_TYPE_STRING, 0x09, 0x04
};

static const char g_rndis_string[RNDIS_STRING_LEN] =
{
  RNDIS_STRING_LEN, USB_DESC_TYPE_STRING, 0x52, 0x00, 0x4e, 0x00, 0x44, 0x00,
  0x49, 0x00, 0x53, 0x00, 0x20, 0x00, 0x43, 0x00, 0x6f, 0x00, 0x6d,
  0x00, 0x6d, 0x00, 0x75, 0x00, 0x6e, 0x00, 0x69, 0x00, 0x63, 0x00,
  0x61, 0x00, 0x74, 0x00, 0x69, 0x00, 0x6f, 0x00, 0x6e, 0x00, 0x73,
  0x00, 0x20, 0x00, 0x43, 0x00, 0x6f, 0x00, 0x6e, 0x00, 0x74, 0x00,
  0x72, 0x00, 0x6f, 0x00, 0x6c, 0x00
};

static const char g_rndis_type_string[RNDIS_TYPE_LEN] =
{
  RNDIS_TYPE_LEN, USB_DESC_TYPE_STRING, 0x52, 0x00, 0x4e, 0x00, 0x44, 0x00,
  0x49, 0x00, 0x53, 0x00, 0x20, 0x00, 0x45, 0x00, 0x74, 0x00, 0x68,
  0x00, 0x65, 0x00, 0x72, 0x00, 0x6e, 0x00, 0x65, 0x00, 0x74, 0x00,
  0x20, 0x00, 0x44, 0x00, 0x61, 0x00, 0x74, 0x00, 0x61, 0x00
};

/* RNDIS does not belong to Communications and CDC Control class.
 * Set classid to 00 (use the class code information from the interface descriptor)
 * to avoid additional driver installation when only rndis is loaded.
 */
static struct usb_devdesc_s g_fether_device_desc =
{
  .len          = sizeof(struct usb_devdesc_s),
  .type         = USB_DESC_TYPE_DEVICE,
  HSETW(.usb,   USB_VERSION_BCD), /* USB version */
  .classid      = 0,
  .subclass     = 0,
  .protocol     = 0,
  .mxpacketsize = USB_EP_MPS,
  HSETW(.vendor,  0),      /* Vendor */
  HSETW(.product, 0),      /* Product */
  HSETW(.device, 0x0318),  /* Device version */
  .imfgr       = 1,
  .iproduct    = 2,
  .serno       = 3,
  .nconfigs    = 1,
};

static struct usbd_string g_fether_device_strings[] =
{
  {0, g_rndis_string_id},
  {1, NULL},
  {2, NULL},
  {3, NULL},
  {4, g_rndis_string},
  {5, g_rndis_type_string},
  USBD_DEVICE_STRINGS_END
};

/* According to USB IF (https://www.usb.org/defined-class-codes),
 * the RNDIS should use specific class code:
 * (1) classid: e0h, subclass:01h, protocol:03h
 * (2) classid: efh, subclass:04h, protocol:01h
 * Select (1) here because it is easier to test.
 * INF installation script of (1): WceIsVista.INF (Windows XP and later).
 */
static struct rndis_hs_function_descriptor g_rndis_hs_func_desc =
{
  {
    .len       = sizeof(struct usb_iaddesc_s),
    .type      = 0x0b,
    .firstif   = 0,
    .nifs      = 0x02,
    .classid   = 0xe0,
    .subclass  = 0x01,
    .protocol  = 0x03,
    .ifunction = 6,
  },
  {
    .len      = sizeof(struct usb_ifdesc_s),
    .type     = USB_DESC_TYPE_INTERFACE,
    .ifno     = 0,
    .alt      = 0,
    .neps     = 1, /* Endpoints Num */
    .classid  = USB_CLASS_CDC,
    .subclass = USB_CLASS_CDC,
    .protocol = USB_CLASS_VENDOR_SPEC,
    .iif      = 0x04,
  },
  {
    .bFunctionLength    = sizeof(struct cdc_header_func_desc),
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = 0,
    .bcdCDC             = 0x0110,
  },
  {
    .bFunctionLength    = sizeof(struct cdc_cm_func_desc),
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = 0x01,
    .bmCapabilities     = 0,
    .bDataInterface     = 0x01, /* data-interfac */
  },
  {
    .bFunctionLength    = sizeof(struct cdc_acm_func_desc),
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = 0x02,
    .bmCapabilities     = 0,
  },
  {
    .bFunctionLength    = sizeof(struct cdc_union_func_desc),
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = 0x06,
    .bMasterInterface   = 0,
    .bSlaveInterface0   = 0,
  },
  {
    .len      = sizeof(struct usb_epdesc_s),
    .type     = USB_DESC_TYPE_ENDPOINT,
    .addr     = USB_DIR_IN,
    .attr     = USB_EP_ATTR_XFER_INT,
    HSETW(.mxpacketsize, 0x0008),
    .interval = 9,
  },
#ifdef USB3_DEVICE_CONTROLLER
  {
    .len      = 6,
    .type     = 0x30,
    .maxburst = 0,
    .attr     = 0,
    HSETW(.interval, 0x0008),
  },
#endif
  {
    .len      = sizeof(struct usb_ifdesc_s),
    .type     = USB_DESC_TYPE_INTERFACE,
    .ifno     = 1,
    .alt      = 0,
    .neps     = 2, /* Endpoints Num */
    .classid  = USB_CLASS_CDC_DATA,
    .subclass = 0, /* interface subclass */
    .protocol = 0, /* interface protocol */
    .iif      = 5,
  },
  {
    .len      = sizeof(struct usb_epdesc_s),
    .type     = USB_DESC_TYPE_ENDPOINT,
    .addr     = USB_DIR_IN,
    .attr     = USB_EP_ATTR_XFER_BULK,
    HSETW(.mxpacketsize, FRNDIS_MAX_PACKET_SIZE),
    .interval = 0,
  },
#ifdef USB3_DEVICE_CONTROLLER
  {
    .len      = 6,
    .type     = 0x30,
    .maxburst = 0,
    .attr     = 0,
    .interval = {0x00}
  },
#endif
  {
    .len      = sizeof(struct usb_epdesc_s),
    .type     = USB_DESC_TYPE_ENDPOINT,
    .addr     = USB_DIR_OUT,
    .attr     = USB_EP_ATTR_XFER_BULK,
    HSETW(.mxpacketsize, FRNDIS_MAX_PACKET_SIZE),
    .interval = 0,
  },
#ifdef USB3_DEVICE_CONTROLLER
  {
    .len      = 6,
    .type     = 0x30,
    .maxburst = 0,
    .attr     = 0,
    .interval = {0x00}
  },
#endif
};

static struct usb_cfgdesc_s g_rndis_hs_config_desc =
{
  .len         = USB_SIZEOF_CFGDESC,
  .type        = USB_DESC_TYPE_CONFIG,
  HSETW(.totallen, USB_SIZEOF_CFGDESC + sizeof(g_rndis_hs_func_desc)),
  .ninterfaces = 2,
  .cfgvalue    = 1,
  .icfg        = 0,
  .attr        = USB_CONFIG_ATTR_SELFPOWER | USB_CONFIG_ATTR_ONE,
  .mxpower     = 1, /* Max power */
};

static const struct rndis_hs_descriptor g_rndis_hs_desc =
{
  .rndis_config = &g_rndis_hs_config_desc,
  .rndis_func   = &g_rndis_hs_func_desc
};

void eth_get_device_dec_info(struct usbd_string **device_strings, struct usb_devdesc_s **device_desc)
{
  *device_strings = &g_fether_device_strings[1];
  *device_desc    = &g_fether_device_desc;
}

void eth_str_desc_free(void)
{
  dev_str_desc_free(&g_fether_device_strings[1]);
}

int rndis_net_lock_init(void)
{
  uint32_t ret;

  ret = LOS_MuxCreate(&g_rndis_mtx);
  if (ret != LOS_OK)
    {
      usb_err("failed, ret:%u\n", ret);
      return -1;
    }

  return 0;
}

void rndis_net_lock_deinit(void)
{
  if (g_rndis_mtx != INVALID_MUX)
    {
      (void)LOS_MuxDelete(g_rndis_mtx);
      g_rndis_mtx = INVALID_MUX;
    }
}

void rndis_net_lock(void)
{
  /* This function is only called in the workqueue task and will not fail */

  (void)LOS_MuxPend(g_rndis_mtx, LOS_WAIT_FOREVER);
}

void rndis_net_unlock(void)
{
  (void)LOS_MuxPost(g_rndis_mtx);
}

uint32_t rndis_spin_lock_irqsave(void)
{
  uint32_t flags;
  LOS_SpinLockSave(&g_rndis_spinlock, &flags);
  return flags;
}

void rndis_spin_unlock_irqrestore(uint32_t flags)
{
  LOS_SpinUnlockRestore(&g_rndis_spinlock, flags);
}

static void link_status_check_thread(void)
{
  uint32_t ret;

  for (;;)
    {
      ret = LOS_EventRead(&g_link_status_event, LINK_STATUS_UP_EVENT | LINK_STATUS_DOWN_EVENT | UNLOAD_STATUS_EVENT,
                          LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
      if (ret & UNLOAD_STATUS_EVENT)
        {
          break;
        }
      if (ret & LINK_STATUS_UP_EVENT)
        {
          link_status_check_func(true);
        }
      else if (ret & LINK_STATUS_DOWN_EVENT)
        {
          link_status_check_func(false);
        }
    }
  (void)LOS_EventWrite(&g_link_status_event, EXIT_STATUS_EVENT);
}

static uint32_t link_status_check_init(void)
{
  uint32_t ret;
  TSK_INIT_PARAM_S param;

  (void)LOS_EventInit(&g_link_status_event);

  (void)memset_s(&param, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));

  param.pfnTaskEntry = (TSK_ENTRY_FUNC)link_status_check_thread;
  param.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
  param.usTaskPrio = LINK_STATUS_TASK_PRI;
  param.pcName = "USB_Status_Task";
  param.uwResved = LOS_TASK_STATUS_DETACHED;

  ret = LOS_TaskCreate(&g_task_id, &param);
  if (ret != LOS_OK)
    {
      (void)LOS_EventDestroy(&g_link_status_event);
    }
  return ret;
}

static void link_status_check_deinit(void)
{
  (void)LOS_EventDestroy(&g_link_status_event);
}

void rndis_link_status_change(bool connected)
{
  if (connected)
    {
      (void)LOS_EventWrite(&g_link_status_event, LINK_STATUS_UP_EVENT);
    }
  else
    {
      (void)LOS_EventWrite(&g_link_status_event, LINK_STATUS_DOWN_EVENT);
    }
}

static void link_status_wait(void)
{
  (void)LOS_EventWrite(&g_link_status_event, UNLOAD_STATUS_EVENT);
  (void)LOS_EventRead(&g_link_status_event, EXIT_STATUS_EVENT, LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
}

static void tx_complete_event_init(void)
{
  (void)LOS_EventInit(&g_tx_complete_event);
}

static void tx_complete_event_deinit(void)
{
  (void)LOS_EventDestroy(&g_tx_complete_event);
}

void tx_complete_event_write(void)
{
  (void)LOS_EventWrite(&g_tx_complete_event, TX_COMPLETE_EVENT);
}

static void tx_complete_check_read(void)
{
  uint32_t ret;
  if (LOS_AtomicRead(&g_buf_count) >= RNDIS_MAX_BUFFER_COUNT)
    {
      ret = LOS_EventRead(&g_tx_complete_event, TX_COMPLETE_EVENT, LOS_WAITMODE_OR |
                          LOS_WAITMODE_CLR, RNDIS_MAX_TX_TIMEOUT);
      if (ret == LOS_ERRNO_EVENT_READ_TIMEOUT)
        {
          usb_err("tx timeout!\n");
        }
    }
}

void tx_complete_check_write(void)
{
  /**
   * If the disconnect interrupt is triggered during packet transfer, it might set g_buf_count to 0.
   * So, it is necessary to prevent g_buf_count from turning over after being reduced by 1.
   */
  if ((g_buf_count == 0) || (LOS_AtomicDecRet(&g_buf_count) <= RNDIS_MIN_BUFFER_COUNT))
    {
      tx_complete_event_write();
    }
}

void rndis_buf_count_init(void)
{
  LOS_AtomicSet(&g_buf_count, 0);
}

void rndis_buf_count_inc(void)
{
  LOS_AtomicInc(&g_buf_count);
}

void rndis_transfer_flag_reset(void)
{
  LOS_AtomicSet(&g_tx_transferring, 0);
}

void rndis_transfer_flag_set(void)
{
  LOS_AtomicSet(&g_tx_transferring, 1);
}

int rndis_transfer_flag_get(void)
{
  return LOS_AtomicRead(&g_tx_transferring);
}

/* If the data to be sent is in the sg list, traverse the sg list of g_netdev in advance,
 * and set buf to NULL and buflen to 0.
 * If there is only one space for the data to be transmitted,
 * directly call this API and transfer the corresponding buf and buflen.
 */

void rndis_send_data(const uint8_t *buf, uint32_t buflen)
{
  struct net_driver_s *net = g_netdev;

  if (LOS_AtomicCmpXchg32bits(&g_rndis_status, RNDIS_STATUS_BUSY, RNDIS_STATUS_IDLE))
    {
      usb_err("rndis is busy or not load.\n");
      return;
    }

  if (buf != NULL && buflen != 0)
    {
      net->buf[0] = (UINTPTR)buf;
      net->len[0] = buflen;
      net->sg_len = 1;
      net->total_len = buflen;
    }
  rndis_tx(net);

  /* In some situation when UDP transfer too fast, we need to block a while
   * until the existing buffers transfer complete.
   */

  tx_complete_check_read();
  if (LOS_AtomicCmpXchg32bits(&g_rndis_status, RNDIS_STATUS_IDLE, RNDIS_STATUS_BUSY))
    {
      (void)LOS_EventWrite(&g_tx_complete_event, RNDIS_UNBIND_EVENT);
    }
}

void rndis_change_connect_status(bool connect)
{
  struct net_driver_s *netdev = g_netdev;

  if (LOS_AtomicCmpXchg32bits(&g_rndis_status, RNDIS_STATUS_BUSY, RNDIS_STATUS_IDLE))
    {
      usb_err("rndis is busy or not load.\n");
      return;
    }

  if (connect)
    {
      netdev->d_ifup(netdev);
    }
  else
    {
      netdev->d_ifdown(netdev);
    }
  rndis_link_status_change(connect);

  if (LOS_AtomicCmpXchg32bits(&g_rndis_status, RNDIS_STATUS_IDLE, RNDIS_STATUS_BUSY))
    {
      (void)LOS_EventWrite(&g_tx_complete_event, RNDIS_UNBIND_EVENT);
    }
}

#ifdef CONFIG_USBDEV_SUPPORT_CERTIFY
static int16_t rndis_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo,
                               uint8_t speed, uint8_t type)
{
  (void)speed;
  (void)type;
#else
static int16_t rndis_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo)
{
#endif
  int16_t total_len = 0;
  int16_t len       = USB_SIZEOF_CFGDESC;
  errno_t ret;

  (void)devinfo;

  /* Copy RNDIS device configure descriptor. */
  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, g_rndis_hs_desc.rndis_config, (uint16_t)len);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail, ret:%d\n", ret);
      return -1;
    }
  total_len += len;

  /* Copy RNDIS device configure descriptor. */

  buf += USB_SIZEOF_CFGDESC;
  len = sizeof(g_rndis_hs_func_desc);
  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ - (uint16_t)total_len, g_rndis_hs_desc.rndis_func, (uint16_t)len);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail, ret:%d\n", ret);
      return -1;
    }
  total_len += len;

  return total_len;
}

static int rndis_mkstrdesc(uint8_t id, struct usb_strdesc_s *buf)
{
  const char *str;
  errno_t ret;
  int i;

  for (i = 0; g_fether_device_strings[i].s != NULL; i++)
    {
      str = g_fether_device_strings[i].s;
      if (g_fether_device_strings[i].id == id)
        {
          ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, str, str[0]);
          if (ret != EOK)
            {
              usb_err("memcpy_s failed, ret = %d\n", ret);
              return -1;
            }
          return str[0];
        }
    }

  usb_err("Can not find the id = %u of string\n", id);
  return -1;
}

static void rndis_mkdevdesc(uint8_t *buf)
{
  errno_t ret;

  if (USB_GETW(g_fether_device_desc.vendor) == 0)
    {
      usb_err("VID is not set!\n");
      return;
    }

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_fether_device_desc, sizeof(g_fether_device_desc));
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return;
    }
}

void rndis_get_composite_devdesc(struct composite_devdesc_s *dev)
{
  (void)memset_s(dev, sizeof(struct composite_devdesc_s), 0, sizeof(struct composite_devdesc_s));

  dev->mkdevdesc  = rndis_mkdevdesc;
  dev->mkconfdesc = rndis_mkcfgdesc;
  dev->mkstrdesc  = rndis_mkstrdesc;
  dev->nconfigs   = ETHER_NCONFIGS; /* Number of configurations supported */
  dev->configid   = ETHER_CONFIGID; /* The only supported configuration ID */

  dev->devinfo.ninterfaces = ETHER_NINTERFACES; /* Number of interfaces in the configuration */
  dev->devinfo.nstrings    = ETHER_NSTRIDS;     /* Number of Strings */
  dev->devinfo.nendpoints  = ETHER_NUM_EPS;
}

struct usbdev_ep_s *rndis_alloc_ep_interrupt(struct usbdev_s *dev)
{
  return DEV_ALLOCEP(dev, g_rndis_hs_func_desc.nepd.addr,
                     (struct usb_epdesc_s *)&g_rndis_hs_func_desc.nepd);
}

struct usbdev_ep_s *rndis_alloc_ep_bulkin(struct usbdev_s *dev)
{
  return DEV_ALLOCEP(dev, g_rndis_hs_func_desc.iepd.addr,
                     (struct usb_epdesc_s *)&g_rndis_hs_func_desc.iepd);
}

struct usbdev_ep_s *rndis_alloc_ep_bulkout(struct usbdev_s *dev)
{
  return DEV_ALLOCEP(dev, g_rndis_hs_func_desc.oepd.addr,
                     (struct usb_epdesc_s *)&g_rndis_hs_func_desc.oepd);
}

int rndis_ep_config_interrupt(struct usbdev_ep_s *ep)
{
  return EP_CONFIGURE(ep, &g_rndis_hs_func_desc.nepd, false);
}

int rndis_ep_config_bulkin(struct usbdev_ep_s *ep, struct usbdev_s *dev)
{
  usbd_configep_byspeed(dev, &g_rndis_hs_func_desc.iepd, USB_DESC_TYPE_CONFIG);
  return EP_CONFIGURE(ep, &g_rndis_hs_func_desc.iepd, false);
}

int rndis_ep_config_bulkout(struct usbdev_ep_s *ep, struct usbdev_s *dev)
{
  usbd_configep_byspeed(dev, &g_rndis_hs_func_desc.oepd, USB_DESC_TYPE_CONFIG);
  return EP_CONFIGURE(ep, &g_rndis_hs_func_desc.oepd, true);
}

int rndis_bind_extend(struct usbdev_devinfo_s *devinfo, struct net_driver_s *netdev)
{
  g_rndis_hs_func_desc.ifcad.firstif = (uint8_t)devinfo->ifnobase;
  g_rndis_hs_func_desc.ifcd.ifno = (uint8_t)devinfo->ifnobase;
  g_rndis_hs_func_desc.cdc_union_desc.bMasterInterface = (uint8_t)devinfo->ifnobase;

  g_rndis_hs_func_desc.ifdd.ifno = (uint8_t)(devinfo->ifnobase + 1);
  g_rndis_hs_func_desc.cdc_union_desc.bSlaveInterface0 = (uint8_t)(devinfo->ifnobase + 1);

  netdev_register(netdev);
  if (link_status_check_init() != LOS_OK)
    {
      netdev_unregister(netdev);
      return -EFAULT;
    }
  tx_complete_event_init();

  g_netdev = netdev;
  LOS_AtomicSet(&g_rndis_status, RNDIS_STATUS_IDLE);
  return OK;
}

void rndis_unbind_extend(struct net_driver_s *netdev)
{
  if (LOS_AtomicRead(&g_rndis_status) == RNDIS_STATUS_INIT)
    {
      return;
    }
  if (LOS_AtomicCmpXchg32bits(&g_rndis_status, RNDIS_STATUS_INIT, RNDIS_STATUS_IDLE))
    {
      LOS_AtomicSet(&g_rndis_status, RNDIS_STATUS_INIT);
      (void)LOS_EventRead(&g_tx_complete_event, RNDIS_UNBIND_EVENT, LOS_WAITMODE_OR |
                          LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
    }

  g_netdev = NULL;
  link_status_wait();
  tx_complete_event_deinit();
  link_status_check_deinit();
  netdev_unregister(netdev);
}

int usbdev_rndis_initialize(void *handle)
{
  struct composite_devdesc_s dev;
  int ret;

  /* Ask the ETH driver to fill in the constants we didn't
   * know here.
   */

  rndis_get_composite_devdesc(&dev);

  usbdev_rndis_initialize_sub(&dev, 0, DEV_ETHERNET);

  ret = composite_initialize_softc(1, &dev, handle);
  if (ret < 0)
    {
      PRINTK("  ** Rndis device initialized failed! **\n");
      return -1;
    }
  PRINTK("  ** Rndis device initialized successfully! **\n");
  return 0;
}

void rndis_workqueue_init(struct work_s *work, worker_t worker)
{
  INIT_WORK(work, worker);
}

int rndis_workqueue_create(void)
{
  g_workqueue = create_workqueue("rndis workqueue");
  if (g_workqueue == NULL)
    {
      return -EFAULT;
    }
  return OK;
}

void rndis_workqueue_destroy(void)
{
  destroy_workqueue(g_workqueue);
  g_workqueue = NULL;
}

int work_queue(int qid, FAR struct work_s *work, worker_t worker,
               FAR void *arg, clock_t delay)
{
  (void)qid;
  (void)worker;
  (void)arg;
  (void)delay;

  return queue_work(g_workqueue, work);
}
