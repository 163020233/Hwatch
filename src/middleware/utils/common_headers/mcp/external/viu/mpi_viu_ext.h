/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: define for external use
 * Author: AuthorNameMagicTag
 * Create: 2021-09-22
 */

#ifndef __MPI_VIU_EXT_H__
#define __MPI_VIU_EXT_H__

#include "drv_ioctl_viu.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 ext_mpi_viu_init(td_void);
td_s32 ext_mpi_viu_deinit(td_void);

td_s32 ext_mpi_viu_set_work_mode(td_s32 pipe_id, td_bool pipe_online, td_bool chn_online);
td_s32 ext_mpi_viu_get_work_mode(td_s32 pipe_id, td_bool *pipe_online, td_bool *chn_online);

td_s32 ext_mpi_viu_snap_suspend(td_s32 pipe_id);
td_s32 ext_mpi_viu_snap_resume(td_s32 pipe_id);

/* 1 for vi device */
td_s32 ext_mpi_viu_set_dev_attr(td_s32 dev_id, const ext_viu_dev_attr *dev_attr);
td_s32 ext_mpi_viu_get_dev_attr(td_s32 dev_id, ext_viu_dev_attr *dev_attr);

td_s32 ext_mpi_viu_enable_dev(td_s32 dev_id);
td_s32 ext_mpi_viu_disable_dev(td_s32 dev_id);

td_s32 ext_mpi_viu_dev_attach(td_s32 dev_id, td_s32 pipe_id);
td_s32 ext_mpi_viu_dev_detach(td_s32 dev_id, td_s32 pipe_id);

td_s32 ext_mpi_viu_set_dev_timing_attr(td_s32 dev_id, const ext_viu_dev_timing_attr *dev_timing_attr);
td_s32 ext_mpi_viu_get_dev_timing_attr(td_s32 dev_id, ext_viu_dev_timing_attr *dev_timing_attr);

 /* 2 for vi pipe */
td_s32 ext_mpi_viu_create_pipe(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr);
td_s32 ext_mpi_viu_destroy_pipe(td_s32 pipe_id);

td_s32 ext_mpi_viu_set_pipe_attr(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr);
td_s32 ext_mpi_viu_get_pipe_attr(td_s32 pipe_id, ext_viu_pipe_attr *pipe_attr);

td_s32 ext_mpi_viu_start_pipe(td_s32 pipe_id);
td_s32 ext_mpi_viu_stop_pipe(td_s32 pipe_id);

td_s32 ext_mpi_viu_set_pipe_dump_attr(td_s32 pipe_id, const ext_viu_dump_attr *dump_attr);
td_s32 ext_mpi_viu_get_pipe_dump_attr(td_s32 pipe_id, ext_viu_dump_attr *dump_attr);

td_s32 ext_mpi_viu_acquire_pipe_frame(td_s32 pipe_id, ext_isp_video_frame_info *frame_info, td_s32 ms);
td_s32 ext_mpi_viu_release_pipe_frame(td_s32 pipe_id, const ext_isp_video_frame_info *frame_info);

td_s32 ext_mpi_viu_acquire_zsl_frame(td_s32 pipe_id, ext_isp_video_frame_info *frame_info, td_s32 milli_sec);
td_s32 ext_mpi_viu_release_zsl_frame(td_s32 pipe_id, const ext_isp_video_frame_info *frame_info);

td_s32 ext_mpi_viu_pipe_attach(td_s32 pipe_id, td_s32 chn_id, soc_mod_id dst_mod, td_s32 dst_id);
td_s32 ext_mpi_viu_pipe_detach(td_s32 pipe_id, td_s32 chn_id, soc_mod_id dst_mod, td_s32 dst_id);

td_s32 ext_mpi_viu_queue_pipe_frame(td_s32 pipe_id, const ext_isp_video_frame_info *frame_info);

td_s32 ext_mpi_viu_set_dynamic_attr(td_s32 pipe_id, const ext_viu_dynamic_attr *dynamic_attr);
td_s32 ext_mpi_viu_get_dynamic_attr(td_s32 pipe_id, ext_viu_dynamic_attr *dynamic_attr);
td_s32 ext_mpi_viu_acquire_zsl_frame(td_s32 pipe_id, ext_isp_video_frame_info *frame_info, td_s32 milli_sec);
td_s32 ext_mpi_viu_release_zsl_frame(td_s32 pipe_id, const ext_isp_video_frame_info *frame_info);

 /* 3 for vi chn */
