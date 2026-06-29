/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu capability
 * Author: AuthorNameMagicTag
 * Create: 2025-02-18
 */

#ifndef __HAL_CAPABILITY_H__
#define __HAL_CAPABILITY_H__

#include "drv_mcp_dependency.h"
#include "drv_viu_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    td_u32 support_bas;
    td_u32 clk_sel;
} hal_cap_videv_instance;

typedef struct {
    hal_cap_videv_instance instance[MCP_VIDEV_INSTANCE_MAX];
} hal_capability_videv;

#define HAL_VICAP_SUPPORT_PIX_FMT_MAX (0x8)

typedef struct {
    td_u32 support_pre_crop;
    td_u32 support_post_crop;

    td_u32 max_w;
    td_u32 max_h;
    td_u32 min_w;
    td_u32 min_h;

    td_u32 isp_clk_sel;
    td_u32 stride_align;

    td_u32 support_pix_fmt_num;
    ext_pixel_format support_pix_fmt[HAL_VICAP_SUPPORT_PIX_FMT_MAX];
    td_bool support_cmp_mode[EXT_ISP_COMPRESS_MODE_MAX];

    td_s32 support_bind_mod;
} hal_cap_vicap_instance;

typedef struct {
    td_u32 clk_sel;
    td_u32 irq_affinity;
    td_u32 irq_num;
    td_char *irq_name;
    hal_cap_vicap_instance instance[MCP_VICAP_INSTANCE_MAX];
} hal_capability_vicap;

typedef struct {
    td_u32 max_w;
    td_u32 max_h;
} hal_cap_viproc_instance;

typedef struct {
    td_u32 clk_sel;
    td_u32 irq_affinity;
    td_u32 irq_num;
    td_char *irq_name;
    hal_cap_viproc_instance instance[MCP_VIPROC_INSTANCE_MAX];
} hal_capability_viproc;

typedef struct {
    hal_capability_videv videv;
    hal_capability_vicap vicap;
    hal_capability_viproc viproc;
} hal_capability_info;

hal_capability_videv* hal_cap_get_videv(td_void);
hal_capability_vicap* hal_cap_get_vicap(td_void);
hal_capability_viproc* hal_cap_get_viproc(td_void);
td_s32 hal_capability_init(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_VI_H__ */
