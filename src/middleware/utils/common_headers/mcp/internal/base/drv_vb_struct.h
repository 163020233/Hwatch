/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vb struct define
 * Author: AuthorNameMagicTag
 * Create: 2021-08-10
 */

#ifndef __DRV_VB_STRUCT_H__
#define __DRV_VB_STRUCT_H__

#include "drv_isp_video_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VB_INVALID_POOLID              0xFFFFFFFF
#define VB_INVALID_HANDLE              0xFFFFFFFF

#define VB_MAX_COMM_POOLS              16
#define VB_MAX_MOD_COMM_POOLS          16

/* For VB */
#define VB_MAX_POOLS                   512

/* Generall common pool use this owner id, module common pool use VB_UID as owner id */
#define POOL_OWNER_COMMON              (-1)

/* Private pool use this owner id */
#define POOL_OWNER_PRIVATE             (-2)

#define RESERVE_MMZ_NAME               "window"

#define EXT_VB_DEFAULT_ALIGN    32U
#define EXT_VB_MAX_ALIGN        1024U
#define EXT_VB_ALIGN_UP(x, a)    ((((x) + ((a) - 1)) / (a)) * (a))
#define EXT_VB_ALIGN_DOWN(x, a)  (((x) / (a)) * (a))

typedef td_u32 vb_blk_handle;
typedef td_u32 vb_pool;
typedef td_u32 vb_blk;

typedef enum {
    VB_REMAP_MODE_NONE = 0,    /* no remap */
    VB_REMAP_MODE_NOCACHE = 1, /* no cache remap */
    VB_REMAP_MODE_CACHED = 2,  /* cache remap, if you use this mode, you should flush cache by yourself */
    VB_REMAP_MODE_MAX
} ext_vb_remap_mode;

/* mmz name max len */
#define MAX_MMZ_NAME_LEN  32
typedef struct {
    td_u64 blk_size;
    td_u32 blk_cnt;
    ext_vb_remap_mode remap_mode;
} ext_vb_pool_config;

#define VB_MAX_COMM_POOLS 16
typedef struct {
    td_u32 max_pool_cnt;
    ext_vb_pool_config comm_pool[VB_MAX_COMM_POOLS];
} ext_vb_config;

typedef struct {
    td_u32 is_comm_pool;
    td_u32 blk_cnt;
    td_u32 free_blk_cnt;
} ext_vb_pool_status;

typedef enum {
    EXT_VB_SOURCE_COMMON  = 0,
    EXT_VB_SOURCE_MODULE  = 1,
    EXT_VB_SOURCE_PRIVATE = 2,
    EXT_VB_SOURCE_USER    = 3,
    EXT_VB_SOURCE_BUTT
} ext_vb_source;

/* user ID for VB */
#define VB_MAX_USER  VB_UID_MAX
typedef enum {
    VB_UID_VIU = 0,
    VB_UID_VIPROC,
    VB_UID_DIS,
    VB_UID_VGS,
    VB_UID_GDC,
    VB_UID_PPU,
    VB_UID_VENC,
    VB_UID_VO,
    VB_UID_USER,
    VB_UID_MAX,
} ext_vb_uid;

typedef struct {
    vb_blk block;
    ext_isp_video_supplement supplement;
} ext_vb_video_supplement_info;

/* dummy struct, used only in 32-bit to 64-bit systems */
typedef struct {
    vb_blk block;
    td_u32 supplement;
} ext_vb_compat_video_supplement_info;

typedef struct {
    td_u32 pool_id;
    td_u32 blk_cnt;
    td_u64 blk_size;
    td_u64 pool_size;
    td_u64 pool_handle;
    td_u64 pool_phy_addr;
    td_u64 pool_vir_addr;
} ext_vb_pool_info;

/* dummy struct, used only in 32-bit to 64-bit systems */
typedef struct {
    td_u32 pool_id;
    td_u32 blk_cnt;
    td_u64 blk_size;
    td_u64 pool_size;
    td_u64 pool_handle;
    td_u64 pool_phy_addr;
    td_u32 pool_vir_addr;
} ext_vb_compat_pool_info;

typedef struct {
    td_mem_handle_t handle;
    td_u64 phy_addr;
    td_u32 size;
} ext_vb_mem_info;

#define VB_SUPPLEMENT_JPEG_MASK        0x1
#define VB_SUPPLEMENT_ISPINFO_MASK     0x2
#define VB_SUPPLEMENT_MOTION_DATA_MASK 0x4
#define VB_SUPPLEMENT_DNG_MASK         0x8
#define VB_SUPPLEMENT_SHARPEN_MASK     0x10
#define VB_SUPPLEMENT_SAVE_REG_MASK    0x20
#define VB_SUPPLEMENT_FSD_MASK         0x40

#define VB_VIPROC_REG_SIZE  0x3000
#define VB_PPU_REG_SIZE     0xb000
#define VB_PPU_FSD_SIZE     0x1000
#define VB_SUPPLEMENT_SAVE_REG_SIZE    (VB_VIPROC_REG_SIZE + VB_PPU_REG_SIZE)

typedef struct {
    td_u32 supplement_config;
} ext_vb_supplement_config;

typedef struct {
    td_u32 vb_size;

    td_u32 head_stride;
    td_u32 head_size;
    td_u32 head_y_size;

    td_u32 main_stride;
    td_u32 main_size;
    td_u32 main_y_size;

    td_u32 ext_stride;
    td_u32 ext_y_size;
} ext_vb_out_params;

