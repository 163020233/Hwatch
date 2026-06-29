/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mpi layer functions that are exposed
 * Author: AuthorNameMagicTag
 * Create: 2025-12-18
 */

#ifndef __MPI_MCP_PIPELINE_EXT_H__
#define __MPI_MCP_PIPELINE_EXT_H__

#include "mpi_cis_ext.h"
#include "mpi_viu_ext.h"
#include "mpi_ppu_ext.h"
#include "drv_isp_comm_struct.h"
#include "isp_ldci_struct.h"
#include "isp_csc_struct.h"
#include "isp_gamma_struct.h"
#include "isp_lcac_struct.h"
#include "isp_gcac_struct.h"
#include "isp_cac_struct.h"
#include "isp_wb_struct.h"
#include "isp_dp_struct.h"
#include "isp_antifalsecolor_struct.h"
#include "isp_af_struct.h"
#include "isp_ae_struct.h"
#include "isp_flicker_struct.h"
#include "isp_ge_struct.h"
#include "isp_acs_struct.h"
#include "isp_blc_struct.h"
#include "isp_drc_struct.h"
#include "isp_demosaic_struct.h"
#include "mpi_isp_fmw_struct.h"
#include "mpi_isp_3a_struct.h"
#include "mpi_isp_alg_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/********************************* struction **********************************/
typedef enum {
    SENSOR_2M = 0,
    SENSOR_5M,
    SENSOR_8M,
    SENSOR_13M,
    SENSOR_5M_BINNING,
} ext_mcp_sns_mode;

typedef enum {
    MCP_DIGITAL_ZOOM_X1 = 0,
    MCP_DIGITAL_ZOOM_X2,
    MCP_DIGITAL_ZOOM_X3,
    MCP_DIGITAL_ZOOM_X4,
    MCP_DIGITAL_ZOOM_X5,
    MCP_DIGITAL_ZOOM_X6,
    MCP_DIGITAL_ZOOM_X7,
    MCP_DIGITAL_ZOOM_X8,
    MCP_DIGITAL_ZOOM_MAX
} ext_mcp_dig_zoom_factor;

typedef enum {
    MCP_DIGITAL_ZOOM_MODE_PREVIEW = 0,
    MCP_DIGITAL_ZOOM_MODE_SNAP,
    MCP_DIGITAL_ZOOM_MODE_MAX,
} ext_mcp_dig_zoom_mode;

typedef struct {
    td_bool viproc_online;
    td_bool vpss_online;
    td_bool venc_online;

    td_u32 raw_num;
    td_u32 viu_out_num;
    td_u32 ppu_out_num;

    td_bool colorbar_en;
    td_s32  sensor_id;
    ext_mcp_sns_mode sns_mode;
    ext_isp_sns_feature sns_feature;

    td_u32 bas_time;
    td_bool vicap_zsl_en;
    td_bool dis_enable;
    ext_isp_video_format viu_video_fmt;
    ext_isp_compress_mode viu_cmp_mode;
    td_u32              viu_rotation_deg;
    td_bool      ppu_chn_en[PPU_MAX_PHY_CHN_NUM];
    ext_isp_size ppu_chn_size[PPU_MAX_PHY_CHN_NUM];
    ext_isp_rotation ppu_rotation[PPU_MAX_PHY_CHN_NUM];
    td_bool      ppu_corner_en[PPU_MAX_PHY_CHN_NUM];
    td_bool      ppu_output_attach;
    td_s32       venc_id;            /* use for ppu attach venc */
    td_u64       listnode_addr;      /* fsd for listnode_addr */
    td_u64       weight_addr;        /* fsd for weight_addr */
    td_u64       lmk_listnode_addr;  /* fsd for lmk_listnode_addr */
    td_u64       lmk_weight_addr;    /* fsd for lmk_weight_addr */
    td_u32       landmark_enable;
} ext_mcp_pipeline_cfg;

td_s32 ext_mpi_mcp_init(td_void);
td_s32 ext_mpi_mcp_deinit(td_void);

td_s32 ext_mpi_mcp_device_open(td_s32 handle, ext_mcp_pipeline_cfg *pipe_cfg);

td_s32 ext_mpi_mcp_device_close(td_s32 handle);

