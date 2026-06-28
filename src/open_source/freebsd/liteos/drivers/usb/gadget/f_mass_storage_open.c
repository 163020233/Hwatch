/* $FreeBSD: releng/14.1/sys/dev/usb/storage/ustorage_fs.c 363664 2023-08-17 01:54:24Z markj $ */
/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (C) 2003-2005 Alan Stern
 * Copyright (C) 2008 Hans Petter Selasky
 * All rights reserved.
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2019. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The names of the above-listed copyright holders may not be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * NOTE: Much of the SCSI statemachine handling code derives from the
 * Linux USB gadget stack.
 */

#include <linux/wait.h>
#include <sys/param.h>
#include <los_mux.h>
#include "gadget/f_mass_storage_pri.h"
#include "storage/storage.h"
#ifdef LOSCFG_USB_MASS_STORAGE_INDEPENDENT_MEMORY_POOL
#include "los_memory.h"
#endif
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
#include "controller/usb_device/pcd.h"
#endif

#undef USB_DEBUG_VAR
#define USB_DEBUG_VAR   g_fmass_debug
#ifdef LOSCFG_USB_DEBUG
static int g_fmass_debug = 0;

void fmass_debug_func(int level)
{
  g_fmass_debug = level;
  PRINTK("The level of usb fmass debug is %d\n", level);
}
DEBUG_MODULE(fmass, fmass_debug_func);
#endif

#ifdef LOSCFG_USB_DEBUG
#define	DPRINTFN(n,fmt,...) do {		\
  if ((USB_DEBUG_VAR) >= (n)) {			\
    PRINTK("%s: " fmt,				\
	   __FUNCTION__ ,##__VA_ARGS__);	\
  }						\
} while (0)
#define	DPRINTF(...)	DPRINTFN(1, __VA_ARGS__)
#else
#define	DPRINTF(...) do { } while (0)
#define	DPRINTFN(...) do { } while (0)
#endif

/* sector size 512 Bytes */
#define SECTOR_SHIFT 9

extern void __init_waitqueue_head(wait_queue_head_t *wait);
extern void __wake_up_interruptible(wait_queue_head_t *wait);

static struct mass_softc *g_fmass = NULL;

static int usbclass_mass_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_mass_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_mass_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                               const struct usb_ctrlreq_s *ctrl, uint8_t *dataout, size_t outlen);
static void usbclass_mass_disconnect(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);

/* USB driver operations */

static const struct usbdevclass_driverops_s g_mass_driverops =
{
  usbclass_mass_bind,
  usbclass_mass_unbind,
  usbclass_mass_setup,
  usbclass_mass_disconnect,
  NULL,
  NULL
};

static int fmass_thread_init(struct mass_dev_s *fmass, struct usbdev_s *dev);
static void fmass_main_thread_signal(struct mass_dev_s *fmass, fmass_task_state state);
static void fmass_task_change_state(struct mass_dev_s *fmass, fmass_task_state new_state);

static int fmass_dev_open(struct mass_dev_s *fmass);
static int fmass_dev_close(struct mass_dev_s *fmass);
static void fmass_dev_capacity(struct mass_dev_s *fmass);

static size_t fmass_dev_read(struct mass_dev_s *fmass,
                             uint8_t *buffer,
                             uint64_t sector,
                             uint32_t nsectors);

static size_t fmass_dev_write(struct mass_dev_s *fmass,
                              uint8_t *buffer,
                              uint64_t sector,
                              uint32_t nsectors);

static void fmass_report_usb_status(struct mass_dev_s *fmass);
static void fmass_notify_report(struct mass_dev_s *fmass, int status);

#define fmass_wait_intr(fm, cond, tm)    \
  wait_event_interruptible_timeout((((struct mass_dev_s *)fm)->xfer_wait), cond, tm)

#define fmass_wakeup(fm)    \
  wake_up_interruptible(&(((struct mass_dev_s *)fm)->xfer_wait))

#define	put_unaligned_be16(p, v) do { \
  *((UINT8 *)(p)) = ((v) >> 8);       \
  *((UINT8 *)((p) + 1)) = (v);        \
} while(0);

#define	put_unaligned_be32(p, v) do {   \
  put_unaligned_be16((p), ((v) >> 16)); \
  put_unaligned_be16(((p) + 2), (v));   \
} while(0);

#define get_unaligned_be16(p) (((UINT8 *)(p))[0] << 8 | ((UINT8 *)(p))[1])
#define get_unaligned_be32(p) (((UINT8 *)(p))[0] << 24 | ((UINT8 *)(p))[1] << 16 | \
  ((UINT8 *)(p))[2] << 8 | ((UINT8 *)(p))[3])

struct fmass_notify_cb
{
  struct fmass_notify notify[MAX_NOFIFY_NUM];
  uint32_t notify_mtx;
  bool init;
};

static struct fmass_notify_cb g_notify =
{
  .notify_mtx = INVALID_MUX,
  .init = false,
};

static inline void fmass_notify_init(void)
{
  if (g_notify.init == false)
    {
      g_notify.init = true;
      (void)memset_s(g_notify.notify, sizeof(struct fmass_notify) * MAX_NOFIFY_NUM,
                     0, sizeof(struct fmass_notify) * MAX_NOFIFY_NUM);
    }
}

#ifdef LOSCFG_USB_MASS_STORAGE_INDEPENDENT_MEMORY_POOL
void *g_fmass_mem_pool = NULL;
uint32_t fmass_set_mem_pool(void *pool, uint32_t size)
{
  uint32_t ret;
  ret = LOS_MemPoolInit(pool, size, 0);
  if (ret == LOS_OK)
    {
      g_fmass_mem_pool = pool;
    }
  return ret;
}

static void *fmass_memalign(uint32_t size, uint32_t align)
{
  return LOS_MemAllocAlign(g_fmass_mem_pool, size, align);
}

static void fmass_mem_free(void *ptr)
{
  (void)LOS_MemFree(g_fmass_mem_pool, ptr);
}
#endif

#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_USB_MODE_USB_1_1
#define FMASS_MAX_PACKET_SIZE   0x040
#else
#define FMASS_MAX_PACKET_SIZE   0x0400
#endif
#else
#define FMASS_MAX_PACKET_SIZE   0x0200
#endif

static struct usb_devdesc_s g_fmass_device_desc =
{
  .len          = sizeof(struct usb_devdesc_s),
  .type         = USB_DESC_TYPE_DEVICE,
  HSETW(.usb, USB_VERSION_BCD), /* USB version 0x0200 */
  .classid      = USB_CLASS_PER_INTERFACE,
  .subclass     = 0,
  .protocol     = 0,
  .mxpacketsize = USB_EP_MPS,
  HSETW(.vendor, 0),      /* Vendor */
  HSETW(.product, 0),     /* Product */
  HSETW(.device, 0x0318), /* Device version */
  .imfgr        = 1,
  .iproduct     = 2,
  .serno        = 3,
  .nconfigs     = 1,
};

struct fmass_config_desc
{
  struct usb_cfgdesc_s confd;
  struct usb_ifdesc_s ifcd;
  struct usb_epdesc_s iepd;
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  struct usb_ss_comp_epdesc_s icompd;
#endif
  struct usb_epdesc_s oepd;
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  struct usb_ss_comp_epdesc_s ocompd;
#endif
} __packed;

static struct fmass_config_desc g_fmass_confd =
{
  .confd =
    {
      .len         = sizeof(struct usb_cfgdesc_s),
      .type        = USB_DESC_TYPE_CONFIG,
      HSETW(.totallen, sizeof(g_fmass_confd)),
      .ninterfaces = 1,
      .cfgvalue    = 1,
      .icfg        = 0,
      .attr        = USB_CONFIG_ATTR_SELFPOWER | USB_CONFIG_ATTR_ONE,
      .mxpower     = 1 /* Max power */
    },
  .ifcd =
    {
      .len       = sizeof(struct usb_ifdesc_s),
      .type      = USB_DESC_TYPE_INTERFACE,
      .ifno      = 0,
      .alt       = 0,
      .neps      = 2,
      .classid   = USB_CLASS_MASS_STORAGE,
      .subclass  = USBMSC_SUBCLASS_SCSI,
      .protocol  = USBMSC_PROTO_BULKONLY,
      .iif       = 1
    },
  .iepd =
    {
      .len      = sizeof(struct usb_epdesc_s),
      .type     = USB_DESC_TYPE_ENDPOINT,
      .addr     = USB_DIR_IN,
      .attr     = USB_EP_ATTR_XFER_BULK,
      HSETW(.mxpacketsize, FMASS_MAX_PACKET_SIZE),
      .interval = 0
    },
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  .icompd =
    {
      .len      = 6,
      .type     = 0x30,
      .maxburst = 0xf,
      .attr     = 0,
      .interval = {0}
    },
#endif
  .oepd =
    {
      .len      = sizeof(struct usb_epdesc_s),
      .type     = USB_DESC_TYPE_ENDPOINT,
      .addr     = USB_DIR_OUT,
      .attr     = USB_EP_ATTR_XFER_BULK,
      HSETW(.mxpacketsize, FMASS_MAX_PACKET_SIZE),
      .interval = 0
    },
#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
  .ocompd =
    {
      .len      = 6,
      .type     = 0x30,
      .maxburst = 0xf,
      .attr     = 0,
      .interval = {0}
    },
#endif
};

