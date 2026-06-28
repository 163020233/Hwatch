/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LiteOS USB Driver ECM
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

#include "f_ecm.h"

struct ecm_dev_s
{
  struct net_driver_s netdev;
  spinlock_t ecm_lock;                 /* Spinlock */
  struct usbdev_s *usbdev;             /* Usb device pointer */
  struct usbdev_devinfo_s devinfo;
  bool connect;                        /* Flags of device action */
  struct usbdev_ep_s *epintin;         /* Interrupt IN endpoint structure */
  struct usbdev_ep_s *epbulkin;        /* Bulk IN endpoint structure */
  struct usbdev_ep_s *epbulkout;       /* Bulk OUT endpoint structure */
  bool epintin_enabled;                /* Interrupt IN endpoint enable flag */
  bool epbulkin_enabled;               /* Bulk IN endpoint enable flag */
  bool epbulkout_enabled;              /* Bulk OUT endpoint enable flag */
  bool rx_task_enable;
  struct usbdev_req_s *ctrlreq;        /* control request */
  struct usbdev_req_s *wrreqs;         /* ECM write buffer */
  struct usbdev_req_s *rdreqs;         /* ECM read buffer */
  EVENT_CB_S tx_event;                 /* tx event */
  EVENT_CB_S rx_event;                 /* rx event */
  uint16_t cdc_filter;                 /* Ethernet packet filtering */
  uint8_t ctrl_id;                     /* Control id of Interface-IDs */
  uint8_t data_id;                     /* Data id of Interface-IDs */
};

struct ecm_driver_s
{
  struct usbdevclass_driver_s drvr;
  struct ecm_dev_s dev;
};

static int usbclass_ecm_bind(struct usbdevclass_driver_s *driver,
                             struct usbdev_s *dev);
static int usbclass_ecm_unbind(struct usbdevclass_driver_s *driver,
                               struct usbdev_s *dev);
static int usbclass_ecm_setup(struct usbdevclass_driver_s *driver,
                              struct usbdev_s *dev,
                              const struct usb_ctrlreq_s *ctrl, uint8_t *dataout,
                              size_t outlen);
static void usbclass_ecm_disconnect(struct usbdevclass_driver_s *driver,
                                    struct usbdev_s *dev);

static void usbdev_ecm_mkdevdesc(uint8_t *buf);
static int usbdev_ecm_mkstrdesc(uint8_t id, struct usb_strdesc_s *buf);
#ifdef CONFIG_USBDEV_SUPPORT_CERTIFY
static int16_t usbdev_ecm_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo,
                                    uint8_t speed, uint8_t type);
#else
static int16_t usbdev_ecm_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo);
#endif

static void ecm_ep_req_deinit(struct usbdevclass_driver_s *driver,
                              struct usbdev_s *dev);

static int ecm_network_connect(struct ecm_dev_s *priv, bool is_connect);
static struct ecm_dev_s *ecm_get_priv(void);
static void ecm_free_ep_req(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);

static struct ecm_dev_s *g_ecm_dev = NULL;
static Atomic g_ecm_status = ECM_STATUS_INIT;

static const struct usbdevclass_driverops_s g_ecm_driverops =
{
  usbclass_ecm_bind,       /* bind */
  usbclass_ecm_unbind,     /* unbind */
  usbclass_ecm_setup,      /* setup */
  usbclass_ecm_disconnect, /* disconnect */
  NULL,                    /* suspend */
  NULL,                    /* resume */
};

static struct usb_devdesc_s g_ecm_device_desc =
{
  .len          = sizeof(struct usb_devdesc_s),
  .type         = USB_DESC_TYPE_DEVICE,
  HSETW(.usb,   USB_VERSION_BCD), /* Usb version */
  .classid      = USB_CLASS_CDC,  /* 0x02 */
  .subclass     = 0x6,            /* subclass code: ETHERNET */
  .protocol     = 0,
  .mxpacketsize = USB_EP_MPS,
  HSETW(.vendor,  0),             /* Vendor id */
  HSETW(.product, 0),             /* Product id */
  HSETW(.device, DEVICE_VERSION), /* Device version */
  .imfgr        = 1,
  .iproduct     = 2,
  .serno        = 3,
  .nconfigs     = 1,
};


