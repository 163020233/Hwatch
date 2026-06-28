/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2021. All rights reserved.
 * Description: Function of mpi_isp_ext.h
 * Author: AuthorNameMagicTag
 * Create: 2012-06-28
 */

#ifndef __MPI_ISP_EXT_H__
#define __MPI_ISP_EXT_H__

#include "mpi_isp_fmw_struct.h"
#include "mpi_isp_alg_struct.h"
#include "mpi_isp_sns_struct.h"
#include "mpi_isp_3a_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ---------------------------------------- Firmware Control API ---------------------------------------- */
td_s32 ext_mpi_isp_init(td_s32 pipe_id);
td_s32 ext_mpi_isp_exit(td_s32 pipe_id);
td_s32 ext_mpi_isp_refresh_alg_param(td_s32 pipe_id);
td_s32 ext_mpi_isp_set_pub_attr(td_s32 pipe_id, const ext_isp_pub_attr *pub_attr);
td_s32 ext_mpi_isp_get_pub_attr(td_s32 pipe_id, ext_isp_pub_attr *pub_attr);
td_s32 ext_mpi_isp_prepare(td_s32 pipe_id);
td_s32 ext_mpi_isp_run(td_s32 pipe_id);
td_s32 ext_mpi_isp_run_once(td_s32 pipe_id);
// if have registered multy libs, set bind attr to appoint the active lib
td_s32 ext_mpi_isp_sensor_reg_callback(td_s32 pipe_id, ext_isp_sns_attr_info *attr, ext_isp_sensor_register *reg);
td_s32 ext_mpi_isp_sensor_unreg_callback(td_s32 pipe_id, td_s32 sensor_id);

td_s32 ext_mpi_isp_set_fmw_state(td_s32 pipe_id, const ext_isp_fmw_state state);
td_s32 ext_mpi_isp_get_fmw_state(td_s32 pipe_id, ext_isp_fmw_state *state);

td_s32 ext_mpi_isp_set_module_control(td_s32 pipe_id, const ext_isp_module_ctrl *mod_ctrl);
td_s32 ext_mpi_isp_get_module_control(td_s32 pipe_id, ext_isp_module_ctrl *mod_ctrl);

td_s32 ext_mpi_isp_set_ctrl_param(td_s32 pipe_id, const ext_isp_ctrl_param *isp_ctrl_param);
td_s32 ext_mpi_isp_get_ctrl_param(td_s32 pipe_id, ext_isp_ctrl_param *isp_ctrl_param);

td_s32 ext_mpi_isp_set_dcf_info(td_s32 pipe_id, const ext_isp_dcf_info *isp_dcf);
td_s32 ext_mpi_isp_get_dcf_info(td_s32 pipe_id, ext_isp_dcf_info *isp_dcf);

td_s32 ext_mpi_isp_query_inner_state_info(td_s32 pipe_id, ext_isp_inner_state_info *inner_state_info);

td_s32 ext_mpi_isp_pq_init(td_void);
td_s32 ext_mpi_isp_pq_deinit(td_void);

/* ---------------------------------------- Algorithm Control API ---------------------------------------- */
// statistics
td_s32 ext_mpi_isp_set_statistics_config(td_s32 pipe_id, const ext_isp_statistics_cfg *stat_cfg);
td_s32 ext_mpi_isp_get_statistics_config(td_s32 pipe_id, ext_isp_statistics_cfg *stat_cfg);
td_s32 ext_mpi_isp_get_ae_statistics(td_s32 pipe_id, ext_isp_ae_statistics *ae_stat);
td_s32 ext_mpi_isp_get_wb_statistics(td_s32 pipe_id, ext_isp_wb_statistics *wb_stat);
td_s32 ext_mpi_isp_get_focus_statistics(td_s32 pipe_id, ext_isp_af_statistics *af_stat);

// calibration
td_s32 ext_mpi_isp_get_lightbox_gain(td_s32 pipe_id, ext_isp_awb_calibration_gain *awb_calibration_gain);
td_s32 ext_mpi_isp_fpn_calibrate(td_s32 pipe_id, ext_isp_fpn_calibrate_attr *calibrate_attr);
td_s32 ext_mpi_isp_set_dp_calibrate(td_s32 pipe_id, const ext_isp_dp_static_calibr *dp_calibrate);
td_s32 ext_mpi_isp_get_dp_calibrate(td_s32 pipe_id, ext_isp_dp_static_calibr *dp_calibrate);
td_s32 ext_mpi_isp_mesh_shading_calibration(td_s32 pipe_id, td_u16 *src_raw,
    ext_isp_mlsc_calibration_cfg *mlsc_cali_cfg, ext_isp_mesh_shading_table *mlsc_table);

// algorithm adjust
td_s32 ext_mpi_isp_cal_flicker_type(td_s32 pipe_id, ext_isp_calc_flicker_input *in_param,
    ext_isp_calc_flicker_output *out_param, ext_isp_video_frame_info frame[], td_u32 frame_num);