#define DT_STRING_ID_LEN   4
static const char g_dt_string_id[DT_STRING_ID_LEN] =
{
  DT_STRING_ID_LEN,
  USB_DESC_TYPE_STRING,
  0x09, 0x04,
};

static char g_dt_string_buf[40] =
{
  0x28, 3, 0x4d, 0, 0x61, 0, 0x73, 0, 0x73, 0,
  0x20, 0, 0x53, 0, 0x74, 0, 0x6f, 0, 0x72, 0,
  0x61, 0, 0x67, 0, 0x65, 0, 0x20, 0, 0x47, 0,
  0x61, 0, 0x64, 0, 0x67, 0, 0x65, 0, 0x74, 0
};

inline static bool taskstate_match(const struct mass_dev_s *fmass, fmass_task_state tarstate)
{
  return fmass->task_state & tarstate;
}

inline static bool taskstate_unmatch(const struct mass_dev_s *fmass, fmass_task_state tarstate)
{
  return !(fmass->task_state & tarstate);
}

static int fmass_device_status(const struct mass_dev_s *fmass)
{
  return fmass->dev_status;
}

static void fmass_handle_bulkin_callback(struct usbdev_ep_s *ep,
                                         struct usbdev_req_s *req)
{
  (void)req;
  struct mass_dev_s *fmass = (struct mass_dev_s *)ep->priv;
  if (taskstate_match(fmass, FMASS_TASK_REQ_DATA_PHASE))
    {
      fmass_task_change_state(fmass, FMASS_TASK_DONE_DATA_PHASE);
    }
  else if (taskstate_match(fmass, FMASS_TASK_REQ_STATUS_PHASE))
    {
      fmass_task_change_state(fmass, FMASS_TASK_DONE_STATUS_PHASE);
    }
  fmass_wakeup(fmass);
}

static void fmass_handle_bulkout_callback(struct usbdev_ep_s *ep,
                                          struct usbdev_req_s *req)
{
  (void)req;
  struct mass_dev_s *fmass = (struct mass_dev_s *)ep->priv;
  if (taskstate_match(fmass, FMASS_TASK_REQ_COMMAND_PHASE))
    {
      fmass_task_change_state(fmass, FMASS_TASK_DONE_COMMAND_PHASE);
    }
  else if (taskstate_match(fmass, FMASS_TASK_REQ_DATA_PHASE))
    {
      fmass_task_change_state(fmass, FMASS_TASK_DONE_DATA_PHASE);
    }
  fmass_wakeup(fmass);
}

static int fmass_bulkin_request(const struct mass_dev_s *fmass,
                                struct usbdev_req_s *req)
{
  struct usbdev_ep_s *ep = fmass->bulkin;
  req->callback = fmass_handle_bulkin_callback;
  return EP_SUBMIT(ep, req);
}

static void fmass_bulkout_request(const struct mass_dev_s *fmass,
                                  struct usbdev_req_s *req)
{
  struct usbdev_ep_s *ep = fmass->bulkout;
  int size;

  size = ep->maxpacket;
  if (size <= 0)
    {
      return;
    }

  req->callback = fmass_handle_bulkout_callback;
  req->len = USB_ALIGN_SIZE(req->len, (unsigned int)size);
  (void)EP_SUBMIT(ep, req);
}

static void report_scsi_command_error(struct mass_dev_s *fmass, uint32_t sense_data)
{
  fmass->sense_data      = sense_data;
  fmass->sense_data_info = 0;
  fmass->info_valid      = 1;
  fmass->csw.cswstatus  = BULK_CSW_STAT_FAIL;
}

int done_scsi_test_unit_ready(struct mass_dev_s *fmass)
{
  if (fmass_device_status(fmass))
    {
      if (fmass->nluns == 0)
        {
          (void)fmass_dev_open(fmass);
          fmass_dev_capacity(fmass);
          fmass->sense_data = SCSI_NOT_READY_TO_READY_TRANSITION;
          fmass->csw.cswstatus = BULK_CSW_STAT_FAIL;
          DPRINTFN(1, "this unint is not ready\n");
        }
    }
  else
    {
      fmass->sense_data = SCSI_MEDIUM_NOT_PRESENT;
      fmass->csw.cswstatus = BULK_CSW_STAT_FAIL;
    }
  return 0;
}

int done_scsi_request_sense(struct mass_dev_s *fmass)
{
  uint8_t *buf = (uint8_t *)fmass->bulkreq->buf;
  uint32_t sd, sdinfo;
  uint32_t valid;
  int ret;

  ret = memset_s(buf, (MAX_BLOCK_RW_SECTORS << SECTOR_SHIFT), 0, 18);
  if (ret != EOK)
    {
      return 0;
    }

  if (!fmass->nluns && fmass_device_status(fmass)) /* Unsupported LUNs are okay */
    {
      sd     = SCSI_LOGICAL_UNIT_NOT_SUPPORTED;
      sdinfo = 0;
      valid  = 0;
    }
  else
    {
      sd     = fmass->sense_data;
      sdinfo = fmass->sense_data_info;
      valid  = fmass->info_valid << 7;
      fmass->sense_data      = SCSI_NO_SENSE;
      fmass->sense_data_info = 0;
      fmass->info_valid      = 0;
    }

  buf[0] = valid | 0x70; /* Valid, current error */
  buf[2] = SK(sd);
  put_unaligned_be32((&buf[3]), sdinfo); /* Sense information */
  buf[7]  = 10;  /* Additional sense length: 18 - 8 */
  buf[12] = ASC(sd);
  buf[13] = ASCQ(sd);
  return min(fmass->data_size, 18);  /* normal len is 18, need support short pkt */
}

int done_scsi_mode_select(struct mass_dev_s *fmass)
{
  (void)fmass;
  return 0;
}

int done_scsi_mode_sense(struct mass_dev_s *fmass)
{
  uint8_t *buf = (uint8_t *)fmass->bulkreq->buf;
  int len;

  buf[0] = 0x03;  /* Mode data length */
  buf[1] = 0x00;
  buf[2] = 0x00;
  buf[3] = 0x00;
  len    = 4;
  fmass->residue = len;
  return len;
}

int done_scsi_read_format_capacities(struct mass_dev_s *fmass)
{
  uint8_t *buf = (uint8_t *)fmass->bulkreq->buf;

  buf[0] = 0;
  buf[1] = 0;
  buf[2] = 0;
  buf[3] = 8; /* Only the Current/Maximum Capacity Descriptor */
  buf   += 4;

  put_unaligned_be32((&buf[0]), fmass->caps[fmass->lun].nsectors);
  put_unaligned_be32((&buf[4]), fmass->caps[fmass->lun].sector_size);    /* Block length */
  buf[4] = 0x02; /* Current capacity */

  return 12;
}

int done_scsi_read_capacity(struct mass_dev_s *fmass)
{
  uint8_t *buf = (uint8_t *)fmass->bulkreq->buf;
  uint32_t sectors;
  uint32_t blk_sizes;

  if (!fmass->nluns)
    {
      (void)fmass_dev_open(fmass);
      fmass_dev_capacity(fmass);
      fmass->sense_data = SCSI_NOT_READY_TO_READY_TRANSITION;
      fmass->csw.cswstatus = BULK_CSW_STAT_FAIL;
      DPRINTFN(0, "fmass->file_node is not exist\n");
      return 0;
    }

  sectors  = fmass->caps[fmass->lun].nsectors - 1;
  blk_sizes = fmass->caps[fmass->lun].sector_size;

  put_unaligned_be32(buf, sectors);
  put_unaligned_be32((buf + 4), blk_sizes);

  return 8;
}

int done_scsi_read_capacity16(struct mass_dev_s *fmass)
{
  uint8_t *buf = (uint8_t *)fmass->bulkreq->buf;
  uint32_t sectors;
  uint32_t blk_sizes;

  if (!fmass->nluns)
    {
      (void)fmass_dev_open(fmass);
      fmass_dev_capacity(fmass);
      fmass->sense_data = SCSI_NOT_READY_TO_READY_TRANSITION;
      fmass->csw.cswstatus = BULK_CSW_STAT_FAIL;
      DPRINTFN(0, "fmass->file_node is not exist\n");
      return 0;
    }

  (void)memset_s(buf, 32, 0, 32); /* READ CAPACITY(16) normal len is 32 */
  sectors  = fmass->caps[fmass->lun].nsectors - 1;
  blk_sizes = fmass->caps[fmass->lun].sector_size;

  put_unaligned_be32(buf + 4, sectors);        /* READ CAPACITY(16) block LBA begin at 4 */
  put_unaligned_be32((buf + 8), blk_sizes);    /* READ CAPACITY(16) block size begin at 8 */

  return 32;  /* READ CAPACITY(16) normal len is 32 */
}

