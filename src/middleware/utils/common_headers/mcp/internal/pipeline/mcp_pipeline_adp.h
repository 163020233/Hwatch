/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mcp pipeline adapter header file
 * Author: AuthorNameMagicTag
 * Create: 2025-12-24
 */

#ifndef __MCP_PIPELINE_ADP_H__
#define __MCP_PIPELINE_ADP_H__

#include "mpi_cis_ext.h"
#include "mpi_viu_ext.h"
#include "mpi_ppu_ext.h"
#include "mpi_mcp_pipeline_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define mcp_print_alert(fmt...)    mcp_log_alert(fmt)
#define mcp_print_fatal(fmt...)    mcp_log_fatal(fmt)
#define mcp_print_err(fmt...)      mcp_log_err(fmt)
#define mcp_print_warn(fmt...)     mcp_log_warn(fmt)
#define mcp_print_notice(fmt...)   mcp_log_notice(fmt)
#define mcp_print_info(fmt...)     mcp_log_info(fmt)
#define mcp_print_dbg(fmt...)      mcp_log_dbg(fmt)
#define mcp_print_trace(fmt...)    mcp_log_trace(fmt)

#define SUPPORT_MAX_SNS_NUM             2
#define DEVICE_MAX_INSTANCE_NUM         2
#define DEVICE_MAX_STREAM_NUM           3
#define DEVICE_MAX_PIPE_NUM             2
#define DEVICE_MAX_CHN_NUM              3
#define DEVICE_VIU_PREVIEW_BUFF_NUM     4
#define DEVICE_PPU_PREVIEW_BUFF_NUM     5
#define DEVICE_VIU_PREVIEW_RAW_NUM      6

typedef enum {
    MCP_SENSOR_2M = 0,
    MCP_SENSOR_5M,
    MCP_SENSOR_8M,
    MCP_SENSOR_13M,
    MCP_SENSOR_5M_BINNING,
    MCP_SENSOR_MAX
} mcp_sns_mode;

typedef struct {
    td_bool viproc_online;
    td_bool vpss_online;
    td_bool venc_online;
    td_u32 viu_preview_buff_num;
    td_u32 ppu_preview_buff_num;
    td_u32 fsd_buff_num;
    td_u32 zsl_buff_num;
    td_u32 user_vb_buff_num;

    td_bool colorbar_en;

    mcp_sns_mode sns_mode;
    mcp_sns_type sns_type;
    ext_isp_sns_obj *sns_obj;
    ext_isp_sns_feature work_sns_attr;

    mcp_device_meta_digital_zoom digital_zoom;
    mcp_device_meta_rotation rotation;
    mcp_device_meta_capture capture;
    mcp_device_meta_dis dis;
    mcp_device_meta_preview preview;
    td_u32 frame_rate;
    td_u32 work_sns_id;
    td_s32 phy_id;

    vb_pool user_pool_id;
} mcp_device_cfg;

typedef enum {
    MCP_CAPTURE_STATE_DEFAULT = 0,
    MCP_CAPTURE_STATE_PREPARE,
    MCP_CAPTURE_STATE_SUCCESS,
    MCP_CAPTURE_STATE_MAX
} mcp_capture_state;

typedef struct {
    mcp_capture_state cap_state;
} mcp_device_state;

typedef struct {
    td_bool is_stream_created;
    td_s32 ppu_grp;
    td_s32 stream_id;
    td_bool enable;
    mcp_stream_attr stream_attr;
    mcp_stream_meta_venc venc;
    mcp_stream_meta_fsd fsd;
    mcp_stream_meta_corner corner;
    mcp_stream_meta_rotation rotation;
    mcp_stream_meta_output_cap output_cap;
    mcp_stream_meta_crop_info crop;
} mcp_stream_cfg;