td_s32 ext_mpi_isp_set_fpn_attr(td_s32 pipe_id, const ext_isp_fpn_attr *fpn_attr);
td_s32 ext_mpi_isp_get_fpn_attr(td_s32 pipe_id, ext_isp_fpn_attr *fpn_attr);

td_s32 ext_mpi_isp_set_dp_static_attr(td_s32 pipe_id, const ext_isp_dp_static_attr *dp_static_attr);
td_s32 ext_mpi_isp_get_dp_static_attr(td_s32 pipe_id,  ext_isp_dp_static_attr *dp_static_attr);
td_s32 ext_mpi_isp_set_dp_dynamic_attr(td_s32 pipe_id, const ext_isp_dp_dynamic_attr *dp_dynamic_attr);
td_s32 ext_mpi_isp_get_dp_dynamic_attr(td_s32 pipe_id, ext_isp_dp_dynamic_attr *dp_dynamic_attr);

td_s32 ext_mpi_isp_set_crosstalk_attr(td_s32 pipe_id, const ext_isp_cr_attr *cr_attr);
td_s32 ext_mpi_isp_get_crosstalk_attr(td_s32 pipe_id, ext_isp_cr_attr *cr_attr);

td_s32 ext_mpi_isp_set_fswdr_attr(td_s32 pipe_id, const ext_isp_wdr_fs_attr *fswdr_attr);
td_s32 ext_mpi_isp_get_fswdr_attr(td_s32 pipe_id, ext_isp_wdr_fs_attr *fswdr_attr);

td_s32 ext_mpi_isp_set_nr_attr(td_s32 pipe_id, const ext_isp_nr_attr *nr_attr);
td_s32 ext_mpi_isp_get_nr_attr(td_s32 pipe_id, ext_isp_nr_attr *nr_attr);

td_s32 ext_mpi_isp_set_mesh_shading_attr(td_s32 pipe_id, const ext_isp_shading_attr *shading_attr);
td_s32 ext_mpi_isp_get_mesh_shading_attr(td_s32 pipe_id, ext_isp_shading_attr *shading_attr);
td_s32 ext_mpi_isp_set_mesh_shading_gain_lut_attr(td_s32 pipe_id, const ext_isp_shading_lut_attr *shading_lut_attr);
td_s32 ext_mpi_isp_get_mesh_shading_gain_lut_attr(td_s32 pipe_id, ext_isp_shading_lut_attr *shading_lut_attr);

td_s32 ext_mpi_isp_set_auto_color_shading_attr(td_s32 pipe_id, const ext_isp_acs_attr *acs_attr);
td_s32 ext_mpi_isp_get_auto_color_shading_attr(td_s32 pipe_id, ext_isp_acs_attr *acs_attr);

td_s32 ext_mpi_isp_set_black_level_attr(td_s32 pipe_id, const ext_isp_black_level *black_level);
td_s32 ext_mpi_isp_get_black_level_attr(td_s32 pipe_id, ext_isp_black_level *black_level);

td_s32 ext_mpi_isp_set_pre_gamma_attr(td_s32 pipe_id, const ext_isp_pregamma_attr *pre_gamma_attr);
td_s32 ext_mpi_isp_get_pre_gamma_attr(td_s32 pipe_id, ext_isp_pregamma_attr *pre_gamma_attr);

td_s32 ext_mpi_isp_set_drc_attr(td_s32 pipe_id, const ext_isp_drc_attr *drc);
td_s32 ext_mpi_isp_get_drc_attr(td_s32 pipe_id, ext_isp_drc_attr *drc);

td_s32 ext_mpi_isp_set_demosaic_attr(td_s32 pipe_id, const ext_isp_demosaic_attr *demosaic_attr);
td_s32 ext_mpi_isp_get_demosaic_attr(td_s32 pipe_id, ext_isp_demosaic_attr *demosaic_attr);

td_s32 ext_mpi_isp_set_anti_false_color_attr(td_s32 pipe_id, const ext_isp_antifalsecolor_attr *anti_false_color);
td_s32 ext_mpi_isp_get_anti_false_color_attr(td_s32 pipe_id, ext_isp_antifalsecolor_attr *anti_false_color);

/* not support 1A22 */
td_s32 ext_mpi_isp_set_local_cac_attr(td_s32 pipe_id, const ext_isp_local_cac_attr *local_cac_attr);
td_s32 ext_mpi_isp_get_local_cac_attr(td_s32 pipe_id, ext_isp_local_cac_attr *local_cac_attr);
td_s32 ext_mpi_isp_set_global_cac_attr(td_s32 pipe_id, const ext_isp_global_cac_attr *global_cac_attr);
td_s32 ext_mpi_isp_get_global_cac_attr(td_s32 pipe_id, ext_isp_global_cac_attr *global_cac_attr);
/* not support end */

