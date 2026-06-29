/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: define for external use
 * Author: AuthorNameMagicTag
 * Create: 2021-06-28
 */

#ifndef __DRV_ISP_STRUCT_H__
#define __DRV_ISP_STRUCT_H__

#include "drv_isp_comm_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define ISP_FLICKER_G_MEAN_LENGTH       512
#ifdef CONFIG_SOCT_1A22_SUPPORT
#define DEFOG_ZONE_NUM                  384
#else
#define DEFOG_ZONE_NUM                  1024
#endif
#define DEFOG_REG_NUM                   2
#define RES_WIDTH_MIN                   120
#define RES_HEIGHT_MIN                  120
#define ISP_ALIGN_WIDTH                 4
#define ISP_ALIGN_HEIGHT                4

#define ISP_BE_BUF_NUM_MAX              9
#define ISP_BE_BUF_NUM_MIN              2

#define AWB_MIN_WIDTH                   60
#define AWB_MIN_HEIGHT                  14

#define AE_MIN_WIDTH                    256
#define AE_MIN_HEIGHT                   120

#define AF_MIN_WIDTH                    256
#define AF_MIN_HEIGHT                   120
#define AF_ALIGN_WIDTH                  8
#define AF_ALIGN_HEIGHT                 2
#define AF_PLGS_NUM                     2

#define MG_ZONE_ROW                     15
#define MG_ZONE_COLUMN                  17

#define LCAC_STRENGTH_NUM               16
#define ISP_MEM_NAME_LEN_MAX            32

#define EXT_ISP_DRC_EXP_COMP_SAMPLE_NUM  8
#define EXT_ISP_DRC_SHP_LOG_CONFIG_NUM   16

#define BAYER_CHN_R                     0
#define BAYER_CHN_GR                    1
#define BAYER_CHN_GB                    2
#define BAYER_CHN_B                     3

#define ISP_INIT_HAND_SIGNAL            0x66778899
#define ISP_EXIT_HAND_SIGNAL            0x99887766

#define ISP_CFG_2_VLD_DLY_LIMIT         8
#define ISP_MAX_UPDATEINFO_BUF_NUM      (ISP_CFG_2_VLD_DLY_LIMIT)

#define ISP_FE_INT_FSTART               0x1
#define ISP_FE_INT_FEND                 (1 << 4)
#define ISP_REFRESH_ALG                 (1 << 31)

#define ISP_SHARPEN_RGAIN_MAX           31
#define ISP_SHARPEN_GGAIN_MAX           255
#define ISP_SHARPEN_BGAIN_MAX           31

#define ISP_STATISTICS_KEY_DEFAULT      0xFFFBBF77
#define ISP_VREG_REL_ALL_MAGIC_WROD     0xFEEFABCD

#define ISP_LUMA_DATA_LEN               50

#define ISP_MSG_DATA_INPUT_MAX_SIZE    50
#define ISP_MSG_DATA_OUTPUT_MAX_SIZE   24

#define is_linear_mode(mode)      ((mode) == EXT_ISP_WDR_MODE_NONE)
#define is_wdr_mode(mode)         ((!is_linear_mode(mode)) && ((mode) < EXT_ISP_WDR_MODE_MAX))
#define is_built_in_wdr_mode(mode) ((mode) == EXT_ISP_WDR_MODE_BUILT_IN)
#define is_fs_wdr_mode(mode) \
    ((!is_linear_mode(mode))&& (!is_built_in_wdr_mode(mode)) && ((mode) < EXT_ISP_WDR_MODE_MAX))

#define is_2to1_wdr_mode(mode) \
    (((mode) == EXT_ISP_WDR_MODE_2TO1_FRAME) || ((mode) == EXT_ISP_WDR_MODE_2TO1_FRAME_FULL_RATE) || \
    ((mode) == EXT_ISP_WDR_MODE_2TO1_LINE) || ((mode) == EXT_ISP_WDR_MODE_QUDRA))
#define is_3to1_wdr_mode(mode)  \
    (((mode) == EXT_ISP_WDR_MODE_3TO1_FRAME) || ((mode) == EXT_ISP_WDR_MODE_3TO1_FRAME_FULL_RATE) || \
    ((mode) == EXT_ISP_WDR_MODE_3TO1_LINE))
