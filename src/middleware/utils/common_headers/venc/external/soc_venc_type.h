/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: common venc
 * Author: Hisilicon multimedia software group
 * Create: 2025/03/15
 * History:
 */
#ifndef SOC_VENC_TYPE_H
#define SOC_VENC_TYPE_H

#include "td_type.h"
#include "td_base.h"
#include "soc_media_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup venc_type venc_type
 * @ingroup  video_codec_api
 * @{
 */

/*------------------------------------------------------ macro define ------------------------------------------------------*/

#define EXT_VENC_INVALID_HANDLE       (0xFFFFFFFF)                  /*!< @if Eng Venc invalid handle.
                                                                         @else   编码无效句柄 @endif */
#define EXT_VENC_BLOCKING_STATE       (-1)                          /*!< @if Eng Block state.
                                                                         @else   阻塞状态 @endif */
#define EXT_VENC_MAX_STREAM_PACK_NUM   2                            /*!< @if Eng Maximum stream pack number.
                                                                         @else   最大码流包数 @endif */
#define EXT_VENC_MAX_USERDATA_LEN      1024                         /*!< @if Eng Maximum userdata length.
                                                                         @else   最大用户数据长度 @endif */
#define EXT_VENC_MAX_ROI_NUM           8                            /*!< @if Eng Maximum ROI number.
                                                                         @else   最大感兴趣区域个数 @endif */
#define EXT_VENC_MAX_OSD_NODE_NUM      30                           /*!< @if Eng Maximum OSD node number.
                                                                         @else   最大OSD节点个数 @endif */
#define EXT_VENC_MAX_OSD_LAYER_NUM     8                            /*!< @if Eng Maximum OSD layer number.
                                                                         @else   最大OSD层数 @endif */
#define EXT_VENC_OSD_ID_ALL           (-1)                          /*!< @if Eng all OSD specific id .
                                                                         @else   全部OSD专用ID @endif */
#define EXT_VENC_MAX_ROI_LEVEL         15                           /*!< @if Eng Maximum ROI level.
                                                                         @else   最大ROI阶数 @endif */
#define EXT_VENC_MAX_ECS_SIZE          65535                        /*!< @if Eng Maximum entropy-coded segment size.
                                                                         @else   最大熵编码段大小 @endif */
#define EXT_VENC_MAX_ECS_NUM           32                           /*!< @if Eng Maximum entropy-coded segment number.
                                                                         @else   最大熵编码段个数 @endif */
#define EXT_VENC_MAX_SLICE_NUM         32                           /*!< @if Eng Maximum slice number.
                                                                         @else   最大切片个数 @endif */
#define EXT_VENC_QT_COEF_NUM           64                           /*!< @if Eng quantization table coefficient number.
                                                                         @else   量化表系数个数 @endif */
#define EXT_VENC_MAX_QFACTOR           99                           /*!< @if Eng Maximum quantization factor.
                                                                         @else   最大量化因子 @endif */
#define EXT_VENC_MIN_QFACTOR           1                            /*!< @if Eng Minimum quantization factor.
                                                                         @else   最小量化因子 @endif */
#define EXT_VENC_MAX_QP                51                           /*!< @if Eng Maximum quantization parameter.
                                                                         @else   最大量化参数 @endif */
#define EXT_VENC_MIN_QP                0                            /*!< @if Eng Minimum quantization parameter.
                                                                         @else   最小量化参数 @endif */
#define EXT_VENC_MAX_QP_DELTA          30                           /*!< @if Eng Maximum quantization parameter delta.
                                                                         @else   最大量化参数调节量 @endif */
#define EXT_VENC_MIN_QP_DELTA         (-10)                         /*!< @if Eng Minimum quantization parameter delta.
                                                                         @else   最小量化参数调节量 @endif */
#define EXT_VENC_MAX_FRAME_QP_DELTA    4                            /*!< @if Eng Maximum frame quantization parameter delta.
                                                                         @else   最大帧级量化参数调节量 @endif */
#define EXT_VENC_MAX_ROW_QP_DELTA      10                           /*!< @if Eng Maximum row quantization parameter delta.
                                                                         @else   最大行级量化参数调节量 @endif */
#define EXT_VENC_MAX_FRAME_RATE        240                          /*!< @if Eng Maximum frame rate.
                                                                         @else   最大帧率 @endif */
#define EXT_VENC_MIN_FRAME_RATE        1                            /*!< @if Eng Minimum frame rate.
                                                                         @else   最小帧率 @endif */
#define EXT_VENC_MAX_BITRATE           163840                       /*!< @if Eng Maximum bit rate.
                                                                         @else   最大比特率 @endif */
#define EXT_VENC_MIN_BITRATE           2                            /*!< @if Eng Minimum bit rate.
                                                                         @else   最小比特率 @endif */
#define EXT_VENC_MAX_STATS_TIME        60                           /*!< @if Eng Maximum bitrate statistics time,in seconds.
                                                                         @else   最大码率统计时间，单位秒 @endif */
#define EXT_VENC_MIN_STATS_TIME        1                            /*!< @if Eng Minimum bitrate statistics time,in seconds.
                                                                         @else   最小码率统计时间，单位秒 @endif */
#define EXT_VENC_MAX_IP_PROPORTION     100                          /*!< @if Eng Maximum proportion of I frame and P frame.
                                                                         @else   IP帧码率差最大比例 @endif */
#define EXT_VENC_MIN_IP_PROPORTION     1                            /*!< @if Eng Minimum proportion of I frame and P frame.
                                                                         @else   IP帧码率差最小比例 @endif */
#define EXT_VENC_TEXTURE_THRESHOLD_NUM 16                           /*!< @if Eng Texture threshold number.
                                                                         @else   纹理阈值个数 @endif */
#define EXT_VENC_MAX_TEXTURE_THRESHOLD 255                          /*!< @if Eng Maximum texture threshold number.
                                                                         @else   最大纹理阈值 @endif */
#define EXT_VENC_MAX_REENCODE_TIME     3                            /*!< @if Eng Maximum reencode time.
                                                                         @else   最大重新编码次数 @endif */
#define EXT_VENC_MAX_GOP               65536                        /*!< @if Eng Maximum group of pictures.
                                                                         @else   最大图像组 @endif */
#define EXT_VENC_MIN_GOP               1                            /*!< @if Eng Minimum group of pictures.
                                                                         @else   最小图像组 @endif */
#define EXT_VENC_MAX_BG_INTERVAL       EXT_VENC_MAX_GOP             /*!< @if Eng Maximum background frame interval.
                                                                         @else   最大背景帧间隔 @endif */
#define EXT_VENC_MAX_ENHANCE           255                          /*!< @if Eng Maximum enhance layer frame num.
                                                                         @else   最大增强层帧数 @endif */
#define EXT_VENC_MAX_CHG_RATIO         100                          /*!< @if Eng Maximum change ratio.
                                                                         @else   最大跳变比例点 @endif */
#define EXT_VENC_MIN_CHG_RATIO         50                           /*!< @if Eng Minimum change ratio.
                                                                         @else   最小跳变比例点 @endif */
#define EXT_VENC_MAX_LOST_TIMES        1000                         /*!< @if Eng Maximum lost times.
                                                                         @else   最大丢帧次数 @endif */

/*----------------------------------------------------- struct define ------------------------------------------------------*/

typedef enum {
    EXT_VENC_SCENE_PHOTO = 0,                                       /*!< @if Eng Photo scene.
                                                                         @else   拍照场景 @endif */
    EXT_VENC_SCENE_VIDEO,                                           /*!< @if Eng Video live scene.
                                                                         @else   视频实时场景 @endif */
    EXT_VENC_SCENE_RECORD,                                          /*!< @if Eng Video record scene.
                                                                         @else   视频录像场景 @endif */
    EXT_VENC_SCENE_MAX                                              /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_scene_type;

typedef enum {
    EXT_VENC_H264_PROFILE_BP = 0,                                   /*!< @if Eng Baseline profile.
                                                                         @else   基准档次 @endif */
    EXT_VENC_H264_PROFILE_MP,                                       /*!< @if Eng Main profile.
                                                                         @else   主要档次 @endif */
    EXT_VENC_H264_PROFILE_HP,                                       /*!< @if Eng High profile.
                                                                         @else   高级档次 @endif */
    EXT_VENC_H264_PROFILE_MAX                                       /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_h264_profile_type;

typedef enum {
    EXT_VENC_H265_PROFILE_MP = 0,                                   /*!< @if Eng Main profile.
                                                                         @else   主要档次 @endif */
    EXT_VENC_H265_PROFILE_MAX                                       /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_h265_profile_type;

typedef enum {
    EXT_VENC_JPEG_PROFILE_BP = 0,                                   /*!< @if Eng Baseline profile.
                                                                         @else   基准档次 @endif */
    EXT_VENC_JPEG_PROFILE_MAX                                       /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_jpeg_profile_type;

typedef enum {
    EXT_VENC_LOW_POWER_BASIC = 0,                                   /*!< @if Eng Basic low power.
                                                                         @else   基础低功耗模式 @endif */
    EXT_VENC_LOW_POWER_ULTRA,                                       /*!< @if Eng Ultra low power.
                                                                         @else   超级低功耗模式 @endif */
    EXT_VENC_LOW_POWER_OFF,                                         /*!< @if Eng low power off.
                                                                         @else   低功耗关闭模式 @endif */
    EXT_VENC_LOW_POWER_MAX                                          /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_low_power_mode;

typedef enum {
    EXT_VENC_H264_NALU_B_SLICE   = 0,                               /*!< @if Eng Bi-directional predictive slice.
                                                                         @else   双向预测切片 @endif */
    EXT_VENC_H264_NALU_P_SLICE   = 1,                               /*!< @if Eng Predictive slice.
                                                                         @else   单向预测切片 @endif */
    EXT_VENC_H264_NALU_I_SLICE   = 2,                               /*!< @if Eng Intra-coded slice.
                                                                         @else   帧内编码切片 @endif */
    EXT_VENC_H264_NALU_IDR_SLICE = 5,                               /*!< @if Eng Instantaneous decoding refresh slice.
                                                                         @else   即时解码刷新切片 @endif */
    EXT_VENC_H264_NALU_SEI       = 6,                               /*!< @if Eng Supplemental enhancement information.
                                                                         @else   补充增强信息 @endif */
    EXT_VENC_H264_NALU_SPS       = 7,                               /*!< @if Eng Sequence parameter set.
                                                                         @else   序列参数集 @endif */
    EXT_VENC_H264_NALU_PPS       = 8,                               /*!< @if Eng Picture parameter set.
                                                                         @else   图像参数集 @endif */
    EXT_VENC_H264_NALU_MAX                                          /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_h264_nalu_type;

typedef enum {
    EXT_VENC_H265_NALU_B_SLICE   = 0,                               /*!< @if Eng Bi-directional predictive slice.
                                                                         @else   双向预测切片 @endif */
    EXT_VENC_H265_NALU_P_SLICE   = 1,                               /*!< @if Eng Predictive slice.
                                                                         @else   单向预测切片 @endif */
    EXT_VENC_H265_NALU_I_SLICE   = 2,                               /*!< @if Eng Intra-coded slice.
                                                                         @else   帧内编码切片 @endif */
    EXT_VENC_H265_NALU_IDR_SLICE = 19,                              /*!< @if Eng Instantaneous decoding refresh slice.
                                                                         @else   即时解码刷新切片 @endif */
    EXT_VENC_H265_NALU_VPS       = 32,                              /*!< @if Eng Video parameter set .
                                                                         @else   视频参数集 @endif */
    EXT_VENC_H265_NALU_SPS       = 33,                              /*!< @if Eng Sequence parameter set.
                                                                         @else   序列参数集 @endif */
    EXT_VENC_H265_NALU_PPS       = 34,                              /*!< @if Eng Picture parameter set .
                                                                         @else   图像参数集 @endif */
    EXT_VENC_H265_NALU_SEI       = 39,                              /*!< @if Eng Supplemental enhancement information.
                                                                         @else   补充增强信息 @endif */
    EXT_VENC_H265_NALU_MAX                                          /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_h265_nalu_type;

typedef enum {
    EXT_VENC_JPEG_PACK_ECS = 5,                                     /*!< @if Eng Pack of Entropy-Coded segment.
                                                                         @else   熵编码段包 @endif */
    EXT_VENC_JPEG_PACK_APP = 6,                                     /*!< @if Eng Pack of application.
                                                                         @else   应用程序标记段包 @endif */
    EXT_VENC_JPEG_PACK_VDO = 7,                                     /*!< @if Eng Pack of video.
                                                                         @else   视频包 @endif */
    EXT_VENC_JPEG_PACK_PIC = 8,                                     /*!< @if Eng Pack of picture.
                                                                         @else   图像包 @endif */
    EXT_VENC_JPEG_PACK_THM = 9,                                     /*!< @if Eng THuMbnail.
                                                                         @else   缩略图包 @endif */
    EXT_VENC_JPEG_PACK_THM_PIC = 10,                                /*!< @if Eng THuMbnail and picture.
                                                                         @else   缩略图与图像包 @endif */
    EXT_VENC_JPEG_PACK_MAX                                          /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_jpeg_pack_type;

typedef enum {
    EXT_VENC_GOP_MODE_I = 0,                                        /*!< @if Eng Gop type of I frame only.
                                                                         @else   仅I帧的GOP类型 @endif */
    EXT_VENC_GOP_MODE_NORMAL_P,                                     /*!< @if Eng Gop type of single reference P frame .
                                                                         @else   单参考P帧的GOP类型 @endif */
    EXT_VENC_GOP_MODE_SMART_P,                                      /*!< @if Eng GOP Type of smart reference P frame.
                                                                         @else   智能参考P帧的GOP类型 @endif */
    EXT_VENC_GOP_MODE_MAX                                           /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_gop_mode;

typedef enum {
    EXT_VENC_RC_MODE_FIXQP = 0,                                     /*!< @if Eng Fixed quantization parameter.
                                                                         @else   固定qp模式 @endif */
    EXT_VENC_RC_MODE_RANGEQP,                                       /*!< @if Eng Range quantization parameter.
                                                                         @else   范围qp模式 @endif */
    EXT_VENC_RC_MODE_CBR,                                           /*!< @if Eng Constant bit rate.
                                                                         @else   固定比特率模式 @endif */
    EXT_VENC_RC_MODE_VBR,                                           /*!< @if Eng Variable bit rate.
                                                                         @else   可变比特率模式 @endif */
    EXT_VENC_RC_MODE_AVBR,                                          /*!< @if Eng Adaptive variable bit rate.
                                                                         @else   自适应可变比特率模式 @endif */
    EXT_VENC_RC_MODE_MAX                                            /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_rc_mode;

typedef enum {
    EXT_VENC_BIT_RATE_OS_NONE = 0,                                  /*!< @if Eng Don't handle bit rate overshoot.
                                                                         @else   不处理码率过冲 @endif */
    EXT_VENC_BIT_RATE_OS_DISCARD,                                   /*!< @if Eng Discard the overshoot frame.
                                                                         @else   丢弃过冲帧 @endif */
    EXT_VENC_BIT_RATE_OS_P_SKIP,                                    /*!< @if Eng Reencode the overshoot frame with p_skip.
                                                                         @else   使用P_SKIP帧重编过冲帧 @endif */
    EXT_VENC_BIT_RATE_OS_MAX                                        /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_bit_rate_overshoot_mode;

typedef enum {
    EXT_VENC_REENCODE_BIT_RATE_FIRST = 0,                           /*!< @if Eng Target bit rate first.
                                                                         @else   目标码率优先 @endif */
    EXT_VENC_REENCODE_FRAME_BITS_FIRST,                             /*!< @if Eng Large frame threshold first.
                                                                         @else   超大帧阈值优先 @endif */
    EXT_VENC_REENCODE_MAX                                           /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_reencode_priority;

typedef enum {
    EXT_VENC_FRAME_BITS_OS_NONE = 0,                                /*!< @if Eng Don't handle frame bits overshoot.
                                                                         @else   不处理单帧比特过冲 @endif */
    EXT_VENC_FRAME_BITS_OS_DISCARD,                                 /*!< @if Eng Discard the overshoot frame.
                                                                         @else   丢弃过冲帧 @endif */
    EXT_VENC_FRAME_BITS_OS_REENCODE,                                /*!< @if Eng Reencode the overshoot frame.
                                                                         @else   重编过冲帧 @endif */
    EXT_VENC_FRAME_BITS_OS_MAX                                      /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_frame_bits_overshoot_mode;

typedef enum {
    EXT_VENC_ROI_FOR_I = 0,                                         /*!< @if Eng ROI applied to I frames.
                                                                         @else   应用于I帧的ROI @endif */
    EXT_VENC_ROI_FOR_P_B,                                           /*!< @if Eng ROI applied to P/B frame.
                                                                         @else   应用于P/B帧的ROI @endif */
    EXT_VENC_ROI_FOR_VI,                                            /*!< @if Eng ROI applied to VI (Virtual I) frames.
                                                                         @else   应用于VI(虚拟I)帧的ROI @endif */
    EXT_VENC_ROI_FOR_MAX                                            /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_roi_for_frame;

typedef enum {
    EXT_VENC_INTRA_REFRESH_ROW = 0,                                 /*!< @if Eng Intra refresh by row.
                                                                         @else   按行帧内刷新 @endif */
    EXT_VENC_INTRA_REFRESH_MAX                                      /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_intra_refresh_mode;

typedef enum {
    EXT_VENC_USD_PRIVATE  = 0,                                      /*!< @if Eng private userdata.
                                                                         @else   私有用户数据类型 @endif */
    EXT_VENC_USD_MAX                                                /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_usd_type;

typedef enum {
    EXT_VENC_STD_JPEG = 0,                                          /*!< @if Eng JPEG standard.
                                                                         @else   JPEG协议 @endif */
    EXT_VENC_STD_MJPEG,                                             /*!< @if Eng MJPEG standard.
                                                                         @else   MJPEG协议 @endif */
    EXT_VENC_STD_H264,                                              /*!< @if Eng H264 standard.
                                                                         @else   H264协议 @endif */
    EXT_VENC_STD_H265,                                              /*!< @if Eng H265 standard.
                                                                         @else   H265协议 @endif */
    EXT_VENC_STD_HEIF,                                              /*!< @if Eng HEIF standard.
                                                                         @else   HEIF协议 @endif */
    EXT_VENC_STD_MAX                                                /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_standard;

typedef enum {
    EXT_VENC_WORK_USER = 0,                                         /*!< @if Eng User work mode.
                                                                         @else   用户送帧工作模式 @endif */
    EXT_VENC_WORK_AUTO,                                             /*!< @if Eng Auto work mode.
                                                                         @else   自动送帧工作模式 @endif */
    EXT_VENC_WORK_MAX                                               /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_work_mode;

typedef enum {
    EXT_VENC_ORI_NORMAL = 0,                                        /*!< @if Eng Picture orientation normal.
                                                                         @else   图像无旋转 @endif */
    EXT_VENC_ORI_FLIP_HOR,                                          /*!< @if Eng Picture flipped horizontally.
                                                                         @else   图像水平翻转 @endif */
    EXT_VENC_ORI_ROTATE_180,                                        /*!< @if Eng Picture rotated 180 degrees.
                                                                         @else   图像旋转180度 @endif */
    EXT_VENC_ORI_FLIP_VER,                                          /*!< @if Eng Picture flipped vertically.
                                                                         @else   图像垂直翻转 @endif */
    EXT_VENC_ORI_FLIP_HOR_ROTATE_270,                               /*!< @if Eng Picture flipped horizontally and rotated 270 degrees.
                                                                         @else   图像水平翻转并旋转270度 @endif */
    EXT_VENC_ORI_ROTATE_90,                                         /*!< @if Eng Picture rotated 90 degrees.
                                                                         @else   图像旋转90度 @endif */
    EXT_VENC_ORI_FLIP_HOR_ROTATE_90,                                /*!< @if Eng Picture flipped horizontally and rotated 90 degrees.
                                                                         @else   图像水平翻转并旋转90度 @endif */
    EXT_VENC_ORI_ROTATE_270,                                        /*!< @if Eng Picture rotated 270 degrees.
                                                                         @else   图像旋转270度 @endif */
    EXT_VENC_ORI_MAX                                                /*!< @if Eng Invalid value.
                                                                         @else   无效值 @endif */
} ext_venc_orientation;

typedef union {
    ext_venc_jpeg_pack_type  jpeg_type;                             /*!< @if Eng Jpeg pack type.
                                                                         @else   JPEG数据类型 @endif */
    ext_venc_h264_nalu_type  h264_type;                             /*!< @if Eng H264 nalu type.
                                                                         @else   H264数据类型 @endif */
    ext_venc_h265_nalu_type  h265_type;                             /*!< @if Eng H265 nalu type.
                                                                         @else   H265数据类型 @endif */
} ext_venc_data_type;

typedef struct {
    ext_venc_scene_type scene;                                      /*!< @if Eng Scene.
                                                                         @else   场景 @endif */
    ext_venc_standard standard;                                     /*!< @if Eng Standard.
                                                                         @else   协议 @endif */
    td_u32 pic_width;                                               /*!< @if Eng Picture width.
                                                                         @else   图像宽度 @endif */
    td_u32 pic_height;                                              /*!< @if Eng Picture height.
                                                                         @else   图像高度 @endif */
    td_u32 src_frame_rate;                                          /*!< @if Eng Source frame rate.
                                                                         @else   源帧率 @endif */
    td_u32 dst_frame_rate;                                          /*!< @if Eng Destination frame rate.
                                                                         @else   目标帧率 @endif */
} ext_venc_scene_info;

typedef struct {
    ext_venc_standard standard;                                     /*!< @if Eng Standard.
                                                                         @else   协议 @endif */
    td_u32 pic_width;                                               /*!< @if Eng Picture width.
                                                                         @else   图像宽度 @endif */
    td_u32 pic_height;                                              /*!< @if Eng Picture height.
                                                                         @else   图像高度 @endif */
    td_u64 pts;                                                     /*!< @if Eng Pts.
                                                                         @else   时间戳 @endif */
    td_u32 pic_index;                                               /*!< @if Eng Picture index.
                                                                         @else   图像索引 @endif */
    td_bool is_frame_end;                                           /*!< @if Eng End of frame.
                                                                         @else   帧结束 @endif */
    ext_venc_data_type data_type;                                   /*!< @if Eng Data type.
                                                                         @else   数据类型 @endif */
    td_u64 phy_addr[EXT_VENC_MAX_STREAM_PACK_NUM];                  /*!< @if Eng Stream pack physical address.
                                                                         @else   码流包物理地址 @endif */
    td_u64 vir_addr[EXT_VENC_MAX_STREAM_PACK_NUM];                  /*!< @if Eng Stream pack virtual address.
                                                                         @else   码流包虚拟地址 @endif */
    td_u32 len[EXT_VENC_MAX_STREAM_PACK_NUM];                       /*!< @if Eng Stream pack length.
                                                                         @else   码流包长度 @endif */
    td_u64 stream_private;                                          /*!< @if Eng Stream private.
                                                                         @else   码流私有数据 @endif */
} ext_venc_stream;

typedef struct {
    td_bool exif_en;                                                /*!< @if Eng Enable exchangeable image file format.
                                                                         @else   使能图像EXIF信息 @endif */
    ext_venc_orientation orientation;                               /*!< @if Eng Picture orientation.
                                                                         @else   图像方向 @endif */
} ext_venc_exif;

typedef struct {
    // static attr
    ext_venc_scene_type scene;                                      /*!< @if Eng Scene.
                                                                         @else   场景 @endif */
    ext_venc_standard standard;                                     /*!< @if Eng Standard.
                                                                         @else   协议 @endif */
    td_u32 profile;                                                 /*!< @if Eng Profile.
                                                                         @else   档次 @endif */
    td_u32 buf_size;                                                /*!< @if Eng Buffer size.
                                                                         @else   码流缓冲大小 @endif */
    td_u32 max_userdata_len;                                        /*!< @if Eng Max user data length.
                                                                         @else   最大用户数据长度 @endif */
    td_u32 max_pic_width;                                           /*!< @if Eng Max picture width.
                                                                         @else   最大图像宽度 @endif */
    td_u32 max_pic_height;                                          /*!< @if Eng Max picture height.
                                                                         @else   最大图像高度 @endif */
    td_bool thumbnail_en;                                           /*!< @if Eng Enable thumbnail.
                                                                         @else   缩略图使能 @endif */
    td_bool ref_rcn_share;                                          /*!< @if Eng Share reference and reconstruction.
                                                                         @else   参考重构帧复用使能 @endif */
    td_u32  frm_buf_ratio;                                          /*!< @if Eng Frame buffer ratio.
                                                                         @else   复用使能帧存比例 @endif */
    // dynamic attr
    ext_venc_low_power_mode low_power_mode;                         /*!< @if Eng Low power mode.
                                                                         @else   低功耗模式 @endif */
    td_u32 pic_width;                                               /*!< @if Eng Picture width.
                                                                         @else   图像宽度 @endif */
    td_u32 pic_height;                                              /*!< @if Eng Picture height.
                                                                         @else   图像高度 @endif */
    td_u32 thumbnail_width;                                         /*!< @if Eng Thumbnail width.
                                                                         @else   缩略图宽度 @endif */
    td_u32 thumbnail_height;                                        /*!< @if Eng Thumbnail height.
                                                                         @else   缩略图高度 @endif */
    ext_venc_exif exif;                                             /*!< @if Eng Exif.
                                                                         @else   照片信息 @endif */
} ext_venc_chn_attr;

typedef struct {
    td_s32 ip_qp_delta;                                             /*!< @if Eng QP delta of I frame and P frame.
                                                                         @else   IP帧QP调节量 @endif */
} ext_venc_gop_normal_p;

typedef struct {
    td_s32 bg_qp_delta;                                             /*!< @if Eng QP delta for background long-term reference frame.
                                                                         @else   背景长期参考帧QP调节量 @endif */
    td_s32 vi_qp_delta;                                             /*!< @if Eng QP delta of virtual I frame.
                                                                         @else   虚拟I帧QP调节量 @endif */
    td_u32 bg_interval;                                             /*!< @if Eng Backgroud long-term reference frame interval.
                                                                                 Must be an integer multiple of gop.
                                                                         @else   背景长期参考帧的间隔 @endif */
} ext_venc_gop_smart_p;

typedef struct {
    ext_venc_gop_mode gop_mode;                                     /*!< @if Eng Gop mode.
                                                                         @else   GOP模式 @endif */
    union {
        ext_venc_gop_normal_p normal_p;                             /*!< @if Eng Single reference P GOP.
                                                                         @else   单参考P帧GOP @endif */
        ext_venc_gop_smart_p  smart_p;                              /*!< @if Eng Smart/Background reference P GOP.
                                                                         @else   智能/背景参考P帧GOP @endif */
    };
} ext_venc_gop_attr;

typedef struct {
    union {
        struct {
            td_u8 qfactor;                                          /*!< @if Eng Quality factor.
                                                                         @else   图像质量因子 @endif */
        }; // jpeg & mjpeg

        struct {
            td_u8 i_qp;                                             /*!< @if Eng QP value for I frame.
                                                                         @else   I帧QP值 @endif */
            td_u8 p_qp;                                             /*!< @if Eng QP value for P frame.
                                                                         @else   P帧QP值 @endif */
            td_u8 vi_qp;                                            /*!< @if Eng QP value for VI frame.
                                                                         @else   VI帧QP值 @endif */
        };
    };
} ext_venc_fixqp_attr;

typedef struct {
    td_u8 i_qp;                                                     /*!< @if Eng QP values for I frame.
                                                                         @else   I帧QP值 @endif */
    td_u8 p_qp;                                                     /*!< @if Eng QP value for P frame.
                                                                         @else   P帧QP值 @endif */
    td_u8 vi_qp;                                                    /*!< @if Eng QP value for VI frame.
                                                                         @else   VI帧QP值 @endif */
    td_u8 max_i_qp;                                                 /*!< @if Eng Maximum QP for I frame.
                                                                         @else   I帧最大QP @endif */
    td_u8 min_i_qp;                                                 /*!< @if Eng Minimum QP for I frame.
                                                                         @else   I帧最小QP @endif */
    td_u8 max_p_qp;                                                 /*!< @if Eng Maximum QP for P frame.
                                                                         @else   P帧最大QP @endif */
    td_u8 min_p_qp;                                                 /*!< @if Eng Minimum QP for P frame.
                                                                         @else   P帧最小QP @endif */
    td_u8 max_vi_qp;                                                /*!< @if Eng Maximum QP for VI frame.
                                                                         @else   VI帧最大QP @endif */
    td_u8 min_vi_qp;                                                /*!< @if Eng Minimum QP for VI frame.
                                                                         @else   VI帧最小QP @endif */
} ext_venc_rangeqp_attr;

typedef struct {
    td_u32 bit_rate;                                                /*!< @if Eng Bit rate.
                                                                         @else   比特率 @endif */
    td_u32 stats_time;                                              /*!< @if Eng Stats time.
                                                                         @else   统计时间窗 @endif */
} ext_venc_cbr_attr;

typedef struct {
    td_u32 max_bit_rate;                                            /*!< @if Eng Max bit rate.
                                                                         @else   最大比特率 @endif */
    td_u32 stats_time;                                              /*!< @if Eng Stats time.
                                                                         @else   统计时间窗 @endif */
} ext_venc_vbr_attr;

typedef struct {
    td_u32 max_bit_rate;                                            /*!< @if Eng Max bit rate.
                                                                         @else   最大比特率 @endif */
    td_u32 stats_time;                                              /*!< @if Eng Stats time.
                                                                         @else   统计时间窗 @endif */
} ext_venc_avbr_attr;

typedef struct {
    ext_venc_standard standard;                                     /*!< @if Eng Standard.
                                                                         @else   协议 @endif */
    td_u32 src_frame_rate;                                          /*!< @if Eng Source frame rate.
                                                                         @else   源帧率 @endif */
    td_u32 dst_frame_rate;                                          /*!< @if Eng Destination frame rate.
                                                                         @else   目标帧率 @endif */
    td_u32 gop;                                                     /*!< @if Eng Group of pictures.
                                                                         @else   图像组 @endif */
    ext_venc_rc_mode rc_mode;                                       /*!< @if Eng Ratio control mode.
                                                                         @else   码率控制模式 @endif */
    union {
        ext_venc_fixqp_attr fixqp_attr;                             /*!< @if Eng Fixed quantization parameter attribute.
                                                                         @else   固定量化参数属性 @endif */
        ext_venc_rangeqp_attr rangeqp_attr;                         /*!< @if Eng Range quantization parameter attribute.
                                                                         @else   范围量化参数属性 @endif */
        ext_venc_cbr_attr cbr_attr;                                 /*!< @if Eng Constant bit rate attribute.
                                                                         @else   固定比特率属性 @endif */
        ext_venc_vbr_attr vbr_attr;                                 /*!< @if Eng Variable bit rate.
                                                                         @else   可变比特率属性 @endif */
        ext_venc_avbr_attr avbr_attr;                               /*!< @if Eng Adaptive variable bit rate attribute.
                                                                         @else   自适应可变比特率属性 @endif */
    };
} ext_venc_rc_attr;

typedef struct {
    ext_venc_chn_attr chn_attr;                                     /*!< @if Eng Channel attribute.
                                                                         @else   通道属性 @endif */
    ext_venc_gop_attr gop_attr;                                     /*!< @if Eng Gop attribute.
                                                                         @else   图像组属性 @endif */
    ext_venc_rc_attr  rc_attr;                                      /*!< @if Eng Ratio control attribute.
                                                                         @else   码率控制属性 @endif */
} ext_venc_attr;

typedef struct {
    td_bool detect_scene_chg_en;                                    /*!< @if Eng Scene change detection switch.
                                                                         @else   场景切换检测开关 @endif */
    td_bool adapt_insert_idr_frame_en;                              /*!< @if Eng Enable adaptive insertion of IDR frame switch.
                                                                         @else   自适应插入IDR帧开关 @endif */
} ext_venc_scene_chg_detect;

typedef struct {
    td_u8 max_reencode_times;                                       /*!< @if Eng Maximum reencode times.
                                                                         @else   最大重编码次数 @endif */
    union {
        struct {
            td_u8  max_qfactor;                                     /*!< @if Eng Maximum quality factor.
                                                                         @else   最大图像质量因子 @endif */
            td_u8  min_qfactor;                                     /*!< @if Eng Minimum quality factor.
                                                                         @else   最小图像质量因子 @endif */
        }; // mjpeg

        struct {
            td_u8  max_i_qp;                                        /*!< @if Eng Maximum QP for I frame.
                                                                         @else   I帧最大QP @endif */
            td_u8  min_i_qp;                                        /*!< @if Eng Minimum QP for I frame.
                                                                         @else   I帧最小QP @endif */
            td_u8  max_p_qp;                                        /*!< @if Eng Maximum QP for P frame.
                                                                         @else   P帧最大QP @endif */
            td_u8  min_p_qp;                                        /*!< @if Eng Minimum QP for P frame.
                                                                         @else   P帧最小QP @endif */
            td_u8  max_vi_qp;                                       /*!< @if Eng Maximum QP for VI frame.
                                                                         @else   VI帧最大QP @endif */
            td_u8  min_vi_qp;                                       /*!< @if Eng Minimum QP for VI frame.
                                                                         @else   VI帧最小QP @endif */
            td_u8  max_i_proportion;                                /*!< @if Eng Ratio of Maximum IP frame bit rate.
                                                                         @else   最大IP帧码率比值 @endif */
            td_u8  min_i_proportion;                                /*!< @if Eng Ratio of Minimum IP frame bit rate.
                                                                         @else   最小IP帧码率比值 @endif */
        };
    };
} ext_venc_cbr_param;

typedef struct {
    td_u8  chg_ratio;                                               /*!< @if Eng Change ratio.
                                                                         @else   变化比例点 @endif */
    td_u8  max_reencode_times;                                      /*!< @if Eng Maximum reencode times.
                                                                         @else   最大重编码次数 @endif */
    union {
        struct {
            td_u8  max_qfactor;                                     /*!< @if Eng Maximum quality factor.
                                                                         @else   最大图像质量因子 @endif */
            td_u8  min_qfactor;                                     /*!< @if Eng Minimum quality factor.
                                                                         @else   最小图像质量因子 @endif */
        }; // mjpeg

        struct {
            td_u8  max_i_qp;                                        /*!< @if Eng Maximum QP for I frame.
                                                                         @else   I帧最大QP @endif */
            td_u8  min_i_qp;                                        /*!< @if Eng Minimum QP for I frame.
                                                                         @else   I帧最小QP @endif */
            td_u8  max_p_qp;                                        /*!< @if Eng Maximum QP for P frame.
                                                                         @else   P帧最大QP @endif */
            td_u8  min_p_qp;                                        /*!< @if Eng Minimum QP for P frame.
                                                                         @else   P帧最小QP @endif */
            td_u8  max_vi_qp;                                       /*!< @if Eng Maximum QP for VI frame.
                                                                         @else   VI帧最大QP @endif */
            td_u8  min_vi_qp;                                       /*!< @if Eng Minimum QP for VI frame.
                                                                         @else   VI帧最小QP @endif */
            td_u8  max_i_proportion;                                /*!< @if Eng Ratio of maximum IP frame bit rate.
                                                                         @else   最大IP帧码率比值 @endif */
            td_u8  min_i_proportion;                                /*!< @if Eng Ratio of Minimum IP frame bit rate.
                                                                         @else   最小IP帧码率比值 @endif */
        };
    };
} ext_venc_vbr_param;

typedef struct {
    td_u8  chg_ratio;                                               /*!< @if Eng Change ratio.
                                                                         @else   变化比例点 @endif */
    td_u8  max_reencode_times;                                      /*!< @if Eng Maximum reencode times.
                                                                         @else   最大重编码次数 @endif */
    td_u8  max_i_qp;                                                /*!< @if Eng Maximum QP for I frames.
                                                                         @else   I帧最大QP @endif */
    td_u8  min_i_qp;                                                /*!< @if Eng Minimum QP for I frames.
                                                                         @else   I帧最小QP @endif */
    td_u8  max_p_qp;                                                /*!< @if Eng Maximum QP for P frame.
                                                                         @else   P帧最大QP @endif */
    td_u8  min_p_qp;                                                /*!< @if Eng Minimum QP for P frame.
                                                                         @else   P帧最小QP @endif */
    td_u8  max_vi_qp;                                               /*!< @if Eng Maximum QP for VI frames.
                                                                         @else   VI帧最大QP @endif */
    td_u8  min_vi_qp;                                               /*!< @if Eng Minimum QP for VI frames.
                                                                         @else   VI帧最小QP @endif */
    td_u8  min_qp_delta;                                            /*!< @if Eng Minimum quantization parameter delta.
                                                                         @else   最小量化参数调节量 @endif */
    td_u8  max_i_proportion;                                        /*!< @if Eng Ratio of maximum IP frame bit rate.
                                                                         @else   最大IP帧码率比值 @endif */
    td_u8  min_i_proportion;                                        /*!< @if Eng Ratio of minimum IP frame bit rate.
                                                                         @else   最小IP帧码率比值 @endif */
    td_u8  min_still_percent;                                       /*!< @if Eng Minimum percentage of target bitrate in a still state.
                                                                         @else   静止状态下目标码率最小百分比 @endif */
    td_u8  max_still_qp;                                            /*!< @if Eng Maximum QP in still state.
                                                                         @else   静止状态最大QP @endif */
    td_u8  motion_sensitivity;                                      /*!< @if Eng Sensitivity of motion.
                                                                         @else   运动敏感度 @endif */
    td_bool save_bitrate_en;                                        /*!< @if Eng Bit rate saving switch.
                                                                         @else   节省码率控制开关 @endif */
} ext_venc_avbr_param;

typedef struct {
    td_u32 threshold_i[EXT_VENC_TEXTURE_THRESHOLD_NUM];             /*!< @if Eng I frame macroblock-level bitrate control MAD threshold.
                                                                         @else   I帧宏块级码率控制的mad门限 @endif */
    td_u32 threshold_p[EXT_VENC_TEXTURE_THRESHOLD_NUM];             /*!< @if Eng P frame macroblock-level bitrate control MAD threshold.
                                                                         @else   P帧宏块级码率控制的mad门限 @endif */
    td_u32 direction;                                               /*!< @if Eng At the macroblock level bitrate control, the subscript
                                                                                 used to control the Thr in the reduction direction.
                                                                         @else   在宏块级码率控制时，用于控制减方向Thr的下标 @endif */
    td_u8  row_qp_delta;                                            /*!< @if Eng Conventional macroblock-level bitrate control, the fluctuation value
                                                                                 of the start QP of each row of macroblocks relative to the frame start QP.
                                                                         @else   常规宏块级码率控制，每一行宏块的起始QP相对于帧起始QP的波动幅度值 @endif */
    td_s8  first_frame_start_qp;                                    /*!< @if Eng Start QP value for the first frame.
                                                                         @else   第一帧的起始QP值 @endif */
    ext_venc_scene_chg_detect scene_chg_detect;                     /*!< @if Eng Control parameters related to scene detection.
                                                                         @else   场景检测相关的控制参数 @endif */
    union {
        ext_venc_cbr_param cbr_param;                               /*!< @if Eng Constant bit rate param.
                                                                         @else   固定比特率参数 @endif */
        ext_venc_vbr_param vbr_param;                               /*!< @if Eng Variable bit rate param.
                                                                         @else   可变比特率参数 @endif */
        ext_venc_avbr_param avbr_param;                             /*!< @if Eng Adaptive variable bit rate param.
                                                                         @else   自适应可变比特率参数 @endif */
    };
} ext_venc_rc_param;

typedef struct {
    union {
        struct {
            td_u8 fg_level;                                         /*!< @if Eng Forground region bitrate reduction level.
                                                                         @else   前景区域降码率等级 @endif */
            td_u8 bg_level;                                         /*!< @if Eng Bakcground region bitrate reduction level.
                                                                         @else   背景区域降码率等级 @endif */
        }; // jpeg & mjpeg

        struct {
            td_bool is_abs_qp[EXT_VENC_ROI_FOR_MAX];                /*!< @if Eng Absolute/Relative QP control switch.
                                                                         @else   绝对/相对量化参数控制开关 @endif */
            td_s8 qp[EXT_VENC_ROI_FOR_MAX];                         /*!< @if Eng QP.
                                                                         @else   量化参数 @endif */
        };
    };
} ext_venc_roi_param;

typedef struct {
    td_u32 rect_num;                                                /*!< @if Eng ROI rectangle number.
                                                                         @else   ROI矩形个数 @endif */
    ext_rect rect[EXT_VENC_MAX_ROI_NUM];                            /*!< @if Eng ROI rectangle.
                                                                         @else   ROI矩形区域 @endif */
} ext_venc_roi_rect;

typedef struct {
    ext_video_frame user_frame;                                     /*!< @if Eng User frame.
                                                                         @else   用户图像 @endif */
    ext_venc_roi_rect roi_rect;                                     /*!< @if Eng ROI rectangle.
                                                                         @else   感兴趣区域 @endif */
} ext_venc_frame;

typedef td_s32 fn_ext_venc_return_frame(td_handle src_handle, const ext_venc_frame *frame);

typedef struct {
    td_handle src_handle;                                           /*!< @if Eng Source handle.
                                                                         @else   源句柄 @endif */
    fn_ext_venc_return_frame *pfn_venc_return_frame;                /*!< @if Eng Return frame callback interface.
                                                                         @else   还帧回调接口 @endif */
} ext_venc_callback;

typedef struct {
    ext_venc_frame frame;                                           /*!< @if Eng Frame.
                                                                         @else   图像 @endif */
    ext_venc_callback callback;                                     /*!< @if Eng Callback.
                                                                         @else   图像回调 @endif */
} ext_venc_task;

typedef struct {
    ext_venc_work_mode mode;                                        /*!< @if Eng Work mode.
                                                                         @else   工作模式 @endif */
    ext_venc_callback callback;                                     /*!< @if Eng Callback.
                                                                         @else   通道回调 @endif */
} ext_venc_opt_create;

typedef struct {
    td_s32 recv_pic_num;                                            /*!< @if Eng Receive picture number.
                                                                         @else   接收图像个数 @endif */
} ext_venc_opt_start;

typedef struct {
    td_s32 time_out;                                                /*!< @if Eng Time out(ms).
                                                                         @else   超时时间（毫秒） @endif */
} ext_venc_opt_get_stream;

typedef struct {
    td_s32 time_out;                                                /*!< @if Eng Time out(ms).
                                                                         @else   超时时间（毫秒） @endif */
} ext_venc_opt_send_frame;

typedef struct {
    td_s32 time_out;                                                /*!< @if Eng Time out(ms).
                                                                         @else   超时时间（毫秒） @endif */
} ext_venc_opt_detach_osd;

typedef struct {
    td_s32 time_out;                                                /*!< @if Eng Time out(ms).
                                                                         @else   超时时间（毫秒） @endif */
} ext_venc_opt_refresh_osd;

typedef struct {
    ext_venc_standard standard;                                     /*!< @if Eng Standard of payload.
                                                                         @else   协议 @endif */
    td_u32 cur_pic_width;                                           /*!< @if Eng Current picture width.
                                                                         @else   当前图像宽度 @endif */
    td_u32 cur_pic_height;                                          /*!< @if Eng Current picture height.
                                                                         @else   当前图像高度 @endif */
    td_u32 status_get_frame_cnt;                                    /*!< @if Eng Frame input count.
                                                                         @else   输入图像个数 @endif */
    td_u32 status_free_frame_cnt;                                   /*!< @if Eng Frame free count.
                                                                         @else   释放图像个数 @endif */
    td_u32 status_slice_int_cnt;                                    /*!< @if Eng Slice interrupt count.
                                                                         @else   Slice中断计数 @endif */
    td_u32 frame_wait_encode_num;                                   /*!< @if Eng Received and wait for encode frame num.
                                                                         @else   已接收待编码图像帧数 @endif */
    td_u32 frame_left_receive_num;                                  /*!< @if Eng Recv_pic_num(start option) - received frame num.
                                                                         @else   剩余待接收的图像帧数 @endif */
    td_u32 frame_left_encode_num;                                   /*!< @if Eng Recv_pic_num(start option) - encoded frame num.
                                                                         @else   剩余待编码的图像帧数 @endif */
    td_u32 stream_wait_receive_packs;                               /*!< @if Eng Wait for receive packs.
                                                                         @else   等待被获取的码流包数 @endif */
} ext_venc_status;

typedef struct {
    td_u32 mcu_per_ecs;                                             /*!< @if Eng Number of MCUs contained in each ECS.
                                                                         @else   每个ECS中包含多少MCU @endif */
    td_bool ecs_output_en;                                          /*!< @if Eng ECS output enable.
                                                                         @else   ECS 输出使能 @endif */
} ext_venc_jpeg_ecs;

typedef struct {
    td_u8 y_qt[EXT_VENC_QT_COEF_NUM];                               /*!< @if Eng Y component quantization table.
                                                                         @else   Y 分量量化表系数 @endif */
    td_u8 cb_qt[EXT_VENC_QT_COEF_NUM];                              /*!< @if Eng CB component quantization table.
                                                                         @else   CB分量量化表系数 @endif */
    td_u8 cr_qt[EXT_VENC_QT_COEF_NUM];                              /*!< @if Eng CR component quantization table.
                                                                         @else   CR分量量化表系数 @endif */
} ext_venc_jpeg_qt;

typedef struct {
    td_bool enable;                                                 /*!< @if Eng Slice enable switch.
                                                                         @else   Slice使能开关 @endif */
    td_u32  split_size;                                             /*!< @if Eng Slice split size in MB/LCU line.
                                                                         @else   Slice划分尺寸 @endif */
    td_bool slice_output_en;                                        /*!< @if Eng Slice output enable.
                                                                         @else   Slice输出使能 @endif */
} ext_venc_slice;

typedef struct {
    td_u32 enhance;                                                 /*!< @if Eng Enhance layer frame num.
                                                                         @else   增强层图像个数 @endif */
} ext_venc_skip_ref;

typedef struct {
    td_bool enable;                                                 /*!< @if Eng Intra refresh enable switch.
                                                                         @else   帧内刷新使能开关 @endif */
    ext_venc_intra_refresh_mode mode;                               /*!< @if Eng Intra refresh mode.
                                                                         @else   帧内刷新模式 @endif */
    td_u8 request_i_qp;                                             /*!< @if Eng Requested I frame QP.
                                                                         @else   请求I帧的量化参数 @endif */
    td_u32 refresh_num;                                             /*!< @if Eng Indicates the number of rows of I macroblock refresh;
                                                                                 constraint: (h264 mb_size = 16, h265 mb_size = lcu_size)
                                                                                 row: refresh_num * max_refresh_frame_in_gop >=
                                                                                 (pic_height + mb_size - 1) / mb_size.
                                                                         @else   帧内刷新宏块行数和计算方法 @endif */
} ext_venc_intra_refresh;

typedef struct {
    ext_venc_bit_rate_overshoot_mode mode;                          /*!< @if Eng Bit rate overshoot strategy mode.
                                                                         @else   码率过冲应对策略模式 @endif */
    td_u32 bit_rate_threshold;                                      /*!< @if Eng Bit rate threshold.
                                                                         @else   码率过冲阈值 @endif */
    td_u32 frame_gap;                                               /*!< @if Eng Frame gap.
                                                                         @else   处理帧间隔 @endif */
} ext_venc_bit_rate_overshoot;

typedef struct {
    td_s32 max_lost_times;                                          /*!< @if Eng Maximum lost frame time.
                                                                         @else   最大丢帧次数 @endif */
} ext_venc_discard_param;

typedef struct {
    ext_venc_reencode_priority priority;                            /*!< @if Eng Re-encode priority.
                                                                         @else   重编优先级 @endif */
} ext_venc_reencode_param;

typedef struct {
    ext_venc_frame_bits_overshoot_mode mode;                        /*!< @if Eng frame bits overshoot strategy mode.
                                                                         @else   单帧过冲应对策略模式 @endif */
    td_u32 i_frame_bits_threshold;                                  /*!< @if Eng Oversize I frame threshold.
                                                                         @else   超大I帧阈值 @endif */
    td_u32 p_frame_bits_threshold;                                  /*!< @if Eng Oversize P frame threshold.
                                                                         @else   超大P帧阈值 @endif */
    ext_venc_discard_param discard;                                 /*!< @if Eng Discard parameter.
                                                                         @else   丢帧参数 @endif */
    ext_venc_reencode_param reencode;                               /*!< @if Eng Reencode parameter.
                                                                         @else   重编参数 @endif */
} ext_venc_frame_bits_overshoot;

typedef struct {
    ext_venc_usd_type type;                                         /*!< @if Eng Userdata type.
                                                                         @else   用户数据类型 @endif */
    td_u64 phy_addr;                                                /*!< @if Eng Userdata physical address.
                                                                         @else   用户数据物理地址 @endif */
    td_u64 vir_addr;                                                /*!< @if Eng Userdata virtual address.
                                                                         @else   用户数据虚拟地址 @endif */
    td_u32 len;                                                     /*!< @if Eng Userdata length.
                                                                         @else   用户数据长度 @endif */
} ext_venc_userdata;

typedef struct {
    td_u64 phy_addr;                                                /*!< @if Eng OSD physical address.
                                                                         @else   OSD 物理地址 @endif */
    td_u64 vir_addr;                                                /*!< @if Eng OSD virtual address.
                                                                         @else   OSD 虚拟地址 @endif */
} ext_venc_osd_node;

typedef struct {
    ext_pixel_format pixel_format;                                  /*!< @if Eng OSD pixel format.
                                                                         @else   OSD 像素格式 @endif */
    td_u32 width;                                                   /*!< @if Eng OSD width.
                                                                         @else   OSD 宽度 @endif */
    td_u32 height;                                                  /*!< @if Eng OSD height.
                                                                         @else   OSD 高度 @endif */
    td_u32 stride;                                                  /*!< @if Eng OSD stride.
                                                                         @else   OSD 跨度 @endif */
    td_u32 node_num;                                                /*!< @if Eng OSD node number.
                                                                         @else   OSD 节点个数 @endif */
    ext_venc_osd_node node[EXT_VENC_MAX_OSD_NODE_NUM];              /*!< @if Eng OSD node.
                                                                         @else   OSD 节点 @endif */
} ext_venc_osd;

typedef struct {
    td_u32 layer;                                                   /*!< @if Eng OSD Layer.
                                                                         @else   OSD 层 @endif */
    ext_point point;                                                /*!< @if Eng OSD coordinate point(left-top).
                                                                         @else   OSD 坐标点（左上角） @endif */
    td_s32 display_time;                                            /*!< @if Eng OSD display time(ms), -1 display all the time.
                                                                         @else   OSD 显示时长（毫秒），-1一直显示 @endif */
    td_bool qp_enable;                                              /*!< @if Eng OSD QP enable.
                                                                         @else   OSD 量化参数使能     @endif */
    td_bool is_abs_qp;                                              /*!< @if Eng OSD absolute/relative QP switch.
                                                                         @else   OSD 绝对/相对量化参数开关 @endif */
    td_s32 qp_value;                                                /*!< @if Eng OSD QP value.
                                                                         @else   OSD 量化参数值 @endif */
} ext_venc_osd_attr;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
