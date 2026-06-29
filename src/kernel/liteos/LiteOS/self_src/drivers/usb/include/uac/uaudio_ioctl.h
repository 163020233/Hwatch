/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UAC ioctl releated HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2025-07-19
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

#ifndef _UAUDIO_IOCTL_H
#define _UAUDIO_IOCTL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define UACIOC_ENUM_FMTS            _IOWR('V', 1, struct uac_formats)
#define UACIOC_SET_FMT              _IOWR('V', 2, struct uac_format)

#define UACIOC_STREAMON             _IOW('V', 3, int)
#define UACIOC_STREAMOFF            _IOW('V', 4, int)

#define UACIOC_REQBUFS              _IOWR('V', 5, int)
#define UACIOC_QBUF                 _IOWR('V', 6, struct uac_buffer_st)
#define UACIOC_DQBUF                _IOWR('V', 7, struct uac_buffer_st)


// This macro was move from liteOS/drivers/usb/uac/uaudio.c
#define CHAN_MAX_ALT 24

struct uac_format {
    uint32_t rates;
    uint8_t channels;
    uint8_t bitwidth;
};

struct uac_formats {
    uint8_t nums;
    struct uac_format format[CHAN_MAX_ALT];
};

struct uac_buffer_st {
    uint32_t addr;        /* pointer to buffer */
    uint16_t size;        /* buffer size, according to uac1, if a pcm buffer occupys 1ms data, 65535 is enough */
};

// when issuing UACIOC_REQBUFS, the maximun count for the cmd, is UAC_MAX_FRAME.
#define UAC_MAX_FRAME 30

// The following definations just for driver_used.
#define UAC_BUF_FLAG_QUEUED   0x00000001
#define UAC_BUF_FLAG_DONE     0x00000002
struct uac_buffer_st_drv {
    uint32_t addr;
    uint16_t size;
    uint32_t flags;
    struct LOS_DL_LIST	frame_queue;
};


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _UAUDIO_IOCTL_H */

