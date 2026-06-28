/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: define for external use
 * Author: AuthorNameMagicTag
 * Create: 2021-08-12
 */

#ifndef __MPI_PPU_EXT_H__
#define __MPI_PPU_EXT_H__

#include "drv_ioctl_ppu.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

td_s32 ext_mpi_ppu_init(td_void);
td_s32 ext_mpi_ppu_deinit(td_void);

/* group settings */
td_s32 ext_mpi_ppu_get_grp_default_attr(td_s32 grp_id, ext_ppu_grp_attr *grp_attr);
td_s32 ext_mpi_ppu_create_grp(td_s32 grp_id, const ext_ppu_grp_attr *grp_attr);
td_s32 ext_mpi_ppu_destroy_grp(td_s32 grp_id);

td_s32 ext_mpi_ppu_start_grp(td_s32 grp_id);
td_s32 ext_mpi_ppu_stop_grp(td_s32 grp_id);

td_s32 ext_mpi_ppu_reset_grp(td_s32 grp_id);

td_s32 ext_mpi_ppu_get_grp_attr(td_s32 grp_id, ext_ppu_grp_attr *grp_attr);
td_s32 ext_mpi_ppu_set_grp_attr(td_s32 grp_id, const ext_ppu_grp_attr *grp_attr);

td_s32 ext_mpi_ppu_queue_frame(td_s32 grp_id, td_s32 grp_pipe_id,
    const ext_isp_video_frame_info *video_frame, td_s32 milli_sec);
td_s32 ext_mpi_ppu_dequeue_frame(td_s32 grp_id, td_s32 grp_pipe_id,
    ext_isp_video_frame_info *video_frame, td_s32 milli_sec);

/* chn settings */
td_s32 ext_mpi_ppu_get_chn_default_attr(td_s32 grp_id, ext_ppu_chn_attr *chn_attr);
td_s32 ext_mpi_ppu_set_chn_attr(td_s32 grp_id, td_s32 chn_id, const ext_ppu_chn_attr *chn_attr);
td_s32 ext_mpi_ppu_get_chn_attr(td_s32 grp_id, td_s32 chn_id, ext_ppu_chn_attr *chn_attr);
td_s32 ext_mpi_ppu_set_chn_extbuf_attr(td_s32 grp_id, td_s32 chn_id, const ext_ppu_chn_extbuf_attr *chn_extbuf_attr);
td_s32 ext_mpi_ppu_get_chn_extbuf_attr(td_s32 grp_id, td_s32 chn_id, ext_ppu_chn_extbuf_attr *chn_extbuf_attr);

td_s32 ext_mpi_ppu_chn_attach(td_s32 grp_id, td_s32 chn_id, td_s32 sink_handlle);
td_s32 ext_mpi_ppu_chn_detach(td_s32 grp_id, td_s32 chn_id, td_s32 sink_handlle);
td_s32 ext_mpi_ppu_enable_chn(td_s32 grp_id, td_s32 chn_id);
td_s32 ext_mpi_ppu_disable_chn(td_s32 grp_id, td_s32 chn_id);

td_s32 ext_mpi_ppu_set_cover(td_s32 grp_id, td_s32 chn_id, const ext_ppu_cover_info *cover_attr);
td_s32 ext_mpi_ppu_get_cover(td_s32 grp_id, td_s32 chn_id, ext_ppu_cover_info *cover_attr);
td_s32 ext_mpi_ppu_set_corner(td_s32 grp_id, td_s32 chn_id, const ext_ppu_corner_info *corner_attr);
td_s32 ext_mpi_ppu_get_corner(td_s32 grp_id, td_s32 chn_id, ext_ppu_corner_info *corner_attr);

td_s32 ext_mpi_ppu_set_chn_ldc_attr(td_s32 grp_id, td_s32 chn_id, const ext_ppu_ldc_attr *ldc_attr);
td_s32 ext_mpi_ppu_get_chn_ldc_attr(td_s32 grp_id, td_s32 chn_id, ext_ppu_ldc_attr *ldc_attr);

td_s32 ext_mpi_ppu_acquire_chn_frame(td_s32 grp_id, td_s32 chn_id,
    ext_isp_video_frame_info *video_frame, td_s32 milli_sec);
td_s32 ext_mpi_ppu_release_chn_frame(td_s32 grp_id, td_s32 chn_id,
    const ext_isp_video_frame_info *video_frame);

td_s32 ext_mpi_ppu_get_region_luma(td_s32 grp_id, td_s32 chn_id, const ext_isp_video_region_info *region_info,
    td_u32 *luma_data, td_s32 milli_sec);

td_s32 ext_mpi_ppu_set_extchn_attr(td_s32 grp_id, td_s32 chn_id, const ext_ppu_extchn_attr *extchn_attr);
td_s32 ext_mpi_ppu_get_extchn_attr(td_s32 grp_id, td_s32 chn_id, ext_ppu_extchn_attr *extchn_attr);

td_s32 ext_mpi_ppu_set_grp_nrx_param(td_s32 grp_id, const ext_pq_grp_nrx_param *nrx_param);
td_s32 ext_mpi_ppu_get_grp_nrx_param(td_s32 grp_id, ext_pq_grp_nrx_param *nrx_param);

td_s32 ext_mpi_ppu_set_grp_wnr_param(td_s32 grp_id, const ext_pq_wnr_param *wnr_param);
td_s32 ext_mpi_ppu_get_grp_wnr_param(td_s32 grp_id, ext_pq_wnr_param *wnr_param);
td_s32 ext_mpi_ppu_set_grp_sharpen_param(td_s32 grp_id, const ext_pq_sharpen_attr *sharpen_param);
td_s32 ext_mpi_ppu_get_grp_sharpen_param(td_s32 grp_id, ext_pq_sharpen_attr *sharpen_param);

td_s32 ext_mpi_ppu_chn_set_fsd_attr(td_s32 grp_id, td_s32 chn_id, const ext_ppu_fsd_attr *fsd_attr);
td_s32 ext_mpi_ppu_chn_get_fsd_attr(td_s32 grp_id, td_s32 chn_id, ext_ppu_fsd_attr *fsd_attr);
td_s32 ext_mpi_ppu_set_chn_proc_mode(td_s32 grp_id, td_s32 chn_id,
    ext_ppu_chn_proc_mode ppu_chn_proc_mode);
td_s32 ext_mpi_ppu_get_chn_proc_mode(td_s32 grp_id, td_s32 chn_id,
    ext_ppu_chn_proc_mode *ppu_chn_proc_mode);

td_s32 ext_mpi_ppu_set_work_mode(const ext_ppu_work_mode_attr *work_mode_attr);
td_s32 ext_mpi_ppu_get_work_mode(ext_ppu_work_mode_attr *work_mode_attr);
td_s32 ext_mpi_ppu_attach_vb_pool(td_s32 grp_id, td_s32 chn_id, vb_pool vb_pool);
td_s32 ext_mpi_ppu_detach_vb_pool(td_s32 grp_id, td_s32 chn_id);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VPSS_ADAPT_H__ */
