/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: hal_ip of crg
 * Author: AuthorNameMagicTag
 * Create: 2025-05-29
 */

#include "hal_capability_intf.h"
#include "chip_core_irq.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* #ifdef __cplusplus */

static hal_capability_info g_viu_cap = {0};

#define VIU_PIPE0_MAX_WIDTH             4096
#define VIU_PIPE0_MAX_HEIGHT            4096
#define VIU_PIPE0_MIN_WIDTH             120
#define VIU_PIPE0_MIN_HEIGHT            120

static td_void capability_init_vicap_0(hal_cap_vicap_instance *instance)
{
    instance->max_w = VIU_PIPE0_MAX_WIDTH;
    instance->max_h = VIU_PIPE0_MAX_HEIGHT;
    instance->min_w = VIU_PIPE0_MIN_WIDTH;
    instance->min_h = VIU_PIPE0_MIN_HEIGHT;

    instance->support_pix_fmt_num = 0x4;
    instance->support_pix_fmt[0] = EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    instance->support_pix_fmt[0x1] = EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_422;
    instance->support_pix_fmt[0x2] = EXT_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    instance->support_pix_fmt[0x3] = EXT_PIXEL_FORMAT_YUV_SEMIPLANAR_422;

    instance->stride_align = 16; // 16 align
}

static td_void capability_init_vicap(hal_capability_vicap *cap_vicap)
{
    cap_vicap->irq_name = "mcp_vicap_int";
    cap_vicap->irq_num = VICAP_INTR_IRQN; // 89 16 irq num
    cap_vicap->clk_sel = 0x8;  /* 3322 vicap sdiom 工作时钟200M */

    if (MCP_VICAP_INSTANCE_MAX > 0x0) {
        capability_init_vicap_0(&cap_vicap->instance[0]);
    }

    if (MCP_VICAP_INSTANCE_MAX > 0x1) {
        capability_init_vicap_0(&cap_vicap->instance[0x1]);
    }
}

hal_capability_videv* hal_cap_get_videv(td_void)
{
    return &g_viu_cap.videv;
}

hal_capability_vicap* hal_cap_get_vicap(td_void)
{
    return &g_viu_cap.vicap;
}

td_s32 hal_capability_init(td_void)
{
    capability_init_vicap(&g_viu_cap.vicap);

    return TD_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */