/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: head file of crg
 * Author: AuthorNameMagicTag
 * Create: 2025-04-23
 */

#ifndef HAL_VIDEV_INTF_H
#define HAL_VIDEV_INTF_H

#include "drv_mcp_dependency.h"
#include "viu_base_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* #ifdef __cplusplus */

typedef struct {
    ext_isp_size valid_size;
    ext_isp_size total_size;

    ext_isp_size bas_valid_size;
    ext_isp_size bas_total_size;
} hal_videv_status;

typedef struct {
    ext_viu_dev_bind_pipe dev_bind_pipe; /* 绑定的PIPE */
    ext_viu_dev_attr viu_dev_attr; /* dev attr */

    td_u32 sensor_bit_width;
    ext_viu_dev_timing_attr viu_timing_attr; /* RAW data read */
} viu_dev_drv_ctx;

td_s32 hal_videv_init(viu_dev_id dev_id, const ext_viu_dev_attr *viu_dev_attr);
td_void hal_videv_deinit(viu_dev_id dev_id);

td_void hal_videv_enable(td_s32 viu_dev_id, td_bool dev_en);

td_void hal_videv_process(viu_dev_id dev_id, const ext_viu_dev_attr *viu_dev_attr);
td_void hal_videv_process_loop(viu_dev_id dev_id, const ext_viu_dev_attr *viu_dev_attr);

td_void hal_videv_set_timing(viu_dev_id dev_id, ext_viu_dev_timing_attr *timing_attr, ext_isp_size *size);

td_void hal_videv_get_status(td_s32 viu_dev_id, hal_videv_status *status);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif