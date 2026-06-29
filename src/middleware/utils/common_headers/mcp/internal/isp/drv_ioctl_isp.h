/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: ioctl
 * Author: AuthorNameMagicTag
 * Create: 2021-06-28
 */

#ifndef __DRV_IOCTL_ISP_H__
#define __DRV_IOCTL_ISP_H__

#include "drv_mcp_dependency.h"
#include "drv_isp_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef CONFIG_SOCT_SHAOLINSPEAR_SUPPORT
#define DEV_ISP_NAME "soc_isp"
#else
#define DEV_ISP_NAME "isp"
#endif

#define ISP_DEV "/dev/" DEV_ISP_NAME

typedef enum {
    IOC_NR_ISP_SET_FD = 0,
    IOC_NR_ISP_VD_GET_FRAME_EDGE,
    IOC_NR_ISP_SET_INT_ENABLE,
    IOC_NR_ISP_GET_VERSION,
    IOC_NR_ISP_STAT_INIT_BUF,
    IOC_NR_ISP_STAT_EXIT_BUF,
    IOC_NR_ISP_STAT_GET_BUF,
    IOC_NR_ISP_STAT_PUT_BUF,
    IOC_NR_ISP_STAT_GET_ACT_BUF,
    IOC_NR_ISP_SET_REG_CFG,
    IOC_NR_ISP_BE_BUF_INIT,
    IOC_NR_ISP_BE_BUF_EXIT,
    IOC_NR_ISP_BE_BUF_GET_FIRST,
    IOC_NR_ISP_BE_BUF_GET_FREE,
    IOC_NR_ISP_BE_BUF_GET_LAST,
    IOC_NR_ISP_BE_BUF_SET_BUSY,
    IOC_NR_ISP_BE_BUF_SETUP,
    IOC_NR_ISP_PROC_INIT,
    IOC_NR_ISP_PROC_WRITE_START_LOCK,
    IOC_NR_ISP_PROC_WRITE_DONE_UNLOCK,
    IOC_NR_ISP_PROC_EXIT,
    IOC_NR_ISP_PROC_GET_PARAM,
    IOC_NR_ISP_TI_INIT_BUF,
    IOC_NR_ISP_TI_EXIT_BUF,
    IOC_NR_ISP_SNAP_PRO_INIT_BUF,
    IOC_NR_ISP_SNAP_PRO_EXIT_BUF,
    IOC_NR_ISP_SET_SYNC_NODE,
    IOC_NR_ISP_SET_WDR_CFG,
    IOC_NR_ISP_SET_INIT_FLAG,
    IOC_NR_ISP_GET_INIT_FLAG,
    IOC_NR_ISP_GET_P2EN_INFO,
    IOC_NR_ISP_SET_PREPARE_FLAG,
    IOC_NR_ISP_SET_RUN_STATE,
    IOC_NR_ISP_RESET_CTX,
    IOC_NR_ISP_SNAP_SET_CONFIG,
    IOC_NR_ISP_SNAP_GET_INFO,
    IOC_NR_ISP_SNAP_PRO_GET_ENABLE,
    IOC_NR_ISP_GET_UPDATE_POS,
    IOC_NR_ISP_GET_FRAME_COUNT,
    IOC_NR_ISP_SNAP_GET_ATTR,
    IOC_NR_ISP_GET_PWM_NUM,
    IOC_NR_ISP_SET_CHN_SELECT,
    IOC_NR_ISP_TI_SET_FRAME_INFO,
    IOC_NR_ISP_TI_GET_FRAME_INFO,
    IOC_NR_ISP_TI_SET_DNG_INFO,
    IOC_NR_ISP_TI_GET_DNG_INFO,
    IOC_NR_ISP_SET_CTRL_PARAM,
    IOC_NR_ISP_GET_CTRL_PARAM,
    IOC_NR_ISP_INIT_WORK_MODE,
    IOC_NR_ISP_GET_WORK_MODE,
    IOC_NR_ISP_EXIT_WORK_MODE,
    IOC_NR_ISP_SET_PRE_BLK_NUM,
    IOC_NR_ISP_GET_HDR_ATTR,
    IOC_NR_ISP_GET_WDR_ATTR,
    IOC_NR_ISP_GET_PIPE_SIZE,
    IOC_NR_ISP_TI_SET_PUB_INFO,
    IOC_NR_ISP_CLUT_INIT_BUF,
    IOC_NR_ISP_CLUT_EXIT_BUF,
    IOC_NR_ISP_CLUT_GET_BUF,
    IOC_NR_ISP_STT_INIT_BUF,
    IOC_NR_ISP_STT_EXIT_BUF,
    IOC_NR_ISP_STT_SET_FE_INIT_ADDR,
    IOC_NR_ISP_STT_GET_BE_LUT_BUF,
    IOC_NR_ISP_SNAP_PRO_SET_CALC_DONE,
    IOC_NR_ISP_TI_SET_DCF_UPDATE_INFO,
    IOC_NR_ISP_BE_BUF_INIT_SYNC_PARA,
    IOC_NR_ISP_LDCI_INIT_BUF,
    IOC_NR_ISP_LDCI_EXIT_BUF,
    IOC_NR_ISP_LDCI_GET_READ_STT_BUF,
    IOC_NR_ISP_SET_AWB_EN,
    IOC_NR_ISP_BE_BUF_GET_NUM,
    IOC_NR_ISP_VREG_SET_FD,
    IOC_NR_ISP_VREG_INIT,
    IOC_NR_ISP_VREG_EXIT,
    IOC_NR_ISP_VREG_CLEAN_UP,
    IOC_NR_ISP_VREG_GET_ADDR,
    IOC_NR_ISP_GET_REPEAT_MODE,
    IOC_NR_ISP_SET_FPN_ATTR,
    IOC_NR_ISP_GET_FPN_ATTR,
    IOC_NR_ISP_GET_LUMA_DATA,

    IOC_NR_ISP_GET_SINGLE_TASK,
    IOC_NR_ISP_SET_SINGLE_RESULT,
    IOC_NR_ISP_THREAD_FORCE_WAKE_UP,
    IOC_NR_ISP_PQ_INIT,
    IOC_NR_ISP_PQ_DEINIT,
    IOC_NR_ISP_KERNEL_RUNONCE,
    IOC_NR_ISP_RUNONCE_INFO,
    IOC_NR_ISP_MAX,
} ioc_nr_isp;

