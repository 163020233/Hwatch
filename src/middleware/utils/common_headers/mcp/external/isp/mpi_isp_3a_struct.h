/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Function of mpi_isp_3a_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2023-01-13
 */

#ifndef __MPI_ISP_3A_STRUCT_H__
#define __MPI_ISP_3A_STRUCT_H__

#include "drv_isp_comm_struct.h"
#include "isp_wdr_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define AE_INT_TIME_NUM          4

typedef struct {
    td_s32 sensor_id;
    td_u8  wdr_mode;
    td_u8  hdr_mode;
    td_u16 black_level;
    td_float fps;
    ext_isp_bayer_format bayer;
    td_s32 rsv;
} ext_isp_ae_param;

/* Smart Info */
#define ISP_PEOPLE_CLASS_MAX   2
typedef struct {
    td_bool  enable;                /* R; Range:[0, 1]; enable smart roi info */
    td_bool  available;             /* R; Range:[0, 1]; whether roi area is available */
    td_u8    luma;                  /* R; Range:[0x00, 0xFF]; luma of roi area */
} ext_isp_people_roi;

typedef enum {
    EXT_ISP_FACE_INDEX = 0,
    EXT_ISP_PEOPLE_INDEX = 1,
    EXT_ISP_PEOPLE_BUTT
} ext_isp_people_type;

#define ISP_TUNNEL_CLASS_MAX   2
typedef struct {
    td_bool  enable;                /* R; Range:[0, 1]; enable tunnel roi info */
    td_bool  available;             /* R; Range:[0, 1]; whether roi area is available */
    td_u32   tunnel_area_ratio;     /* R; Range:[0, 10000]; the proportion of tunnels in the whole picture */
    td_u32   tunnel_exp_perf;       /* R; Range:[0, 10000]; the performance of tunnels exposure result */
} ext_isp_tunnel_roi;

typedef enum {
    EXT_ISP_TUNNEL_IN_INDEX = 0,
    EXT_ISP_TUNNEL_OUT_INDEX = 1,
    EXT_ISP_TUNNEL_BUTT
} ext_isp_tunnel_type;

#define EXT_ISP_FACE_NUM 5
typedef struct {
    td_bool  enable;                     /* R; Range:[0, 1]; enable fast face roi info */
    td_bool  available;                  /* R; Range:[0, 1]; whether roi area is available */
    td_u64   frame_pts;                  /* pts of face info */
    ext_isp_rect face_rect[EXT_ISP_FACE_NUM];  /* face roi rect */
} ext_isp_face_roi;

typedef struct {
    ext_isp_people_roi  people_roi[ISP_PEOPLE_CLASS_MAX]; /* people ae info */
    ext_isp_tunnel_roi  tunnel_roi[ISP_TUNNEL_CLASS_MAX]; /* tunnels ae info */
    ext_isp_face_roi face_roi;                               /* fast face ae info */
} ext_isp_smart_info;

typedef struct {
    td_u32  frame_cnt;    /* the counting of frame */
    td_u64  frame_pts;
    td_u16  frame_width;
    td_u16  frame_height;
    ext_isp_smart_info smart_info;

    ext_isp_fe_ae_stat_1 *fe_ae_stat1;
    ext_isp_fe_ae_stat_2 *fe_ae_stat2;           /* Not support for 1A22 */
    ext_isp_fe_ae_stat_3 *fe_ae_stat3;           /* Not support for 1A22 */

    ext_isp_be_ae_stat_1 *be_ae_stat1;
    ext_isp_be_ae_stat_2 *be_ae_stat2;
    ext_isp_be_ae_stat_3 *be_ae_stat3;           /* Not support for 1A22 */
    ext_isp_be_ae_stat_4 *be_ae_stat4;           /* Not support for 850 */
} ext_isp_ae_info;

typedef struct {
    td_bool change;

    td_bool hist_adjust;
    td_u8 ae_be_sel;
    td_u8 four_plane_mode;
    td_u8 hist_offset_x;
    td_u8 hist_offset_y;
    td_u8 hist_skip_x;
    td_u8 hist_skip_y;

    td_bool mode_update;
    td_u8 hist_mode;
    td_u8 aver_mode;
    td_u8 max_gain_mode;

    td_bool wight_table_update;
    td_u8 weight_table[ISP_MAX_PIPE_NUM][ISP_AE_ZONE_ROW][ISP_AE_ZONE_COLUMN];
    td_u8 be_weight_table[ISP_MAX_PIPE_NUM][ISP_BE_AE_ZONE_ROW][ISP_BE_AE_ZONE_COLUMN];
} ext_isp_ae_stat_attr;

typedef struct {
    td_u32  int_time[AE_INT_TIME_NUM];
    td_u32  isp_dgain;
    td_u32  again;
    td_u32  dgain;
    td_u32  iso;
    td_u32  isp_dgain_sf;
    td_u32  again_sf;
    td_u32  dgain_sf;
    td_u32  iso_sf;
    td_u8   ae_run_interval;

    td_bool piris_valid;
    td_s32  piris_pos;
    td_u32  piris_gain;
    td_u32  sns_lhcg_exp_ratio;

    ext_isp_fswdr_mode fswdr_mode;
    td_u32  wdr_gain[ISP_WDR_FRAME_MAX];
    td_u32  hmax_times; /* unit is ns */
    td_u32  vmax; /* unit is line */

    td_u16  hist_array_error;
    td_u16  ae_tolerance;
    td_bool max_gain_time_flag;

    ext_isp_ae_stat_attr stat_attr;
    ext_isp_dcf_update_info update_info;
} ext_isp_ae_result;