int done_scsi_read(struct mass_dev_s *fmass)
{
  uint8_t *buf = (uint8_t *)fmass->bulkreq->buf;
  struct fmass_data_buf_t *db;
  uintptr_t num_sectors = fmass->caps[fmass->lun].nsectors;
  uint32_t lba;
  uint32_t amount_left;
  uint32_t amount;
  uint32_t nread_sector;
  uint32_t doread;
  uint32_t data_size;
  uint32_t hi, di;

  /* The implementation is based on to FreeBSD open source code */

  if (fmass->cmd[0] == SCSI_READ_6)
    {
      lba = ((((uint32_t)fmass->cmd[1]) << 16) |
             get_unaligned_be16(&fmass->cmd[2]));
    }
  else
    {
      lba = get_unaligned_be32(&fmass->cmd[2]);

      /* We allow DPO (Disable Page Out = don't save data in the
       * cache) and FUA (Force Unit Access = write directly to the
       * medium).  We don't implement DPO; we implement FUA by
       * performing synchronous output.
       */

      if ((fmass->cmd[1] & ~0x18) != 0)
        {
          report_scsi_command_error(fmass, SCSI_INVALID_FIELD_IN_CDB);
          return -EIO;
        }
    }

  if (lba >= num_sectors)
    {
      report_scsi_command_error(fmass, SCSI_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE);
      return -1;
    }
  hi = fmass->data_size;
  di = fmass->data_size_from_cmd << SECTOR_SHIFT;
  if (di == 0 || hi == 0)
    {
      report_scsi_command_error(fmass, SCSI_UNRECOVERED_READ_ERROR);
      return -EIO;
    }

  if (hi < di)
    {
      fmass->residue = hi;
      nread_sector = hi >> SECTOR_SHIFT;
      /* hi < di need transfer data first, THEN report CSW error. Do not return */
      report_scsi_command_error(fmass, SCSI_INVALID_COMMAND);
    }
  else
    {
      nread_sector = fmass->data_size_from_cmd;
    }

  amount_left = nread_sector;

  fmass->bulkreq->is_complete = 1;

  for (;;)
    {
      if (taskstate_match(fmass, FMASS_TASK_DISCONNECT))
        {
          report_scsi_command_error(fmass, SCSI_UNRECOVERED_READ_ERROR);
          return -EIO;
        }

      amount = min(MAX_BLOCK_RW_SECTORS, amount_left);
      db = fmass->databuf_fill;
      buf = (uint8_t *)db->buf;
      doread = fmass_dev_read(fmass, buf, lba, amount);
      data_size = doread << SECTOR_SHIFT;
      lba += doread;
      amount_left -= doread;
      fmass->residue -= data_size;    /* doread * 512 */
      fmass->bulkreq->len = data_size;
      fmass->bulkreq->buf = buf;
      DPRINTFN(1, "read: db= 0x%x buf= 0x%x lba= %x doread= %u  nread= %u  amount_left = %u   residue= %u\n",
               db, buf, lba - doread , doread, amount, amount_left, fmass->residue);

      if (!fmass_wait_intr(fmass, ((fmass->bulkreq->is_complete != 0) ||
          taskstate_match(fmass, FMASS_TASK_DISCONNECT)), (2 * LOSCFG_BASE_CORE_TICK_PER_SECOND)))
        {
          fmass->bulkreq->is_complete = 1;
          report_scsi_command_error(fmass, SCSI_UNRECOVERED_READ_ERROR);
          return -EIO;
        }

      if (doread < amount)
        {
          report_scsi_command_error(fmass, SCSI_UNRECOVERED_READ_ERROR);
          break;
        }

      if (amount_left == 0)
        {
          break;
        }

      (void)fmass_bulkin_request(fmass, fmass->bulkreq);
      fmass->databuf_fill = db->next;
    }
  fmass->bulkreq->is_complete = 1;
  return -EIO;
}

int done_scsi_write(struct mass_dev_s *fmass)
{
  uint32_t lba;
  uint32_t idx;
  uint32_t amount;
  uint32_t nwrite_sector;
  uint32_t num_sectors = (uint32_t)fmass->caps[fmass->lun].nsectors;
  uint32_t total_do_write = 0;
  uint32_t do_write;
  uint32_t len;
  uint32_t write_size;
  uint32_t left_size;

  if (fmass->caps[fmass->lun].read_only)
    {
      report_scsi_command_error(fmass,SCSI_WRITE_PROTECTED);
      return -EINVAL;
    }

  /* The implementation is based on to FreeBSD open source code */

  if (fmass->cmd[0] == SCSI_WRITE_6)
    {
      lba = ((((uint32_t)fmass->cmd[1]) << 16) |
             get_unaligned_be16(&fmass->cmd[2]));
    }
  else
    {
      lba = get_unaligned_be32(&fmass->cmd[2]);

      /* We allow DPO (Disable Page Out = don't save data in the
       * cache) and FUA (Force Unit Access = write directly to the
       * medium).  We don't implement DPO; we implement FUA by
       * performing synchronous output.
       */

      if ((fmass->cmd[1] & ~0x18) != 0)
        {
          report_scsi_command_error(fmass, SCSI_INVALID_FIELD_IN_CDB);
          return -EIO;
        }
      if (fmass->cmd[1] & 0x08)
        {
          /* FUA */
          /* XXX set SYNC flag here */

        }
    }

  len = min(fmass->data_size >> SECTOR_SHIFT, fmass->data_size_from_cmd);
  left_size = len << SECTOR_SHIFT;
  len += lba;
  if (len < lba || len > num_sectors || lba >= num_sectors)
    {
      report_scsi_command_error(fmass, SCSI_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE);
      return -1;
    }

  len -= lba;
  if (fmass->data_size_from_cmd == 0)
    {
      return -EIO; /* No default reply */
    }

  amount = howmany(len, MAX_BLOCK_RW_SECTORS);
  for (idx = 0; idx < amount; idx++)
    {
      fmass_task_change_state(fmass, FMASS_TASK_REQ_DATA_PHASE);
      DPRINTFN(1, "------ Direct Data Phase ------\n");

      write_size = min(MAX_BLOCK_RW_SECTORS << SECTOR_SHIFT, left_size);

      fmass->bulkreq->len = write_size;
      fmass_bulkout_request(fmass, fmass->bulkreq);

      if (!fmass_wait_intr(fmass, (taskstate_match(fmass, FMASS_TASK_DONE_DATA_PHASE) ||
          taskstate_match(fmass, FMASS_TASK_DISCONNECT)), (5 * LOSCFG_BASE_CORE_TICK_PER_SECOND)))
        {
          usb_err("error wait intr\n");
          return -EIO;
        }

      if (taskstate_unmatch(fmass, FMASS_TASK_DONE_DATA_PHASE))
        {
          usb_err("invalid task state : %u ,unfinish transfer  Data packet\n", fmass->task_state);
          return -EIO;
        }

      nwrite_sector = fmass->bulkreq->xfrd >> SECTOR_SHIFT;  /* bytes -> sectors */
      nwrite_sector = min(nwrite_sector, write_size >> SECTOR_SHIFT);
      do_write = fmass_dev_write(fmass, fmass->bulkreq->buf, lba, nwrite_sector);
      if (do_write != nwrite_sector) /* write fail */
        {
          usb_err("error write io\n");
          break;
        }

      lba += do_write;
      fmass->residue -= do_write << SECTOR_SHIFT; /* dowrite * 512 */
      total_do_write += do_write;

      left_size -= do_write << SECTOR_SHIFT;
    }

  if (total_do_write < len)
    {
      fmass->sense_data = SCSI_UNRECOVERED_READ_ERROR;
      fmass->csw.cswstatus = BULK_CSW_STAT_FAIL;
    }

  fmass->data_size = 0;
  return -EIO;
}

int done_scsi_inquiry(struct mass_dev_s *fmass)
{
  errno_t ret;
  uint8_t *buf = (uint8_t *)fmass->bulkreq->buf;
  if (fmass->data_size == 0)
    {
      report_scsi_command_error(fmass, SCSI_INVALID_COMMAND);
      fmass->residue = 0;
      return -EIO;
    }

  buf[0] = 0;        /* Peripheral Qualifier && Device Type */
  buf[1] = 0x80;     /* Removable Medium */
  buf[2] = 0;        /* ANSI SCSI level 2 */
  buf[3] = 1;        /* SCSI-2 INQUIRY data format */
  buf[4] = 31;       /* Additional length */
  buf[5] = 0;        /* SCCS SCC Support: 0 */
  buf[6] = 0;
  buf[7] = 0;

  ret = memcpy_s(buf + 8, ((MAX_BLOCK_RW_SECTORS << SECTOR_SHIFT) - 8), fmass->inquiry_str, sizeof(fmass->inquiry_str));
  if (ret != EOK)
    {
      return -EIO;
    }

  fmass->bulkreq->len = min(fmass->data_size, 36); /* normal len is 36, need support short pkt */
  fmass->residue = fmass->data_size - fmass->bulkreq->len;
  return -EIO;
}

