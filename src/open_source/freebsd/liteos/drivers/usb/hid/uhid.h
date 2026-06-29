/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2025. All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net) at
 * Carlstedt Research & Technology.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * HID spec: http://www.usb.org/developers/devclass_docs/HID1_11.pdf
 */

/* Refer to sys/dev/usb/input/uhid.c */
#ifndef UHID_H
#define UHID_H

#include "implementation/global_implementation.h"

#define	UHID_BSIZE	1024	/* bytes, buffer size */
#define	UHID_FRAME_NUM	50	/* bytes, frame number */

enum {
	UHID_INTR_DT_WR,
	UHID_INTR_DT_RD,
	UHID_CTRL_DT_WR,
	UHID_CTRL_DT_RD,
	UHID_N_TRANSFER,
};

struct uhid_softc {
	struct usb_fifo_sc sc_fifo;
	struct mtx sc_mtx;

	struct usb_xfer *sc_xfer[UHID_N_TRANSFER];
	struct usb_device *sc_udev;
	void   *sc_repdesc_ptr;

	uint32_t sc_isize;
	uint32_t sc_osize;
	uint32_t sc_fsize;

	uint16_t sc_repdesc_size;

	uint8_t	sc_iface_no;
	uint8_t	sc_iface_index;
	uint8_t	sc_iid;
	uint8_t	sc_oid;
	uint8_t	sc_fid;
	uint8_t	sc_flags;
	uint8_t	sc_port;
#define	UHID_FLAG_IMMED		  0x01	/* set if read should be immediate */
#define	UHID_FLAG_STATIC_DESC 0x04	/* set if report descriptors are static */
#define	UHID_FLAG_ACTIVE	  0x08	/* set if uhid is active */
};

#endif /* UHID_H */