#define is_4to1_wdr_mode(mode)    \
    (((mode) == EXT_ISP_WDR_MODE_4TO1_FRAME) || ((mode) == EXT_ISP_WDR_MODE_4TO1_FRAME_FULL_RATE) || \
    ((mode) == EXT_ISP_WDR_MODE_4TO1_LINE))
#define is_full_wdr_mode(mode)    \
    (((mode) == EXT_ISP_WDR_MODE_2TO1_FRAME_FULL_RATE) || ((mode) == EXT_ISP_WDR_MODE_3TO1_FRAME_FULL_RATE) || \
    ((mode) == EXT_ISP_WDR_MODE_4TO1_FRAME_FULL_RATE))
#define is_half_wdr_mode(mode)   \
    (((mode) == EXT_ISP_WDR_MODE_2TO1_FRAME) || ((mode) == EXT_ISP_WDR_MODE_3TO1_FRAME) || \
    ((mode) == EXT_ISP_WDR_MODE_4TO1_FRAME))
#define is_line_wdr_mode(mode)    \
    (((mode) == EXT_ISP_WDR_MODE_2TO1_LINE) || ((mode) == EXT_ISP_WDR_MODE_3TO1_LINE) || \
    ((mode) == EXT_ISP_WDR_MODE_4TO1_LINE) || ((mode) == EXT_ISP_WDR_MODE_QUDRA))

#define is_offline_mode(mode)    ((mode) == ISP_MODE_RUNNING_OFFLINE)
#define is_online_mode(mode)     ((mode) == ISP_MODE_RUNNING_ONLINE)
#define is_sidebyside_mode(mode) ((mode) == ISP_MODE_RUNNING_SIDEBYSIDE)
#define is_striping_mode(mode)   ((mode) == ISP_MODE_RUNNING_STRIPING)

typedef enum {
    ISP_MEM_TYPE_AUTO = 0,
    ISP_MEM_TYPE_SMMU,
    ISP_MEM_TYPE_MMZ,
    ISP_MEM_TYPE_MAX,
} ext_isp_mem_type;

typedef enum {
    ISP_MEM_OP_ALLOC = 0,
    ISP_MEM_OP_ALLOC_MAP,
    ISP_MEM_OP_ALLOC_MAP_CACHE,
    ISP_MEM_OP_MAX,
} ext_isp_mem_op;

typedef struct {
    td_mem_handle_t handle;
    td_u64 dma_buf;
} ext_isp_mem_priv;

typedef struct {
    ext_isp_mem_type type;
    td_char buf_name[ISP_MEM_NAME_LEN_MAX];
    td_u64   vir_addr;
    td_u64   phy_addr;
    td_u32   size;
    ext_isp_mem_op op;

    td_u32 create_fd_num;
    td_u32 pid[EXT_ISP_MAX_PID_NUM];
    ext_isp_mem_priv priv[EXT_ISP_MAX_PID_NUM];
} ext_isp_mem_info;

typedef enum {
    ISP_MODE_RUNNING_OFFLINE = 0,
    ISP_MODE_RUNNING_ONLINE,
    ISP_MODE_RUNNING_SIDEBYSIDE,
    ISP_MODE_RUNNING_STRIPING,
    ISP_MODE_RUNNING_BUTT,
} isp_running_mode;

typedef enum {
    ISP_MODE_RAW = 0,
    ISP_MODE_BT1120_YUV,
    ISP_MODE_RGB,
    ISP_MODE_DATAINPUT_BUTT,
} isp_data_input_mode;

typedef struct {
    td_u8                pre_block_num;
    td_u8                block_num;
    td_u32               over_lap;
    ext_isp_size              frame_rect;  /* BE full frame  information */
    ext_isp_rect              block_rect[ISP_STRIPING_MAX_NUM];
    isp_running_mode     running_mode;
    isp_data_input_mode  data_input_mode;
} isp_working_mode;

typedef struct {
    td_u8   block_num;
    td_u8   pre_block_num;
    td_u32  over_lap;
    ext_isp_size frame_rect;  /* BE full frame  information */
    ext_isp_rect block_rect[ISP_STRIPING_MAX_NUM];
    isp_running_mode running_mode;
} isp_block_attr;