int done_scsi_allow_medium_removal(struct mass_dev_s *fmass)
{
  fmass->sense_data = SCSI_INVALID_COM;
  fmass->csw.cswstatus = BULK_CSW_STAT_FAIL;
  return 0;
}

static int fmass_check_receive_cbw(struct mass_dev_s *fmass)
{
  struct bulk_cbw *cbw = (struct bulk_cbw *)fmass->bulkreq->buf;
  errno_t ret;

  DPRINTFN(1, "\t -----Received cbw packet -----cbwtag: 0x%x  cbwlun:%u\n", cbw->cbwtag, cbw->cbwlun);

  /* Check the CBW is validid */

  if (fmass->bulkreq->xfrd != BULK_CBW_WRAP_LEN ||
      cbw->cbwsig != BULK_CBW_SIGN)
    {
      DPRINTFN(0, "invalid CBW: len %u sig 0x%x ,cvt_sig 0x%x  lct_sig 0x%x\n",
               fmass->bulkreq.xfrd, cbw->cbwsig, cbw->cbwsig, BULK_CBW_SIGN);
      return -1;
    }

  /* Check the CBW is meaningful */

  if (cbw->cbwlun >= FMASS_MAX_LUNS || cbw->cbwflags & ~BULK_CBW_FLAG_IN ||
      cbw->cbwlen == 0 || cbw->cbwlen > MAX_COMMAND_SIZE)
    {
      DPRINTFN(0, "non-meaningful CBW: lun = %u, flags = 0x%x, cmdlen = %u\n",
               cbw->cbwlun, cbw->cbwflags, cbw->cbwlen);
      return -1;
    }

  /* Save the command for later */

  fmass->cmd_size = cbw->cbwlen;
  ret = memcpy_s(fmass->cmd, MAX_COMMAND_SIZE, cbw->cdb, fmass->cmd_size);
  if (ret != EOK)
    {
      DPRINTFN(0, "memcpy_s failed, %d\n", ret);
      return -1;
    }
  if (cbw->cbwflags & BULK_CBW_FLAG_IN)
    {
      fmass->data_dir = DATA_DIRECT_TO_HOST;
    }
  else
    {
      fmass->data_dir = DATA_DIRECT_FROM_HOST;
    }
  fmass->data_size = cbw->cbwdatalen;
  if (fmass->data_size == 0)
    {
      fmass->data_dir = DATA_DIRECT_NONE;
    }

  fmass->tag = cbw->cbwtag;

  fmass->residue = fmass->data_size;

  return 0;
}

static void fmass_do_scsi_command(struct mass_dev_s *fmass)
{
  int i;
  int reply = -1;
  uint32_t reply_temp;

  uint8_t cmd = fmass->cmd[0];

  switch (cmd)
    {
    case SCSI_TEST_UNIT_READY:
      fmass->data_size_from_cmd = 0;
      reply = done_scsi_test_unit_ready(fmass);
      break;

    case SCSI_REQUEST_SENSE:
      fmass->data_size_from_cmd = fmass->cmd[4];
      reply = done_scsi_request_sense(fmass);
      break;

    case SCSI_INQUIRY:
      fmass->data_size_from_cmd = fmass->cmd[4];
      reply = done_scsi_inquiry(fmass);
      break;

    case SCSI_MODE_SELECT_6:
      fmass->data_size_from_cmd = fmass->cmd[4];
      reply = done_scsi_mode_select(fmass);
      break;

    case SCSI_MODE_SELECT_10:
      fmass->data_size_from_cmd = get_unaligned_be16(&fmass->cmd[7]);
      reply = done_scsi_mode_select(fmass);
      break;

    case SCSI_MODE_SENSE_6:
      fmass->data_size_from_cmd = fmass->cmd[4];
      reply = done_scsi_mode_sense(fmass);
      break;

    case SCSI_MODE_SENSE_10:
      fmass->data_size_from_cmd = get_unaligned_be16(&fmass->cmd[7]);
      reply = done_scsi_mode_sense(fmass);
      break;

    case SCSI_READ_FORMAT_CAPACITIES:
      fmass->data_size_from_cmd = get_unaligned_be16(&fmass->cmd[7]);
      reply = done_scsi_read_format_capacities(fmass);
      break;

    case SCSI_READ_CAPACITY:
      fmass->data_size_from_cmd = 8;
      if (fmass_device_status(fmass))
        {
          reply = done_scsi_read_capacity(fmass);
        }
      else
        {
          /* device not connect need report CSW */
          report_scsi_command_error(fmass, SCSI_MEDIUM_NOT_PRESENT);
        }
      break;

    case SCSI_READ_CAPACITY16:
      fmass->data_size_from_cmd = 32;  /* need return 32 byte */
      if (fmass_device_status(fmass))
        {
          reply = done_scsi_read_capacity16(fmass);
        }
      else
        {
          /* device not connect need report CSW */
          report_scsi_command_error(fmass, SCSI_MEDIUM_NOT_PRESENT);
        }
      break;

    case SCSI_READ_6:
      i = fmass->cmd[4];
      fmass->data_size_from_cmd = (i == 0) ? 256 : i;
      reply = done_scsi_read(fmass);
      break;

    case SCSI_READ_10:
      fmass->data_size_from_cmd = get_unaligned_be16(&fmass->cmd[7]);
      reply = done_scsi_read(fmass);
      break;

    case SCSI_READ_12:
      fmass->data_size_from_cmd = get_unaligned_be32(&fmass->cmd[6]);
      reply = done_scsi_read(fmass);
      break;

    case SCSI_WRITE_6:
      i = fmass->cmd[4];
      fmass->data_size_from_cmd = (i == 0) ? 256 : i;
      reply = done_scsi_write(fmass);
      break;

    case SCSI_WRITE_10:
      fmass->data_size_from_cmd = get_unaligned_be16(&fmass->cmd[7]);
      reply = done_scsi_write(fmass);
      break;

    case SCSI_WRITE_12:
      fmass->data_size_from_cmd = get_unaligned_be32(&fmass->cmd[6]);
      reply = done_scsi_write(fmass);
      break;

    case SCSI_ALLOW_MEDIUM_REMOVAL:
      fmass->data_size_from_cmd = 0;
      reply = done_scsi_allow_medium_removal(fmass);
      break;

    case START_STOP:
      fmass->data_size_from_cmd = 0;
      reply = 0;
      if (fmass->cmd[4] == 1)  /* cmd 4 need adapt */
        {
          /* start, do nothing */
          break;
        }
      fmass->dev_status = DEV_ST_DISCONNECT;
      break;

    default:
      DPRINTFN(0, "********[invalid SCSI command! %x]***********\n", cmd);
      fmass->data_size_from_cmd = 0;
      report_scsi_command_error(fmass, SCSI_INVALID_COMMAND);
      break;
    }

  if (reply >= 0 && fmass->data_dir == DATA_DIRECT_TO_HOST)
    {
      fmass->bulkreq->len = reply;
      reply_temp = min((uint32_t)reply, fmass->data_size_from_cmd);
      fmass->residue -= reply_temp;
    }
 }

void fmass_handle_scsi_data(struct mass_dev_s *fmass)
{
  int is_data_phase = 0;

  DPRINTFN(1, "\t-----REQ DATA PHASE-----\n");
  switch (fmass->data_dir)
    {
    case DATA_DIRECT_NONE:
      break;    /* Nothing to send */

    case DATA_DIRECT_UNKNOWN:
      break;

    /* All but the last buffer of data must have already been sent */

    case DATA_DIRECT_TO_HOST:
      if (fmass->data_size == 0)
        {
          /* do nothing */

        }
      else if (fmass->residue == 0)
        {
          (void)fmass_bulkin_request(fmass, fmass->bulkreq);
        }
      else
        {
          (void)fmass_bulkin_request(fmass, fmass->bulkreq);
          if (!fmass_wait_intr(fmass, taskstate_unmatch(fmass, FMASS_TASK_REQ_DATA_PHASE),
                               (2 * LOSCFG_BASE_CORE_TICK_PER_SECOND)))
            {
              DPRINTFN(0, "--- [Data Phase] timeout! state: %u  residue : %u ---\n",
                       fmass->task_state, fmass->residue);
              if (taskstate_match(fmass, FMASS_TASK_REQ_DATA_PHASE))
                {
                  fmass_task_change_state(fmass, FMASS_TASK_IDLE);
                }
              return;
            }

          (void)EP_RESUME(fmass->bulkin);
          fmass_task_change_state(fmass, FMASS_TASK_REQ_STATUS_PHASE);
          return;
        }
      is_data_phase = 1;
      break;

    case DATA_DIRECT_FROM_HOST:
      break;

    default:
      break;
    }

  if (is_data_phase)
    {
      /* Wait for the data transfer done */

      if (!fmass_wait_intr(fmass, taskstate_unmatch(fmass, FMASS_TASK_REQ_DATA_PHASE),
                           (5 * LOSCFG_BASE_CORE_TICK_PER_SECOND)))
        {
          DPRINTFN(0, "--- [Data Phase] timeout! state: %u ---\n", fmass->task_state);
          if (taskstate_match(fmass, FMASS_TASK_REQ_DATA_PHASE))
            {
              fmass_task_change_state(fmass, FMASS_TASK_IDLE);
            }
          return;
        }
      if (taskstate_unmatch(fmass, FMASS_TASK_DONE_DATA_PHASE))
        {
          DPRINTFN(0, "--- [Data Phase] unmatch! unexpected state: %u ---\n", fmass->task_state);
          return;
        }
      fmass_task_change_state(fmass, FMASS_TASK_REQ_STATUS_PHASE);
    }
  else
    {
      fmass_task_change_state(fmass, FMASS_TASK_REQ_STATUS_PHASE);
    }
}

