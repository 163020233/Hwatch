/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: rm blur
 */

#ifndef GPU_API_RM_RM_BLUR_H
#define GPU_API_RM_RM_BLUR_H

#include "vau_rm_api.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

td_void rm_product_blur_coef(td_float sigma, drv_vau_blur_opt *drv_opt);
td_void rm_product_blur_matrix(ext_rect src_rect, ext_rect dst_rect, td_bool is_vertial, td_float scale,
    ext_vau_matrix *matrix);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* GPU_API_RM_RM_BLUR_H */