typedef struct {
    td_u64  base_addr;
    ext_isp_mem_info vreg_mem;
} drv_vreg_args;

typedef struct {
    td_u32 magic;
} isp_version;

typedef struct {
    td_bool fusion_mode;
    td_bool wdr_mdt_en;
    td_u16  short_thr;
    td_u16  long_thr;
    td_u16  offset0;
} isp_fswdr_sync_cfg;

typedef struct {
    td_u8 wdr_mode;
    td_u32 exp_ratio[EXP_RATIO_NUM];
    isp_fswdr_sync_cfg wdr_reg_cfg;
} isp_wdr_cfg;

typedef struct {
    td_bool init;
    ext_isp_mem_info mem_info;
} isp_proc_mem;

typedef struct {
    td_u64  phy_addr;
    td_u64  vir_addr;
} isp_mmz_buf;

typedef struct {
    td_bool       init;
    ext_isp_mem_info mem_info;
    isp_mmz_buf dng_info;
    isp_mmz_buf atta_info;
    isp_mmz_buf color_gammut_info;
    isp_mmz_buf frame_info;
    isp_mmz_buf update_info;
} isp_trans_info_buf;

typedef struct {
    td_bool       init;
    ext_isp_mem_info mem_info;
    isp_mmz_buf pro_nr_param_info;
    isp_mmz_buf pro_shp_param_info;
} isp_pro_info_buf;

/*
 * 0 = default mode:no snap
 * 1 = only preview
 * 2 = only picture
 * 3 = preview and picture is the same pipe
 */
typedef enum {
    ISP_SNAP_NONE            = 0,
    ISP_SNAP_PREVIEW,
    ISP_SNAP_PICTURE,
    ISP_SNAP_PREVIEW_PICTURE,
    ISP_SNAP_BUTT
} isp_snap_pipe_mode;

typedef struct {
    td_bool    pq_ai_en;
    td_s32      ai_pipe_id;
    td_s32      base_pipe_id;
} pq_ai_attr;

typedef struct {
    td_u8  buf_num;
    td_u8  buf_idx;
    td_u64 head_phy_addr;
    ext_isp_mem_info mem_info;
    isp_buf_rec read_buf[ISP_BE_BUF_NUM_MAX];
} isp_ldci_read_stt_buf;

typedef struct {
    td_u32  rsv;         /* H;need to add member */
} ext_isp_dbg_attr;

typedef struct {
    td_u32  frm_num_bgn;
    td_u32  rsv;           /* H;need to add member */
    td_u32  frm_num_end;
} ext_isp_dbg_status;

typedef union {
    td_u32  key;
    struct {
        td_u32  bit1_awb_cfg     : 1;   /* [0] */
        td_u32 bit1_be_af_cfg : 1; /* [1] */
        td_u32 bit30_rsv : 30; /* [2:31] */
    };
} isp_reg_kernel_cfg_key;

typedef struct {
    ext_isp_statistics_ctrl  stat_key;
    td_u64   phy_addr;
    td_void *virt_addr;                 /* (isp_stat *) */
    td_u64   offset;
    td_u64   total_size;
    td_mem_handle_t handle;
    ext_isp_mem_priv priv[EXT_ISP_MAX_PID_NUM];
    td_u32 create_fd_num;
    td_u32 pid[EXT_ISP_MAX_PID_NUM];
} isp_stat_info;

typedef struct {
    td_u64  exposure;
    td_u64  exposure_sf;
    td_u32  int_time[ISP_WDR_FRAME_MAX];
    td_u32  isp_dgain;
    td_u32  wdr_gain[ISP_WDR_FRAME_MAX];

    td_bool piris_valid;
    td_s32  piris_pos;

    ext_isp_fswdr_mode fs_wdr_mode;
} isp_ae_reg_cfg_2;

typedef  struct {
    td_u32 colum;
    td_u32 row;
    td_u32 zone_bin;
} awb_zone_cfg;

typedef struct {
    td_u32 colum;
    td_u32 row;
} af_zone_cfg;

typedef struct {
    td_u32 column;
    td_u32 row;
} ae_zone_cfg;