void fmass_set_scsi_status(struct mass_dev_s *fmass)
{
  struct bulk_csw *csw = (struct bulk_csw *)(fmass->bulkreq->buf);
  uint8_t status       = fmass->csw.cswstatus;

  DPRINTFN(1, "\t -----Response CSW packet -------cbwtag:0x%x   residue: %u   status: %u\n",
           fmass->tag, fmass->residue, status);

  /* Store and send the Bulk-only CSW */

  csw->cswsig       = BULK_CSW_SIGN;
  csw->cswtag       = fmass->tag;
  csw->cswresidue   = fmass->residue;
  csw->cswstatus    = status;

  /* Submit csw packet to controller */

  fmass->bulkreq->len = BULK_CSW_WRAP_LEN;
  (void)fmass_bulkin_request(fmass, fmass->bulkreq);

  /* Wait csw packet transfer finish */

  if (!fmass_wait_intr(fmass, taskstate_unmatch(fmass, FMASS_TASK_REQ_STATUS_PHASE),
                       (5 * LOSCFG_BASE_CORE_TICK_PER_SECOND)))
    {
      DPRINTFN(0, "--- [CSW Phase] timeout! state: %u ---\n", fmass->task_state);
      if (taskstate_match(fmass, FMASS_TASK_REQ_STATUS_PHASE))
        {
          fmass_task_change_state(fmass, FMASS_TASK_IDLE);
        }
      return;
    }

  if (taskstate_unmatch(fmass, FMASS_TASK_DONE_STATUS_PHASE))
    {
      DPRINTFN(0, "--- [CSW Phase] unmatch! unexpected state: %u ---\n", fmass->task_state);
      return;
    }

  DPRINTFN(1, "\t -----CSW Transfer Finish -----\n");
  fmass_task_change_state(fmass, FMASS_TASK_REQ_COMMAND_PHASE);
  fmass->csw.cswstatus = BULK_CSW_STAT_OK;
}

static void fmass_parts_registered(struct mass_dev_s *fmass)
{
  los_part *part;
  int i;

  for (i = 0; i < MAX_FILE_STORAGE_LUNS; i++)
    {
      part = get_part(i);
      if (part->dev != NULL)
        {
          fmass->file_node[fmass->nluns] = part->dev;
          fmass->parts[fmass->nluns]     = part;
          fmass->nluns++;
        }
    }

  return;
}

static int fmass_dev_open(struct mass_dev_s *fmass)
{
  uint32_t i;
  struct inode *inode = NULL;
  int ret;

  fmass_parts_registered(fmass);

  if (fmass->nluns == 0)
    {
      usb_err("device inode is unavailable!\n");
      return -1;
    }

  for (i = 0; i < MAX_DATA_BUFFER_NUM; i++)
    {
      struct fmass_data_buf_t *db = &fmass->databuf[i];

      if (db->buf != NULL)
        {
          continue;
        }
#ifdef LOSCFG_USB_MASS_STORAGE_INDEPENDENT_MEMORY_POOL
      db->buf = fmass_memalign(SKB_DATA_ALIGN(MAX_BLOCK_RW_SECTORS << SECTOR_SHIFT), USB_CACHE_ALIGN_SIZE);
#else
      db->buf = memalign(USB_CACHE_ALIGN_SIZE, SKB_DATA_ALIGN(MAX_BLOCK_RW_SECTORS << SECTOR_SHIFT));
#endif
      if (db->buf == NULL)
        {
            return -1;
        }
      (void)memset(db->buf, 0, SKB_DATA_ALIGN(MAX_BLOCK_RW_SECTORS << SECTOR_SHIFT));
      db->state = DBUF_STATE_EMPTY;
      db->next  = db + 1;
    }
  fmass->databuf[i - 1].next = &fmass->databuf[0];

  fmass->dev_status = DEV_ST_CONNECTTED;
  fmass_notify_report(fmass, fmass->dev_status);

  ret = open_blockdriver(fmass->path, O_RDWR, &inode);
  if (inode == NULL || ret != ENOERR)
    {
      usb_err("open_blockdriver failed!ret:%d\n", ret);
      return -1;
    }

  return 0;
}

static int fmass_dev_close(struct mass_dev_s *fmass)
{
  uint32_t i;

  if (fmass->nluns == 0)
    {
      return -1;
    }
  (void)close_blockdriver(fmass->file_node[fmass->lun]);
  for (i = 0; i < fmass->nluns; i++)
    {
      fmass->file_node[i] = NULL;
      fmass->parts[i]     = NULL;
    }

  for (i = 0; i < MAX_DATA_BUFFER_NUM; i++)
    {
      if (fmass->databuf[i].buf != NULL)
        {
#ifdef LOSCFG_USB_MASS_STORAGE_INDEPENDENT_MEMORY_POOL
          fmass_mem_free(fmass->databuf[i].buf);
#else
          free(fmass->databuf[i].buf);
#endif
          fmass->databuf[i].buf = NULL;
        }
    }
  fmass->nluns = 0;
  free(fmass->path);
  fmass->path = NULL;
  return 0;
}

static void fmass_dev_capacity(struct mass_dev_s *fmass)
{
  uint32_t i;
  uint64_t nsectors = 0;
  los_part *part;
  struct fmass_capacity *cap;

  for (i = 0; i < fmass->nluns; i++)
    {
      part = fmass->parts[i];
      cap  = &fmass->caps[i];
      cap->sector_size = 0;
      cap->nsectors    = 0;

      (void)los_part_ioctl(part->part_id, GET_SECTOR_COUNT, &nsectors);
      if (nsectors > UINT32_MAX)
        {
          cap->nsectors = UINT32_MAX;
          usb_err("nsectors exceed the maximum value of size_t, nsectors: %llu\n", nsectors);
        }
      else
        {
          cap->nsectors = (size_t)nsectors;
        }

      (void)los_part_ioctl(part->part_id, GET_SECTOR_SIZE, &cap->sector_size);
      PRINTK("*** %u  %u Bytes/Sector, Total %u Sectors ***\n", i, cap->sector_size, cap->nsectors);
    }
}

static size_t fmass_dev_read(struct mass_dev_s *fmass,
                             uint8_t *buffer,
                             uint64_t sector,
                             uint32_t nsectors)
{
  los_part *part  = fmass->parts[fmass->lun];

  if (part != NULL)
    {
      if (part->type != EMMC)
        {
          sector += part->sector_start;
        }
      if (los_part_read(part->part_id, buffer, sector, nsectors) == 0)
        {
          return nsectors;
        }
    }
  return 0;
}

static size_t fmass_dev_write(struct mass_dev_s *fmass,
                              uint8_t *buffer,
                              uint64_t sector,
                              uint32_t nsectors)
{
  los_part *part  = fmass->parts[fmass->lun];

  if (part != NULL)
    {
      if (part->type != EMMC)
        {
          sector += part->sector_start;
        }
      if (los_part_write(part->part_id, buffer, sector, nsectors) == 0)
        {
          return nsectors;
        }
    }
  return 0;
}

void fmass_reset(struct mass_dev_s *fmass, struct usbdev_s *dev)
{
  fmass_task_change_state(fmass, FMASS_TASK_REQ_COMMAND_PHASE);
}

void fmass_set_config(struct mass_dev_s *fmass, struct usbdev_s *dev)
{
  if (dev->ep0 == NULL || dev->ep0->handle_req == NULL)
    {
      usb_err("set config fail!\n");
      return;
    }

  if (fmass->nluns == 0)
    {
      if (fmass_dev_open(fmass) == 0)
        {
          fmass_dev_capacity(fmass);
        }
      else
        {
          usb_err("can not found inode!\n");
          return;
        }
    }

  if (fmass->bulk_in_enabled)
    {
      fmass->bulk_in_enabled = 0;
      (void)EP_DISABLE(fmass->bulkin);
    }

  if (fmass->bulk_out_enabled)
    {
      fmass->bulk_out_enabled = 0;
      (void)EP_DISABLE(fmass->bulkout);
    }

  usbd_configep_byspeed(dev, &g_fmass_confd.iepd, USB_SPEED_HIGH);
  (void)EP_CONFIGURE(fmass->bulkin, (const struct usb_epdesc_s *)&g_fmass_confd.iepd, 0);
  fmass->bulk_in_enabled = 1;

  usbd_configep_byspeed(dev, &g_fmass_confd.oepd, USB_SPEED_HIGH);
  (void)EP_CONFIGURE(fmass->bulkout, (const struct usb_epdesc_s *)&g_fmass_confd.oepd, 0);
  fmass->bulk_out_enabled = 1;


  fmass->databuf_fill = &fmass->databuf[0];
  fmass->bulkreq->buf = (uint8_t *)fmass->databuf[0].buf;
  fmass->bulkreq->is_complete = 1;

  fmass_task_change_state(fmass, FMASS_TASK_REQ_COMMAND_PHASE);
}

