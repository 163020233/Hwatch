/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: audio src component define
 * Author: audio
 */

#ifndef __ASRC_STRUCT_H__
#define __ASRC_STRUCT_H__

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum {
    ASRC_SOC = 0,  /* SOC asrc */
    ASRC_IMEDIA,   /* Imedia asrc */
    ASRC_XTENSA,   /* Tensilica XA asrc */
    ASRC_MAX,
} asrc_id;

typedef struct {
    td_u32 fs_in;
    td_u32 fs_out;
    td_u32 channels;
    td_u32 bit_depth;
    td_void *scratch_buf;
    td_u32 scratch_buf_size;
} asrc_config;

typedef struct {
    const td_char *name;
    td_u32 id; /* see asrc_id */
    td_void *(*create)(const asrc_config *cfg);
    td_void (*destroy)(td_void *handle);
    td_u32 (*process)(td_void *handle, const td_s32 *in_buf, td_s32 *out_buf, td_u32 samples);
    td_void (*flush)(td_void *handle);
} asrc_entry;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* __ASRC_STRUCT_H__ */