typedef struct {
    ae_zone_cfg               be_ae_zone_cfg;
    awb_zone_cfg              be_awb_zone_cfg;
    af_zone_cfg               be_af_zone_cfg;
} isp_reg_kernel_cfg;

typedef struct {
    isp_reg_kernel_cfg_key    cfg_key;
    isp_reg_kernel_cfg        alg_kernel_cfg[ISP_STRIPING_MAX_NUM];
} isp_kernel_reg_cfg;

/* the statistics of af alg */
typedef struct {
    td_u16  v1;
    td_u16  h1;
    td_u16  v2;
    td_u16  h2;
    td_u16  y;
    td_u16  hl_cnt;
} ext_isp_af_zone;

typedef struct {
    td_u64 fv1;
    td_u64 fv2;
    ext_isp_af_zone zone_metrics[ISP_AF_ZONE_ROW][ISP_AF_ZONE_COLUMN]; /* R; the zoned measure of contrast */
} ext_isp_be_af_stat;

typedef struct {
    td_u16  metering_mem_array_avg_r[AWB_ZONE_NUM];
    td_u16  metering_mem_array_avg_g[AWB_ZONE_NUM];
    td_u16  metering_mem_array_avg_b[AWB_ZONE_NUM];
    td_u16  metering_mem_array_count_all[AWB_ZONE_NUM];
} ext_isp_awb_stat_2;

/* the statistics of isp */
typedef struct {
    td_u16  defect_pixel_count[ISP_STRIPING_MAX_NUM];
    td_u32  defect_pixel_lut[ISP_STRIPING_MAX_NUM][STATIC_DP_COUNT_NORMAL];
} isp_dp_stat;

typedef struct {
    td_u32 cur_diff_gr[ISP_STRIPING_MAX_NUM];                            /* s24.0, current frame gr sum */
    td_u32 cur_diff_gb[ISP_STRIPING_MAX_NUM];                            /* s24.0, current frame gb sum */
    td_u32 cur_abs_gr[ISP_STRIPING_MAX_NUM];                             /* u23.0 */
    td_u32 cur_abs_gb[ISP_STRIPING_MAX_NUM];                             /* u23.0 */
    td_u32 g_mean[ISP_STRIPING_MAX_NUM][ISP_FLICKER_G_MEAN_LENGTH];    /* u22.0 */
} isp_flicker_stat;

typedef struct {
    td_u32  max_stat_dout[ISP_STRIPING_MAX_NUM][DEFOG_ZONE_NUM];
    td_u32  min_dout[ISP_STRIPING_MAX_NUM][DEFOG_ZONE_NUM / DEFOG_REG_NUM];
} isp_dehaze_stat;

typedef struct {
    td_u16  zone_avg[MG_ZONE_ROW][MG_ZONE_COLUMN][ISP_BAYER_CHN_NUM];
} isp_mg_stat;

typedef struct {
    td_u32  global_map[EXT_ISP_LDCI_STAT_WND_Y][EXT_ISP_LDCI_STAT_WND_X];
} isp_ldci_stat;

typedef struct {
    td_u32  gdci_bin_y[EXT_ISP_GDCI_BIN_NUM];
    td_u32  gdci_bin_cb[EXT_ISP_GDCI_BIN_NUM];    /* 0 */
    td_u32  gdci_bin_cr[EXT_ISP_GDCI_BIN_NUM];    /* 1 */
    td_u32  dci_large_grad_cnt;
} isp_gdci_stat;

typedef struct {
    td_u16  white_balance_gain[ISP_BAYER_CHN_NUM];
} isp_comm_stat;

typedef struct {
    ext_isp_fe_ae_stat_1  fe_ae_stat1;
    ext_isp_fe_ae_stat_2  fe_ae_stat2;
    ext_isp_fe_ae_stat_3  fe_ae_stat3;
    ext_isp_be_ae_stat_1  be_ae_stat1;
    ext_isp_be_ae_stat_2  be_ae_stat2;
    ext_isp_be_ae_stat_3  be_ae_stat3;
    ext_isp_be_ae_stat_4  be_ae_stat4;
    ext_isp_awb_stat_1    awb_stat1;
    ext_isp_awb_stat_2    awb_stat2;
    ext_isp_awb_cte_stat  awb_cte;
    ext_isp_be_af_stat    be_af_stat;
    isp_dp_stat         dp_stat;
    isp_comm_stat       comm_stat;
    isp_dehaze_stat     dehaze_stat;
    isp_mg_stat       mg_stat;
    isp_flicker_stat    flicker_stat;
    isp_ldci_stat       ldci_stat;
    isp_gdci_stat       gdci_stat;
    ext_isp_face_stat   face_stat;
    td_bool             be_update;
} isp_stat;

