/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. */

#ifndef GPU_CRG_H
#define GPU_CRG_H

#include "td_base.h"
#include "gpu_crg_intf.h"
#include "drv_gpu_intf.h"
#include "common_def.h"

typedef enum {
    GPU_CRG_FREQ_KEEP    = 0,  // 保持现状，和配置一致
    GPU_CRG_FREQ_CHANGED = 1,  // 频率已切换
    GPU_CRG_FREQ_MAX
} gpu_crg_freq_state;

td_void gpu_crg_set_vau_clken(td_bool clken);
td_void gpu_crg_vau_reset(td_bool reset_req);

td_void gpu_crg_set_dpu_clken(td_bool clken);
td_void gpu_crg_dpu_reset(td_bool reset_req);
td_u32 gpu_crg_set_dpu_clk(dpu_clk_info *cfg_info);
td_void gpu_crg_get_dpu_clk(drv_dpu_work_scene work_scene, dpu_clk_info *clk_info);

td_u32 gpu_crg_set_vau_clk(const vau_clk_info *cfg_info);
td_void gpu_crg_get_vau_clk(td_u32 cfg_clk_vau, vau_clk_info *clk_info);

td_void gpu_crg_set_jpeg_clken(td_bool clken);
td_void gpu_crg_set_jpeg_apben(td_bool clken);
td_void gpu_crg_jpeg_reset(td_bool reset_req);

td_void video_power_ctrl(gpu_module_id id, video_power_ctrl_mode mode);

#endif