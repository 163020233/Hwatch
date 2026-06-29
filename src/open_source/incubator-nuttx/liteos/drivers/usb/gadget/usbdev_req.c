/****************************************************************************
 * drivers/usbdev/usbdev_req.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/usb/usbdev.h>

#ifndef CONFIG_USBDEV_DMA
#  include <nuttx/kmalloc.h>
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: usbdev_allocreq
 *
 * Description:
 *   Allocate a request instance along with its buffer
 *
 ****************************************************************************/

FAR struct usbdev_req_s *usbdev_allocreq(FAR struct usbdev_ep_s *ep,
                                         size_t len)
{
  FAR struct usbdev_req_s *req;

  req = EP_ALLOCREQ(ep);
  if (req != NULL)
    {
      req->len = len;
#ifdef CONFIG_COMPOSITE_ALT_EPBUF_NONCACHE
      req->buf = EP_ALLOCBUFFER_NONCACHE(ep, len);
#else
      req->buf = EP_ALLOCBUFFER(ep, len);
#endif
      req->flags = USBDEV_REQFLAGS_NULLPKT;

      if (req->buf == NULL)
        {
          EP_FREEREQ(ep, req);
          req = NULL;
        }
    }

  return req;
}

/****************************************************************************
 * Name: usbdev_freereq
 *
 * Description:
 *   Free a request instance along with its buffer
 *
 ****************************************************************************/

void usbdev_freereq(FAR struct usbdev_ep_s *ep,
                    FAR struct usbdev_req_s *req)
{
  if (ep != NULL && req != NULL)
    {
      if (req->buf != NULL)
        {
#ifdef CONFIG_COMPOSITE_ALT_EPBUF_NONCACHE
          EP_FREEBUFFER_NONCACHE(ep, req->buf);
#else
          EP_FREEBUFFER(ep, req->buf);
#endif
        }

      EP_FREEREQ(ep, req);
    }
}