#define IOC_TYPE_ISP         'I'

#define ISP_SET_FD                      _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_SET_FD, td_s32)
#define ISP_GET_VERSION                 _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_GET_VERSION, isp_version)
#define ISP_SET_INIT_FLAG               _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_SET_INIT_FLAG, td_bool)
#define ISP_GET_INIT_FLAG               _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_GET_INIT_FLAG, td_bool)
#define ISP_SET_PREPARE_FLAG            _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_SET_PREPARE_FLAG, td_bool)
#define ISP_RESET_CTX                   _IO(IOC_TYPE_ISP,   IOC_NR_ISP_RESET_CTX)
#define ISP_SET_INT_ENABLE              _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_SET_INT_ENABLE, td_bool)
#define ISP_SET_REG_CFG                 _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_SET_REG_CFG, isp_kernel_reg_cfg)
#define ISP_GET_P2EN_INFO               _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_GET_P2EN_INFO, td_bool)
#define ISP_SET_RUN_STATE               _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_SET_RUN_STATE, td_u64)
#define ISP_GET_UPDATE_POS              _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_GET_UPDATE_POS, td_u32)
#define ISP_GET_FRAME_COUNT             _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_GET_FRAME_COUNT, td_u32)
#define ISP_SET_SYNC_NODE               _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_SET_SYNC_NODE, isp_sync_cfg_buf_node)
#define ISP_SET_WDR_CFG                 _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_SET_WDR_CFG, isp_wdr_cfg)
#define ISP_GET_PIPE_SIZE               _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_GET_PIPE_SIZE, ext_isp_size)
#define ISP_SET_PRE_BLK_NUM             _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_SET_PRE_BLK_NUM, td_u8)
#define ISP_SET_CHN_SELECT              _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_SET_CHN_SELECT, td_u32)
#define ISP_SET_AWB_EN                  _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_SET_AWB_EN, td_bool)
#define ISP_GET_PWM_NUM                 _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_GET_PWM_NUM, td_u32)
#define ISP_GET_HDR_ATTR                _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_GET_HDR_ATTR, ext_viu_pipe_hdr_attr)
#define ISP_GET_WDR_ATTR                _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_GET_WDR_ATTR, ext_viu_pipe_wdr_attr)
#define ISP_INIT_WORK_MODE              _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_INIT_WORK_MODE, isp_block_attr)
#define ISP_EXIT_WORK_MODE              _IO(IOC_TYPE_ISP,   IOC_NR_ISP_EXIT_WORK_MODE)
#define ISP_GET_WORK_MODE               _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_GET_WORK_MODE, isp_working_mode)
#define ISP_SET_CTRL_PARAM              _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_SET_CTRL_PARAM, ext_isp_ctrl_param)
#define ISP_GET_CTRL_PARAM              _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_GET_CTRL_PARAM, ext_isp_ctrl_param)

