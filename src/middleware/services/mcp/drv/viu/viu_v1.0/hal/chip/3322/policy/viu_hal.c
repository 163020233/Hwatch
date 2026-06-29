/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_hal.c
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#include "viu_hal.h"
#include "viu_public.h"
#include "hal_adapt_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static volatile vicap_regs_type *viu_cap_reg[ISP_VICAP_IP_NUM];

td_void viu_hal_set_init_vicap_reg(volatile vicap_regs_type *reg)
{
    viu_cap_reg[0] = reg;
}

td_void viu_hal_set_wch_buffer_wrap_line(td_s32 wch, td_u32 cache_line)
{
    td_u32 cap_id;
    cap_id = viu_hal_get_cap_id_by_pipe(wch);

    {
        u_ch_wch_y_cfg t;

        t.u32 = viu_cap_reg[cap_id]->ch_wch_y_cfg.u32;
        t.bits.wch_y_wrap_line = cache_line - 1;
        viu_cap_reg[cap_id]->ch_wch_y_cfg.u32 = t.u32;
    }
}

td_void viu_hal_set_wch_buffer_wrap_enable(td_s32 wch, td_bool enable)
{
    td_u32 cap_id;
    cap_id = viu_hal_get_cap_id_by_pipe(wch);

    {
        u_ch_wch_y_cfg t;

        t.u32 = viu_cap_reg[cap_id]->ch_wch_y_cfg.u32;
        t.bits.wch_y_wrap_en = enable;
        viu_cap_reg[cap_id]->ch_wch_y_cfg.u32 = t.u32;
    }
}

/********************************* vicap *********************************/
td_u32 viu_hal_get_cap_id_by_pipe(td_s32 viu_pipe_id)
{
    viu_unused(viu_pipe_id);
    return 0;
}

td_void viu_hal_set_wch_cmp_bayer_mode(td_s32 viu_pipe_id, td_u32 bayer_mode)
{
    viu_unused(viu_pipe_id);
    viu_unused(bayer_mode);

    return;
}

td_void viu_hal_set_isp_mode(td_s32 isp_id, td_bool isp_bypass, td_bool isp_in_yuv)
{
    td_u32 cap_id;
    u_isp_mode t;

    cap_id = viu_hal_get_cap_id_by_pipe(isp_id);

    t.u32 = viu_cap_reg[cap_id]->isp_mode.u32;
    switch (isp_id) {
        case 0: { /* pipe0 */
            t.bits.isp0_mode = isp_bypass;
            t.bits.isp0_in_yuv_mode = isp_in_yuv;

            break;
        }

        default: { /* pipe1 */
            t.bits.isp1_mode = isp_bypass; /* 打开后 fe 使能 */
            t.bits.isp1_in_yuv_mode = isp_in_yuv;
            break;
        }
    }

    viu_cap_reg[cap_id]->isp_mode.u32 = t.u32;
}

td_void viu_hal_set_isp_id_sel(td_s32 viu_pipe_id, td_u32 id_sel)
{
    td_u32 cap_id;
    u_cmux_id_cfg un_cmux_id_cfg;
    cap_id = viu_hal_get_cap_id_by_pipe(viu_pipe_id);

    un_cmux_id_cfg.u32 = viu_cap_reg[cap_id]->cmux_id_cfg.u32;

    switch (viu_pipe_id) {
        case 0: /* pipe0 */
            un_cmux_id_cfg.bits.chn0_id = id_sel & 0xF;
            break;

        default: /* pipe1 */
            un_cmux_id_cfg.bits.chn1_id = id_sel & 0xF;
            break;
    }

    viu_cap_reg[cap_id]->cmux_id_cfg.u32 = un_cmux_id_cfg.u32;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