void fmass_set_next_command(struct mass_dev_s *fmass)
{
  fmass_task_state next_state;

  DPRINTFN(1, "\n\t -----REQUEST CBW packet -------\n");

  fmass->bulkreq->len = BULK_CBW_WRAP_LEN;

  /* Start a request to receive a CBW packet */

  fmass_bulkout_request(fmass, fmass->bulkreq);

  /* Wait for the CBW packet */

  if (!fmass_wait_intr(fmass, taskstate_unmatch(fmass, FMASS_TASK_REQ_COMMAND_PHASE), osWaitForever))
    {
      DPRINTFN(0, "--- [CBW Phase] error! state: %u ---\n", fmass->task_state);
      if (taskstate_match(fmass, FMASS_TASK_REQ_COMMAND_PHASE))
        {
          fmass_task_change_state(fmass, FMASS_TASK_IDLE);
        }
      return;
    }

  if (taskstate_unmatch(fmass, FMASS_TASK_DONE_COMMAND_PHASE))
    {
      DPRINTFN(0, "--- [CBW Phase] unmatch! unexpected state: %u ---\n", fmass->task_state);
      return;
    }

  /* Check and receive CBW packet */

  if (fmass_check_receive_cbw(fmass) < 0)
    {
      /* CBW error need report. DO NOT return */
      fmass->data_size = 0;
      fmass->data_size_from_cmd = 0;
      report_scsi_command_error(fmass, SCSI_INVALID_COMMAND);
    }
  else
    {
      /* Handle CBW packet */
      fmass_do_scsi_command(fmass);
    }

  if (taskstate_unmatch(fmass, FMASS_TASK_DISCONNECT))
    {
      if (fmass->data_size)
        {
          next_state = FMASS_TASK_REQ_DATA_PHASE;
        }
      else
        {
          next_state = FMASS_TASK_REQ_STATUS_PHASE;
        }
      fmass_task_change_state(fmass, next_state);
    }
}

static void *fmass_main_thread(void *para)
{
  struct mass_dev_s *fmass = ((struct mass_usb_dev_s *)para)->fmass;
  struct usbdev_s *dev     = ((struct mass_usb_dev_s *)para)->dev;
  PEVENT_CB_S fmass_event  = &fmass->task_event;
  uint32_t oldstate        = FMASS_TASK_IDLE;
  uint32_t curstate        = fmass->task_state;
  uint32_t ret;
  uint32_t flags;

  while (1)
    {
      if (oldstate == curstate)
        {
          DPRINTFN(1, "\n -- process thread wait --(curstate:%u)\n", fmass->task_state);
          ret = LOS_EventRead(fmass_event, (FMASS_DATA_PROC | FMASS_NEED_EXIT),
                              LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
          if (ret & FMASS_NEED_EXIT)
            {
              break;
            }
          curstate = fmass->task_state;
        }
      oldstate = curstate;

      if (taskstate_match(fmass, FMASS_TASK_DISCONNECT))
        {
          fmass_report_usb_status(fmass);
          fmass->task_state &= ~FMASS_TASK_DISCONNECT;
          ret = LOS_EventPoll(&(fmass_event->uwEventID), FMASS_NEED_EXIT,
                              LOS_WAITMODE_OR | LOS_WAITMODE_CLR);
          if (ret == FMASS_NEED_EXIT)
            {
              break;
            }
        }
      else
        {
          switch (curstate)
            {
            case FMASS_TASK_CONFIG_CHANGE:
              flags = LOS_IntLock();
              fmass_set_config(fmass, dev);
              LOS_IntRestore(flags);
              break;

            case FMASS_TASK_RESET:
              flags = LOS_IntLock();
              fmass_reset(fmass, dev);
              LOS_IntRestore(flags);
              break;

            case FMASS_TASK_REQ_COMMAND_PHASE:
              fmass_set_next_command(fmass);
              break;

            case FMASS_TASK_REQ_DATA_PHASE:
              fmass_handle_scsi_data(fmass);
              break;

            case FMASS_TASK_REQ_STATUS_PHASE:
              fmass_set_scsi_status(fmass);
              break;

            default:
              break;
            }
        }

      curstate = fmass->task_state;
    }

  (void)LOS_EventWrite(&(fmass->task_event), FMASS_THREAD_EXITED);

  free(para);
  return NULL;
}

static void fmass_task_change_state(struct mass_dev_s *fmass, fmass_task_state new_state)
{
  uint32_t flags;
  uint32_t old_state = fmass->task_state;

  spin_lock_irqsave(&fmass->lock, flags);

  if (taskstate_unmatch(fmass, FMASS_TASK_DISCONNECT))
    {
      fmass->task_state &= ~old_state;
    }
  fmass->task_state |= new_state;

  spin_unlock_irqrestore(&fmass->lock, flags);
}

static void fmass_main_thread_signal(struct mass_dev_s *fmass, fmass_task_state state)
{
  DPRINTFN(1, " - process thread signal -(curstate:%u   newstate: %d)\n", fmass->task_state, state);
  fmass_task_change_state(fmass, state);
  (void)LOS_EventWrite(&(fmass->task_event), FMASS_DATA_PROC);
}

static uint32_t usb_task_create(uint32_t *taskid, TSK_ENTRY_FUNC func, uint16_t prio,
                                const char *nm, UINTPTR para, UINTPTR para1)
{
  uint32_t ret;
  TSK_INIT_PARAM_S attr;
  struct mass_usb_dev_s *mass_usb_dev = (struct mass_usb_dev_s *)malloc(sizeof(struct mass_usb_dev_s));
  if (mass_usb_dev == NULL)
    {
      usb_err("mass_usb_dev malloc fail\n");
      return LOS_NOK;
    }

  (void)memset_s(&attr, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));

  attr.pfnTaskEntry = func;
  attr.uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;

  attr.usTaskPrio   = prio;
  attr.pcName       = (char *)nm;
  attr.uwResved     = LOS_TASK_STATUS_DETACHED;
  mass_usb_dev->fmass = (struct mass_dev_s *)para;
  mass_usb_dev->dev   = (struct usbdev_s *)para1;
  USB_TASK_PARAM_INIT_ARG(attr, mass_usb_dev);
  ret = LOS_TaskCreate(taskid, &attr);
  if (ret != LOS_OK)
    {
      free(mass_usb_dev);
      usb_err("create %s task error!\n", nm);
    }
  return ret;
}

static int fmass_thread_init(struct mass_dev_s *fmass, struct usbdev_s *dev)
{
  uint32_t td = 0;
  uint32_t ret;

  (void)LOS_EventInit(&fmass->task_event);
  init_waitqueue_head(&fmass->xfer_wait);

  fmass_task_change_state(fmass, FMASS_TASK_IDLE);

  ret = usb_task_create(&td, (TSK_ENTRY_FUNC)fmass_main_thread, 10, "USB_FMASS_Task", (UINTPTR)fmass, (UINTPTR)dev);
  return (ret == 0) ? 0 : -1;
}

static int fmass_thread_delete(struct mass_dev_s *fmass)
{
  uint32_t ret;

  (void)LOS_EventWrite(&(fmass->task_event), FMASS_NEED_EXIT);
  ret = LOS_EventRead(&fmass->task_event, FMASS_THREAD_EXITED, LOS_WAITMODE_OR | LOS_WAITMODE_CLR,
                      (10 * LOSCFG_BASE_CORE_TICK_PER_SECOND));
  if (ret == LOS_ERRNO_EVENT_READ_TIMEOUT)
    {
      usb_err("fmass, wait thread exit timeout\n");
      return -1;
    }

  (void)LOS_EventDestroy(&fmass->task_event);

  return 0;
}

static void fmass_notify_report(struct mass_dev_s *fmass, int status)
{
  int i;
  struct fmass_notify *notify;

  DPRINTFN(1, "\n< fmass storage %s >\n", (status == DEV_ST_CONNECTTED) ? "connected" : "disconnected");
  for (i = 0; i < MAX_NOFIFY_NUM; i++)
    {
      notify = &fmass->notify[i];
      if (notify->is_used && notify->notify_cb != NULL)
        {
          notify->notify_cb(notify->notify_data, status);
        }
    }
}