td_s32 ext_mpi_isp_set_color_tone_attr(td_s32 pipe_id, const ext_isp_color_tone_attr *ct_attr);
td_s32 ext_mpi_isp_get_color_tone_attr(td_s32 pipe_id, ext_isp_color_tone_attr *ct_attr);

td_s32 ext_mpi_isp_set_clut_attr(td_s32 pipe_id, const ext_isp_clut_attr *clut_attr);
td_s32 ext_mpi_isp_get_clut_attr(td_s32 pipe_id, ext_isp_clut_attr *clut_attr);
td_s32 ext_mpi_isp_set_clut_coeff(td_s32 pipe_id, const ext_isp_clut_lut *clut_lut);
td_s32 ext_mpi_isp_get_clut_coeff(td_s32 pipe_id, ext_isp_clut_lut *clut_lut);

td_s32 ext_mpi_isp_set_gamma_attr(td_s32 pipe_id, const ext_isp_gamma_attr *gamma_attr);
td_s32 ext_mpi_isp_get_gamma_attr(td_s32 pipe_id, ext_isp_gamma_attr *gamma_attr);

td_s32 ext_mpi_isp_set_csc_attr(td_s32 pipe_id, const ext_isp_csc_attr *csc_attr);
td_s32 ext_mpi_isp_get_csc_attr(td_s32 pipe_id, ext_isp_csc_attr *cscf_attr);

td_s32 ext_mpi_isp_set_ldci_attr(td_s32 pipe_id, const ext_isp_ldci_attr *ldci_attr);
td_s32 ext_mpi_isp_get_ldci_attr(td_s32 pipe_id, ext_isp_ldci_attr *ldci_attr);

td_s32 ext_mpi_isp_set_ca_attr(td_s32 pipe_id, const ext_isp_ca_attr *ca_attr);
td_s32 ext_mpi_isp_get_ca_attr(td_s32 pipe_id, ext_isp_ca_attr *ca_attr);

td_s32 ext_mpi_isp_set_sharpen_attr(td_s32 pipe_id, const ext_isp_sharpen_attr *shp_attr);
td_s32 ext_mpi_isp_get_sharpen_attr(td_s32 pipe_id, ext_isp_sharpen_attr *shp_attr);

td_s32 ext_mpi_isp_set_dehaze_attr(td_s32 pipe_id, const ext_isp_dehaze_attr *dehaze_attr);
td_s32 ext_mpi_isp_get_dehaze_attr(td_s32 pipe_id, ext_isp_dehaze_attr *dehaze_attr);

td_s32 ext_mpi_isp_set_pro_sharpen_param(td_s32 pipe_id, const ext_isp_pro_sharpen_param *isp_shp_param);
td_s32 ext_mpi_isp_get_pro_sharpen_param(td_s32 pipe_id, ext_isp_pro_sharpen_param *isp_shp_param);
td_s32 ext_mpi_isp_set_pro_bnr_param(td_s32 pipe_id, const ext_isp_pro_nr_param *bnr_param);
td_s32 ext_mpi_isp_get_pro_bnr_param(td_s32 pipe_id, ext_isp_pro_nr_param *bnr_param);

/* just 1A22 support */
td_s32 ext_mpi_isp_set_cac_attr(td_s32 pipe_id, const ext_isp_cac_attr *cac_attr);
td_s32 ext_mpi_isp_get_cac_attr(td_s32 pipe_id, ext_isp_cac_attr *cac_attr);

td_s32 ext_mpi_isp_set_gdci_attr(td_s32 pipe_id, const ext_isp_gdci_attr *gdci_attr);
td_s32 ext_mpi_isp_get_gdci_attr(td_s32 pipe_id, ext_isp_gdci_attr *gdci_attr);

td_s32 ext_mpi_isp_set_debug(td_s32 pipe_id, const ext_isp_debug_info *isp_debug);
td_s32 ext_mpi_isp_get_debug(td_s32 pipe_id, ext_isp_debug_info *isp_debug);

td_s32 ext_mpi_isp_set_smart_info(td_s32 pipe_id, const ext_isp_smart_info *smart_info);
td_s32 ext_mpi_isp_get_smart_info(td_s32 pipe_id, ext_isp_smart_info *smart_info);

td_s32 ext_mpi_isp_vreg_map(td_s32 pipe_id);
td_s32 ext_mpi_isp_vreg_unmap(td_s32 pipe_id);

td_s32 ext_mpi_isp_set_af_attr(td_s32 pipe_id, const ext_isp_af_attr *af_attr);
td_s32 ext_mpi_isp_get_af_attr(td_s32 pipe_id, ext_isp_af_attr *af_attr);
td_s32 ext_mpi_isp_query_af_info(td_s32 pipe_id, ext_isp_af_info *exp_info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
