/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description : function define
 * Author      : image
 * Create      : 2019-04-12
 */

#ifndef __DRV_VIDEO_EXT_H__
#define __DRV_VIDEO_EXT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum {
    EXT_DRV_SOURCE_DTV = 0,
    EXT_DRV_SOURCE_MEDIA,
    EXT_DRV_SOURCE_ATV,
    EXT_DRV_SOURCE_SCART,
    EXT_DRV_SOURCE_SVIDEO,
    EXT_DRV_SOURCE_CVBS,
    EXT_DRV_SOURCE_VGA,
    EXT_DRV_SOURCE_YPBPR,
    EXT_DRV_SOURCE_HDMI,
    EXT_DRV_SOURCE_GRAPHIC,
    EXT_DRV_SOURCE_NETWORK,
    EXT_DRV_SOURCE_CAST,

    EXT_DRV_SOURCE_MAX
} ext_drv_source;

typedef enum {
    EXT_DRV_MODULE_VDEC = 0,
    EXT_DRV_MODULE_VPP,
    EXT_DRV_MODULE_BUTT
} ext_drv_module_type;

typedef enum {
    /* RGB formats */
    EXT_DRV_PIXEL_FMT_RGB332 = 0, /*  8  RGB-3-3-2     */
    EXT_DRV_PIXEL_FMT_RGB444,     /* 16  xxxxrrrr ggggbbbb */
    EXT_DRV_PIXEL_FMT_RGB555,     /* 16  RGB-5-5-5     */
    EXT_DRV_PIXEL_FMT_RGB565,     /* 16  RGB-5-6-5     */
    EXT_DRV_PIXEL_FMT_BGR565,     /* 16  RGB-5-6-5     */
    EXT_DRV_PIXEL_FMT_RGB555X,    /* 16  RGB-5-5-5 BE  */
    EXT_DRV_PIXEL_FMT_RGB565X,    /* 16  RGB-5-6-5 BE  */
    EXT_DRV_PIXEL_FMT_BGR666,     /* 18  BGR-6-6-6   */
    EXT_DRV_PIXEL_FMT_BGR24,      /* 24  BGR-8-8-8     */
    EXT_DRV_PIXEL_FMT_RGB24,      /* 24  RGB-8-8-8     */
    EXT_DRV_PIXEL_FMT_BGR32,      /* 32  BGR-8-8-8-8   */
    EXT_DRV_PIXEL_FMT_RGB32,      /* 32  RGB-8-8-8-8   */

    /* SOC CLUT formats */
    EXT_DRV_PIXEL_FMT_CLUT_1BPP,
    EXT_DRV_PIXEL_FMT_CLUT_2BPP,
    EXT_DRV_PIXEL_FMT_CLUT_4BPP,
    EXT_DRV_PIXEL_FMT_CLUT_8BPP,
    EXT_DRV_PIXEL_FMT_ACLUT_44,
    EXT_DRV_PIXEL_FMT_ACLUT_88,

    /* SOC 16bit RGB formats */
    EXT_DRV_PIXEL_FMT_ARGB4444,
    EXT_DRV_PIXEL_FMT_ABGR4444,
    EXT_DRV_PIXEL_FMT_RGBA4444,

    EXT_DRV_PIXEL_FMT_ARGB1555,
    EXT_DRV_PIXEL_FMT_ABGR1555,
    EXT_DRV_PIXEL_FMT_RGBA5551,

    /* SOC 24bit RGB formats */
    EXT_DRV_PIXEL_FMT_ARGB8565,
    EXT_DRV_PIXEL_FMT_ABGR8565,
    EXT_DRV_PIXEL_FMT_RGBA5658,

    EXT_DRV_PIXEL_FMT_ARGB6666,
    EXT_DRV_PIXEL_FMT_RGBA6666,

    /* SOC 32bit RGB formats */
    EXT_DRV_PIXEL_FMT_ARGB8888,
    EXT_DRV_PIXEL_FMT_ABGR8888,
    EXT_DRV_PIXEL_FMT_RGBA8888,

    EXT_DRV_PIXEL_FMT_AYUV8888,
    EXT_DRV_PIXEL_FMT_YUVA8888,

    /* grey formats */
    EXT_DRV_PIXEL_FMT_GREY, /*  8  greyscale     */
    EXT_DRV_PIXEL_FMT_Y4,   /*  4  greyscale     */
    EXT_DRV_PIXEL_FMT_Y6,   /*  6  greyscale     */
    EXT_DRV_PIXEL_FMT_Y10,  /* 10  greyscale     */
    EXT_DRV_PIXEL_FMT_Y12,  /* 12  greyscale     */
    EXT_DRV_PIXEL_FMT_Y16,  /* 16  greyscale     */

    /* grey bit-packed formats */
    EXT_DRV_PIXEL_FMT_Y10BPACK, /* 10  greyscale bit-packed */

    /* palette formats */
    EXT_DRV_PIXEL_FMT_PAL8, /*  8  8-bit palette */

    /* luminance+chrominance formats */
    EXT_DRV_PIXEL_FMT_YVU410,  /*  9  YVU 4:1:0     */
    EXT_DRV_PIXEL_FMT_YVU420,  /* 12  YVU 4:2:0     */
    EXT_DRV_PIXEL_FMT_YUYV,    /* 16  YUV 4:2:2     */
    EXT_DRV_PIXEL_FMT_YYUV,    /* 16  YUV 4:2:2     */
    EXT_DRV_PIXEL_FMT_YVYU,    /* 16  YVU 4:2:2 */
    EXT_DRV_PIXEL_FMT_UYVY,    /* 16  YUV 4:2:2     */
    EXT_DRV_PIXEL_FMT_VYUY,    /* 16  YUV 4:2:2     */
    EXT_DRV_PIXEL_FMT_YUV422P, /* 16  YVU422 planar */
    EXT_DRV_PIXEL_FMT_YUV411P, /* 16  YVU411 planar */
    EXT_DRV_PIXEL_FMT_Y41P,    /* 12  YUV 4:1:1     */
    EXT_DRV_PIXEL_FMT_YUV444,  /* 16  xxxxyyyy uuuuvvvv */
    EXT_DRV_PIXEL_FMT_YUV555,  /* 16  YUV-5-5-5     */
    EXT_DRV_PIXEL_FMT_YUV565,  /* 16  YUV-5-6-5     */
    EXT_DRV_PIXEL_FMT_YUV32,   /* 32  YUV-8-8-8-8   */
    EXT_DRV_PIXEL_FMT_YUV410,  /*  9  YUV 4:1:0     */
    EXT_DRV_PIXEL_FMT_YUV420,  /* 12  YUV 4:2:0     */
    EXT_DRV_PIXEL_FMT_HI240,   /*  8  8-bit color   */
    EXT_DRV_PIXEL_FMT_HM12,    /*  8  YUV 4:2:0 16x16 macroblocks */
    EXT_DRV_PIXEL_FMT_M420,    /* 12  YUV 4:2:0 2 lines y, 1 line uv interleaved */

    /* two planes -- one Y, one cr + cb interleaved  */
    EXT_DRV_PIXEL_FMT_NV08,     /* 08  Y/cb_cr 4:0:0 @ */
    EXT_DRV_PIXEL_FMT_NV80,     /* 08  Y/cr_cb 4:0:0 @ */
    EXT_DRV_PIXEL_FMT_NV12,     /* 12  Y/cb_cr 4:2:0  */
    EXT_DRV_PIXEL_FMT_NV21,     /* 12  Y/cr_cb 4:2:0  */
    EXT_DRV_PIXEL_FMT_NV12_411, /* 12  Y/cb_cr 4:1:1  @ */
    EXT_DRV_PIXEL_FMT_NV16,     /* 16  Y/cb_cr 4:2:2  */
    EXT_DRV_PIXEL_FMT_NV61,     /* 16  Y/cr_cb 4:2:2  */
    EXT_DRV_PIXEL_FMT_NV16_2X1, /* 16  Y/cb_cr 4:2:2 2X1 @ */
    EXT_DRV_PIXEL_FMT_NV61_2X1, /* 16  Y/cr_cb 4:2:2  2X1 @ */
    EXT_DRV_PIXEL_FMT_NV24,     /* 24  Y/cb_cr 4:4:4  */
    EXT_DRV_PIXEL_FMT_NV42,     /* 24  Y/cr_cb 4:4:4  */
    EXT_DRV_PIXEL_FMT_NV42_RGB, /* 24  G/RB 4:4:4  */

    /* two non contiguous planes - one Y, one cr + cb interleaved  */
    EXT_DRV__PIXEL_FMT_NV12M,  /* 12  Y/cb_cr 4:2:0  */
    EXT_DRV__PIXEL_FMT_NV12MT, /* 12  Y/cb_cr 4:2:0 64x32 macroblocks */

    /* three non contiguous planes - Y, cb, cr */
    EXT_DRV_PIXEL_FMT_YUV420M, /* 12  YUV420 planar */

    /* bayer formats */
    EXT_DRV_PIXEL_FMT_SBGGR8,  /*  8  BGBG.. GRGR.. */
    EXT_DRV_PIXEL_FMT_SGBRG8,  /*  8  GBGB.. RGRG.. */
    EXT_DRV_PIXEL_FMT_SGRBG8,  /*  8  GRGR.. BGBG.. */
    EXT_DRV_PIXEL_FMT_SRGGB8,  /*  8  RGRG.. GBGB.. */
    EXT_DRV_PIXEL_FMT_SBGGR10, /* 10  BGBG.. GRGR.. */
    EXT_DRV_PIXEL_FMT_SGBRG10, /* 10  GBGB.. RGRG.. */
    EXT_DRV_PIXEL_FMT_SGRBG10, /* 10  GRGR.. BGBG.. */
    EXT_DRV_PIXEL_FMT_SRGGB10, /* 10  RGRG.. GBGB.. */
    EXT_DRV_PIXEL_FMT_SBGGR12, /* 12  BGBG.. GRGR.. */
    EXT_DRV_PIXEL_FMT_SGBRG12, /* 12  GBGB.. RGRG.. */
    EXT_DRV_PIXEL_FMT_SGRBG12, /* 12  GRGR.. BGBG.. */
    EXT_DRV_PIXEL_FMT_SRGGB12, /* 12  RGRG.. GBGB.. */

    /* SOC three non contiguous planes - Y, cb, cr */
    EXT_DRV_PIXEL_FMT_YUV400,     /* 08  YUV400 planar @ */
    EXT_DRV_PIXEL_FMT_YUV410P,    /* 10  YUV410 planar @ */
    EXT_DRV_PIXEL_FMT_YUV420P,    /* 12  YUV420 planar @ */
    EXT_DRV_PIXEL_FMT_YUV411,     /* 12  YUV411  planar @ */
    EXT_DRV_PIXEL_FMT_YUV422_1X2, /* 16  YUV422  planar 1X2 @ */
    EXT_DRV_PIXEL_FMT_YUV422_2X1, /* 16  YUV422  planar 2X1@ */
    EXT_DRV_PIXEL_FMT_YUV_444,    /* 24  YUV444  planar @ */

    /* SOC three non contiguous planes - Y, cb, cr */
    EXT_DRV_PIXEL_MAX
} ext_drv_pixel_format;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* __DRV_VIDEO_EXT_H__ */