td_s32 ext_mpi_mcp_stream_acquire(td_s32 handle, int stream_id, ext_isp_video_frame_info *video_frame);
td_s32 ext_mpi_mcp_stream_release(td_s32 handle, int stream_id, ext_isp_video_frame_info *video_frame);

td_s32 ext_mpi_mcp_proc_show(td_s32 mod_id);

#define MCP_SNS_FORMATS_NUM        5
#define MCP_DEVICE_SNS_NUM         2
#define MAX_MCP_INSTANCE_NUM       2
#define MCP_STREAM_FRAME_RATE_NUM  5
#define MCP_DFX_MAX_CMD_LEN        15

typedef enum {
    MCP_SNS_TYPE_FRONT = 0,
    MCP_SNS_TYPE_BACK,
    MCP_SNS_TYPE_MAX,
} mcp_sns_type;

typedef struct {
    td_u32                   format_index;
    td_u32                   width;
    td_u32                   height;
    td_u32                   frame_rate;
    ext_isp_wdr_mode         wdr_mode;
    ext_isp_data_bitwidth    bitwidth;
} mcp_sns_feature;

typedef struct {
    mcp_sns_type     sns_type;
    td_bool          support_low_power;
    mcp_sns_feature  support_formats[MCP_SNS_FORMATS_NUM];
    td_u32           support_num;
} mcp_sns_basic_feature;

typedef struct {
    td_u32             sns_id;
    mcp_sns_type       sns_type;
    mcp_sns_feature    format;
} mcp_device_sns_attr;

typedef enum {
    MCP_PREVIEW_MODE_CAPTURE = 0,
    MCP_PREVIEW_MODE_VIDEO_RECORD,
    MCP_PREVIEW_MODE_VIDEO_CALL,
    MCP_PREVIEW_MODE_CAPTURE_FUN,
    MCP_PREVIEW_MODE_MAX
} mcp_preview_mode;

typedef struct {
    mcp_device_sns_attr sns_attr[MCP_DEVICE_SNS_NUM];
    td_u32              sns_num;
    td_u32              work_sns_id;
    mcp_preview_mode    preview_mode;
} mcp_device_attr;

typedef enum {
    MCP_DIGITAL_ZOOM_RATIO_CUSTOM = 0,
    MCP_DIGITAL_ZOOM_RATIO_1,
    MCP_DIGITAL_ZOOM_RATIO_2,
    MCP_DIGITAL_ZOOM_RATIO_3,
    MCP_DIGITAL_ZOOM_RATIO_4,
    MCP_DIGITAL_ZOOM_RATIO_5,
    MCP_DIGITAL_ZOOM_RATIO_6,
    MCP_DIGITAL_ZOOM_RATIO_7,
    MCP_DIGITAL_ZOOM_RATIO_8,
    MCP_DIGITAL_ZOOM_RATIO_9,
    MCP_DIGITAL_ZOOM_RATIO_10,
    MCP_DIGITAL_ZOOM_RATIO_MAX
} mcp_digital_zoom_mode;

typedef struct {
    mcp_digital_zoom_mode    zoom_mode;
    td_u16                   custom_zoom;
} mcp_device_meta_digital_zoom;

typedef struct {
    ext_isp_rotation rotation_angle;
} mcp_device_meta_rotation;

typedef enum {
    MCP_CAPTURE_MODE_NORMAL = 0,
    MCP_CAPTURE_MODE_ZSL,
    MCP_CAPTURE_MODE_MAX
} mcp_capture_mode;

typedef struct {
    mcp_capture_mode capture_mode;
} mcp_device_meta_capture;

typedef struct {
    td_bool dis_enable;
} mcp_device_meta_dis;

typedef struct {
    mcp_preview_mode preview_mode;
} mcp_device_meta_preview;

typedef enum {
    MCP_DEVICE_META_DIGITAL_ZOOM = 0,
    MCP_DEVICE_META_ROTATION,
    MCP_DEVICE_META_CAPTURE,
    MCP_DEVICE_META_DIS,
    MCP_DEVICE_META_PREVIEW,
    MCP_DEVICE_META_FRAME_RATE,
    MCP_DEVICE_META_SNS_ID,
    MCP_DEVICE_META_MAX
} mcp_device_meta_type;