#define ISP_VD_GET_FRAME_EDGE           _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_VD_GET_FRAME_EDGE, td_u32)

#define ISP_TI_INIT_BUF                 _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_TI_INIT_BUF, isp_trans_info_buf)
#define ISP_TI_EXIT_BUF                 _IO(IOC_TYPE_ISP,   IOC_NR_ISP_TI_EXIT_BUF)
#define ISP_TI_SET_PUB_INFO             _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_TI_SET_PUB_INFO, ext_isp_pub_attr)
#define ISP_TI_SET_FRAME_INFO           _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_TI_SET_FRAME_INFO, ext_isp_frame_alg_info)
#define ISP_TI_GET_FRAME_INFO           _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_TI_GET_FRAME_INFO, ext_isp_frame_alg_info)
#define ISP_TI_SET_DCF_UPDATE_INFO      _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_TI_SET_DCF_UPDATE_INFO, ext_isp_dcf_update_info)

#define ISP_STAT_INIT_BUF               _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_STAT_INIT_BUF, ext_isp_mem_info)
#define ISP_STAT_EXIT_BUF               _IO(IOC_TYPE_ISP,   IOC_NR_ISP_STAT_EXIT_BUF)
#define ISP_STAT_GET_BUF                _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_STAT_GET_BUF, isp_stat_info)
#define ISP_STAT_PUT_BUF                _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_STAT_PUT_BUF, isp_stat_info)
#define ISP_STAT_GET_ACT_BUF            _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_STAT_GET_ACT_BUF, isp_stat_info)

#define ISP_BE_BUF_GET_NUM              _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_BE_BUF_GET_NUM, td_u8)
#define ISP_BE_BUF_INIT                 _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_BE_BUF_INIT, ext_isp_mem_info)
#define ISP_BE_BUF_EXIT                 _IO(IOC_TYPE_ISP,   IOC_NR_ISP_BE_BUF_EXIT)
#define ISP_BE_BUF_SETUP                _IO(IOC_TYPE_ISP,   IOC_NR_ISP_BE_BUF_SETUP)
#define ISP_BE_BUF_INIT_SYNC_PARA       _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_BE_BUF_INIT_SYNC_PARA, isp_be_sync_para)
#define ISP_BE_BUF_GET_FIRST            _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_BE_BUF_GET_FIRST, td_u64)
#define ISP_BE_BUF_GET_LAST             _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_BE_BUF_GET_LAST, td_u64)
#define ISP_BE_BUF_GET_FREE             _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_BE_BUF_GET_FREE, isp_buf_rec)
#define ISP_BE_BUF_SET_BUSY             _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_BE_BUF_SET_BUSY, isp_buf_rec)

#define ISP_CLUT_INIT_BUF               _IO(IOC_TYPE_ISP,   IOC_NR_ISP_CLUT_INIT_BUF)
#define ISP_CLUT_EXIT_BUF               _IO(IOC_TYPE_ISP,   IOC_NR_ISP_CLUT_EXIT_BUF)
#define ISP_CLUT_GET_BUF                _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_CLUT_GET_BUF, ext_isp_mem_info)
#define ISP_LDCI_INIT_BUF               _IO(IOC_TYPE_ISP,   IOC_NR_ISP_LDCI_INIT_BUF)
#define ISP_LDCI_EXIT_BUF               _IO(IOC_TYPE_ISP,   IOC_NR_ISP_LDCI_EXIT_BUF)
#define ISP_LDCI_GET_READ_STT_BUF       _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_LDCI_GET_READ_STT_BUF, isp_ldci_read_stt_buf)
#define ISP_STT_INIT_BUF                _IO(IOC_TYPE_ISP,   IOC_NR_ISP_STT_INIT_BUF)
#define ISP_STT_EXIT_BUF                _IO(IOC_TYPE_ISP,   IOC_NR_ISP_STT_EXIT_BUF)
#define ISP_STT_SET_FE_INIT_ADDR        _IO(IOC_TYPE_ISP,   IOC_NR_ISP_STT_SET_FE_INIT_ADDR)
#define ISP_STT_GET_BE_LUT_BUF          _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_STT_GET_BE_LUT_BUF, ext_isp_mem_info)