struct ecm_hs_function_descriptor g_ecm_hs_func_desc =
{
  {
    .len       = sizeof(struct usb_iaddesc_s),
    .type      = USB_DESC_TYPE_INTERFACEASSOCIATION,
    .firstif   = 0,
    .nifs      = 2,
    .classid   = 2, /* UFCLASS_CDC */
    .subclass  = USB_CDC_SUBCLASS_ECM,
    .protocol  = 0,
    .ifunction = 6,
  },
  {
    .len      = sizeof(struct usb_ifdesc_s),
    .type     = USB_DESC_TYPE_INTERFACE,
    .ifno     = 0,
    .alt      = 0,
    .neps     = 0x1,
    .classid  = USB_CLASS_CDC,
    .subclass = USB_CDC_SUBCLASS_ECM,
    .protocol = 0,
    .iif      = 6
  },
  {
    .bFunctionLength    = sizeof(struct cdc_header_func_desc),
    .bDescriptorType    = USB_DESC_TYPE_CSINTERFACE,
    .bDescriptorSubtype = USB_CDC_HEADER_FUNC,
    .bcdCDC             = 0x0110
  },
  {
    .bFunctionLength    = sizeof(struct cdc_union_func_desc),
    .bDescriptorType    = USB_DESC_TYPE_CSINTERFACE,
    .bDescriptorSubtype = USB_CDC_UNION_FUNC,
    .bMasterInterface   = 0,
    .bSlaveInterface0   = 1
  },
  {
    .bFunctionLength      = sizeof(struct cdc_ecm_func_desc),
    .bDescriptorType      = USB_DESC_TYPE_CSINTERFACE,
    .bDescriptorSubtype   = USB_CDC_ECM_FUNC,
    .iMACAddress          = 5,
    .bmEthernetStatistics = 0x0,
    .wMaxSegmentSize      = ECM_NET_PKTSIZE,
    .wNumberMCFilters     = 0x0,
    .bNumberPowerFilters  = 0x0
  },
  {
    .len      = sizeof(struct usb_epdesc_s),
    .type     = USB_DESC_TYPE_ENDPOINT,
    .addr     = USB_DIR_IN,
    .attr     = USB_EP_ATTR_XFER_INT,
    HSETW(.mxpacketsize, 16),
    .interval = 5
  },
#ifdef USB3_DEVICE_CONTROLLER
  {
    .len      = 6,
    .type     = 0x30,
    .maxburst = 0,
    .attr     = 0,
    .interval = {0}
  },
#endif
  {
    .len      = sizeof(struct usb_ifdesc_s),
    .type     = USB_DESC_TYPE_INTERFACE,
    .ifno     = 1,
    .alt      = 0,
    .neps     = 2,
    .classid  = USB_CLASS_CDC_DATA,
    .subclass = 0,
    .protocol = 0x0,
    .iif      = 6
  },
  {
    .len      = sizeof(struct usb_epdesc_s),
    .type     = USB_DESC_TYPE_ENDPOINT,
    .addr     = USB_DIR_IN,
    .attr     = USB_EP_ATTR_XFER_BULK,
    HSETW(.mxpacketsize, ECM_MAX_PACKET_SIZE),
    .interval = 0
  },
#ifdef USB3_DEVICE_CONTROLLER
  {
    .len      = 6,
    .type     = 0x30,
    .maxburst = 0,
    .attr     = 0,
    .interval = {0}
  },
#endif
  {
    .len      = sizeof(struct usb_epdesc_s),
    .type     = USB_DESC_TYPE_ENDPOINT,
    .addr     = USB_DIR_OUT,
    .attr     = USB_EP_ATTR_XFER_BULK,
    HSETW(.mxpacketsize, ECM_MAX_PACKET_SIZE),
    .interval = 0
  },
#ifdef USB3_DEVICE_CONTROLLER
  {
    .len      = 6,
    .type     = 0x30,
    .maxburst = 0,
    .attr     = 0,
    .interval = {0}
  }
#endif
};

/* Config contain func */

static const struct usb_cfgdesc_s g_ecm_hs_config_desc =
{
  .len         = USB_SIZEOF_CFGDESC,
  .type        = USB_DESC_TYPE_CONFIG,
  HSETW(.totallen, USB_SIZEOF_CFGDESC + sizeof(g_ecm_hs_func_desc)),
  .ninterfaces = 2,
  .cfgvalue    = 1,
  .icfg        = 4,
  .attr        = USB_CONFIG_ATTR_SELFPOWER | USB_CONFIG_ATTR_ONE, /* 0xc0 */
  .mxpower     = 1 /* Max power */
};

#define DT_STRING_ID_LEN 4
static const char g_dt_string_id[DT_STRING_ID_LEN] =
{
  DT_STRING_ID_LEN,
  USB_DESC_TYPE_STRING,
  0x09, 0x04
};

#define DT_STRING_4_LEN 30
static const char g_dt_string_4[DT_STRING_4_LEN] =
{
  DT_STRING_4_LEN,
  USB_DESC_TYPE_STRING,
  0x43, 0, 0x44, 0, 0x43, 0, 0x20, 0, 0x45, 0, 0x43, 0, 0x4d, 0,
  0x20, 0, 0x63, 0, 0x6f, 0, 0x6e, 0, 0x66, 0, 0x69, 0, 0x67, 0
};

#define DT_STRING_5_LEN 26
static char g_dt_string_5[DT_STRING_5_LEN] =
{
  DT_STRING_5_LEN,
  USB_DESC_TYPE_STRING,
  0x33, 0, 0x61, 0, 0x38, 0, 0x32, 0, 0x64, 0, 0x36, 0, 0x36, 0, 0x36, 0, 0x31, 0, 0x66, 0,
  0x33, 0, 0x61, 0
};

