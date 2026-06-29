/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: vau rm api
 */

#ifndef GPU_API_RM_VAU_RM_API_H
#define GPU_API_RM_VAU_RM_API_H

#include "soc_vau_type.h"
#include "drv_vau_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

td_void uapi_vau_rm_product_blur_coef(td_float sigma, drv_vau_blur_opt *drv_opt);
td_void uapi_vau_rm_product_mapping_matrix(const ext_vau_surface *api_src, drv_vau_surface *drv_src);
td_void uapi_vau_rm_product_mapping_info(const ext_vau_surface *api_src,
    const ext_vau_surface *api_dst, drv_vau_surface *drv_src);
td_void uapi_vau_rm_product_blur_matrix(ext_rect src_rect, ext_rect dst_rect, td_bool is_vertial, td_float scale,
    ext_vau_matrix *matrix);
td_s32 uapi_vau_rm_update_mesh_model_config(drv_vau_mesh_type type, drv_vau_model_surface *model_surface);
td_void uapi_vau_rm_update_mesh_model_deconfig(drv_vau_model_surface *model_surface);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* GPU_API_RM_VAU_RM_API_H */
