/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2024-10-11
 */

#ifndef SOC_VDEC_TYPE_H
#define SOC_VDEC_TYPE_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup vdec_type vdec_type
 * @ingroup  vdec_api
 * @{
 */

/*-------------------------------- macro define -------------------------------------------*/
#define EXT_VDEC_INVALID_HANDLE     0xFFFFFFFF  /*!< @if Eng vdec invaild handle.
                                                  @else   解码无效句柄 @endif */
#define EXT_VDEC_MAX_EXT_BUF_NUM    20          /*!< @if Eng Maximum number of external buffers.
                                                  @else   最大外部buffer个数 @endif */
#define EXT_VDEC_MAX_ERR_THRESHOLD  100         /*!< @if Eng Maximum number of error threshold.
                                                  @else   最大错误率阈值 @endif */
#define EXT_VDEC_MAX_DEC_WIDTH      4096         /*!< @if Eng Maximum decode width.
                                                  @else   最大解码宽度 @endif */
#define EXT_VDEC_MAX_DEC_HEIGHT     4096         /*!< @if Eng Maximum decode height.
                                                  @else   最大解码高度 @endif */

/*-------------------------------- struct define ------------------------------------------*/

typedef enum {
    EXT_VDEC_STD_UNKNOWN = 0,                /*!< @if Eng Unknown standard.
                                                  @else   未知协议 @endif */
    EXT_VDEC_STD_MJPEG,                      /*!< @if Eng MJPEG standard.
                                                  @else   MJPEG 协议 @endif */
    EXT_VDEC_STD_H264,                       /*!< @if Eng H.264 standard.
                                                  @else   H.264 协议 @endif */
    EXT_VDEC_STD_H265,                       /*!< @if Eng H.265 standard.
                                                  @else   H.265 协议 @endif */
    EXT_VDEC_STD_HEIF,                       /*!< @if Eng HEIF  standard.
                                                  @else   HEIF  协议 @endif */
    EXT_VDEC_STD_MAX                         /*!< @if Eng Invalid value.
                                                  @else   无效值 @endif */
} ext_vdec_standard;

typedef enum {
    EXT_VDEC_OUT_ORDER_DISPLAY = 0,          /*!< @if Eng Output in display order(default).
                                                  @else   显示序输出（默认） @endif */
    EXT_VDEC_OUT_ORDER_DECODE,               /*!< @if Eng Output in decode order.
                                                  @else   解码序输出 @endif */
    EXT_VDEC_OUT_ORDER_MAX                   /*!< @if Eng Invalid value.
                                                  @else   无效值 @endif */
} ext_vdec_output_order;

typedef enum {
    EXT_VDEC_CS_UNKNOWN = 0,                 /*!< @if Eng Unknown color space.
                                                  @else   未知颜色空间 @endif */
    EXT_VDEC_CS_BT601_YUV_LIMITED,           /*!< @if Eng Bt601 limited color space.
                                                  @else   bt601有限制颜色空间 @endif */
    EXT_VDEC_CS_BT601_YUV_FULL,              /*!< @if Eng Bt601 full color space.
                                                  @else   bt601全颜色空间 @endif */
    EXT_VDEC_CS_BT709_YUV_LIMITED,           /*!< @if Eng Bt709 limited color space.
                                                  @else   bt709有限制颜色空间 @endif */
    EXT_VDEC_CS_BT709_YUV_FULL,              /*!< @if Eng Bt709 full color space.
                                                  @else   bt709全颜色空间 @endif */
    EXT_VDEC_CS_BT2020_YUV_LIMITED,          /*!< @if Eng Bt2020 limited color space.
                                                  @else   bt709有限制颜色空间 @endif */
    EXT_VDEC_CS_BT2020_YUV_FULL,             /*!< @if Eng Bt2020 full color space.
                                                  @else   bt2020全颜色空间 @endif */
    EXT_VDEC_CS_MAX                          /*!< @if Eng Invalid value.
                                                  @else   无效值 @endif */
} ext_vdec_color_space;

typedef enum {
    EXT_VDEC_SAMPLE_UNKNOWN = 0,             /*!< @if Eng Unknown sample.
                                                  @else   未知采样模式 @endif */
    EXT_VDEC_PROGRESSIVE,                    /*!< @if Eng Progressive.
                                                  @else   逐行采样 @endif */
    EXT_VDEC_INTERLACED,                     /*!< @if Eng Interlaced.
                                                  @else   隔行采样 @endif */
    EXT_VDEC_SAMPLE_MAX                      /*!< @if Eng Invalid value.
                                                  @else   无效值 @endif */
} ext_vdec_sampling_type;

typedef enum {
    EXT_VDEC_FR_TYPE_PTS = 0,                /*!< @if Eng Frame rate calculated from pts.
                                                  @else   从pts计算出帧率 @endif */
    EXT_VDEC_FR_TYPE_STREAM,                 /*!< @if Eng Frame rate come from stream.
                                                  @else   从码流识别帧率 @endif */
    EXT_VDEC_FR_TYPE_USER,                   /*!< @if Eng Frame rate setted by user.
                                                  @else   用户设置帧率 @endif */
    EXT_VDEC_FR_TYPE_USER_PTS,               /*!< @if Eng Frame rate from user until second I frame,
                                                          then use frame rate from pts.
                                                  @else   第二个I帧前使用用户设置帧率，之后使用pts计算出帧率 @endif */
    EXT_VDEC_FR_TYPE_MAX                     /*!< @if Eng Invalid value.
                                                  @else   无效值 @endif */
} ext_vdec_frame_rate_type;

typedef enum {
    EXT_VDEC_FMT_UNKNOWN = 0,                /*!< @if Eng Unknown format.
                                                  @else   未知格式 @endif */
    EXT_VDEC_FMT_YUV400SP,                   /*!< @if Eng Spatial sampling format 4:0:0, semi-planner.
                                                  @else   空间采样格式4:0:0 @endif */
    EXT_VDEC_FMT_YUV420SP,                   /*!< @if Eng Spatial sampling format 4:2:0, semi-planner.
                                                  @else   空间采样格式4:2:0 @endif */
    EXT_VDEC_FMT_MAX                         /*!< @if Eng Invalid value.
                                                  @else   无效值 @endif */
} ext_vdec_frame_format;

typedef enum {
    EXT_VDEC_FRM_TYPE_UNKNOWN = 0,           /*!< @if Eng Unknown type.
                                                  @else   未知类型 @endif */
    EXT_VDEC_FRM_TYPE_I,                     /*!< @if Eng I frame.
                                                   @else  I帧 @endif */
    EXT_VDEC_FRM_TYPE_P,                     /*!< @if Eng P frame.
                                                  @else   P帧 @endif */
    EXT_VDEC_FRM_TYPE_B,                     /*!< @if Eng B frame.
                                                  @else   B帧 @endif */
    EXT_VDEC_FRM_TYPE_MAX                    /*!< @if Eng Invalid value.
                                                  @else   无效值 @endif */
} ext_vdec_frame_type;

typedef enum {
    EXT_VDEC_FRM_INTERNAL,                   /*!< @if Eng Vdec output buffer allocated internally.
                                                  @else   解码器帧存缓冲由内部分配 @endif */
    EXT_VDEC_FRM_EXTERNAL,                   /*!< @if Eng Vdec output buffer allocated externally.
                                                  @else   解码器帧存缓冲由外部分配 @endif */
    EXT_VDEC_FRM_MAX                         /*!< @if Eng Invalid value.
                                                  @else   无效值 @endif */
} ext_vdec_frame_buf_mode;

typedef struct {
    td_bool is_max_dec_w_h_en;                 /*!< @if Eng Enable maximum width and height.
                                                  @else   使能最大解码宽高 @endif */
    td_u32 max_dec_width;                      /*!< @if Eng Maximum width.
                                                  @else   最大解码宽度 @endif */
    td_u32 max_dec_height;                     /*!< @if Eng Maximum height.
                                                  @else   最大解码高度 @endif */
} ext_vdec_max_dec_w_h;

typedef struct {
    td_u32 phy_addr;                         /*!< @if Eng Frame buffer physical address.
                                                  @else   帧存物理地址 @endif */
    td_u32 frame_size;                       /*!< @if Eng Frame buffer size.
                                                  @else   帧存缓冲大小 @endif */
} ext_vdec_frame_buf;

typedef enum {
    EXT_VDEC_SPLICE_NONE = 0,
    EXT_VDEC_SPLICE_AYUV_TAB,
    EXT_VDEC_SPLICE_MAX
} ext_vdec_splice_type;

typedef struct {
    td_u32 ori_width;
    td_u32 ori_height;
    ext_vdec_splice_type type;
} ext_vdec_splice_info;

typedef struct {
    td_u32 new_width;                        /*!< @if Eng New width.
                                                  @else   变分辨率后的宽 @endif */
    td_u32 new_height;                       /*!< @if Eng New_height.
                                                  @else   变分辨率后的高 @endif */
    td_u32 new_stride;                       /*!< @if Eng New stride.
                                                  @else   变分辨率后的步幅 @endif */
    td_u32 new_bit_depth;                    /*!< @if Eng New bit depth.
                                                  @else   变分辨率后的比特深度 @endif */
    td_u32 new_frame_num;                    /*!< @if Eng New frame number.
                                                  @else   变分辨率后的帧存个数 @endif */
    td_u32 new_frame_size;                   /*!< @if Eng New frame size.
                                                  @else   变分辨率后的帧存大小 @endif */
    ext_vdec_splice_info new_splice_info;    /*!< @if Eng New splice info.
                                                  @else   变分辨率后的拼接信息 @endif */
} ext_vdec_size_change;

typedef enum {
    EXT_VDEC_EVT_UNKNOWN       = 0,          /*!< @if Eng Unknown event.
                                                  @else   未知事件 @endif */
    EXT_VDEC_EVT_NEW_FRAME     = 1 << 0,     /*!< @if Eng New frame event.
                                                  @else   新帧事件 @endif */
    EXT_VDEC_EVT_FREE_STREAM   = 1 << 1,     /*!< @if Eng Free stream event.
                                                  @else   空闲码流事件 @endif */
    EXT_VDEC_EVT_EOS           = 1 << 2,     /*!< @if Eng End of stream event.
                                                  @else   码流结束事件 @endif */
    EXT_VDEC_EVT_ERR_UNSUPPORT = 1 << 3,     /*!< @if Eng Unsupport event.
                                                  @else   不支持事件 @endif */
    EXT_VDEC_EVT_ERR_FRAME     = 1 << 4,     /*!< @if Eng Error frame event.
                                                  @else   错误帧事件 @endif */
    EXT_VDEC_EVT_ERR_DECODE    = 1 << 5,     /*!< @if Eng Error decode event.
                                                  @else   解码异常事件 @endif */
    EXT_VDEC_EVT_SIZE_CHG      = 1 << 6,     /*!< @if Eng Resolution change event.
                                                  @else   变分辨率事件 @endif */
    EXT_VDEC_EVT_OUT_OF_MEMORY = 1 << 7,     /*!< @if Eng Out of memory.
                                                  @else   内存不足事件 @endif */
    EXT_VDEC_EVT_ALL           = 0xFFFFFFFF  /*!< @if Eng All.
                                                  @else   所有事件 @endif */
} ext_vdec_event_id;

typedef enum {
    EXT_VDEC_ERROR_NONE = 0,                 /*!< @if Eng No error.
                                                  @else   无错误 @endif */
    EXT_VDEC_ERROR_VPS,                      /*!< @if Eng Vps error.
                                                  @else   视频参数集错误 @endif */
    EXT_VDEC_ERROR_SPS,                      /*!< @if Eng Sps error.
                                                  @else   序列参数集错误 @endif */
    EXT_VDEC_ERROR_PPS,                      /*!< @if Eng Pps error.
                                                  @else   图像参数集错误 @endif */
    EXT_VDEC_ERROR_SLICE_HDR,                /*!< @if Eng Slice header error.
                                                  @else   切片头错误 @endif */
    EXT_VDEC_ERROR_REF_FRAME,                /*!< @if Eng Reference frame error.
                                                  @else   参考帧错误 @endif */
    EXT_VDEC_ERROR_UNDEFINED,                /*!< @if Eng Undefined error.
                                                  @else   未定义错误 @endif */
    EXT_VDEC_ERROR_MAX,                      /*!< @if Eng Invalid value.
                                                  @else   无效值 @endif */
} ext_vdec_error_type;

typedef struct {
    ext_vdec_frame_type frame_type;          /*!< @if Eng Frame type.
                                                  @else   错误帧存类型 @endif */
    td_u32 error_ratio;                      /*!< @if Eng Error ratio.
                                                  @else   错误率[0,100] @endif */
} ext_vdec_error_frame;

typedef struct {
    td_bool is_last_frame_taken;             /*!< @if Eng 0:Last frame has not been taken, 1:Last frame has been taken.
                                                  @else   0:最后一帧还未被取走，1：最后一帧已经被取走 @endif */
} ext_vdec_eos_param;

typedef struct {
    td_s64 pts;                              /*!< @if Eng Pts.
                                                  @else   时间戳 @endif */
    ext_vdec_error_type error_type;          /*!< @if Eng Error type.
                                                  @else   解码错误类型 @endif */
} ext_vdec_error_decode;

typedef struct {
    ext_vdec_event_id event_id;              /*!< @if Eng Event id.
                                                  @else   事件ID @endif */
    union {
        ext_vdec_eos_param eos_param;        /*!< @if Eng End of stream param.
                                                  @else   码流结束事件参数 @endif */
        ext_vdec_error_frame error_frame;    /*!< @if Eng Error frame param.
                                                  @else   帧错误事件参数 @endif */
        ext_vdec_error_decode error_decode;  /*!< @if Eng Error decode param.
                                                  @else   解码错误事件参数 @endif */
        ext_vdec_size_change size_change;    /*!< @if Eng Resolution change param.
                                                  @else   解码变分辨率事件参数 @endif */
    } event_param;
} ext_vdec_event;


typedef enum {
    EXT_VDEC_FLUSH_IN = 0,                   /*!< @if Eng Flush input.
                                                  @else   刷新输入 @endif */
    EXT_VDEC_FLUSH_OUT,                      /*!< @if Eng Flush output.
                                                  @else   刷新输出 @endif */
    EXT_VDEC_FLUSH_ALL,                      /*!< @if Eng Flush input and output.
                                                  @else   刷新输入输出 @endif */
    EXT_VDEC_FLUSH_MAX                       /*!< @if Eng Invalid value.
                                                  @else   无效值 @endif */
} ext_vdec_flush_type;

typedef enum {
    EXT_VDEC_BIT_DEPTH_AUTO = 0,             /*!< @if Eng Auto depth.
                                                  @else   自动像素深度，由码流本身决定 @endif */
    EXT_VDEC_BIT_DEPTH_8,                    /*!< @if Eng 8bit depth.
                                                  @else   8比特深度 @endif */
    EXT_VDEC_BIT_DEPTH_10,                   /*!< @if Eng 10bit depth.
                                                  @else   10比特深度 @endif */
    EXT_VDEC_BIT_DEPTH_MAX                   /*!< @if Eng Invalid value.
                                                  @else   无效值 @endif */
} ext_vdec_bit_depth;

typedef enum {
    EXT_VDEC_FRM_LAYOUT_AUTO = 0,            /*!< @if Eng Auto layout.
                                                  @else   默认帧存yuv布局，由解码器决定 @endif */
    EXT_VDEC_FRM_LAYOUT_LINEAR,              /*!< @if Eng Linear layout.
                                                  @else   线性yuv布局 @endif */
    EXT_VDEC_FRM_LAYOUT_MAX                  /*!< @if Eng Invalid value.
                                                  @else   无效值 @endif */
} ext_vdec_frame_layout;

typedef enum {
    EXT_VDEC_DEC_MODE_IPB = 0,               /*!< @if Eng Decode I/P/B frames.
                                                  @else   解码I帧/P帧/B帧 @endif */
    EXT_VDEC_DEC_MODE_IP,                    /*!< @if Eng Decode I/P frames only.
                                                  @else   只解码I帧/P帧 @endif */
    EXT_VDEC_DEC_MODE_I,                     /*!< @if Eng Decode I frames only.
                                                  @else   只解码I帧 @endif */
    EXT_VDEC_DEC_MODE_MAX                    /*!< @if Eng Invalid value.
                                                  @else   无效值 @endif */
} ext_vdec_decode_mode;

typedef enum {
    EXT_VDEC_COMPRESS_MODE_OFF = 0,          /*!< @if Eng Turn off compression.
                                                  @else   压缩关闭 @endif */
    EXT_VDEC_COMPRESS_MODE_MAX               /*!< @if Eng Invalid value.
                                                  @else   无效值 @endif */
} ext_vdec_compress_mode;

typedef enum {
    EXT_VDEC_HEIF_ORIGINAL = 0,               /*!< @if Eng decode heif original image.
                                                  @else   解码HEIF原图 @endif */
    EXT_VDEC_HEIF_THUMBNAIL,                 /*!< @if Eng decode heif thumbnail image.
                                                  @else   解码HEIF缩略图 @endif */
    EXT_VDEC_HEIF_MAX                        /*!< @if Eng Invalid value.
                                                  @else   无效值 @endif */
} ext_vdec_heif_select;

typedef enum {
    EXT_VDEC_ORI_NORMAL = 0,                 /*!< @if Eng Picture orientation normal.
                                                  @else   图像无旋转 @endif */
    EXT_VDEC_ORI_FLIP_HOR,                   /*!< @if Eng Picture flipped horizontally.
                                                  @else   图像水平翻转 @endif */
    EXT_VDEC_ORI_ROTATE_180,                 /*!< @if Eng Picture rotated 180 degrees.
                                                  @else   图像旋转180度 @endif */
    EXT_VDEC_ORI_FLIP_VER,                   /*!< @if Eng Picture flipped vertically.
                                                  @else   图像垂直翻转 @endif */
    EXT_VDEC_ORI_FLIP_HOR_ROTATE_270,        /*!< @if Eng Picture flipped horizontally and rotated 270 degrees.
                                                  @else   图像水平翻转并旋转270度 @endif */
    EXT_VDEC_ORI_ROTATE_90,                  /*!< @if Eng Picture rotated 90 degrees.
                                                  @else   图像旋转90度 @endif */
    EXT_VDEC_ORI_FLIP_HOR_ROTATE_90,         /*!< @if Eng Picture flipped horizontally and rotated 90 degrees.
                                                  @else   图像水平翻转并旋转90度 @endif */
    EXT_VDEC_ORI_ROTATE_270,                 /*!< @if Eng Picture rotated 270 degrees.
                                                  @else   图像旋转270度 @endif */
    EXT_VDEC_ORI_MAX                         /*!< @if Eng Invalid value.
                                                  @else   无效值 @endif */
} ext_vdec_orientation;

typedef struct {
    ext_vdec_frame_rate_type type;           /*!< @if Eng The source of frame rate.
                                                  @else   帧率源类型 @endif */
    td_u32 frame_rate;                       /*!< @if Eng Frame rate, 1000 times the original value.
                                                  @else   帧率值x1000 @endif */
} ext_vdec_frame_rate;

typedef struct {
    ext_vdec_standard standard;              /*!< @if Eng Standard.
                                                  @else   码流协议类型 @endif */
    td_u32 sub_version;                      /*!< @if Eng Stream sub version of protocol.
                                                  @else   子协议版本号 @endif */
    td_u32 profile;                          /*!< @if Eng Stream profile.
                                                  @else   码流的profile @endif */
    td_u32 level;                            /*!< @if Eng Stream level.
                                                  @else   码流的level @endif */
    td_u32 dec_width;                        /*!< @if Eng Decode width.
                                                  @else   解码图像宽 @endif */
    td_u32 dec_height;                       /*!< @if Eng Decode height.
                                                  @else   解码图像高 @endif */
    td_u32 disp_width;                       /*!< @if Eng Display width.
                                                  @else   显示图像宽 @endif */
    td_u32 disp_height;                      /*!< @if Eng Display height.
                                                  @else   显示图像高 @endif */
    td_u32 disp_center_x;                    /*!< @if Eng Horizontal coordinate of the center of the output picture.
                                                  @else   显示图像中心横坐标，以输出图像的左上角为坐标原点 @endif */
    td_u32 disp_center_y;                    /*!< @if Eng Vertical coordinate of the center of the output picture.
                                                  @else   显示图像中心纵坐标，以输出图像的左上角为坐标原点 @endif */
    td_u32 aspect_width;                     /*!< @if Eng Output aspect ratio: width.
                                                  @else   输出宽高比之宽值 @endif */
    td_u32 aspect_height;                    /*!< @if Eng Output aspect ratio: height.
                                                  @else   输出宽高比之高值 @endif */
    td_u32 bit_rate;                         /*!< @if Eng Bit rate, in kbit/s.
                                                  @else   码流的码率, Kbps @endif */
    td_u32 frame_rate;                       /*!< @if Eng Frame rate, 1000 times the original value.
                                                  @else   帧率的1000倍 @endif */
    ext_vdec_bit_depth bit_depth;            /*!< @if Eng Bit depth.
                                                  @else   比特深度 @endif */
    ext_vdec_sampling_type sampling_type;    /*!< @if Eng Sampling type.
                                                  @else   采样类型 @endif */
    ext_vdec_color_space color_space;        /*!< @if Eng Color space.
                                                  @else   颜色空间 @endif */
} ext_vdec_feature;

typedef struct {
    ext_vdec_standard standard;              /*!< @if Eng Standard.
                                                  @else   码流协议类型 @endif */
    ext_vdec_decode_mode decode_mode;        /*!< @if Eng Decode mode.
                                                  @else   解码模式 @endif */
    ext_vdec_output_order output_order;      /*!< @if Eng Output order.
                                                  @else   输出顺序 @endif */
    ext_vdec_frame_layout frame_layout;      /*!< @if Eng Frame layout.
                                                  @else   帧存yuv布局方式 @endif */
    ext_vdec_frame_buf_mode frame_buf_mode;  /*!< @if Eng Frame buffer mode.
                                                  @else   帧存缓冲分配模式 @endif */
    ext_vdec_max_dec_w_h max_dec_w_h;        /*!< @if Eng Maximum decode width and height.
                                                  @else   最大解码宽高 @endif */
    td_u32 max_stream_size;                  /*!< @if Eng Max stream size.
                                                  @else   最大码流缓冲大小 @endif */
    td_u32 max_stream_num;                   /*!< @if Eng Max stream number.
                                                  @else   最大码流缓冲个数 @endif */
    td_u32 extra_frame_num;                  /*!< @if Eng Extra frame number.
                                                  @else   额外解码帧存个数，不包含解码所需的最少帧存数 @endif */
    td_u32 error_threshold;                  /*!< @if Eng Error threshold.
                                                  @else   错误阈值[0,100]，当一帧的error_ratio大于错误阈值时，
                                                          此帧会被解码丢弃，小于等于错误阈值的帧输出
                                                          0：所有有错误的帧都会被丢弃，100：所有帧全部输出 @endif */
} ext_vdec_attr;

typedef struct {
    td_bool low_delay;                       /*!< @if Low delay enable flag.
                                                  @else   低延迟使能标志 @endif */
    ext_vdec_frame_rate frame_rate;          /*!< @if Frame rate parameter.
                                                  @else   视频帧率信息 @endif */
} ext_vdec_param;

typedef struct {
    td_u32 start_addr;                       /*!< @if Eng Buffer start address.
                                                  @else   缓冲起始地址 @endif */
    td_u8 *vir_addr;                         /*!< @if Eng Buffer virtual address.
                                                  @else   缓冲虚拟地址 @endif */
    td_u32 length;                           /*!< @if Eng Buffer size(byte).
                                                  @else   缓冲大小 @endif */
    td_s64 pts;                              /*!< @if Eng Stream pts, -1 invalid.
                                                  @else   码流pts，-1为无效值 @endif */
    td_bool is_end_of_frame;                 /*!< @if Eng End of frame flag.
                                                  @else   码流是否包含完整一帧标志 @endif */
} ext_vdec_stream;

typedef struct {
    td_u32 bit_rate_in_bps;                  /*!< @if Eng Bit rate in bps.
                                                  @else   比特率 @endif */
    td_u32 total_stream_buffer_size;         /*!< @if Eng Total stream buffer size.
                                                  @else   所有码流缓冲大小 @endif */
    td_u32 free_stream_buffer_size;          /*!< @if Eng Free stream buffer size.
                                                  @else   空闲码流缓冲大小 @endif */
    td_u32 used_stream_buffer_size;          /*!< @if Eng Used stream buffer size.
                                                  @else   已用码流缓冲大小 @endif */
    td_u32 undecode_stream_buffer_size;      /*!< @if Eng Undecode stream buffer size.
                                                  @else   未解码码流缓冲大小 @endif */
    td_u32 undecode_stream_num;              /*!< @if Eng Undecode stream num.
                                                  @else   未解码码流个数 @endif */
} ext_vdec_stream_status;

typedef struct {
    td_u32 error_frame_num;                  /*!< @if Eng Error frame number.
                                                  @else   错误帧个数 @endif */
    td_u32 total_output_frame_num;           /*!< @if Eng Total output frame num.
                                                  @else   累计输出帧个数 @endif */
    td_u32 wait_output_frame_num;            /*!< @if Eng Wait output frame num.
                                                  @else   等待输出帧个数 @endif */
    td_u32 display_frame_rate;               /*!< @if Eng Display frame rate.
                                                  @else   显示帧率 @endif */
    td_u32 decode_frame_rate;                /*!< @if Eng Decode frame rate.
                                                  @else   解码帧率 @endif */
} ext_vdec_frame_status;

typedef struct {
    ext_vdec_stream_status stream_state;     /*!< @if Eng Stream state.
                                                  @else   码流状态 @endif */
    ext_vdec_frame_status frame_state;       /*!< @if Eng Frame state.
                                                  @else   帧存状态 @endif */
} ext_vdec_status;

typedef struct {
    ext_vdec_flush_type type;                /*!< @if Eng Flush type.
                                                  @else   刷新类型 @endif */
} ext_vdec_opt_flush;

typedef struct {
    ext_vdec_heif_select select;             /*!< @if Eng HEIF image decode select.
                                                  @else   HEIF图像解码选择 @endif */
} ext_vdec_opt_heif;

typedef struct {
    td_s32 time_out;                         /*!< @if Eng Time out -1: block-waiting, [0, INT_MAX]: timeout-waiting.
                                                  @else   超时时间 -1：阻塞等待，[0, INT_MAX]：超时等待 @endif */
    ext_vdec_opt_heif heif;                  /*!< @if Eng HEIF decode control option.
                                                  @else   HEIF解码控制选项 @endif */
} ext_vdec_opt_decode;

typedef struct {
    td_s32 time_out;                         /*!< @if Eng Time out -1: block-waiting, [0, INT_MAX]: timeout-waiting.
                                                  @else   超时时间 -1：阻塞等待，[0, INT_MAX]：超时等待 @endif */
} ext_vdec_opt_listen;

typedef struct {
    td_u32 expect_size;                      /*!< @if Eng Expect size of stream buffer.
                                                  @else   期望获取码流的大小 @endif */
} ext_vdec_opt_get_stream;

typedef struct {
    td_s32 time_out;                         /*!< @if Eng Time out -1: block-waiting, [0, INT_MAX]: timeout-waiting.
                                                  @else   超时时间 -1：阻塞等待，[0, INT_MAX]：超时等待 @endif */
} ext_vdec_opt_acq_frame;

typedef struct {
    ext_vdec_standard standard;              /*!< @if Eng Standard.
                                                  @else   协议类型 @endif */
    td_u32 max_width;                        /*!< @if Eng Max width.
                                                  @else   最大宽 @endif */
    td_u32 max_height;                       /*!< @if Eng Max height.
                                                  @else   最大高 @endif */
    td_u32 max_frame_rate;                   /*!< @if Eng Max_frame rate.
                                                  @else   最大帧率 @endif */
    td_u32 bit_depth_num;                    /*!< @if Eng Bit depth capability num.
                                                  @else   比特深度能力个数 @endif */
    ext_vdec_bit_depth bit_depth[EXT_VDEC_BIT_DEPTH_MAX]; /*!< @if Eng Bit depth capability.
                                                  @else   比特深度能力集 @endif */
    td_u32 frame_layout_num;                 /*!< @if Eng Frame layout capability num.
                                                  @else   帧存布局能力个数 @endif */
    ext_vdec_frame_layout frame_layout[EXT_VDEC_FRM_LAYOUT_MAX]; /*!< @if Eng Frame layout capability.
                                                                      @else   帧存布局类型集 @endif */
} ext_vdec_capability;

typedef struct {
    td_u32 type;                             /*!< @if Eng Private data type.
                                                  @else   私有数据类型 @endif */
    td_u32 size;                             /*!< @if Eng Private data size.
                                                  @else   私有数据大小 @endif */
    td_u64 data;                             /*!< @if Eng Private data address.
                                                  @else   私有数据地址 @endif */
} ext_vdec_frame_private;

typedef struct {
    td_u32 start_addr;                       /*!< @if Eng Frame buffer start address.
                                                  @else   帧存起始地址 @endif */
    td_u32 offset_y;                         /*!< @if Eng Luma offset.
                                                  @else   亮度地址偏移 @endif */
    td_u32 offset_c;                         /*!< @if Eng Chroma offset.
                                                  @else   色度地址偏移 @endif */
    td_u32 stride_y;                         /*!< @if Eng Luma stride.
                                                  @else   亮度跨度 @endif */
    td_u32 stride_c;                         /*!< @if Eng Chroma stride.
                                                  @else   色度跨度 @endif */
    td_u8 *vir_addr_y;                       /*!< @if Eng Luma virtual address.
                                                  @else   亮度虚拟地址 @endif */
    td_u8 *vir_addr_c;                       /*!< @if Eng Chroma virtual address.
                                                  @else   色度虚拟地址 @endif */
} ext_vdec_frame_addr;

typedef struct {
    td_u32 start_addr;                       /*!< @if Eng Tunnel start address.
                                                  @else   隧道起始地址 @endif */
} ext_vdec_frame_tunnel;

typedef struct {
    ext_vdec_compress_mode cmp_mode;         /*!< @if Eng Compress mode.
                                                  @else   压缩模式 @endif */
} ext_vdec_frame_compress;

typedef struct {
    td_u32 rect_x;                           /*!< @if Eng Start x-axis.
                                                  @else   起始横坐标 x @endif */
    td_u32 rect_y;                           /*!< @if Eng Start y-axis.
                                                  @else   起始纵坐标 y @endif */
    td_u32 rect_w;                           /*!< @if Eng Width.
                                                  @else   宽度 @endif */
    td_u32 rect_h;                           /*!< @if Eng Height.
                                                  @else   高度 @endif */
} ext_vdec_frame_rect;

typedef struct {
    td_u32 frame_index;                      /*!< @if Eng Frame index.
                                                  @else   帧序 @endif */
    td_u32 width;                            /*!< @if Eng Width.
                                                  @else   显示宽 @endif */
    td_u32 height;                           /*!< @if Eng Height.
                                                  @else   显示高 @endif */
    td_u32 aspect_width;                     /*!< @if Eng Output aspect ratio: width.
                                                  @else   输出宽高比之宽值 @endif */
    td_u32 aspect_height;                    /*!< @if Eng Output aspect ratio: height.
                                                  @else   输出宽高比之高值 @endif */
    td_s64 pts;                              /*!< @if Eng Pts after adjust.
                                                  @else   经过解码调整过的pts @endif */
    td_s64 src_pts;                          /*!< @if Eng Source pts from the stream.
                                                  @else   原始未改pts @endif */
    td_u32 frame_rate;                       /*!< @if Eng Frame_rate, 1000 times the original value, 0 means unknown.
                                                  @else   帧率的1000倍 @endif */
    td_u32 src_frame_rate;                   /*!< @if Eng Source frame rate.
                                                  @else   原始帧率 @endif */
    td_u32 error_ratio;                      /*!< @if Eng Error ratio.
                                                  @else   解码错误率[0,100] @endif */
    ext_vdec_splice_info splice_info;        /*!< @if Eng Splice info.
                                                  @else   拼接信息 @endif */
    ext_vdec_standard standard;              /*!< @if Eng Standard.
                                                  @else   协议类型 @endif */
    ext_vdec_bit_depth bit_depth;            /*!< @if Eng Bit depth.
                                                  @else   比特深度 @endif */
    ext_vdec_color_space color_space;        /*!< @if Eng Color space.
                                                  @else   颜色空间 @endif */
    ext_vdec_sampling_type sampling_type;    /*!< @if Eng Sampling type.
                                                  @else   采样类型 @endif */
    ext_vdec_frame_rect frame_rect;          /*!< @if Eng Frame rectangle.
                                                  @else   帧显示位置及大小 @endif */
    ext_vdec_frame_format frame_format;      /*!< @if Eng Frame format.
                                                  @else   像素格式 @endif */
    ext_vdec_frame_compress frame_compress;  /*!< @if Eng Frame compress.
                                                  @else   帧压缩信息 @endif */
    ext_vdec_frame_addr frame_addr;          /*!< @if Eng Frame address.
                                                  @else   帧地址信息 @endif */
    ext_vdec_frame_tunnel frame_tunnel;      /*!< @if Eng Frame tunnel.
                                                  @else   帧隧道信息，用于低延迟 @endif */
    ext_vdec_orientation orientation;        /*!< @if Eng Picture orientation.
                                                  @else   图像方向 @endif */
    ext_vdec_frame_private frame_private;    /*!< @if Eng Frame private.
                                                  @else   视频私有信息 @endif */
} ext_vdec_frame;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SOC_VDEC_TYPE_H */