#define DT_STRING_6_LEN 26
static const char g_dt_string_6[DT_STRING_6_LEN] =
{
  DT_STRING_6_LEN,
  USB_DESC_TYPE_STRING,
  0x43, 0, 0x44, 0, 0x43, 0, 0x20, 0, 0x45, 0,
  0x43, 0, 0x4d, 0, 0x20, 0, 0x44, 0, 0x61, 0, 0x74, 0, 0x61, 0
};

#define ECM_STR_NUM 8
static struct usbd_string g_ecm_device_strings[ECM_STR_NUM] =
{
  { 0, g_dt_string_id },
  { 1, NULL },
  { 2, NULL },
  { 3, NULL },
  { 4, g_dt_string_4 },
  { 5, g_dt_string_5 },
  { 6, g_dt_string_6 },
  USBD_DEVICE_STRINGS_END
};

static struct ecm_dev_s *ecm_get_priv(void)
{
  return g_ecm_dev;
}

void ecm_get_device_dec_info(struct usbd_string **device_strings,
                             struct usb_devdesc_s **device_desc)
{
  *device_strings = &g_ecm_device_strings[1];
  *device_desc    = &g_ecm_device_desc;
}

static void ecm_reset_eps_config(struct ecm_dev_s *priv)
{
  if (priv->epintin_enabled == true)
    {
      EP_DISABLE(priv->epintin);
      priv->epintin_enabled = false;
    }

  if (priv->epbulkin_enabled == true)
    {
      EP_DISABLE(priv->epbulkin);
      priv->epbulkin_enabled = false;
    }

  if (priv->epbulkout_enabled == true)
    {
      EP_DISABLE(priv->epbulkout);
      priv->epbulkout_enabled = false;
    }
}

static int ecm_set_ep_config(struct usbdev_ep_s *ep,
                             enum ecm_ep_index epid,
                             const struct usb_epdesc_s *ep_desc,
                             struct usbdev_s *dev,
                             bool last)
{
  if (epid != ECM_INTERRUPT_EP_IDX)
    {
      usbd_configep_byspeed(dev, (struct usb_epdesc_s *)ep_desc, USB_DESC_TYPE_CONFIG);
    }
  return EP_CONFIGURE(ep, ep_desc, last);
}

static int ecm_send_out_req(struct ecm_dev_s *priv, struct usbdev_req_s *rdreq)
{
  struct usbdev_ep_s *ep;
  uint32_t flags;
  int ret = 0;

  spin_lock_irqsave(&priv->ecm_lock, flags);
  ep         = priv->epbulkout;
  rdreq->len = ECM_NET_OUT_PKTSIZE;
  ret        = EP_SUBMIT(ep, rdreq);
  spin_unlock_irqrestore(&priv->ecm_lock, flags);
  return ret;
}

static int ecm_set_config(struct ecm_dev_s *priv, struct usbdev_s *dev)
{
  struct usbdev_req_s *rdcontainer;
  struct ecm_hs_function_descriptor *func_desc;
  int ret;

  priv->connect = false;
  link_status_check_func(priv->connect);

  rdcontainer = priv->rdreqs;

  if (priv->epintin_enabled == true)
    {
      EP_DISABLE(priv->epintin);
      priv->epintin_enabled = false;
    }

  func_desc = &g_ecm_hs_func_desc;
  ret = ecm_set_ep_config(priv->epintin, ECM_INTERRUPT_EP_IDX,
                          (const struct usb_epdesc_s *)&func_desc->nepd,
                          dev, false);
  if (ret < 0)
    {
      usb_err("config interrupt ep failed!\n");
      goto errout;
    }

  priv->epintin_enabled = true;

  if (priv->epbulkin_enabled == true)
    {
      EP_DISABLE(priv->epbulkin);
      priv->epbulkin_enabled = false;
    }

  ret = ecm_set_ep_config(priv->epbulkin, ECM_BULKIN_EP_IDX,
                          (const struct usb_epdesc_s *)&func_desc->iepd,
                          dev, false);
  if (ret < 0)
    {
      usb_err("config bulk in ep failed!\n");
      goto errout;
    }

  priv->epbulkin_enabled = true;

  if (priv->epbulkout_enabled == true)
    {
      EP_DISABLE(priv->epbulkout);
      priv->epbulkout_enabled = false;
    }

  ret = ecm_set_ep_config(priv->epbulkout, ECM_BULKOUT_EP_IDX,
                          (const struct usb_epdesc_s *)&func_desc->oepd,
                          dev, false);
  if (ret < 0)
    {
      usb_err("config bulk out ep failed!\n");
      goto errout;
    }

  priv->epbulkout_enabled = true;

  ret = ecm_send_out_req(priv, rdcontainer);
  if (ret != LOS_OK)
    {
      goto errout;
    }

  priv->connect = true;
  link_status_check_func(priv->connect);
  return LOS_OK;

errout:
  ecm_reset_eps_config(priv);
  return ret;
}