typedef struct {
    mcp_device_meta_type device_type;
    union {
        mcp_device_meta_digital_zoom digital_zoom;
        mcp_device_meta_rotation rotation;
        mcp_device_meta_capture capture;
        mcp_device_meta_dis dis;
        mcp_device_meta_preview preview;
        td_u32 frame_rate;
        td_u32 work_sns_id;
    } device_meta_data;
} mcp_device_meta;

typedef struct {
    td_s32 pipe_id;
    ext_isp_pub_attr pub_attr;
} mcp_device_pq_pub_attr;

/* Firmware Control API */
typedef struct {
    td_s32 pipe_id;
    ext_isp_fmw_state state;
} mcp_device_pq_fmw_state;

typedef struct {
    td_s32 pipe_id;
    ext_isp_module_ctrl mod_ctrl;
} mcp_device_pq_module_ctrl;

typedef struct {
    td_s32 pipe_id;
    ext_isp_ctrl_param ctrl_param;
} mcp_device_pq_ctrl_param;

typedef struct {
    td_s32 pipe_id;
    ext_isp_dcf_info dcf_info;
} mcp_device_pq_dcf_info;

typedef struct {
    td_s32 pipe_id;
    ext_isp_inner_state_info inner_state_info;
} mcp_device_pq_inner_state_info;

/* Statistics API */
typedef struct {
    td_s32 pipe_id;
    ext_isp_statistics_cfg stat_cfg;
} mcp_device_pq_statistics_cfg;

typedef struct {
    td_s32 pipe_id;
    ext_isp_ae_statistics ae_stat;
} mcp_device_pq_ae_statistics;

typedef struct {
    td_s32 pipe_id;
    ext_isp_wb_statistics wb_stat;
} mcp_device_pq_wb_statistics;

typedef struct {
    td_s32 pipe_id;
    ext_isp_af_statistics af_stat;
} mcp_device_pq_af_statistics;

/* Calibration API */
typedef struct {
    td_s32 pipe_id;
    ext_isp_awb_calibration_gain awb_calibration_gain;
} mcp_device_pq_awb_calibration_gain;

typedef struct {
    td_s32 pipe_id;
    ext_isp_fpn_calibrate_attr calibrate_attr;
} mcp_device_pq_fpn_calibrate;

typedef struct {
    td_s32 pipe_id;
    ext_isp_dp_static_calibr dp_calibrate;
} mcp_device_pq_dp_calibrate;

typedef struct {
    td_s32 pipe_id;
    td_u16 src_raw;
    ext_isp_mlsc_calibration_cfg mlsc_cali_cfg;
    ext_isp_mesh_shading_table mlsc_table;
} mcp_device_pq_mesh_shading_calibration;

typedef struct {
    td_s32 pipe_id;
    ext_isp_calc_flicker_input in_param;
    ext_isp_calc_flicker_output out_param;
} mcp_device_pq_calc_flicker;

/* Algorithm Adjust API */
typedef struct {
    td_s32 pipe_id;
    ext_isp_fpn_attr fpn_attr;
} mcp_device_pq_fpn_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_dp_static_attr dp_static_attr;
} mcp_device_pq_dp_static_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_dp_dynamic_attr dp_dynamic_attr;
} mcp_device_pq_dp_dynamic_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_cr_attr cr_attr;
} mcp_device_pq_crosstalk_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_wdr_fs_attr fswdr_attr;
} mcp_device_pq_fswdr_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_nr_attr nr_attr;
} mcp_device_pq_nr_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_shading_attr shading_attr;
} mcp_device_pq_mesh_shading_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_shading_lut_attr shading_lut_attr;
} mcp_device_pq_mesh_shading_gain_lut_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_acs_attr acs_attr;
} mcp_device_pq_auto_color_shading_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_black_level black_level;
} mcp_device_pq_black_level_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_pregamma_attr pre_gamma_attr;
} mcp_device_pq_pre_gamma_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_drc_attr drc_attr;
} mcp_device_pq_drc_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_demosaic_attr demosaic_attr;
} mcp_device_pq_demosaic_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_antifalsecolor_attr anti_false_color;
} mcp_device_pq_anti_false_color_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_local_cac_attr local_cac_attr;
} mcp_device_pq_local_cac_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_global_cac_attr global_cac_attr;
} mcp_device_pq_global_cac_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_color_tone_attr ct_attr;
} mcp_device_pq_color_tone_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_clut_attr clut_attr;
} mcp_device_pq_clut_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_clut_lut clut_lut;
} mcp_device_pq_clut_coeff;