typedef struct {
    td_s32 (*pfn_ae_init)(td_s32 handle, const ext_isp_ae_param *ae_param);
    td_s32 (*pfn_ae_run)(td_s32 handle,
                         const ext_isp_ae_info *ae_info,
                         ext_isp_ae_result *ae_result,
                         td_s32 rsv);
    td_s32 (*pfn_ae_ctrl)(td_s32 handle, td_u32 cmd, td_void *value);
    td_s32 (*pfn_ae_exit)(td_s32 handle);
} ext_isp_ae_exp_func;

typedef struct {
    ext_isp_ae_exp_func ae_exp_func;
} ext_isp_ae_register;

typedef struct {
    td_s32 sensor_id;
    td_u8 wdr_mode;
    td_u8 awb_zone_row;
    td_u8 awb_zone_col;
    td_u8 awb_zone_bin;
    td_u16 awb_width;
    td_u16 awb_height;
    td_u32 init_iso;
    td_s8 rsv;
} ext_isp_awb_param;

typedef struct {
    td_u16 *zone_avg_r;
    td_u16 *zone_avg_g;
    td_u16 *zone_avg_b;
    td_u16 *zone_count;
} ext_isp_awb_stat_result;

typedef struct {
    td_u32  frame_cnt;

    ext_isp_awb_stat_1 *awb_stat1;
    ext_isp_awb_cte_stat *awb_cte;
    ext_isp_awb_stat_result awb_stat2;
    td_u8  awb_gain_switch;
    td_u8  awb_stat_switch;
    td_bool wb_gain_in_sensor;
    td_u32 wdr_wb_gain[ISP_BAYER_CHN_NUM];
} ext_isp_awb_info;

/* the statistics's attr of awb alg */
typedef struct {
    td_bool stat_cfg_update;

    td_u16  metering_white_level_awb;
    td_u16  metering_black_level_awb;
    td_u16  metering_cr_ref_max_awb;
    td_u16  metering_cb_ref_max_awb;
    td_u16  metering_cr_ref_min_awb;
    td_u16  metering_cb_ref_min_awb;
} ext_isp_awb_raw_stat_attr;

/* the final calculate of awb alg */
typedef struct {
    td_u32  white_balance_gain[ISP_BAYER_CHN_NUM];
    td_u16  color_matrix[EXT_ISP_CCM_MATRIX_SIZE];
    td_u32  color_temp;
    td_u8   saturation;
    td_u16  sector_matrix[EXT_ISP_COLOR_SECTOR_NUM][EXT_ISP_CCM_MATRIX_SIZE]; /* Not supported for 850 */
    ext_isp_awb_raw_stat_attr raw_stat_attr;

    td_u16  gain_offset[ISP_BAYER_CHN_NUM];  /* RW; gain offset for white balance */
    td_s32  wb_para[AWB_CURVE_PARA_NUM];      /* RW; parameter for wb curve,p1,p2,q1,a1,b1,c1 */
} ext_isp_awb_result;

typedef struct {
    td_s32 (*pfn_awb_init)(td_s32 handle, const ext_isp_awb_param *awb_param, ext_isp_awb_result *awb_result);
    td_s32 (*pfn_awb_run)(td_s32 handle,
                          const ext_isp_awb_info *awb_info,
                          ext_isp_awb_result *awb_result,
                          td_s32 rsv);
    td_s32 (*pfn_awb_ctrl)(td_s32 handle, td_u32 cmd, td_void *value);
    td_s32 (*pfn_awb_exit)(td_s32 handle);
} ext_isp_awb_exp_func;

typedef struct {
    ext_isp_awb_exp_func awb_exp_func;
} ext_isp_awb_register;

typedef enum {
    FOCUS_IN = 0,
    FOCUS_OUT,
    FOCUS_LOCK,
    FOCUS_MAX
} af_status;

typedef struct {
    td_u32 motor_next_pos;
    td_u32 motor_cur_pos;
    td_u64 cur_fv;
    td_u32 motor_best_pos;
    td_s32 max_fv;
    af_status af_stat;
    td_u32 step;
    td_u16 focus_time;
} ext_isp_af_result;

#define ALG_LIB_NAME_SIZE_MAX           20

typedef struct {
    td_s32  id;
    td_char lib_name[ALG_LIB_NAME_SIZE_MAX];
} ext_isp_alg_lib;

typedef struct {
    td_bool af_traverse;
    td_u32 min_motor_pos; // range [0, 1023]
    td_u32 max_motor_pos; // range [0, 1023]
    td_u32 motor_cur_pos; // range [0, 1023]
} ext_isp_af_manual_attr;

typedef struct {
    td_u32 min_motor_pos; // range [0, 1023]
    td_u32 max_motor_pos; // range [0, 1023]
    td_u32 step; // range [0, 1023]
    td_u8 interval; // range [0, 16]
    td_u8 af_weight[ISP_AF_ZONE_ROW*ISP_AF_ZONE_COLUMN]; // range [0, 255]
} ext_isp_af_auto_attr;

typedef struct {
    td_bool af_en;
    td_bool face_af_en;
    ext_isp_op_type op_type;
    ext_isp_af_manual_attr manual_attr;
    ext_isp_af_auto_attr auto_attr;
} ext_isp_af_attr;

typedef struct {
    td_u32 motor_cur_pos;
    td_s32 focus_value;
    td_s32 max_fv;
    td_u16 focus_time;
} ext_isp_af_info;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
