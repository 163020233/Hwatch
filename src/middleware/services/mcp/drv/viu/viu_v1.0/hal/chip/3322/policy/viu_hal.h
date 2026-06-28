/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_hal.h
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef VIU_HAL_H
#define VIU_HAL_H

#include "drv_mcp_dependency.h"
#include "drv_viu_struct.h"
#include "vicap_reg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VIU_HAL_CHN0                  0

/* vi图像存储信息 */
typedef struct {
    td_u64 y_addr;
    td_u64 u_addr;
    td_u64 v_addr;
    td_u32 y_stride;
    td_u32 u_stride;
    td_u32 v_stride;
    ext_compress_mode compress_mode;
    td_u64 hy_addr;     /* Y压缩头地址 */
    td_u64 hc_addr;     /* C压缩头地址 */
    td_u32 h_stride;    /* 压缩头stride */
}viu_hal_addr;

typedef enum {
    VIU_PACK_8BIT = 8,
    VIU_PACK_10BIT = 10,
    VIU_PACK_12BIT = 12,
    VIU_PACK_14BIT = 14,
    VIU_PACK_16BIT = 16,
    VIU_PACK_32BIT = 32,
    VIU_PACK_BUTT
} viu_pack_bit_width;

td_void viu_hal_set_init_vicap_reg(volatile vicap_regs_type *reg);
td_u32 viu_hal_get_cap_id_by_pipe(td_s32 viu_pipe_id);

td_void viu_hal_set_isp_mode(td_s32 isp_id, td_bool isp_bypass, td_bool isp_in_yuv);

td_void viu_hal_set_wch_cmp_bayer_mode(td_s32 viu_pipe_id, td_u32 bayer_mode);

td_void viu_hal_set_isp_id_sel(td_s32 viu_pipe_id, td_u32 id_sel);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __VIU_HAL_H__ */