typedef struct {
    td_s32 pipe_id;
    ext_isp_gamma_attr gamma_attr;
} mcp_device_pq_gamma_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_csc_attr csc_attr;
} mcp_device_pq_csc_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_ldci_attr ldci_attr;
} mcp_device_pq_ldci_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_ca_attr ca_attr;
} mcp_device_pq_ca_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_sharpen_attr shp_attr;
} mcp_device_pq_sharpen_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_pro_sharpen_param isp_shp_param;
} mcp_device_pq_pro_sharpen_param;

typedef struct {
    td_s32 pipe_id;
    ext_isp_pro_nr_param bnr_param;
} mcp_device_pq_pro_bnr_param;

typedef struct {
    td_s32 pipe_id;
    ext_isp_cac_attr cac_attr;
} mcp_device_pq_cac_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_gdci_attr gdci_attr;
} mcp_device_pq_gdci_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_af_attr af_attr;
} mcp_device_pq_af_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_af_info exp_info;
} mcp_device_pq_af_info;

/* AE API */
typedef struct {
    td_s32 pipe_id;
    ext_isp_exposure_attr exp_attr;
} mcp_device_pq_ae_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_wdr_exposure_attr wdr_exp_attr;
} mcp_device_pq_ae_wdr_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_ae_route ae_route_attr;
} mcp_device_pq_ae_route_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_ae_route_ex ae_route_attr_ex;
} mcp_device_pq_ae_route_attr_ex;

typedef struct {
    td_s32 pipe_id;
    ext_isp_exp_info exp_info;
} mcp_device_pq_ae_query_info;

/* AWB API */
typedef struct {
    td_s32 pipe_id;
    ext_isp_wb_attr wb_attr;
} mcp_device_pq_awb_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_color_matrix_attr ccm_attr;
} mcp_device_pq_awb_ccm_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_ccm_dgain_builtin_attr dgain_attr;
} mcp_device_pq_awb_ccm_dgain_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_saturation_attr sat_attr;
} mcp_device_pq_awb_saturation_attr;

typedef struct {
    td_s32 pipe_id;
    ext_isp_wb_info wb_info;
} mcp_device_pq_awb_query_info;

typedef struct {
    td_s32 pipe_id;
    ext_isp_wb_attr wb_attr;
    td_u16 color_temp;
    td_s16 shift;
    td_u16 awb_gain;
} mcp_device_pq_awb_cal_gain_by_temp;

typedef struct {
    td_s32 pipe_id;
    ext_isp_awb_attr_ex awb_attr_ex;
} mcp_device_pq_awb_attr_ex;

typedef struct {
    td_s32 pipe_id;
    ext_isp_color_sector_attr sector_attr;
} mcp_device_pq_color_sector_attr;