/**
 * ext_vb_in_params - vb buffer size结构体描述;
 * @width     : 图像宽度;
 * @height    : 图像高度;
 * @pix_fmt   : 像素格式;
 * @cmp_mode  : 压缩模式;
 * @cmp_ratio : 压缩率;
 * @align     : 图像stride对齐;
 *
 * note:
 * 1.支持RAW8/10/12/14/16, NONE/LINE/FRAME压缩模式
 * 2.支持YUV/YVU 400/420/444, NONE/SEG压缩模式
 * 3.支持RGB8/10/12/14/16, NONE压缩模式
 * 4.cmp_ratio = 0, 使用内部参数
 * 5.align = 0, 使用内部参数
 */
typedef struct {
    td_u32                width;
    td_u32                height;
    ext_isp_pixel_format  pix_fmt;
    ext_isp_data_bitwidth bit_width;
    ext_isp_compress_mode cmp_mode;
    ext_isp_video_format  video_format;
    td_u32                cmp_ratio;
    td_u32                align;
} ext_vb_in_params;

/**
 * ext_vb_memory_type - VB内存类型;
 * @EXT_VB_MEMORY_PRIVATE: 直接物理内存访问类型;
 * @EXT_VB_MEMORY_MMZ:     MMZ(CMA)内存类型;
 * @EXT_VB_MEMORY_SMMU:    SMMU(IOMMU)内存类型;
 *
 */
typedef enum {
    EXT_VB_MEMORY_PRIVATE       = 1,
    EXT_VB_MEMORY_MMZ           = 2,
    EXT_VB_MEMORY_SMMU          = 3,
    EXT_VB_MEMORY_UNKNOWN       = 4,
} ext_vb_memory_type;

/**
 * ext_vb_requestbuffers - 用户请求buf结构体描述;
 * @count:                 需要申请的vb buf数目;
 * @memory:                需要申请的vb buf内存类型;
 */
typedef struct {
    td_u32             count;
    ext_vb_memory_type memory;
} ext_vb_requestbuffers;

/**
 * ext_vb_supplement - 图像metadata信息;
 * @jpeg_dcf:          图像JPEG DCF相关信息;
 * @alg_info:          图像ISP算法相关信息;
 * @motion_data:       图像运动偏移信息;
 * @dng_info:          图像DNG信息;
 *
 */
typedef struct {
    ext_isp_jpeg_dcf               jpeg_dcf;     /* used in JPEG DCF */
    ext_isp_frame_alg_info         alg_info;     /* used to obtain ISP info, when get raw and send raw */
    ext_isp_motion_data            motion_data;  /* vpss 3dnr use: gme motion data, filter motion data, gyro data. */
    ext_isp_dng_image_dynamic_info dng_info;     /* used in raw, DNG info */
} ext_vb_supplement;

/**
 * ext_vb_video - 视频帧信息;
 * @pts:           时间戳信息, 输出参数;
 * @sequence:      帧索引计数, 输出参数;
 * @width:         图像宽度, 输出参数;
 * @height:        图像高度, 输出参数;
 * @field:         图像顶底场信息, 输出参数
 * @piexl_format:  图像像素格式，输出参数;
 * @video_format:  图像排布格式, 输出参数;
 * @compress_mode: 图像压缩格式, 输出参数;
 * @dynamic_range: 图像动态范围格式, 输出参数;
*
*/
typedef struct {
    td_u64                 pts;
    td_u32                 sequence;
    td_u32                 width;
    td_u32                 height;
    ext_isp_video_field        field;
    ext_isp_pixel_format       pixel_format;
    ext_isp_video_format       video_format;
    ext_isp_compress_mode      compress_mode;
    ext_isp_dynamic_range      dynamic_range;
    ext_isp_color_gamut        color_gamut;
} ext_vb_video;

#define EXT_VB_BUF_FLAG_NORMAL   			(0x00000000)
#define EXT_VB_BUF_FLAG_IN_FENCE			(0x00000002)
#define EXT_VB_BUF_FLAG_OUT_FENCE			(0x00000004)
#define EXT_VB_BUF_FLAG_ERROR   			(0x00000008)

/**
 * ext_vb_buffer - 外部帧存结构体描述;
 * @index:         buffer索引, 输入参数;
 * @memory:        buffer内存类型, 输入参数;
 * @fd:            DMABUF内存类型的文件描述fd, 输入参数;
 * @phyaddr:       平台系统内存物理地址(如MMZ或SMMU内存物理地址), 输入参数;
 * @fence_fd:      fence同步fd描述符，代表in_fence或out_fence, 通过flags区分;
 * @flags:         区分是in_fence或out_fence, 定义为EXT_VB_BUF_FLAG_XXX;
 * @length:        外部帧存总大小, 输入参数;
 * @supplement_need: 是否需要拷贝supplement信息
 * @video:         图像综合信息, 输出参数;
 * @supplemnt:     图像附加信息, 输出参数;
 */
typedef struct {
    td_u32               index;

    /* memory location */
    ext_vb_memory_type   memory;
    union {
        td_u64           phyaddr;
        td_s64           mem_handle;
    } m;
    td_u32               length;
    td_s32               fence_fd;

    td_u32               flags;
    td_bool              supplement_need;

    /* vid frame info description */
    ext_vb_video         video;
    ext_vb_supplement    supplement;
} ext_vb_user_buffer;

typedef struct {
    soc_mod_id    mod_id;
    td_s32      dev_id;
    td_s32      chn_id;
} ext_mpp_chn;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DRV_VB_STRUCT_H__ */