static struct usbdev_req_s *ecm_alloc_req(struct usbdev_ep_s *ep, uint16_t len)
{
  struct usbdev_req_s *req;

  if (ep == NULL)
    {
      return NULL;
    }

  req = EP_ALLOCREQ(ep);
  if (req == NULL)
    {
      return NULL;
    }

  req->len = len;
  req->buf = EP_ALLOCBUFFER(ep, len);
  if (req->buf == NULL)
    {
      EP_FREEREQ(ep, req);
      req = NULL;
    }

  return req;
}

static void ecm_free_req(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  if (ep == NULL || req == NULL)
    {
      return;
    }

  if (req->buf != NULL)
    {
      EP_FREEBUFFER(ep, req->buf);
    }

  EP_FREEREQ(ep, req);
}

static int ecm_init_eps(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct ecm_dev_s *priv = &((struct ecm_driver_s *)driver)->dev;
  struct composite_dev_s *cdev;
  struct composite_devobj_s *devobj;
  struct usbdev_devinfo_s *devinfo;
  struct ecm_hs_function_descriptor *func_desc;
  int ret;

  priv->usbdev = dev;

  cdev = dev->ep0->priv;
  priv->connect = false;
  (void)LOS_EventInit(&priv->tx_event);
  (void)LOS_EventInit(&priv->rx_event);
  spin_lock_init(&priv->ecm_lock);

  devobj = usbclass_devobj_get(cdev, DEV_ECM);
  if (devobj == NULL)
    {
      ret = -EINVAL;
      goto errout;
    }

  devinfo = &devobj->compdesc.devinfo;

  func_desc = &g_ecm_hs_func_desc;

  priv->epintin = DEV_ALLOCEP(dev, func_desc->nepd.addr, &func_desc->nepd);
  if (priv->epintin == NULL)
    {
      usb_err("alloc epintin fail!");
      ret = -ENODEV;
      goto errout;
    }

  ECM_SET_INTERRUPT_EP_NUM(devinfo) = priv->epintin->eplog;

  priv->epbulkin = DEV_ALLOCEP(dev, func_desc->iepd.addr, &func_desc->iepd);
  if (priv->epbulkin == NULL)
    {
      usb_err("alloc epbulkin fail!\n");
      ret = -ENODEV;
      goto errout;
    }

  ECM_SET_BULKIN_EP_NUM(devinfo) = priv->epbulkin->eplog;
  priv->epbulkin->priv = priv;

  priv->epbulkout = DEV_ALLOCEP(dev, func_desc->oepd.addr, &func_desc->oepd);
  if (priv->epbulkout == NULL)
    {
      usb_err("alloc epbulkout fail!");
      ret = -ENODEV;
      goto errout;
    }

  ECM_SET_BULKOUT_EP_NUM(devinfo) = priv->epbulkout->eplog;
  priv->epbulkout->priv = priv;

  return LOS_OK;

errout:
  return ret;
}

static void ecm_intin_notify_callback(struct usbdev_ep_s *ep,
                                      struct usbdev_req_s *req)
{
  (void)ep;
  (void)req;
  return;
}

static void ecm_rdcomplete_callback(struct usbdev_ep_s *ep,
                                    struct usbdev_req_s *req)
{
  struct ecm_dev_s *priv = (struct ecm_dev_s *)ep->priv;

  if (req->result == LOS_OK)
    {
      (void)LOS_EventWrite(&priv->rx_event, ECM_RX_EVENT_RECEIVE);
    }
}

static void ecm_wrcomplete_callback(struct usbdev_ep_s *ep,
                                    struct usbdev_req_s *req)
{
  struct ecm_dev_s *priv = (struct ecm_dev_s *)ep->priv;

  ecm_free_req(priv->epbulkin, priv->wrreqs);
  priv->wrreqs = NULL;
  if (req->result == LOS_OK)
    {
      (void)LOS_EventWrite(&priv->tx_event, ECM_TX_COMPLETE_EVENT);
    }
}

static int ecm_reqs_init(struct usbdevclass_driver_s *driver)
{
  struct ecm_dev_s *priv = &((struct ecm_driver_s *)driver)->dev;
  struct usbdev_ep_s *epintin;
  struct usbdev_ep_s *epbulkout;
  struct usbdev_req_s *rdreq;
  uint16_t reqlen;

  /* initialize interrupt req */

  epintin = priv->epintin;
  epintin->handle_req = ecm_alloc_req(epintin,
                                      sizeof(struct cdc_notification_element) + sizeof(uint16_t));
  if (epintin->handle_req == NULL)
    {
      return -1;
    }

  epintin->handle_req->callback = ecm_intin_notify_callback;

  /* initialize read req */

  reqlen = ECM_NET_OUT_PKTSIZE;

  epbulkout = priv->epbulkout;
  epbulkout->maxpacket = reqlen;

  priv->rdreqs = ecm_alloc_req(epbulkout, reqlen);
  if (priv->rdreqs == NULL)
    {
      usb_err("alloc rd_req failed!\n");
      return -1;
    }
  rdreq = priv->rdreqs;

  rdreq->priv = priv;
  rdreq->callback = ecm_rdcomplete_callback;

  return LOS_OK;
}

