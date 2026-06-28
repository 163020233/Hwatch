/*-
 * SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0
 *
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * GPL LICENSE SUMMARY
 *
 * Copyright(c) 2008 - 2011 Intel Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 * The full GNU General Public License is included in this distribution
 * in the file called LICENSE.GPL.
 *
 * BSD LICENSE
 *
 * Copyright(c) 2008 - 2010 Intel Corporation. All rights reserved.
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2019. All rights reserved.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD: releng/14.1/sys/dev/isci/scil/intel_scsi.h 326255 2023-08-17 01:54:11Z pfg $
 */
/**
 * @file
 * @brief This file defines all of the SCSI related constants, enumerations,
 *        and types.  Please note that this file does not necessarily contain
 *        an exhaustive list of all constants, commands, sub-commands, etc.
 */

#ifndef _F_MASS_STORAGE_PRI_H
#define _F_MASS_STORAGE_PRI_H

#include <fs/fs.h>
#include <disk.h>
#include "gadget/composite.h"
#include "gadget/f_mass_storage.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Command Block Wrapper.
 * Refer to USB Mass Storage Class. Bulk Only Transport 5.1 section.
 */

typedef struct bulk_cbw
{
  uint32_t   cbwsig;                 /* Command Block Wrapper Signature */
  uint32_t   cbwtag;                 /* Command Block Tag */
  uint32_t   cbwdatalen;             /* Data Transfer Length */
  uint8_t    cbwflags;
  uint8_t    cbwlun;                 /* Logical Unit Number */
  uint8_t    cbwlen;                 /* Command Block Length */
  uint8_t    cdb[16];
} bulk_cbw_t;

/* Command Status Wrapper.
 * Refer to USB Mass Storage Class. Bulk Only Transport 5.2 section.
 */

typedef struct bulk_csw
{
  uint32_t   cswsig;           /* Command Status Wrapper Signature */
  uint32_t   cswtag;           /* Command Status Tag */
  uint32_t   cswresidue;
  uint8_t    cswstatus;
} bulk_csw_t;

#define BULK_CBW_WRAP_LEN   31
#define BULK_CBW_SIGN       0x43425355
#define BULK_CBW_FLAG_IN    (1 << 7)
#define BULK_CBW_FLAG_OUT   0

#define BULK_CSW_WRAP_LEN    13
#define BULK_CSW_SIGN        0x53425355
#define BULK_CSW_STAT_OK     0
#define BULK_CSW_STAT_FAIL   1
#define BULK_CSW_STAT_PHASE  2

/* Bulk-only class specific requests */

#define USB_BULK_RESET_REQUEST   0xff
#define USB_BULK_GET_MAX_LUN     0xfe

/* The implementation is based on to FreeBSD open source code */

#define SCSI_TEST_UNIT_READY        0x00
#define SCSI_REQUEST_SENSE          0x03
#define SCSI_FORMAT_UNIT            0x04
#define SCSI_INQUIRY                0x12
#define SCSI_MODE_SELECT_6          0x15
#define SCSI_MODE_SELECT_10         0x55
#define SCSI_MODE_SENSE_6           0x1a
#define SCSI_MODE_SENSE_10          0x5a
#define SCSI_ALLOW_MEDIUM_REMOVAL   0x1e
#define SCSI_READ_FORMAT_CAPACITIES 0x23
#define SCSI_READ_CAPACITY          0x25
#define SCSI_READ_CAPACITY16        0x9e
#define SCSI_READ_6                 0x08
#define SCSI_READ_10                0x28
#define SCSI_READ_12                0xa8
#define SCSI_WRITE_6                0x0a
#define SCSI_WRITE_10               0x2a
#define SCSI_WRITE_12               0xaa
#define START_STOP                  0x1b

/* SCSI Sense Key/Additional Sense Code/ASC Qualifier values */

#define SCSI_NO_SENSE                           0
#define SCSI_COMMUNICATION_FAILURE              0x040800
#define SCSI_INVALID_COMMAND                    0x052000
#define SCSI_INVALID_FIELD_IN_CDB               0x052400
#define SCSI_INVALID_COM                        0x052600
#define SCSI_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE 0x052100
#define SCSI_LOGICAL_UNIT_NOT_SUPPORTED         0x052500
#define SCSI_MEDIUM_NOT_PRESENT                 0x023a00
#define SCSI_MEDIUM_REMOVAL_PREVENTED           0x055302
#define SCSI_UNIT_ATTENTION                     0x060000
#define SCSI_NOT_READY_TO_READY_TRANSITION      0x062800
#define SCSI_RESET_OCCURRED                     0x062900
#define SCSI_SAVING_PARAMETERS_NOT_SUPPORTED    0x053900
#define SCSI_UNRECOVERED_READ_ERROR             0x031100
#define SCSI_WRITE_ERROR                        0x030c02
#define SCSI_WRITE_PROTECTED                    0x072700

#define SK(x)         ((uint8_t) ((x) >> 16))    /* Sense Key byte, etc. */
#define ASC(x)        ((uint8_t) ((x) >> 8))
#define ASCQ(x)       ((uint8_t) (x))

#define FMASS_MAX_LUNS    8

