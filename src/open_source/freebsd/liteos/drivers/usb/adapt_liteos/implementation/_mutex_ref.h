/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1997 Berkeley Software Design, Inc. All rights reserved.
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2025. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Berkeley Software Design Inc's name may not be used to endorse or
 *    promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY BERKELEY SOFTWARE DESIGN INC ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL BERKELEY SOFTWARE DESIGN INC BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from BSDI $Id: mutex.h,v 2.7.2.35 2000/04/27 03:10:26 cp Exp $
 */

#ifndef _MUTEX_REF_H
#define	_MUTEX_REF_H

#include <los_mux.h>

/*Ref. usb_process.c*/
#define	MA_OWNED	1
#define	MA_NOTOWNED	2
#define MTX_NOT_INIT 0xffff

typedef struct mtx {
	uint32_t mux;
} mtx_s;

#define	mtx_init(m, n, t, o)	(void)usb_mutex_init((m), (o))
#define	mtx_destroy(m)		(void)usb_mutex_destroy((m))
#define	mtx_lock(m)		(void)usb_mutex_lock((m))
#define	mtx_unlock(m)		(void)usb_mutex_unlock((m))
#define	mtx_lock_spin(m)
#define	mtx_unlock_spin(m)
#define	mtx_owned(m)		usb_mutex_owned((m))

#ifdef USB_HOST_MTX_DEBUG
#define	mtx_assert(m, what) do {		\
	switch (what) {		\
	case MA_OWNED:	\
		break;	\
	case MA_NOTOWNED:	\
		if (mtx_owned(m)) {panic("mtx assert at %s:%d\n", __FUNCTION__, __LINE__);}		\
		break;	\
	default:	\
		{panic("mtx assert at %s:%d\n", __FUNCTION__, __LINE__);}	\
	}	\
} while (0)
#else
#define	mtx_assert(m, what)
#endif

int	usb_mutex_init(struct mtx *mutex, unsigned int flag);
int	usb_mutex_owned( const struct mtx *mutex);
int	usb_mutex_destroy(const struct mtx *mutex);
int	usb_mutex_lock(const struct mtx *mutex);
int	usb_mutex_unlock(const struct mtx *mutex);

static INLINE void usb_lock_init(struct mtx *mtx)
{
	if (mtx->mux == MTX_NOT_INIT)
		(void)usb_mutex_init(mtx, 0);
}

static INLINE void usb_lock_destroy(struct mtx *mtx)
{
	if (mtx->mux != MTX_NOT_INIT) {
		(void)usb_mutex_destroy(mtx);
		mtx->mux = MTX_NOT_INIT;
	}
}

#endif /* _MUTEX_REF_H */