static int ecm_ep_req_init(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  int ret;

  ret = ecm_init_eps(driver, dev);
  if (ret != LOS_OK)
    {
      return ret;
    }

  ret = ecm_reqs_init(driver);
  if (ret != LOS_OK)
    {
      return ret;
    }

  return LOS_OK;
}

static void ecm_transmit(struct ecm_dev_s *priv)
{
  struct net_driver_s *netdev = &priv->netdev;
  uint32_t flags;
  uint8_t *data_buf;
  uint32_t len = 0;
  int ret;

  if (netdev->total_len == 0 || netdev->total_len > ECM_NET_PKTSIZE)
    {
      return;
    }

  priv->wrreqs = ecm_alloc_req(priv->epbulkin, netdev->total_len);
  if (priv->wrreqs == NULL)
    {
      usb_err("alloc wr_req failed!\n");
      return;
    }

  if (priv->wrreqs->len % priv->epbulkin->maxpacket == 0)
    {
      priv->wrreqs->zero = 1;
    }

  priv->wrreqs->priv = priv;
  priv->wrreqs->callback = ecm_wrcomplete_callback;
  data_buf = priv->wrreqs->buf;

  for (uint32_t i = 0; i < netdev->sg_len; i++)
    {
      ret = memcpy_s((void *)(data_buf + len), netdev->total_len - len,
                     (const void *)(netdev->buf[i]), netdev->len[i]);
      if (ret != EOK)
        {
          usb_err("memcpy_s failed!\n");
          ecm_free_req(priv->epbulkin, priv->wrreqs);
          priv->wrreqs = NULL;
          return;
        }
      len += netdev->len[i];
    }

  spin_lock_irqsave(&priv->ecm_lock, flags);
  if (!priv->connect)
    {
      usb_err("ecm is disconnect.\n");
      goto TX_EXIT;
    }

  ret = EP_SUBMIT(priv->epbulkin, priv->wrreqs);
  if (ret != LOS_OK)
    {
      usb_err("submit failed!\n");
      goto TX_EXIT;
    }

  spin_unlock_irqrestore(&priv->ecm_lock, flags);
  return;

TX_EXIT:
  ecm_free_req(priv->epbulkin, priv->wrreqs);
  priv->wrreqs = NULL;
  spin_unlock_irqrestore(&priv->ecm_lock, flags);
}

void ecm_send_data(const uint8_t *buf, uint32_t buflen)
{
  struct ecm_dev_s *priv = ecm_get_priv();
  uint32_t ret;

  if (LOS_AtomicCmpXchg32bits(&g_ecm_status, ECM_STATUS_BUSY, ECM_STATUS_IDLE))
    {
      usb_err("ECM is busy or not load.\n");
      return;
    }

  if (buf != NULL && buflen != 0)
    {
      priv->netdev.buf[0] = (UINTPTR)buf;
      priv->netdev.len[0] = buflen;
      priv->netdev.sg_len = 1;
      priv->netdev.total_len = buflen;
    }

  ecm_transmit(priv);

  ret = LOS_EventRead(&priv->tx_event, ECM_TX_COMPLETE_EVENT, LOS_WAITMODE_OR |
                      LOS_WAITMODE_CLR, ECM_MAX_TX_TIMEOUT);
  if (ret == LOS_ERRNO_EVENT_READ_TIMEOUT)
    {
      usb_err("tx timeout!\n");
    }

  if (LOS_AtomicCmpXchg32bits(&g_ecm_status, ECM_STATUS_IDLE, ECM_STATUS_BUSY))
    {
      (void)LOS_EventWrite(&priv->tx_event, ECM_UNBIND_EVENT);
    }
}

void ecm_change_connect_status(bool connect)
{
  struct ecm_dev_s *priv = ecm_get_priv();

  if (LOS_AtomicCmpXchg32bits(&g_ecm_status, ECM_STATUS_BUSY, ECM_STATUS_IDLE))
    {
      usb_err("ECM is busy or not load.\n");
      return;
    }

  (void)ecm_network_connect(priv, connect);
  link_status_check_func(connect);

  if (LOS_AtomicCmpXchg32bits(&g_ecm_status, ECM_STATUS_IDLE, ECM_STATUS_BUSY))
    {
      (void)LOS_EventWrite(&priv->tx_event, ECM_UNBIND_EVENT);
    }
}