/* DRC register configure start */
typedef struct {
    td_u8   shp_log;
    td_u8   shp_exp;
    td_bool is_offline_repeat_mode;
    td_s32  prev_luma_delta[EXT_ISP_DRC_EXP_COMP_SAMPLE_NUM - 1];
} isp_drc_sync_cfg;

typedef struct {
    td_bool de_enable;
    td_bool bnr_enable;
} isp_de_reg_kernel;

typedef struct {
    td_u32  be_white_balance_gain[ISP_BAYER_CHN_NUM];
    td_u32  wdr_wb_gain[ISP_BAYER_CHN_NUM];
    td_u16  color_matrix[EXT_ISP_CCM_MATRIX_SIZE];
    ext_isp_awb_switch be_awb_switch;
} isp_awb_sync_cfg;

typedef struct {
    td_bool resh_dyna;
    td_bool fe_blc1_en;      // 1A22
    /* FE module */
    isp_blc_dyna_cfg fe_dg_blc;
    isp_blc_dyna_cfg fe_wb_blc;
    isp_blc_dyna_cfg fe_ae_blc;
    isp_blc_dyna_cfg fe_blc;
    isp_blc_dyna_cfg rc_blc;
} isp_fe_blc_dyna_cfg;

typedef struct {
    td_bool valid;
    ext_isp_sns_regs_info sns_regs_info;
    isp_ae_reg_cfg_2     ae_reg_cfg;
    isp_fswdr_sync_cfg   wdr_reg_cfg;
    isp_drc_sync_cfg     drc_reg_cfg;
    isp_de_reg_kernel    de_reg_cfg;
    isp_awb_sync_cfg      awb_reg_cfg;
    isp_fe_blc_dyna_cfg  fe_blc_reg_cfg;
    isp_be_blc_dyna_cfg  be_blc_reg_cfg;
    ext_isp_sharpen_reg_cfg  sharpen_reg_cfg;
} isp_sync_cfg_buf_node;

typedef struct {
    ext_isp_video_frame_info img_in;
    ext_isp_video_frame_info img_out;
    td_u32 array_size;
    ext_isp_rect rect[ISP_LUMA_DATA_LEN];
    td_u64 data[ISP_LUMA_DATA_LEN];
} isp_luma_data;

typedef enum {
    ISP_ALG_MODE_DIS = 0x0,
    ISP_ALG_MODE_MAX,
} isp_alg_mode_alone;

typedef enum {
    ISP_ALG_CMD_RUN = 0x0,
    ISP_ALG_CMD_CTRL,
    ISP_ALG_CMD_INIT,
    ISP_ALG_CMD_EXIT,
    ISP_ALG_CMD_MAX
} isp_alg_cmd_id;

typedef struct {
    td_u32 len;
    td_u32 data[ISP_MSG_DATA_INPUT_MAX_SIZE];
} isp_alg_task_input;

typedef struct {
    td_u32 len;
    td_u32 data[ISP_MSG_DATA_OUTPUT_MAX_SIZE];
} isp_alg_task_output;

typedef struct {
    isp_alg_mode_alone alg_mode;
    isp_alg_cmd_id cmd_id;
    isp_alg_task_input cmd_data;
} isp_alg_task;

typedef struct {
    isp_alg_mode_alone alg_mode;
    td_u32 cmd_id;
    td_s32 ret_val;
    isp_alg_task_output result_data;
} isp_alg_task_result;

typedef struct {
    td_u64 scale_coeff_fd;
    td_u32 scale_coeff_size;
} isp_pq_init_param;

typedef enum {
    EXT_ISP_MD_MODE              = 0,
    EXT_ISP_PURE_TNR_MODE        = 1,
    EXT_ISP_3DNR_MODE            = 2,
    EXT_ISP_BNR_MAX
} ext_isp_bnr_ref_mode;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