td_s32 ext_mpi_viu_set_chn_attr(td_s32 pipe_id, td_s32 chn_id, const ext_viu_chn_attr *chn_attr);
td_s32 ext_mpi_viu_get_chn_attr(td_s32 pipe_id, td_s32 chn_id, ext_viu_chn_attr *chn_attr);

td_s32 ext_mpi_viu_enable_chn(td_s32 pipe_id, td_s32 chn_id);
td_s32 ext_mpi_viu_disable_chn(td_s32 pipe_id, td_s32 chn_id);

td_s32 ext_mpi_viu_set_ext_chn_attr(td_s32 pipe_id, td_s32 chn_id, const ext_viu_ext_chn_attr *ext_chn_attr);
td_s32 ext_mpi_viu_get_ext_chn_attr(td_s32 pipe_id, td_s32 chn_id, ext_viu_ext_chn_attr *ext_chn_attr);

td_s32 ext_mpi_viu_acquire_chn_frame(td_s32 pipe_id, td_s32 chn_id, ext_isp_video_frame_info *frame_info, td_s32 ms);
td_s32 ext_mpi_viu_acquire_dsf_frame(td_s32 pipe_id, td_s32 chn_id, ext_isp_video_frame_info *frame_info, td_s32 ms);
td_s32 ext_mpi_viu_release_chn_frame(td_s32 pipe_id, td_s32 chn_id, const ext_isp_video_frame_info *frame_info);

td_s32 ext_mpi_viu_queue_chn_frame(td_u32 pipe_num, td_s32 pipe_id[], const ext_isp_video_frame_info *frame_info[]);
td_s32 ext_mpi_viu_dequeue_chn_frame(td_s32 pipe_id, ext_isp_video_frame_info *frame_info, td_s32 ms);
td_s32 ext_mpi_viu_wait_chn_frame_done(
    td_u32 pipe_num, td_s32 pipe_id[], const ext_isp_video_frame_info *frame_info[], td_s32 timeout_ms);
/* 4 for vi alg */
td_s32 ext_mpi_viu_set_fpn_attr(td_s32 pipe_id, ext_viu_fpn_attr *fpn_attr);
td_s32 ext_mpi_viu_get_fpn_attr(td_s32 pipe_id, ext_viu_fpn_attr *fpn_attr);

td_s32 ext_mpi_viu_set_pipe_nrx_param(td_s32 pipe_id, const ext_viu_pipe_nrx_param *nrx_param);
td_s32 ext_mpi_viu_get_pipe_nrx_param(td_s32 pipe_id, ext_viu_pipe_nrx_param *nrx_param);

td_s32 ext_mpi_viu_set_chn_ldc_attr(td_s32 pipe_id, td_s32 chn_id, const ext_viu_ldc_attr *ldc_attr);
td_s32 ext_mpi_viu_get_chn_ldc_attr(td_s32 pipe_id, td_s32 chn_id, ext_viu_ldc_attr *ldc_attr);

td_s32 ext_mpi_viu_ldc_pos_query_dst_to_src(td_s32 pipe_id, td_s32 chn_id,
    const ext_isp_point *dst_point_in, ext_isp_point *src_point_out);
td_s32 ext_mpi_viu_ldc_pos_query_src_to_dst(td_s32 pipe_id, td_s32 chn_id,
    const ext_isp_point *src_point_in, ext_isp_point *dst_point_out);

/* 4 for vi dis */
td_s32 ext_mpi_viu_set_chn_dis_param(td_s32 viu_pipe_id, td_s32 viu_chn_id, const ext_dis_param *dis_param);
td_s32 ext_mpi_viu_get_chn_dis_param(td_s32 viu_pipe_id, td_s32 viu_chn_id, ext_dis_param *dis_param);

td_s32 ext_mpi_viu_set_chn_dis_config(td_s32 viu_pipe_id, td_s32 viu_chn_id, const ext_dis_config *dis_config);
td_s32 ext_mpi_viu_get_chn_dis_config(td_s32 viu_pipe_id, td_s32 viu_chn_id, ext_dis_config *dis_config);

td_s32 ext_mpi_viu_set_chn_dis_attr(td_s32 viu_pipe_id, td_s32 viu_chn_id, const ext_dis_attr *dis_attr);
td_s32 ext_mpi_viu_get_chn_dis_attr(td_s32 viu_pipe_id, td_s32 viu_chn_id, ext_dis_attr *dis_attr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* End of __MPI_VIU_EXT_H__ */

