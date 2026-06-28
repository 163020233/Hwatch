/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. */

#ifndef GPU_CRG_INTF_H
#define GPU_CRG_INTF_H

#include "drv_gpu_module.h"
#include "soc_dpu_type.h"
#include "drv_dpu_type.h"
#include "drv_gpu_intf.h"

#define FREQ_K_PER_M 1000
#define FREQ_PER_M   1000000
#define GPU_ROUND_DIV_UP(a, b) (((a) + (b) - 1) / (b)) // 除法向上取整

typedef enum {
    GPU_CLK_CFG_INIT    = 0, /* 初始化 */
    GPU_CLK_CFG_SWITCH  = 1, /* 切频 */
    GPU_CLK_CFG_SUSPEND = 2, /* 挂起 */
    GPU_CLK_CFG_RESUME  = 3, /* 恢复 */

    GPU_CLK_CFG_MAX
} gpu_clk_cfg_scene;

typedef struct {
    gpu_clk_cfg_scene cfg_scene;
    drv_dpu_work_scene work_scene;
    td_u32 clk_gpu; // dpu
    td_u32 pixel_clk;
    td_u32 work_clk;
} dpu_clk_info;

typedef struct {
    gpu_clk_cfg_scene cfg_scene;
    drv_vau_work_scene work_scene;
    td_u32 vau_clk;
    td_u32 axi_clk;
} vau_clk_info;

typedef struct {
    gpu_clk_cfg_scene cfg_scene;
    drv_dpu_work_scene work_scene;
    td_bool is_cmp_out;
    td_bool is_rotate;
} dpu_clk_scene_info;

typedef struct {
    gpu_clk_cfg_scene cfg_scene;
    drv_vau_work_scene work_scene;
} vau_clk_scene_info;

td_void gpu_crg_init(gpu_module_id id, td_void *private_data);
td_void gpu_crg_deinit(gpu_module_id id, td_void *private_data);
td_u32 gpu_crg_set_clk(gpu_module_id id, td_void *private_data);
td_void gpu_crg_get_clk(gpu_module_id id, td_void *private_data, td_void *clk_data);

td_void gpu_crg_apb_enable(gpu_module_id id, td_bool apb_en);

td_void gpu_crg_set_dpu_normal_pixel_clk(td_u32 pixel_clk);
#endif
