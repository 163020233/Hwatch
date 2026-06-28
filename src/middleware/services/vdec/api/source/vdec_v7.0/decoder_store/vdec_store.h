/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#ifndef __VDEC_STORE_H__
#define __VDEC_STORE_H__

#include "vdec_decoder_intf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_s32  vdec_store_get_cap(ext_vdec_capability *cap);
td_s32  vdec_store_open(td_s32 *chan_id);
td_s32  vdec_store_close(td_s32 chan_id);
td_s32  vdec_store_register_decoder(const td_char *lib, td_u32 len);
td_s32  vdec_store_unregister_decoder(const td_char *lib, td_u32 len);
td_s32  vdec_store_query_decoder(ext_vdec_standard std_type, vdec_decoder **decoder);
td_void vdec_store_get_default_decoder(vdec_decoder **decoder);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