typedef enum data_direct_t
{
  DATA_DIRECT_UNKNOWN = 0,
  DATA_DIRECT_FROM_HOST,
  DATA_DIRECT_TO_HOST,
  DATA_DIRECT_NONE
} data_direct;

typedef enum fmass_task_state_t
{
  FMASS_TASK_IDLE               = 0x0,
  FMASS_TASK_DISCONNECT         = 0x01,
  FMASS_TASK_CONFIG_CHANGE      = 0x02,
  FMASS_TASK_REQ_COMMAND_PHASE  = 0x04,
  FMASS_TASK_DONE_COMMAND_PHASE = 0x08,
  FMASS_TASK_REQ_DATA_PHASE     = 0x10,
  FMASS_TASK_DONE_DATA_PHASE    = 0x20,
  FMASS_TASK_REQ_STATUS_PHASE   = 0x40,
  FMASS_TASK_DONE_STATUS_PHASE  = 0x80,
  FMASS_TASK_REPORT_USB_STATUS  = 0x100,
  FMASS_TASK_RESET              = 0x200
} fmass_task_state;

/* Length of a SCSI Command Data Block */

#define MAX_COMMAND_SIZE       16

#ifdef LOSCFG_DRIVERS_USB_MASS_STORAGE_MAX_RW_SECTORS
#define MAX_BLOCK_RW_SECTORS   LOSCFG_DRIVERS_USB_MASS_STORAGE_MAX_RW_SECTORS
#else
#define MAX_BLOCK_RW_SECTORS   4
#endif

#ifdef LOSCFG_DRIVERS_USB_MASS_STORAGE_MAX_BUF_NUM
#define MAX_DATA_BUFFER_NUM    LOSCFG_DRIVERS_USB_MASS_STORAGE_MAX_BUF_NUM
#else
#define MAX_DATA_BUFFER_NUM   2
#endif

#define MAX_FILE_STORAGE_LUNS  SYS_MAX_PART
#define MAX_NOFIFY_NUM         10
typedef void (*fmass_notify_cb)(void *context, int status);

struct fmass_notify
{
  fmass_notify_cb notify_cb;
  void *notify_data;
  int   is_used;
};

enum fmass_data_buf_state
{
  DBUF_STATE_EMPTY = 0,
  DBUF_STATE_FULL,
  DBUF_STATE_BUSY
};

struct fmass_data_buf_t
{
  void    *buf;
  uint32_t filled_bit;
  struct fmass_data_buf_t  *next;
  enum fmass_data_buf_state state;
};

struct fmass_capacity
{
  size_t nsectors;     /* Number of sectors on the device */
  size_t sector_size;  /* Size of one sector */
  int    read_only;
};

struct mass_dev_s
{
  data_direct data_dir;
  uint32_t    data_size;
  uint32_t    data_size_from_cmd;
  uint32_t    tag;
  uint32_t    residue;
  uint32_t    usb_amount_left;


  int     cmd_size;
  uint8_t cmd[MAX_COMMAND_SIZE];

  uint32_t    sense_data;
  uint32_t    sense_data_info;
  uint32_t    unit_attention_data;
  uint32_t    info_valid;

  uint8_t bulk_in_enabled;
  uint8_t bulk_out_enabled;

  struct bulk_csw csw;

  /* Vendor (8 chars), product (16 chars), release (4
   * hexadecimal digits) and NULL byte
   */

  char        inquiry_str[8 + 16 + 4 + 1];
  uint32_t    nluns;
  uint32_t    lun;

#define GET_SECTOR_COUNT  1 /* Get media size (needed at _USE_MKFS == 1) */
#define GET_SECTOR_SIZE   2 /* Get sector size (needed at _MAX_SS != _MIN_SS) */

  char                    *path;
  los_part                *parts[MAX_FILE_STORAGE_LUNS];
  FAR struct inode        *file_node[MAX_FILE_STORAGE_LUNS];
  struct fmass_capacity    caps[MAX_FILE_STORAGE_LUNS];
  struct fmass_data_buf_t *databuf_fill;
  struct fmass_data_buf_t  databuf[MAX_DATA_BUFFER_NUM];
  uint32_t fmass_db_bitmap;

#define FMASS_DATA_PROC     0x01
#define FMASS_NEED_EXIT     0x02
#define FMASS_THREAD_EXITED 0x04
  EVENT_CB_S task_event;

  wait_queue_head_t xfer_wait;
  uint32_t          task_state;
  spinlock_t        lock;

  struct fmass_notify *notify;
  int                  dev_status;    /* 0: usb device disconnect , 1 : usb device connected */
#define DEV_ST_DISCONNECT 0
#define DEV_ST_CONNECTTED 1

  struct usbdev_req_s *bulkreq;
  struct usbdev_ep_s *bulkout;
  struct usbdev_ep_s *bulkin;
};

struct mass_driver_s
{
  struct usbdevclass_driver_s drvr;
  struct mass_dev_s          *dev;
};

struct mass_softc
{
  struct mass_dev_s    dev;
  struct mass_driver_s drvr;
};

struct mass_usb_dev_s
{
  struct mass_dev_s *fmass;
  struct usbdev_s *dev;
};

void usbdev_mass_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _F_MASS_STORAGE_PRI_H */