typedef enum {
    MCP_DEVICE_PQ_PUB_ATTR = 0,
    MCP_DEVICE_PQ_FMW_STATE,
    MCP_DEVICE_PQ_MODULE_CONTROL,
    MCP_DEVICE_PQ_CTRL_PARAM,
    MCP_DEVICE_PQ_DCF_INFO,
    MCP_DEVICE_PQ_INNER_STATE_INFO,
    MCP_DEVICE_PQ_STATISTICS_CFG,
    MCP_DEVICE_PQ_AE_STATISTICS,
    MCP_DEVICE_PQ_WB_STATISTICS,
    MCP_DEVICE_PQ_AF_STATISTICS,
    MCP_DEVICE_PQ_AWB_CALIBRATION_GAIN,
    MCP_DEVICE_PQ_FPN_CALIBRATE,
    MCP_DEVICE_PQ_DP_CALIBRATE,
    MCP_DEVICE_PQ_MESH_SHADING_CALIBRATION,
    MCP_DEVICE_PQ_CALC_FLICKER,
    MCP_DEVICE_PQ_FPN_ATTR,
    MCP_DEVICE_PQ_DP_STATIC_ATTR,
    MCP_DEVICE_PQ_DP_DYNAMIC_ATTR,
    MCP_DEVICE_PQ_CROSSTALK_ATTR,
    MCP_DEVICE_PQ_FSWDR_ATTR,
    MCP_DEVICE_PQ_NR_ATTR,
    MCP_DEVICE_PQ_MESH_SHADING_ATTR,
    MCP_DEVICE_PQ_MESH_SHADING_GAIN_LUT_ATTR,
    MCP_DEVICE_PQ_AUTO_COLOR_SHADING_ATTR,
    MCP_DEVICE_PQ_BLACK_LEVEL_ATTR,
    MCP_DEVICE_PQ_PRE_GAMMA_ATTR,
    MCP_DEVICE_PQ_DRC_ATTR,
    MCP_DEVICE_PQ_DEMOSAIC_ATTR,
    MCP_DEVICE_PQ_ANTI_FALSE_COLOR_ATTR,
    MCP_DEVICE_PQ_LOCAL_CAC_ATTR,
    MCP_DEVICE_PQ_GLOBAL_CAC_ATTR,
    MCP_DEVICE_PQ_COLOR_TONE_ATTR,
    MCP_DEVICE_PQ_CLUT_ATTR,
    MCP_DEVICE_PQ_CLUT_COEFF,
    MCP_DEVICE_PQ_GAMMA_ATTR,
    MCP_DEVICE_PQ_CSC_ATTR,
    MCP_DEVICE_PQ_LDCI_ATTR,
    MCP_DEVICE_PQ_CA_ATTR,
    MCP_DEVICE_PQ_SHARPEN_ATTR,
    MCP_DEVICE_PQ_PRO_SHARPEN_PARAM,
    MCP_DEVICE_PQ_PRO_BNR_PARAM,
    MCP_DEVICE_PQ_CAC_ATTR,
    MCP_DEVICE_PQ_GDCI_ATTR,
    MCP_DEVICE_PQ_AF_ATTR,
    MCP_DEVICE_PQ_AF_INFO,
    MCP_DEVICE_PQ_AE_ATTR,
    MCP_DEVICE_PQ_AE_WDR_ATTR,
    MCP_DEVICE_PQ_AE_ROUTE_ATTR,
    MCP_DEVICE_PQ_AE_ROUTE_ATTR_EX,
    MCP_DEVICE_PQ_AE_QUERY_INFO,
    MCP_DEVICE_PQ_AWB_ATTR,
    MCP_DEVICE_PQ_AWB_CCM_ATTR,
    MCP_DEVICE_PQ_AWB_CCM_DGAIN_ATTR,
    MCP_DEVICE_PQ_AWB_SATURATION_ATTR,
    MCP_DEVICE_PQ_AWB_QUERY_INFO,
    MCP_DEVICE_PQ_AWB_CAL_GAIN_BY_TEMP,
    MCP_DEVICE_PQ_AWB_ATTR_EX,
    MCP_DEVICE_PQ_COLOR_SECTOR_ATTR,
    MCP_DEVICE_PQ_MAX
} mcp_device_pq_type;

