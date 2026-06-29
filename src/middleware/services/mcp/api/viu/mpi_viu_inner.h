/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 * Notes:
 * History:
 */

#ifndef __MPI_VIU_INNER_H__
#define __MPI_VIU_INNER_H__


#include "drv_ioctl_viu.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* bit[31,24] is stitch or mcf grp , bit[23,16] is dev, bit[15,8] is pipe, bit[7,0] is chn */
#define stitch_iocfd(grp) (((grp) & 0xffU) << 24)
#define dev_iocfd(viu_dev_id)    (((viu_dev_id)& 0xffU) << 16)
#define pipe_iocfd(viu_pipe_id)  (((viu_pipe_id) & 0xffU) << 8)
#define chn_iocfd(viu_chn_id)    ((viu_chn_id) & 0xffU)

#define viu_unused(x)  (td_void)(x)

#undef LOG_MODULE_ID
#define LOG_MODULE_ID SOC_ID_VIU

#define viu_alert_trace(fmt, ...) mcp_alert_trace(SOC_ID_VIU, fmt, ##__VA_ARGS__)
#define viu_err_trace(fmt, ...) mcp_err_trace(SOC_ID_VIU, fmt, ##__VA_ARGS__)
#define viu_warn_trace(fmt, ...) mcp_warn_trace(SOC_ID_VIU, fmt, ##__VA_ARGS__)
#define viu_info_trace(fmt, ...) mcp_info_trace(SOC_ID_VIU, fmt, ##__VA_ARGS__)
#define viu_debug_trace(fmt, ...) mcp_debug_trace(SOC_ID_VIU, fmt, ##__VA_ARGS__)

#define viu_make_iocfd(grp, viu_dev_id, viu_pipe_id, viu_chn_id) \
    (stitch_iocfd(grp) | dev_iocfd(viu_dev_id) | pipe_iocfd(viu_pipe_id) | chn_iocfd(viu_chn_id))

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __MPI_VIU_INNER_H__ */
