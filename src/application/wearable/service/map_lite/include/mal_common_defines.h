/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-11
 */

/**
 * MAL层公共结构 awk.h mal_defines.h
 */
#ifndef _MAL_COMMON_DEFINES_H
#define _MAL_COMMON_DEFINES_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 图像像素类型
 */
typedef enum {
    MAL_PIXEL_MODE_GREY,  //!< 8位灰度图，每一像素占用1个字节
    MAL_PIXEL_MODE_RGB_332,  //!< 8位RGB图，每一像素占用1个字节，字节顺序：高3位为红色，中3位为绿色，低2位为蓝色
    MAL_PIXEL_MODE_RGB_565,  //!< 16位RGB图，每一像素占用2个字节，字节顺序：字节0高3位绿色，低5位为蓝色；字节1低3位为绿色，高5位为红色
    MAL_PIXEL_MODE_RGB_888,  //!< 24位RGB图，每一像素占用3个字节，字节顺序：字节0为红色，字节1为绿色，字节2为蓝色
    MAL_PIXEL_MODE_ARGB_8888,  //!< 32位RGB图，每一像素占用4个字节，字节顺序：字节0为Alpha,
                               //!< 字节1为红色，字节2为绿色，字节3为蓝色
    MAL_PIXEL_MODE_RGBA_8888,  //!< 32位RGB图，每一像素占用4个字节，字节顺序：字节0为红色，字节1为绿色，字节2为蓝色，字节3为Alpha
    MAL_PIXEL_MODE_BGR_233,  //!< 8位BGR图，每一像素占用1个字节，字节顺序：高2位为蓝色，中3位为绿色，低3位为红色
    MAL_PIXEL_MODE_BGR_565,  //!< 16位BGR图，每一像素占用2个字节，字节顺序：字节0低3位为绿色，高5位为蓝色；字节1高3位为绿色，低5位为红色
    MAL_PIXEL_MODE_BGR_888,  //!< 24位BGR图，每一像素占用3个字节，字节顺序：字节0为蓝色，字节1为绿色，字节2为红色
    MAL_PIXEL_MODE_BGRA_8888,  //!< 32位BGR图，每一像素占用4个字节，字节顺序：字节0为蓝色，字节1为绿色，字节2为红色，字节3为Alpha
    MAL_PIXEL_MODE_ABGR_8888,  //!< 32位BGR图，每一像素占用4个字节，字节顺序：字节0为Alpha，字节1为蓝色，字节2为绿色，字节3为红色
} mal_pixel_mode_t;

/**
 * @brief 画布点坐标，原点是以左上角为(0,0)
 */
typedef struct _mal_point_t {
    int32_t x;  //!< x坐标
    int32_t y;  //!< y坐标
} mal_point_t;

/**
 * @brief 矩形区域定义
 */
typedef struct _mal_rect_area_t {
    int32_t x;       //!< x坐标
    int32_t y;       //!< y坐标
    int32_t width;   //!< 区域宽度, 单位：像素
    int32_t height;  //!< 区域高度, 单位：像素
} mal_rect_area_t;

/**
 * @brief SVG定义
 */
typedef struct _mal_svg_viewbox_t {
    uint32_t x;       //!< x坐标
    uint32_t y;       //!< y坐标
    uint32_t width;   //!< 区域宽度, 单位：像素
    uint32_t height;  //!< 区域高度, 单位：像素
} mal_svg_viewbox_t;

/**
 * @brief 位图定义
 */
typedef struct _mal_bitmap_t {
    mal_pixel_mode_t pixel_mode;  //!< 位图格式
    uint8_t *buffer;              //!< 位图buffer内容
    uint32_t buffer_size;         //!< 位图buffer大小
    uint32_t width;               //!< 位图宽度, 单位：像素
    uint32_t height;              //!< 位图高度, 单位：像素
    uint32_t stride;              //!< 每像素需要的字节数，如ARGB8888为4字节，RGB888为3字节，RGB565为2字节，GREY图为1字节
    bool pre_multiplied;          //!< 是否已经预乘alpha值，只有ARGB8888才有效
    mal_svg_viewbox_t view_box;   //!< viewbox结构体
} mal_bitmap_t;

/**
 * @brief 图像格式类型
 */
typedef enum {
    MAL_IMAGE_FORMAT_JPEG = 0,  //!< 二进制为jpeg格式
} mal_image_format_t;

/**
 * @brief 位图定义
 */
typedef struct _mal_image_buf_info_t {
    mal_image_format_t image_format;  //!< 图像内容格式
    uint8_t *buffer;                  //!< 图像buffer内容
    uint32_t buffer_size;             //!< 图像buffer大小
} mal_image_buf_info_t;

typedef enum {
    MAL_API_SUCCESS = 0,        // 成功
    MAL_API_NO_INIT = -1,       // 没有初始化
    MAL_API_NULL_PARAM = -2,    // 传入参数为NULL
    MAL_API_THREAD_ERROR = -3,  // 初始化的线程和当前调用不一致
} mal_api_result_t;

#ifdef __cplusplus
}
#endif

#endif
