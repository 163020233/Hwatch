/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu public header
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 * Notes:
 * History:
 */

#ifndef __VIU_PUBLIC_H__
#define __VIU_PUBLIC_H__

#include "drv_mcp_dependency.h"

#include "drv_viu_struct.h"
#include "hal_chip_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VIU_PRIV                      static

#define VIU_INVALID_FRMRATE            (-1)

#define VIU_CHN0                       0
#define VIU_CHN1                       1
#define VIU_CHN2                       2
#define VIU_CHN3                       3

#define VIU_MAX_VC_NUM                 4U

#define viu_unused(x)               ((td_void)(x))
#define viu_align_up(x, a)          ((((x) + ((a) - 1)) / (a)) * (a))

#define VICAP_BUF_NUM           4

#define UMAP_GET_CHN(f) (*((td_u32*)(f)))
#define UMAP_SET_CHN(f, chn) (*((td_u32*)(f)) = (chn))

#define viu_get_dev_by_iocfd(f)              (((f) >> 16) & 0xffU)
#define viu_get_pipe_by_iocfd(f)             (((f) >> 8) & 0xffU)
#define viu_get_chn_by_iocfd(f)              ((f) & 0xffU)

#undef LOG_MODULE_ID
#define LOG_MODULE_ID MCP_MOD_ID_VIU

#if defined(CONFIG_CHIP_3322)
#define viu_alert_trace(fmt...) osal_printk(fmt)
#define viu_err_trace(fmt...) osal_printk(fmt)
#define viu_warn_trace(fmt...)
#define viu_notice_trace(fmt...)
#define viu_info_trace(fmt...)
#define viu_debug_trace(fmt...)
#else
#define viu_alert_trace(fmt, ...) mcp_log_alert(fmt, ##__VA_ARGS__)
#define viu_err_trace(fmt, ...) mcp_log_err(fmt, ##__VA_ARGS__)
#define viu_warn_trace(fmt, ...) mcp_log_warn(fmt, ##__VA_ARGS__)
#define viu_notice_trace(fmt, ...) mcp_log_notice(fmt, ##__VA_ARGS__)
#define viu_info_trace(fmt, ...) mcp_log_info(fmt, ##__VA_ARGS__)
#define viu_debug_trace(fmt, ...) mcp_log_dbg(fmt, ##__VA_ARGS__)
#endif

#define is_phy_chn(chn_id) \
    (((chn_id) < VIU_MAX_PHY_CHN_NUM) && ((chn_id) >= 0))

#define is_ext_chn(chn_id) \
    (((chn_id) < VIU_MAX_CHN_NUM) && ((chn_id) >= VIU_MAX_PHY_CHN_NUM))

__inline static td_u32 viu_get_low_addr(td_u64 phyaddr)
{
    return (td_u32)phyaddr;
}

__inline static td_u32 viu_get_high_addr(td_u64 phyaddr)
{
    return (td_u32)(phyaddr >> 32); /* shift32 */
}

__inline static td_bool is_format_raw(ext_pixel_format format)
{
    if ((format == EXT_PIXEL_FORMAT_RGB_BAYER_8BPP) || (format == EXT_PIXEL_FORMAT_RGB_BAYER_10BPP) ||
        (format == EXT_PIXEL_FORMAT_RGB_BAYER_12BPP) || (format == EXT_PIXEL_FORMAT_RGB_BAYER_14BPP) ||
        (format == EXT_PIXEL_FORMAT_RGB_BAYER_16BPP)) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

__inline static td_bool is_format_yuv_support(ext_pixel_format format)
{
    if ((format == EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_422) ||
        (format == EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_420) ||
        (format == EXT_PIXEL_FORMAT_YUV_400)) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

__inline static td_bool is_format_rgb_support(ext_pixel_format format)
{
    if ((format == EXT_PIXEL_FORMAT_RGB_888) ||
        (format == EXT_PIXEL_FORMAT_RGB_10BPP) ||
        (format == EXT_PIXEL_FORMAT_RGB_12BPP)) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

td_bool read_user_linear_space_valid(td_u8 *addr_start, td_u32 len);   /* from mm_ext.h */
td_bool write_user_linear_space_valid(td_u8 *addr_start, td_u32 len);

td_s32 viu_check_null_ptr(const td_void *ptr);
td_s32 viu_check_mod_id(mcp_soc_mod_id mod_id);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* End of __VIU_PUBLIC_H__ */
