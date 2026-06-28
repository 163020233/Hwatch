/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2006-2024. All rights reserved.
 * Description: vfmw config
 * Author: AuthorNameMagicTag
 * Create: 2006-01-01
 */

#ifndef VFMW_CONFIG_H
#define VFMW_CONFIG_H

#include "memory_config.h"

/* product memory config */
// vfmw manage psram memory pool
#define VFMW_MANAGE_MEM_SUPPORT
#define VFMW_CHECK_MEM_SUPPORT

#ifdef VFMW_OVER_SPEC_SUPPORT
#define VFMW_MEM_ADDR    (DDR_VCODEC_MEM_POOL_ADDR)
#define VFMW_EXTERN_LEN  (DDR_MCP_MEM_POOL_SIZE)
#define VFMW_MEM_LEN     (DDR_VCODEC_MEM_POOL_SIZE + VFMW_EXTERN_LEN)
#elif defined VFMW_MANAGE_MEM_SUPPORT
#define VENC_MEM_LEN     (0x1E00000) /* 30M */
#define VFMW_MEM_ADDR    (DDR_VCODEC_MEM_POOL_ADDR + VENC_MEM_LEN)
#define VFMW_MEM_LEN     (DDR_VCODEC_MEM_POOL_SIZE - VENC_MEM_LEN)
#else
#define VFMW_MEM_ADDR    (0x0)
#define VFMW_MEM_LEN     (0x0)
#endif

/* vfmw config */
#define VFMW_MAX_CHAN_NUM      3
#define VFMW_H264_SUPPORT
#define VFMW_HEVC_SUPPORT
#define H264_B_FRAME_SUPPORT
#define HEVC_B_FRAME_SUPPORT
#ifdef VFMW_OVER_SPEC_SUPPORT
#define H264_MAX_PIX_WIDTH     3840
#define H264_MAX_PIX_HEIGHT    2160
#define H264_MIN_PIX_WIDTH     80
#define H264_MIN_PIX_HEIGHT    96
#define H264_MAX_REF_NUM       16
#define H264_MAX_SLICE_NUM     1024
#define H264_FIELD_SUPPORT
#define H264_RESOLUTION_CHANGE_SUPPORT

#define HEVC_MAX_PIX_WIDTH     4128
#define HEVC_MAX_PIX_HEIGHT    4128
#define HEVC_MIN_PIX_WIDTH     114
#define HEVC_MIN_PIX_HEIGHT    114
#define HEVC_MAX_BIT_DEPTH     10
#define HEVC_MAX_NUM_REF_PICS  16
#define HEVC_MAX_SLICE_NUM     1024
#define HEVC_RESOLUTION_CHANGE_SUPPORT

#define MAX_RAW_NUM            1024
#define MAX_SEG_NUM            1024
#else
#define H264_MAX_PIX_WIDTH     1280
#define H264_MAX_PIX_HEIGHT    1440 /* 1440: 720x2 support spliced stream:sp420+A8 */
#define H264_MIN_PIX_WIDTH     114
#define H264_MIN_PIX_HEIGHT    114
#define H264_MAX_SLICE_NUM     100
#define H264_MAX_REF_NUM       2

#define HEVC_MAX_PIX_WIDTH     4096
#define HEVC_MAX_PIX_HEIGHT    4096
#define HEVC_MIN_PIX_WIDTH     114
#define HEVC_MIN_PIX_HEIGHT    114
#define HEVC_MAX_BIT_DEPTH     8
#define HEVC_MAX_NUM_REF_PICS  2
#define HEVC_MAX_SLICE_NUM     100

#define MAX_RAW_NUM            400
#define MAX_SEG_NUM            400
#endif
#define MJPEG_MAX_PIX_WIDTH    600
#define MJPEG_MAX_PIX_HEIGHT   600

#define VFMW_VPP_SUPPORT

#define SCD_TIME_OUT           4000
#define VDH_TIME_OUT           10000
#define MAX_DPS_NUM            2
#define VFMW_FSP_MAX_FRAME_NUM 32

#define VDH_DEC_SUPPORT
#ifndef VDEC_NO_DEBUG_LOG_SUPPORT
#define VFMW_DPRINT_SUPPORT
#endif
#define VFMW_STREAM_SUPPORT
#define VFMW_LOCAL_SUPPORT
#define VFMW_USER_SUPPORT

// scd
#define RAW_MEM_LESS
#define IP_UNID_SUPPORT

// fpga
#define VCODEC_FPGA_SUPPORT

#endif