static void ecm_work_task(void)
{
  struct ecm_dev_s *priv = ecm_get_priv();
  uint32_t ret;

  for (;;)
    {
      ret = LOS_EventRead(&priv->rx_event, ECM_RX_EVENT_EXIT | ECM_RX_EVENT_RECEIVE,
                          LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
      if (ret & ECM_RX_EVENT_EXIT)
        {
          break;
        }
      if (ret & ECM_RX_EVENT_RECEIVE)
        {
          eth_rx(priv->rdreqs->buf, priv->rdreqs->xfrd);
          (void)EP_SUBMIT(priv->epbulkout, priv->rdreqs);
        }
    }

  (void)LOS_EventWrite(&priv->rx_event, ECM_RX_EVENT_UNLOAD);
}

static int ecm_work_task_create(void)
{
  uint32_t ret;
  TSK_INIT_PARAM_S param;
  uint32_t ecm_task_id;

  (void)memset_s(&param, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));

  param.pfnTaskEntry = (TSK_ENTRY_FUNC)ecm_work_task;
  param.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
  param.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
  param.pcName = "ECM_Work_Task";
  param.uwResved = LOS_TASK_STATUS_DETACHED;

  ret = LOS_TaskCreate(&ecm_task_id, &param);
  if (ret != LOS_OK)
    {
      usb_err("ECM_Work_Task create failed!\n");
      return -1;
    }
  return LOS_OK;
}

void ecm_init_macaddr_desc(char *macaddr_desc)
{
  uint8_t macaddr[ECM_MAC_ADDR_LEN] = {0};
  uint8_t left_value;
  uint8_t right_value;
  uint8_t i;

  eth_random_addr(macaddr);

  memset_s(macaddr_desc, DT_STRING_5_LEN, 0, DT_STRING_5_LEN);
  macaddr_desc[0] = DT_STRING_5_LEN;
  macaddr_desc[1] = USB_DESC_TYPE_STRING;

  for (i = 0; i < ECM_MAC_ADDR_LEN; i++)
    {
      left_value = GET_HEX_LEFT_VALUE(macaddr[i]);
      right_value = GET_HEX_RIGHT_VALUE(macaddr[i]);

      macaddr_desc[ECM_DESC_GET_INDEX1(i)] = DIGIT_TO_CHARACTER(left_value);
      macaddr_desc[ECM_DESC_GET_INDEX2(i)] = DIGIT_TO_CHARACTER(right_value);
    }
}

static int usbclass_ecm_bind(struct usbdevclass_driver_s *driver,
                             struct usbdev_s *dev)
{
  struct ecm_dev_s *priv = &((struct ecm_driver_s *)driver)->dev;
  struct composite_devobj_s *devobj;
  struct usbdev_devinfo_s *devinfo;
  struct composite_dev_s *cdev;
  int ret;

  cdev = dev->ep0->priv;
  if (cdev->ctrlreq == NULL)
    {
      return -EINVAL;
    }
  priv->ctrlreq = cdev->ctrlreq;

  devobj = usbclass_devobj_get(cdev, DEV_ECM);
  if (devobj == NULL)
    {
      return -EINVAL;
    }
  devinfo = &devobj->compdesc.devinfo;

  priv->ctrl_id = (uint8_t)devinfo->ifnobase;
  priv->data_id = (uint8_t)devinfo->ifnobase + 1;
  g_ecm_dev = priv;

  ret = ecm_ep_req_init(driver, dev);
  if (ret != LOS_OK)
    {
      goto BIND_ERR;
    }

  ret = ecm_work_task_create();
  if (ret != LOS_OK)
    {
      goto BIND_ERR;
    }
  priv->rx_task_enable = true;

  netdev_register(&priv->netdev);
  ecm_init_macaddr_desc(g_dt_string_5);
  LOS_AtomicSet(&g_ecm_status, ECM_STATUS_IDLE);

  return LOS_OK;

BIND_ERR:
  ecm_ep_req_deinit(driver, dev);
  return ret;
}

static int usbclass_ecm_unbind(struct usbdevclass_driver_s *driver,
                               struct usbdev_s *dev)
{
  struct ecm_dev_s *priv;

  if (driver == NULL || dev == NULL)
    {
      usb_err("no device!\n");
      return -EINVAL;
    }

  priv = &((struct ecm_driver_s *)driver)->dev;