typedef struct {
    td_u32               sns_index;
    mcp_sns_mode         sns_mode;
    mcp_sns_type         sns_type;
    ext_isp_sns_obj      *sns_obj;
    td_u32               support_feature_num;
    ext_isp_sns_feature  support_feature[MCP_SNS_FORMATS_NUM];
} mcp_sns_cap;

typedef struct {
    td_u32              sns_width;
    td_u32              sns_height;
    td_u32              bas_out_width;
    td_u32              bas_out_height;
    ext_isp_crop_info   pipe_crop_info;

    ext_isp_size        ppu_zme_out;
    ext_isp_crop_info   ppu_crop;
} mcp_pipeline_reso;

typedef struct {
    td_s32 pipe_id;
    td_bool pipe_enable;
    td_s32 chn_id[DEVICE_MAX_CHN_NUM];
} mcp_pipeline_pipe_info;

typedef struct {
    td_bool is_instance_created;
    td_handle instance_handle;
    td_bool enable;
    mcp_device_attr device_attr;
    mcp_pipeline_reso path_reso;
    mcp_device_cfg device_cfg;
    mcp_device_state device_state;
    mcp_stream_cfg stream_cfg[DEVICE_MAX_STREAM_NUM];

    td_s32 dev_id;
    mcp_pipeline_pipe_info pipe_info[DEVICE_MAX_PIPE_NUM];
    td_s32 ppu_grp;

#ifdef CONFIG_CHIP_PLATFORM_WEAR
    osal_task *isp_handle;
#else
    pthread_t isp_handle;
#endif

    td_bool is_mcp_init;
    td_bool is_vb_init;
    td_bool is_mipirx_init;
    td_bool is_isp_thread_init;
    td_bool is_isp_init;
    td_bool is_viu_pipe_create;
    td_bool is_ppu_grp_create;
} mcp_device_ctx;

typedef struct {
    td_bool is_pipeline_init;
#ifdef CONFIG_CHIP_PLATFORM_WEAR
    osal_task *isp_handle;
#else
    pthread_t isp_handle;
#endif
    ext_mcp_pipeline_cfg pipeline_cfg;
    mcp_stream_cfg stream_cfg;
    td_bool is_mcp_init;
    td_bool is_vb_init;
    td_bool is_mipirx_init;
    td_bool is_isp_thread_init;
    td_bool is_isp_init;
    td_bool is_viu_pipe_create;
    td_bool is_viu_start;
    td_bool is_ppu_init;
    td_bool is_ppu_grp_create;
    mcp_device_ctx device_ctx[DEVICE_MAX_INSTANCE_NUM];
    td_u32 support_sns_number;
    mcp_sns_cap support_sns_cap[SUPPORT_MAX_SNS_NUM];
} mcp_pipeline_ctx;

typedef td_s32 (*mcp_device_meta_handler)(td_handle handle, mcp_device_meta *device_meta);
typedef struct {
    mcp_device_meta_type meta_type;
    mcp_device_meta_handler handler;
} mcp_device_meta_handle_case;

typedef td_s32 (*mcp_device_pq_handler)(td_handle handle, mcp_device_pq *device_pq);
typedef struct {
    mcp_device_pq_type pq_type;
    mcp_device_pq_handler handler;
} mcp_device_pq_handle_case;

typedef td_s32 (*mcp_stream_meta_handler)(td_handle handle, td_s32 stream_id, mcp_stream_meta *stream_meta);
typedef struct {
    mcp_stream_meta_type stream_type;
    mcp_stream_meta_handler handler;
} mcp_stream_meta_handle_case;

#define pipeline_check_ret_no_return(ret, info) do { \
    if ((ret) != TD_SUCCESS) { \
        mcp_print_err("%s, ret %x\n", info, (ret)); \
    } \
} while (0)

#define pipeline_check_ret_with_return(ret, return_code, info) do { \
    if ((ret) != TD_SUCCESS) { \
        mcp_print_err("%s, ret %x\n", info, (ret)); \
        return return_code; \
    } \
} while (0)

