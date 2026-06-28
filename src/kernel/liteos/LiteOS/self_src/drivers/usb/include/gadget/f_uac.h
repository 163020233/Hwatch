/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: LiteOS USB Driver UAC Protocol HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2017-12-12
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

#ifndef _USB_UAC_INCLUDE_H
#define _USB_UAC_INCLUDE_H

#include <los_typedef.h>
#include "los_event.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct uac_opts
{
  int c_chmask;          /* Capture channel mask. */
  int c_srate;           /* Capture default sample rate. */
  int c_ssize;           /* Capture default sample size. */

  int p_chmask;          /* Playback channel mask. */
  int p_srate;           /* Playback default sample rate. */
  int p_ssize;           /* Playback default sample size. */

  uint8_t c_freq_num;    /* Capture frequency number.  */
  uint8_t p_freq_num;    /* Playback frequency number. */

#ifdef LOSCFG_DRIVERS_USB_UAC_GADGET_FU
  bool c_mute;           /* Capture mute. */
  int16_t c_volume_min;  /* Capture min volume. */
  int16_t c_volume_max;  /* Capture max volume. */
  int16_t c_volume_res;  /* Capture volume adjustment resolution. */
  int16_t c_volume_curr; /* Capture the current volume value. */

  bool p_mute;           /* Playback mute. */
  int16_t p_volume_min;  /* Playback min volume. */
  int16_t p_volume_max;  /* Playback max volume. */
  int16_t p_volume_res;  /* Playback volume adjustment resolution. */
  int16_t p_volume_curr; /* Playback the current volume value. */
#endif
#ifdef LOSCFG_DRIVERS_USB_UAC_DYNAMIC_SPEAKER
  uint8_t speaker_num;   /* speaker number, only support 0/1. */
#endif
};

int fuac_opts_set(const struct uac_opts *opts);
int fuac_get_opts(struct uac_opts *opts);
int fuac_rate_get(void);

#if LOSCFG_DRIVERS_USB_UAC_GADGET_SPEAKER
int fuac_receive_message(void *buf, int len, int timeout);
#endif

/* The type of events  */
typedef enum {
  UAC_UNBIND =            (1 << 0),
  UAC_SET_CONFIG =        (1 << 1),
  UAC_START_PLAYBACK =    (1 << 2),
  UAC_STOP_PLAYBACK =     (1 << 3),
  UAC_MUTE =              (1 << 4),
  UAC_VOLUME_INC =        (1 << 5),
  UAC_VOLUME_DEC =        (1 << 6),
  UAC_PSRATE_UPDATE =     (1 << 7),
  UAC_CSRATE_UPDATE =     (1 << 8),
  UAC_PSSIZE_UPDATE =     (1 << 9),
  UAC_START_CAPTURE =     (1 << 10),
  UAC_STOP_CAPTURE =      (1 << 11),
} uac_event_type;

typedef struct {
  EVENT_CB_S         notification_event;
  uac_event_type     event_types;
  int32_t            psrate;
  int32_t            csrate;
  int32_t            pssize;
  int32_t            volume;
} event_message;

#define UAC_NOTIFICATION_EVENT   0x5A
#define INVALID                  INT32_MAX    /* Invalid value */

/* UAC function error codes */
#define UAC_OK                   0x00     /* No error, function has successfully completed */
#define UAC_ERROR_NOMATCH        0x01     /* The operation can not be complete, due to mismatch of UAC state */
#define UAC_ERROR_PTR            0x02     /* Invalid pointer */
#define UAC_ERROR_MEMORY         0x03     /* Memory error */
#define UAC_ERROR_VALUE          0x04     /* Incorrect value */
#define UAC_ERROR_FATAL          0x05     /* Fatal internal error, please contact the one who wrote the code */
#define UAC_ERROR_EVENT          0x06     /* Event read or write error */
#define UAC_ERROR_TIMEOUT        0x07     /* Event read timeout */
#define UAC_ERROR_DISCONNECT     0x08     /* USB disconnect */

#define UAC_WAIT_HOST_NOP         0x0
#define UAC_WAIT_HOST_FOREVER     0x1
#define UAC_WAIT_HOST_OUT_NOP     0x2
#define UAC_WAIT_HOST_OUT_FOREVER 0x3

/**
 * Brief: achieve the address of notification field of uac device structure.
 * Note: return value is the address of the structure internal field, which is not dynamic memory allocation,
 *       and don't need to be free manually.
 */
event_message *fuac_get_notification(void);

int fuac_reqbuf_init(uint32_t buf_count, uint32_t data_len);
int fuac_reqbuf_deinit(void);
uint8_t *fuac_reqbuf_get(uint32_t *buf_index);
int fuac_send_message(const uint8_t *buf, uint32_t len, uint32_t buf_index);

int uac_wait_host(int wait_option);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _USB_UAC_INCLUDE_H */