  if ((LOS_AtomicRead(&g_ecm_status) != ECM_STATUS_INIT) &&
      LOS_AtomicCmpXchg32bits(&g_ecm_status, ECM_STATUS_INIT, ECM_STATUS_IDLE))
    {
      LOS_AtomicSet(&g_ecm_status, ECM_STATUS_INIT);
      (void)LOS_EventRead(&priv->tx_event, ECM_UNBIND_EVENT, LOS_WAITMODE_OR |
                          LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
    }

  if (priv->rx_task_enable)
    {
      (void)LOS_EventWrite(&priv->rx_event, ECM_RX_EVENT_EXIT);
      (void)LOS_EventRead(&priv->rx_event, ECM_RX_EVENT_UNLOAD, LOS_WAITMODE_OR |
                          LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
      priv->rx_task_enable = false;
    }

  g_ecm_dev = NULL;
  netdev_unregister(&priv->netdev);
  ecm_ep_req_deinit(driver, dev);
  return LOS_OK;
}

static void usbdev_ecm_uninitialize(struct usbdevclass_driver_s *classdev)
{
  struct ecm_driver_s *drvr = (struct ecm_driver_s *)classdev;

  dev_str_desc_free(&g_ecm_device_strings[1]);

  if (drvr == NULL)
    {
      usb_err("Invalid pointer!\n");
      return;
    }

  free(drvr);
}

static void ecm_free_ep_req(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct ecm_dev_s *priv = &((struct ecm_driver_s *)driver)->dev;

  /* Free the pre-allocated interrupt Request */

  if (priv->epintin != NULL && priv->epintin->handle_req != NULL)
    {
      ecm_free_req(priv->epintin, priv->epintin->handle_req);
      priv->epintin->handle_req = NULL;
    }

  /* Free the interrupt endpoint */

  if (priv->epintin != NULL)
    {
      DEV_FREEEP(dev, priv->epintin);
      priv->epintin = NULL;
    }

  /* Free pre-allocated read request */

  if (priv->rdreqs != NULL)
    {
      ecm_free_req(priv->epbulkout, priv->rdreqs);
      priv->rdreqs = NULL;
    }

  /* Free the bulk_out endpoint */

  if (priv->epbulkout != NULL)
    {
      DEV_FREEEP(dev, priv->epbulkout);
      priv->epbulkout = NULL;
    }

  /* Free pre-allocated write request */

  if (priv->wrreqs != NULL)
    {
      ecm_free_req(priv->epbulkin, priv->wrreqs);
      priv->wrreqs = NULL;
    }

  /* Free the bulk_in endpoint */

  if (priv->epbulkin != NULL)
    {
      DEV_FREEEP(dev, priv->epbulkin);
      priv->epbulkin = NULL;
    }

  /* Destroy read event */

  (void)LOS_EventDestroy(&priv->tx_event);
  (void)LOS_EventDestroy(&priv->rx_event);
}

static int usbdev_ecm_classobject(int minor, struct usbdev_devinfo_s *devinfo,
                                  struct usbdevclass_driver_s **classdev)
{
  struct ecm_dev_s *priv;
  struct ecm_driver_s *ecm_drvr;

  ecm_drvr = (struct ecm_driver_s *)zalloc(sizeof(struct ecm_driver_s));
  if (ecm_drvr == NULL)
    {
      usb_err("malloc failed!\n");
      return -ENOMEM;
    }

  priv = &ecm_drvr->dev;

  (void)memcpy_s(&priv->devinfo, sizeof(struct usbdev_devinfo_s), devinfo, sizeof(struct usbdev_devinfo_s));

  ecm_drvr->drvr.speed = USB_SPEED_HIGH;
  ecm_drvr->drvr.ops   = &g_ecm_driverops;
  *classdev            = &ecm_drvr->drvr;

  priv->netdev.d_private = priv;

  return 0;
}

uint32_t ecm_set_connect_state(struct ecm_dev_s *priv, struct usbdev_req_s *req, bool is_connect)
{
  struct cdc_notification_element *state_buf;

  state_buf = (struct cdc_notification_element *)req->buf;
  state_buf->bmRequestType = USB_REQ_DIR_IN | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
  state_buf->bNotification = CDC_NOTIFY_NETWORK_CONNECTION;
  state_buf->wValue        = is_connect;
  state_buf->wIndex        = priv->ctrl_id;
  state_buf->wLength       = 0;

  return CDC_NOTIFY_SIZE;
}

static int ecm_network_connect(struct ecm_dev_s *priv, bool is_connect)
{
  struct usbdev_req_s *req;
  struct usbdev_ep_s *ep;
  int ret;

  ep  = priv->epintin;
  if (ep == NULL)
    {
      return -EINVAL;
    }

  req = ep->handle_req;
  if (req == NULL)
    {
      return -EINVAL;
    }

  req->len = ecm_set_connect_state(priv, req, is_connect);

  ret = EP_SUBMIT(ep, req);
  if (ret != LOS_OK)
    {
      usb_err("Can not send notify data to host!\n");
    }

  return ret;
}

static void usbclass_ecm_disconnect(struct usbdevclass_driver_s *driver,
                                    struct usbdev_s *dev)
{
  struct ecm_dev_s *priv = &((struct ecm_driver_s *)driver)->dev;

  if (priv->connect)
    {
      priv->connect = false;
      (void)ecm_network_connect(priv, 0);
      ecm_reset_eps_config(priv);
      link_status_check_func(priv->connect);
    }
}

static void ecm_ep_req_deinit(struct usbdevclass_driver_s *driver,
                              struct usbdev_s *dev)
{
  /* Close device */

  usbclass_ecm_disconnect(driver, dev);

  ecm_free_ep_req(driver, dev);
}

static int usbclass_ecm_setup(struct usbdevclass_driver_s *driver,
                              struct usbdev_s *dev,
                              const struct usb_ctrlreq_s *ctrl, uint8_t *dataout,
                              size_t outlen)
{
  struct ecm_dev_s *priv;
  struct usbdev_req_s *ctrlreq;
  int ret = -EOPNOTSUPP;

  if (driver == NULL || dev == NULL || ctrl == NULL)
    {
      return -EINVAL;
    }

  (void)dataout;
  (void)outlen;

  priv = &((struct ecm_driver_s *)driver)->dev;
  ctrlreq = priv->ctrlreq;

  if ((ctrl->type & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_STANDARD)
    {
      switch (ctrl->req)
        {
        case USB_REQ_SETCONFIGURATION:
          {
            return ecm_set_config(priv, dev);
          }

        case USB_REQ_SETINTERFACE:
          {
            return 0;
          }

        default:
          {
            usb_warn("ctrl->req:%u\n", ctrl->req);
            break;
          }
        }
    }
  else if ((ctrl->type & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS)
    {
      switch (ctrl->req)
        {
        case ECM_SET_PACKET_FILTER:
          {
            priv->cdc_filter = ctrl->value[0];
            ret = LOS_OK;
          }
          break;
        default:
          {
            usb_warn("ctrl->req:%u\n", ctrl->req);
            break;
          }
        }
    }

  if (ret >= 0)
    {
      /* Configure the response */

      ctrlreq->len = (uint32_t)ret;
      ret          = EP_SUBMIT(dev->ep0, ctrlreq);
      if (ret != LOS_OK)
        {
          usb_err("ecm response on err %d\n", ret);
          ctrlreq->result = LOS_OK;
        }
    }
  return ret;
}

void usbdev_ecm_initialize_dev(struct composite_devdesc_s *dev, int ifnobase, int minor)
{
  (void)memset_s(dev, sizeof(struct composite_devdesc_s), 0, sizeof(struct composite_devdesc_s));

  /* Determine the necessary interfaces. */

  dev->mkdevdesc  = usbdev_ecm_mkdevdesc;
  dev->mkconfdesc = usbdev_ecm_mkcfgdesc;
  dev->mkstrdesc  = usbdev_ecm_mkstrdesc;

  dev->classobject  = usbdev_ecm_classobject;
  dev->uninitialize = usbdev_ecm_uninitialize;

  /* Init ifnobase of usbdev_devinfo_s for judegment ctrl->index */

  dev->nconfigs = ECM_CONFIG_NUM; /* Number of configurations supported */
  dev->configid = ECM_CONFIG_ID;  /* The only supported configuration ID */
  dev->minor    = minor;          /* The minor interface number */

  dev->devinfo.strbase     = 0;                 /* Offset to String Numbers */
  dev->devinfo.ifnobase    = ifnobase;          /* Offset to Interface-IDs */
  dev->devinfo.ninterfaces = ECM_INTERFACE_NUM; /* Number of interfaces in the configuration */
  dev->devinfo.nstrings    = ECM_STR_ID_NUM;
  dev->devinfo.nendpoints  = ECM_TOTAL_EP_NUM;
}

/* Init composite public action */

int usbdev_ecm_initialize(void *handle)
{
  struct composite_devdesc_s dev;
  int ret;

  usbdev_ecm_initialize_dev(&dev, 0, DEV_ECM);
  ret = composite_initialize_softc(1, &dev, handle);
  if (ret)
    {
      return -1;
    }
  PRINTK(" ** ECM device initialized successfully! **\n ");

  return 0;
}

static void usbdev_ecm_mkdevdesc(uint8_t *buf)
{
  errno_t ret;

  if (USB_GETW(g_ecm_device_desc.vendor) == 0)
    {
      usb_err("VID is not set!\n");
      return;
    }

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_ecm_device_desc, sizeof(g_ecm_device_desc));
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
    }
}

#ifdef CONFIG_USBDEV_SUPPORT_CERTIFY
static int16_t usbdev_ecm_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo,
                                    uint8_t speed, uint8_t type)
{
  (void)speed;
  (void)type;
#else
static int16_t usbdev_ecm_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo)
{
#endif
  (void)devinfo;
  int16_t total_len = 0;
  errno_t ret;

  /* Copy ECM device configure descriptor. */

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, (const void *)&g_ecm_hs_config_desc, USB_SIZEOF_CFGDESC);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail, ret:%d\n", ret);
      return -1;
    }
  total_len += USB_SIZEOF_CFGDESC;

  /* Copy ECM device function descriptor. */

  buf += USB_SIZEOF_CFGDESC;
  ret = memcpy_s(buf, (USB_COMP_EP0_BUFSIZ - USB_SIZEOF_CFGDESC),
                 (const void *)&g_ecm_hs_func_desc, sizeof(g_ecm_hs_func_desc));
  if (ret != EOK)
    {
      usb_err("memcpy_s fail, ret:%d\n", ret);
      return -1;
    }
  total_len += sizeof(g_ecm_hs_func_desc);

  return total_len;
}

static int usbdev_ecm_mkstrdesc(uint8_t id, struct usb_strdesc_s *buf)
{
  errno_t ret;
  const char *str;
  int i;

  for (i = 0; g_ecm_device_strings[i].s != NULL; i++)
    {
      str = g_ecm_device_strings[i].s;
      if (g_ecm_device_strings[i].id == id)
        {
          ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, (const void *)str, (uint32_t)str[0]);
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