#define ISP_SNAP_SET_CONFIG             _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_SNAP_SET_CONFIG, ext_isp_config_info)
#define ISP_SNAP_GET_INFO               _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_SNAP_GET_INFO, ext_isp_snap_info)
#define ISP_SNAP_GET_ATTR               _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_SNAP_GET_ATTR, isp_snap_attr)
#define ISP_SNAP_PRO_INIT_BUF           _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_SNAP_PRO_INIT_BUF, isp_pro_info_buf)
#define ISP_SNAP_PRO_EXIT_BUF           _IO(IOC_TYPE_ISP,   IOC_NR_ISP_SNAP_PRO_EXIT_BUF)
#define ISP_SNAP_PRO_GET_ENABLE         _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_SNAP_PRO_GET_ENABLE, td_bool)
#define ISP_SNAP_PRO_SET_CALC_DONE      _IO(IOC_TYPE_ISP,   IOC_NR_ISP_SNAP_PRO_SET_CALC_DONE)

#define ISP_PROC_INIT                   _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_PROC_INIT, isp_proc_mem)
#define ISP_PROC_EXIT                   _IO(IOC_TYPE_ISP,   IOC_NR_ISP_PROC_EXIT)
#define ISP_PROC_WRITE_START_LOCK       _IO(IOC_TYPE_ISP,   IOC_NR_ISP_PROC_WRITE_START_LOCK)
#define ISP_PROC_WRITE_DONE_UNLOCK      _IO(IOC_TYPE_ISP,   IOC_NR_ISP_PROC_WRITE_DONE_UNLOCK)
#define ISP_PROC_GET_PARAM              _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_PROC_GET_PARAM, td_u32)

#define ISP_VREG_SET_FD                 _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_VREG_SET_FD, td_u32)
#define ISP_VREG_INIT                   _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_VREG_INIT, drv_vreg_args)
#define ISP_VREG_EXIT                   _IOW(IOC_TYPE_ISP,  IOC_NR_ISP_VREG_EXIT, drv_vreg_args)
#define ISP_VREG_CLEAN_UP               _IOR(IOC_TYPE_ISP,  IOC_NR_ISP_VREG_CLEAN_UP, td_u32)
#define ISP_VREG_GET_ADDR               _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_VREG_GET_ADDR, drv_vreg_args)

#define ISP_GET_REPEAT_TIMES            _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_GET_REPEAT_MODE, td_u32)
#define ISP_GET_FPN_ATTR                _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_GET_FPN_ATTR, ext_viu_fpn_attr)
#define ISP_SET_FPN_ATTR                _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_SET_FPN_ATTR, ext_viu_fpn_attr)
#define ISP_GET_LUMA_DATA               _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_GET_LUMA_DATA, isp_luma_data)

#define ISP_GET_ALG_SINGLE_TASK         _IOR(IOC_TYPE_ISP, IOC_NR_ISP_GET_SINGLE_TASK, isp_alg_task)
#define ISP_SET_ALG_SINGLE_RESULT       _IOW(IOC_TYPE_ISP, IOC_NR_ISP_SET_SINGLE_RESULT, isp_alg_task_result)
#define ISP_THREAD_FORCE_WAKE_UP        _IO(IOC_TYPE_ISP, IOC_NR_ISP_THREAD_FORCE_WAKE_UP)
#define ISP_PQ_INIT                     _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_PQ_INIT, isp_pq_init_param)
#define ISP_PQ_DEINIT                   _IO(IOC_TYPE_ISP,   IOC_NR_ISP_PQ_DEINIT)

#define ISP_KERNEL_RUNONCE              _IO(IOC_TYPE_ISP, IOC_NR_ISP_KERNEL_RUNONCE)
#define ISP_RUNONCE_INFO                _IOW(IOC_TYPE_ISP, IOC_NR_ISP_RUNONCE_INFO, td_bool)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