typedef struct {
    mcp_device_pq_type pq_type;
    union {
        mcp_device_pq_pub_attr pq_pub_attr;
        mcp_device_pq_fmw_state fmw_state;
        mcp_device_pq_module_ctrl module_ctrl;
        mcp_device_pq_ctrl_param ctrl_param;
        mcp_device_pq_dcf_info dcf_info;
        mcp_device_pq_inner_state_info inner_state_info;
        mcp_device_pq_statistics_cfg statistics_cfg;
        mcp_device_pq_ae_statistics ae_statistics;
        mcp_device_pq_wb_statistics wb_statistics;
        mcp_device_pq_af_statistics af_statistics;
        mcp_device_pq_awb_calibration_gain awb_calibration_gain;
        mcp_device_pq_fpn_calibrate fpn_calibrate;
        mcp_device_pq_dp_calibrate dp_calibrate;
        mcp_device_pq_mesh_shading_calibration mesh_shading_calibration;
        mcp_device_pq_calc_flicker calc_flicker;
        mcp_device_pq_fpn_attr fpn_attr;
        mcp_device_pq_dp_static_attr dp_static_attr;
        mcp_device_pq_dp_dynamic_attr dp_dynamic_attr;
        mcp_device_pq_crosstalk_attr crosstalk_attr;
        mcp_device_pq_fswdr_attr fswdr_attr;
        mcp_device_pq_nr_attr nr_attr;
        mcp_device_pq_mesh_shading_attr mesh_shading_attr;
        mcp_device_pq_mesh_shading_gain_lut_attr mesh_shading_gain_lut_attr;
        mcp_device_pq_auto_color_shading_attr auto_color_shading_attr;
        mcp_device_pq_black_level_attr black_level_attr;
        mcp_device_pq_pre_gamma_attr pre_gamma_attr;
        mcp_device_pq_drc_attr drc_attr;
        mcp_device_pq_demosaic_attr demosaic_attr;
        mcp_device_pq_anti_false_color_attr anti_false_color_attr;
        mcp_device_pq_local_cac_attr local_cac_attr;
        mcp_device_pq_global_cac_attr global_cac_attr;
        mcp_device_pq_color_tone_attr color_tone_attr;
        mcp_device_pq_clut_attr clut_attr;
        mcp_device_pq_clut_coeff clut_coeff;
        mcp_device_pq_gamma_attr gamma_attr;
        mcp_device_pq_csc_attr csc_attr;
        mcp_device_pq_ldci_attr ldci_attr;
        mcp_device_pq_ca_attr ca_attr;
        mcp_device_pq_sharpen_attr sharpen_attr;
        mcp_device_pq_pro_sharpen_param pro_sharpen_param;
        mcp_device_pq_pro_bnr_param pro_bnr_param;
        mcp_device_pq_cac_attr cac_attr;
        mcp_device_pq_gdci_attr gdci_attr;
        mcp_device_pq_af_attr af_attr;
        mcp_device_pq_af_info af_info;
        mcp_device_pq_ae_attr ae_attr;
        mcp_device_pq_ae_wdr_attr ae_wdr_attr;
        mcp_device_pq_ae_route_attr ae_route_attr;
        mcp_device_pq_ae_route_attr_ex ae_route_attr_ex;
        mcp_device_pq_ae_query_info ae_query_info;
        mcp_device_pq_awb_attr awb_attr;
        mcp_device_pq_awb_ccm_attr awb_ccm_attr;
        mcp_device_pq_awb_ccm_dgain_attr awb_ccm_dgain_attr;
        mcp_device_pq_awb_saturation_attr awb_saturation_attr;
        mcp_device_pq_awb_query_info awb_query_info;
        mcp_device_pq_awb_cal_gain_by_temp awb_cal_gain_by_temp;
        mcp_device_pq_awb_attr_ex awb_attr_ex;
        mcp_device_pq_color_sector_attr color_sector_attr;
    } device_pq_data;
} mcp_device_pq;

typedef enum {
    MCP_STREAM_TYPE_AUTO = 0,
    MCP_STREAM_TYPE_MAIN,
    MCP_STREAM_TYPE_SUB,
    MCP_STREAM_TYPE_MAX
} mcp_stream_type;

typedef struct {
    mcp_stream_type stream_type;
    td_u32 width;
    td_u32 height;
    ext_isp_pixel_format pixelformat;
    td_u32 frame_rate;
    td_u32 buffer_num;
    td_bool enable_mirror;
    td_bool enable_flip;
    td_bool memory_alloc_by_user;
    ext_isp_crop_info crop_info;
} mcp_stream_attr;

typedef struct {
    td_handle venc_handle;
} mcp_stream_meta_venc;

typedef struct {
    td_bool    face_enable;
    td_bool    landmark_enable;
    td_bool    scene_enable;
} mcp_stream_meta_fsd;

typedef struct {
    td_bool face_enable;
    td_bool photo_enable;
} mcp_stream_meta_corner;

typedef struct {
    ext_isp_rotation rotation_angle;
} mcp_stream_meta_rotation;

typedef struct {
    td_u32 width;
    td_u32 height;
    ext_isp_pixel_format pixelformat;
    td_u32 frame_rate[MCP_STREAM_FRAME_RATE_NUM];
    td_u32 frame_rate_num;
} mcp_stream_meta_output_cap;

typedef struct {
    ext_isp_crop_info crop_info;
} mcp_stream_meta_crop_info;