static void fmass_report_usb_status(struct mass_dev_s *fmass)
{
  if (fmass->dev_status == DEV_ST_DISCONNECT)
    {
      if (fmass->nluns == 0) /* Device is not ready, skip disconnect status */
        {
          fmass_task_change_state(fmass, FMASS_TASK_IDLE);
          return;
        }
      (void)fmass_dev_close(fmass);
    }

  fmass_notify_report(fmass, fmass->dev_status);
  fmass_task_change_state(fmass, FMASS_TASK_IDLE);
}

/* Status 0: disconnect  1: connect */

int fmass_register_notify(void(*notify)(void *context, int status), void *context)
{
  int i;
  struct fmass_notify *f_notify = g_notify.notify;

  if (notify == NULL)
    {
      usb_err("notify is NULL!\n");
      return -1;
    }

  if (LOS_MuxPend(g_notify.notify_mtx, LOS_WAIT_FOREVER) != LOS_OK)
    {
      return -1;
    }

  fmass_notify_init();
  for (i = 0; i < MAX_NOFIFY_NUM; i++)
    {
      if (f_notify[i].is_used == 0)
        {
          f_notify[i].is_used     = 1;
          f_notify[i].notify_cb   = notify;
          f_notify[i].notify_data = context;
          break;
        }
    }
  (void)LOS_MuxPost(g_notify.notify_mtx);
  return (i < MAX_NOFIFY_NUM) ? i : -1;
}

int fmass_unregister_notify(int handle)
{
  struct fmass_notify *f_notify = g_notify.notify;

  if (handle < 0 || handle >= MAX_NOFIFY_NUM)
    {
      return -1;
    }

  if (LOS_MuxPend(g_notify.notify_mtx, LOS_WAIT_FOREVER) != LOS_OK)
    {
      return -1;
    }

  if (f_notify[handle].is_used == 0)
    {
      (void)LOS_MuxPost(g_notify.notify_mtx);
      return -1;
    }

  f_notify[handle].is_used     = 0;
  f_notify[handle].notify_cb   = NULL;
  f_notify[handle].notify_data = NULL;
  (void)LOS_MuxPost(g_notify.notify_mtx);

  return 0;
}

int fmass_partition_startup(const char *path)
{
  struct mass_softc *mass = g_fmass;
  struct mass_dev_s *fmass;
  FAR struct inode *inode = NULL;
  uint32_t i;
  int ret;

  if (mass == NULL)
    {
      return -1;
    }
  fmass = &mass->dev;
  ret = find_blockdriver(path, O_RDWR, &inode);
  if (inode == NULL || ret != ENOERR)
    {
      DPRINTFN(0, "open blockdriver fail  ret = %d\n", ret);
      return -1;
    }

  fmass->path = (char *)malloc(strlen(path) + 1);
  if (fmass->path == NULL)
    {
      usb_err("malloc failed\n");
      inode_release(inode);
      return -1;
    }
  memcpy(fmass->path, path, strlen(path) + 1);

  for (i = 0; i < fmass->nluns; i++)
    {
      if (inode == fmass->file_node[i])
        {
          fmass->lun = i;
          break;
        }
    }

  inode_release(inode);

  if (i == fmass->nluns)
    {
      usb_err("The device path is invalid\n");
      return -1;
    }

  return 0;
}

void *fmass_bind(void)
{
  return (void *)g_fmass;
}

static void fmass_source_free(void)
{
  g_fmass = NULL;
}

static int usbclass_mass_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct usbdev_ep_s *ep;
  struct mass_driver_s *mass_drvr;
  struct mass_dev_s *mass_dev;

  if (driver == NULL || dev == NULL)
    {
      return -1;
    }

  mass_drvr = (struct mass_driver_s *)driver;
  mass_dev  = mass_drvr->dev;
  if (mass_dev == NULL)
    {
      return -1;
    }

  (void)snprintf_s(mass_dev->inquiry_str, sizeof(mass_dev->inquiry_str), sizeof(mass_dev->inquiry_str) - 1,
                   "%-8s%-16s%-4s","Mass","Storage Device","1.00");
  mass_dev->notify = g_notify.notify;
  mass_dev->lun    = 0;
  mass_dev->nluns  = 0;

  ep = DEV_ALLOCEP(dev, g_fmass_confd.iepd.addr,
                   (struct usb_epdesc_s *)&g_fmass_confd.iepd);
  if (ep == NULL)
    {
      usb_err("ep error\n");
      return -1;
    }
  ep->priv = (void *)mass_dev;
  mass_dev->bulkin = ep;
  DPRINTFN(1, "bulkin:%#x, %02x\n", ep, ep->eplog);

  mass_dev->bulkreq = EP_ALLOCREQ(ep);
  if (mass_dev->bulkreq == NULL)
    {
      usb_err("bulkreq alloc failed!\n");
      goto error_free_epin;
    }
  ep->handle_req = mass_dev->bulkreq;

  ep = DEV_ALLOCEP(dev, g_fmass_confd.oepd.addr,
                   (struct usb_epdesc_s *)&g_fmass_confd.oepd);
  if (ep == NULL)
    {
      usb_err("ep error\n");
      goto error_free_req;
    }
  ep->priv = (void *)mass_dev;
  ep->handle_req = mass_dev->bulkreq;
  mass_dev->bulkout = ep;
  DPRINTFN(1, "bulkout:%#x, %02x\n", ep, ep->eplog);

  if (LOS_MuxCreate(&g_notify.notify_mtx) != LOS_OK)
    {
      goto error_free_epout;
    }

#ifdef LOSCFG_USB_MASS_STORAGE_INDEPENDENT_MEMORY_POOL
  if (fmass_set_mem_pool(USB_MASS_STORAGE_MEM_ADDR, USB_MASS_STORAGE_MEM_LENGTH) != 0)
    {
      usb_err("fmass set mem pool failed\r\n");
      goto fail;
    }
#endif

  if (fmass_thread_init(mass_dev, dev) < 0)
    {
      usb_err("fmass_thread_init failed\n");
      goto fail;
    }

  return 0;

error_free_epout:
  DEV_FREEEP(dev, mass_dev->bulkout);

error_free_req:
  EP_FREEREQ(mass_dev->bulkin, mass_dev->bulkreq);

error_free_epin:
  DEV_FREEEP(dev, mass_dev->bulkin);
  return -1;

fail:
  usb_err("composite_fmass_bind failed\n");
  (void)usbclass_mass_unbind(driver, dev);
  return -1;
}

static int usbclass_mass_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct mass_driver_s *mass_drvr;
  struct mass_dev_s *mass_dev;
  int ret;

  if (driver == NULL || dev == NULL)
    {
      return -1;
    }

  mass_drvr = (struct mass_driver_s *)driver;
  mass_dev  = mass_drvr->dev;
  if (mass_dev == NULL)
    {
      return -1;
    }

  if (driver->ops != NULL && driver->ops->disconnect != NULL)
    {
      driver->ops->disconnect(driver, dev);
    }

  ret = fmass_thread_delete(mass_dev);
  if (ret < 0)
    {
      return -1;
    }

  if (mass_dev->dev_status == DEV_ST_CONNECTTED)
    {
      ret = fmass_dev_close(mass_dev);
      if (ret < 0)
        {
          usb_err("fmass dev close fail\n");
          return -1;
        }
    }

  if (mass_dev->bulkreq != NULL)
    {
      EP_FREEREQ(mass_dev->bulkin, mass_dev->bulkreq);
      mass_dev->bulkreq = NULL;
    }

  DEV_FREEEP(dev, mass_dev->bulkin);
  DEV_FREEEP(dev, mass_dev->bulkout);

  fmass_source_free();
#ifdef LOSCFG_USB_MASS_STORAGE_INDEPENDENT_MEMORY_POOL
  ret = memset_s((void*)USB_MASS_STORAGE_MEM_ADDR, USB_MASS_STORAGE_MEM_LENGTH, 0, USB_MASS_STORAGE_MEM_LENGTH);
  if (ret != 0)
    {
      return ret;
    }
#endif
  (void)LOS_MuxDelete(g_notify.notify_mtx);
  g_notify.notify_mtx = INVALID_MUX;

  return 0;
}

static int usbclass_mass_set_alt(struct mass_dev_s *mass, unsigned intf, unsigned alt)
{
  (void)intf;
  (void)alt;

  fmass_main_thread_signal(mass, FMASS_TASK_CONFIG_CHANGE);
  fmass_wakeup(mass);

  /* The '1' indecates that the invoking of 'usbd_endpoint_request' is in fmass's protocol */

  return 1;
}

static int usbclass_mass_reset(struct mass_dev_s *mass, unsigned intf, unsigned alt)
{
  (void)intf;
  (void)alt;

  fmass_main_thread_signal(mass, FMASS_TASK_RESET);
  fmass_wakeup(mass);

  /* The '1' indecates that the invoking of 'usbd_endpoint_request' is in fmass's protocol */

  return 1;
}