#define pipeline_check_init_with_return(in, expected, return_code, info) do { \
    if ((in) != (expected)) { \
        mcp_print_err("%s\n", info); \
        return return_code; \
    } \
} while (0)

#define check_ret_with_goto(ret, goto_branch, info) do { \
    if ((ret) != TD_SUCCESS) { \
        mcp_print_err("%s, ret %x\n", info, (ret)); \
        goto goto_branch; \
    } \
} while (0)

#define check_null_pointer_with_return(p, return_code, info) do { \
    if ((p) == TD_NULL) { \
        mcp_print_err("%s\n", info); \
        return return_code; \
    } \
} while (0)

mcp_pipeline_ctx *mcp_pipeline_get_ctx(td_void);

td_s32 mcp_pipeline_adp_init(fsd_coef_mem_config *mem_config);
td_s32 mcp_pipeline_adp_deinit(td_void);

td_s32 mcp_pipeline_adp_device_open(td_s32 handle, ext_mcp_pipeline_cfg *pipe_cfg);
td_s32 mcp_pipeline_adp_device_close(td_s32 handle);
td_s32 mcp_pipeline_adp_proc_show(td_s32 mod_id);
td_s32 mcp_pipeline_adp_cmd_write(td_u32 mod_id, td_u32 argc, const td_char *argv[]);

td_s32 mcp_pipeline_adp_sns_get_number(td_u32 *sns_number);
td_s32 mcp_pipeline_adp_sns_get_feature(td_s32 chn_id, mcp_sns_basic_feature *basic_feature);

td_s32 mcp_pipeline_adp_device_create(mcp_device_attr *device_attr, td_handle *handle);
td_s32 mcp_pipeline_adp_device_enable(td_handle handle);
td_s32 mcp_pipeline_adp_device_disable(td_handle handle);
td_s32 mcp_pipeline_adp_device_destroy(td_handle handle);
td_s32 mcp_pipeline_adp_device_set_meta(td_handle handle, const mcp_device_meta *device_meta);
td_s32 mcp_pipeline_adp_device_get_meta(td_handle handle, mcp_device_meta *device_meta);
td_s32 mcp_pipeline_adp_device_set_pq(td_handle handle, const mcp_device_pq *device_pq);
td_s32 mcp_pipeline_adp_device_get_pq(td_handle handle, mcp_device_pq *device_pq);
td_s32 mcp_pipeline_adp_device_capture(td_handle handle, mcp_capture_config *cap_cfg,
    ext_isp_video_frame_info *out_video_frame);
td_s32 mcp_pipeline_adp_device_capture_release(td_handle handle, ext_isp_video_frame_info *out_video_frame);
td_s32 mcp_pipeline_adp_device_start_video(td_handle handle, td_s32 stream_id, mcp_video_mode video_mode);
td_s32 mcp_pipeline_adp_device_stop_video(td_handle handle, td_s32 stream_id);

td_s32 mcp_pipeline_adp_stream_create(td_handle handle, mcp_stream_attr *init_attr, td_s32 *stream_id);
td_s32 mcp_pipeline_adp_stream_destroy(td_handle handle, td_s32 stream_id);
td_s32 mcp_pipeline_adp_stream_set_meta(td_handle handle, td_s32 stream_id,
    const mcp_stream_meta *stream_meta);
td_s32 mcp_pipeline_adp_stream_get_meta(td_handle handle, td_s32 stream_id, mcp_stream_meta *stream_meta);
td_s32 mcp_pipeline_adp_stream_on(td_handle handle, td_s32 stream_id);
td_s32 mcp_pipeline_adp_stream_off(td_handle handle, td_s32 stream_id);

td_s32 mcp_pipeline_adp_stream_acquire(td_s32 handle, int stream_id, ext_isp_video_frame_info *video_frame);
td_s32 mcp_pipeline_adp_stream_release(td_s32 handle, int stream_id, ext_isp_video_frame_info *video_frame);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __MCP_PIPELINE_ADP_H__ */