typedef enum {
    MCP_STREAM_META_ATTACH_VENC = 0,
    MCP_STREAM_META_FSD,
    MCP_STREAM_META_CORNER,
    MCP_STREAM_META_ROTATION,
    MCP_STREAM_META_OUTPUT_CAP,
    MCP_STREAM_META_CROP,
    MCP_STREAM_META_MAX,
} mcp_stream_meta_type;

typedef struct {
    mcp_stream_meta_type stream_type;
    union {
        mcp_stream_meta_venc venc;
        mcp_stream_meta_fsd fsd;
        mcp_stream_meta_corner corner;
        mcp_stream_meta_rotation rotation;
        mcp_stream_meta_output_cap output_cap;
        mcp_stream_meta_crop_info crop;
    } stream_meta_data;
} mcp_stream_meta;

typedef enum {
    MCP_CAPTURE_PROFILE_MAIN,
    MCP_CAPTURE_PROFILE_HIGH,
    MCP_CAPTURE_PROFILE_MAX,
} mcp_capture_profile;

typedef struct {
    td_u32 capture_num;
    td_s32 capture_interval_ms;
    mcp_capture_profile profile;
} mcp_capture_config;

typedef enum {
    MCP_VIDEO_MODE_VIDEO_RECORD,
    MCP_VIDEO_MODE_VIDEO_CALL,
    MCP_VIDEO_MODE_MAX
} mcp_video_mode;

td_s32 mcp_init(td_void);
td_s32 mcp_deinit(td_void);

td_s32 mcp_sns_get_number(td_u32 *sns_number);
td_s32 mcp_sns_get_basic_feature(td_u32 sns_id, mcp_sns_basic_feature *basic_feature);

td_s32 mcp_device_create(td_handle *handle, const mcp_device_attr *device_attr);
td_s32 mcp_device_destroy(td_handle handle);

td_s32 mcp_device_enable(td_handle handle);
td_s32 mcp_device_disable(td_handle handle);

td_s32 mcp_device_set_meta(td_handle handle, const mcp_device_meta *device_meta);
td_s32 mcp_device_get_meta(td_handle handle, mcp_device_meta *device_meta);

td_s32 mcp_device_set_pq(td_handle handle, const mcp_device_pq *device_pq);
td_s32 mcp_device_get_pq(td_handle handle, mcp_device_pq *device_pq);

td_s32 mcp_device_capture(td_handle handle, const mcp_capture_config *cap_cfg, ext_video_frame *out_video_frame);
td_s32 mcp_device_capture_release(td_handle handle, ext_video_frame *out_video_frame);

td_s32 mcp_device_start_video(td_handle handle, td_s32 stream_id, mcp_video_mode video_mode);
td_s32 mcp_device_stop_video(td_handle handle, td_s32 stream_id);

td_s32 mcp_stream_create(td_handle handle, const mcp_stream_attr *init_attr, td_s32 *stream_id);
td_s32 mcp_stream_destroy(td_handle handle, td_s32 stream_id);

td_s32 mcp_stream_set_meta(td_handle handle, td_s32 stream_id, const mcp_stream_meta *stream_meta);
td_s32 mcp_stream_get_meta(td_handle handle, td_s32 stream_id, mcp_stream_meta *stream_meta);

td_s32 mcp_stream_on(td_handle handle, td_s32 stream_id);
td_s32 mcp_stream_off(td_handle handle, td_s32 stream_id);

td_s32 mcp_stream_acquire(td_handle handle, td_s32 stream_id, ext_video_frame *video_frame);
td_s32 mcp_stream_release(td_handle handle, td_s32 stream_id, ext_video_frame *video_frame);

td_s32 mcp_stream_get_qbuf_size(td_handle handle, td_s32 stream_id, td_u32 *size);
td_s32 mcp_stream_qbuf(td_handle handle, td_s32 stream_id, ext_video_frame *video_frame);
td_s32 mcp_stream_dqbuf(td_handle handle, td_s32 stream_id, ext_video_frame *video_frame);

td_s32 mcp_dfx_show_proc(td_u32 mod_id);
td_s32 mcp_dfx_write_cmd(td_u32 argc, const td_char *argv[]);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __MPI_MCP_PIPELINE_EXT_H__ */