static uint8_t pre_alt = 0;
static int usbclass_mass_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                               const struct usb_ctrlreq_s *ctrl, uint8_t *dataout, size_t outlen)
{
  struct mass_dev_s *mass;
  struct mass_driver_s *drvr;
  uint16_t w_index;
  uint16_t w_value;
  uint16_t w_length;
  struct usbdev_req_s *req;
  errno_t ret;
  uint32_t nlun_info;

  (void)dataout;
  (void)outlen;

  if (driver == NULL || ctrl == NULL || dev == NULL)
    {
      return -1;
    }

  drvr = (struct mass_driver_s *)driver;
  mass = drvr->dev;
  if (mass == NULL)
    {
      return -1;
    }
  w_index  = USB_GETW(ctrl->index);
  w_value  = USB_GETW(ctrl->value);
  w_length = USB_GETW(ctrl->len);

  req = dev->ep0->handle_req;

  switch (ctrl->req)
    {
    case USB_REQ_GETINTERFACE:
      req->buf[0] = pre_alt;
      req->len = 1;
      ret = EP_SUBMIT(dev->ep0, req);
      if (ret != 0)
        {
          usb_err("Endpoint send fail!\n");
          req->result = 0;
          return -1;
        }
      break;
    case USB_REQ_SETCONFIGURATION:
    case USB_REQ_SETINTERFACE:
      {
        pre_alt = w_value;
        return usbclass_mass_set_alt(mass, w_index, w_value);
      }
    case USB_BULK_GET_MAX_LUN:
      {
        if (ctrl->type != (USB_DIR_IN |
            USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE))
          {
            break;
          }

        if (w_index != 0 || w_value != 0 || w_length != 1)
          {
            return -1;
          }

        nlun_info = 0;
        ret = memcpy_s(req->buf, USB_COMP_EP0_BUFSIZ, &nlun_info, sizeof(nlun_info));
        if (ret != EOK)
          {
            usb_err("memcpy_s fail, %d \n", ret);
            return -1;
          }
        req->len = 1;
        (void)EP_SUBMIT(dev->ep0, req);
        PRINTK("****** USB BULK MAX LUN %u ******\n", mass->nluns);
      }
      break;
    case USB_BULK_RESET_REQUEST:
      {
        if (ctrl->type == USBMSC_TYPE_SETUPOUT)
          {
            usbclass_mass_reset(mass, w_index, w_value);
            req->len   = 0;
            req->flags = USBDEV_REQFLAGS_NULLPKT;
            ret        = EP_SUBMIT(dev->ep0, req);
          } else {
            return -1;
          }
      }
      break;
    default:
      break;
    }
  return 0;
}

static void usbclass_mass_disconnect(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct mass_dev_s *mass;
  struct mass_driver_s *drvr;
  uint32_t flags;

  if (driver == NULL || dev == NULL)
    {
      return;
    }

  drvr = (struct mass_driver_s *)driver;
  mass = drvr->dev;
  if (mass == NULL)
    {
      return;
    }

  spin_lock_irqsave(&mass->lock, flags);
  if (taskstate_match(mass, FMASS_TASK_CONFIG_CHANGE))
    {
      spin_unlock_irqrestore(&mass->lock, flags);
      DPRINTFN(0, "Setting config, no need proc disconnect\n");
      return;
    }

  mass->dev_status = DEV_ST_DISCONNECT;
  spin_unlock_irqrestore(&mass->lock, flags);

  fmass_task_change_state(mass, FMASS_TASK_DISCONNECT);
  fmass_wakeup(mass);

  if (mass->bulk_in_enabled)
    {
      mass->bulk_in_enabled = 0;
      (void)EP_DISABLE(mass->bulkin);
    }

  if (mass->bulk_out_enabled)
    {
      mass->bulk_out_enabled = 0;
      (void)EP_DISABLE(mass->bulkout);
    }
}

static struct usbd_string g_fmass_device_strings[6] =
{
  { 0, g_dt_string_id },
  { 1, NULL },
  { 2, NULL },
  { 3, NULL },
  { 4, g_dt_string_buf },
  USBD_DEVICE_STRINGS_END
};

void mass_get_device_dec_info(struct usbd_string **device_strings, struct usb_devdesc_s **device_desc)
{
  *device_strings = &g_fmass_device_strings[1];
  *device_desc    = &g_fmass_device_desc;
}

void mass_mkdevdesc(uint8_t *buf)
{
  errno_t ret;

  if (USB_GETW(g_fmass_device_desc.vendor) == 0)
    {
      usb_err("VID is not set!\n");
      return;
    }

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_fmass_device_desc, sizeof(g_fmass_device_desc));
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return;
    }
}
#ifdef CONFIG_DRIVERS_USB_SUPPORT_CERTIFY
int16_t mass_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo, uint8_t speed, uint8_t type)
{
  (void)devinfo;
  (void)type;
  (void)speed;
  uint16_t total = USB_GETW(g_fmass_confd.confd.totallen);
  errno_t ret;

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_fmass_confd, total);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return -1;
    }

  return (int16_t)total;
}
#else
int16_t mass_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo)
{
  (void)devinfo;
  uint16_t total = USB_GETW(g_fmass_confd.confd.totallen);
  errno_t ret;

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_fmass_confd, total);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return -1;
    }

  return (int16_t)total;
}
#endif

int mass_mkstrdesc(uint8_t id, struct usb_strdesc_s *buf)
{
  errno_t ret;
  const char *str;
  int i;

  for (i = 0; g_fmass_device_strings[i].s != NULL; i++)
    {
      str = g_fmass_device_strings[i].s;
      if (g_fmass_device_strings[i].id == id)
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

#define MASS_NCONFIGS    1
#define MASS_CONFIGID    0
#define MASS_NINTERFACES 1
#define MASS_NSTRIDS     5
#define MASS_NUM_EPS     2
void mass_get_composite_devdesc(struct composite_devdesc_s *dev)
{
  (void)memset_s(dev, sizeof(struct composite_devdesc_s), 0, sizeof(struct composite_devdesc_s));

  dev->mkdevdesc  = mass_mkdevdesc;
  dev->mkconfdesc = mass_mkcfgdesc;
  dev->mkstrdesc  = mass_mkstrdesc;

  dev->nconfigs = MASS_NCONFIGS;           /* Number of configurations supported */
  dev->configid = MASS_CONFIGID;           /* The only supported configuration ID */

  dev->devinfo.ninterfaces = MASS_NINTERFACES; /* Number of interfaces in the configuration */
  dev->devinfo.nstrings    = MASS_NSTRIDS;     /* Number of Strings */
  dev->devinfo.nendpoints  = MASS_NUM_EPS;
}

int mass_classobject(int minor, struct usbdev_devinfo_s *devinfo,
                     struct usbdevclass_driver_s **classdev)
{
  struct mass_softc *mass_s;
  struct mass_dev_s *priv;
  struct mass_driver_s *drvr;

  (void)minor;
  (void)devinfo;

  /* Allocate the structures needed */

  mass_s = (struct mass_softc *)malloc(sizeof(struct mass_softc));
  if (mass_s == NULL)
    {
      return -1;
    }
  g_fmass = mass_s;

  /* Convenience pointers into the allocated blob */

  priv = &mass_s->dev;
  drvr = &mass_s->drvr;

  (void)memset_s(priv, sizeof(struct mass_dev_s), 0, sizeof(struct mass_dev_s));
  spin_lock_init(&priv->lock);

  drvr->drvr.speed = USB_SPEED_HIGH;
  drvr->drvr.ops   = &g_mass_driverops;
  drvr->dev        = priv;

  *classdev = &drvr->drvr;
  return 0;
}

void mass_uninitialize(struct usbdevclass_driver_s *classdev)
{
  struct mass_driver_s *mass_drvr = (struct mass_driver_s *)classdev;
  struct mass_dev_s *priv;
  struct mass_softc *mass_s;

  dev_str_desc_free(&g_fmass_device_strings[1]);

  if (mass_drvr == NULL)
    {
      return;
    }

  priv = mass_drvr->dev;
  if (priv == NULL)
    {
      return;
    }

  mass_s = container_of(mass_drvr, struct mass_softc, drvr);
  free(mass_s);
}

void usbdev_mass_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor)
{
  mass_get_composite_devdesc(dev);

  dev->classobject  = mass_classobject;
  dev->uninitialize = mass_uninitialize;

  dev->devinfo.ifnobase = ifnobase; /* Offset to Interface-IDs */
  dev->minor            = minor;    /* The minor interface number */

  dev->devinfo.strbase = 0;         /* Offset to String Numbers */
}

int usbdev_mass_initialize(void *handle)
{
  struct composite_devdesc_s dev;
  int ret;

  usbdev_mass_initialize_sub(&dev, 0, DEV_MASS);

  ret = composite_initialize_softc(1, &dev, handle);
  if (ret < 0)
    {
      return -1;
    }

  PRINTK("  ** Mass device initialized successfully! **\n");
  return 0;
}

#undef USB_DEBUG_VAR
