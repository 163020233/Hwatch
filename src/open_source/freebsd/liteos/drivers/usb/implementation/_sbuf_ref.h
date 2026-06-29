/* ----------------------------------------------------------------------------
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2009 Andrew Thompson
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2025. All rights reserved.
 * Description: Sound buffer header File
 * Author: Huawei LiteOS Team
 * Create: 2025-07-21
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* from sys/sys/sbuf.h */


#ifndef _SBUF_REF_H_
#define _SBUF_REF_H_

typedef int (sbuf_drain_func)(void *, const char *, int);
struct sbuf {
    char            *s_buf; /* storage buffer */
    sbuf_drain_func *s_drain_func; /* drain function */
    void           *s_drain_arg; /* user-supplied drain argument */
    int            s_error; /* current error code */
    ssize_t        s_size; /* size of storage buffer */
    ssize_t        s_len; /* current length of string */
#define SBUF_FIXEDLEN   0x00000000 /* fixed length buffer (default) */
#define SBUF_AUTOEXTEND 0x00000001 /* automatically extend buffer */
#define SBUF_INCLUDENUL 0x00000002 /* nulterm byte is counted in len */
#define SBUF_DRAINTOEOR 0x00000004 /* use section 0 as drain EOR marker */
#define SBUF_NOWAIT     0x00000008 /* Extend with non-blocking malloc */
#define SBUF_USRFLAGMSK 0x0000ffff /* mask of flags the user may specify */
#define SBUF_DYNAMIC    0x00010000 /* s_buf must be freed */
#define SBUF_FINISHED   0x00020000 /* set by sbuf_finish() */
#define SBUF_DYNSTRUCT  0x00080000 /* sbuf must be freed */
#define SBUF_INSECTION  0x00100000 /* set by sbuf_start_section() */
    int          s_flags; /* flags */
    ssize_t      s_sect_len; /* current length of section */
    ssize_t      s_rec_off; /* current record start offset */
};

#endif // _SBUF_REF_H_
