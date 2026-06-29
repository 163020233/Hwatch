/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: header file for TTS frontend module. \n
 * Author: @CompanyNameTag \n
 */

#ifndef __SOC_UAPI_TTS_FRONTEND_H__
#define __SOC_UAPI_TTS_FRONTEND_H__

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define AUDIO_SUCCESS 0
#define AUDIO_FAILURE (-1)

#define AUDIO_CHS_MAX_IN_NUM 50
#define AUDIO_CHS_MAX_IN_BYTE (AUDIO_CHS_MAX_IN_NUM << 1)
#define AUDIO_CHS_MAX_OUT_NUM (AUDIO_CHS_MAX_IN_BYTE << 2)
#define AUDIO_PIN_YIN_MAX_OUT_SIZE 100

typedef enum {
    PIN_YIN_ERR_MEM_APPLY_FAIL   = 0x80005001,
    PIN_YIN_ERR_IN_BUFFER_NULL   = 0x80005002,
    PIN_YIN_ERR_HANDLE_PTR_NULL  = 0x80005003,
    PIN_YIN_ERR_NOT_ENOUGH_BUF   = 0x80005004,
    PIN_YIN_ERR_IN_BUF_OVERFLOW  = 0x80005005,
    PIN_YIN_ERR_IN_BYTES_ERR     = 0x80005006,
} pin_yin_err_code;

typedef struct {
    uint16_t  *out_buffer;
    uint8_t   *out_buffer_flag;
    uint32_t   out_buffer_size;
    char      *in_buffer;
    uint32_t   in_buffer_size;
    uint32_t   total_index;
    uint32_t   index;
    uint32_t   next_offset;
} pin_yin_stru;

int audio_pinyin_convert_init(void **handle, uint8_t *buf, uint32_t buf_size);
int audio_pinyin_convert_process(void *handle, char *in, uint16_t in_len);
int audio_pinyin_convert_deinit(void *handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SOC_UAPI_TTS_FRONTEND_H__ */