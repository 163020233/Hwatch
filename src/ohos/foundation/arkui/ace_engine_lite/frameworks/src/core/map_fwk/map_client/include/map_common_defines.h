/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: map common defines.
 * Author: CompanyName
 * Create: 2026-02-27
 */
// 公共层结构体
#ifndef MAP_COMMON_DEFINES_H
#define MAP_COMMON_DEFINES_H

namespace OHOS {
namespace ACELite {
/**
 * @brief 画布点坐标，原点是以左上角为(0,0)
 */
typedef struct {
    int32_t x;  //!< x坐标
    int32_t y;  //!< y坐标
} map_point;

/**
 * @brief 矩形区域定义
 */
typedef struct {
    int32_t x;       //!< x坐标
    int32_t y;       //!< y坐标
    int32_t width;   //!< 区域宽度, 单位：像素
    int32_t height;  //!< 区域高度, 单位：像素
} map_rect_area;

/**
 * @brief 图像像素类型
 */
typedef enum {
    MAP_PIXEL_MODE_GREY,  //!< 8位灰度图，每一像素占用1个字节
    MAP_PIXEL_MODE_RGB_332,  //!< 8位RGB图，每一像素占用1个字节，字节顺序：高3位为红色，中3位为绿色，低2位为蓝色
    MAP_PIXEL_MODE_RGB_565,  //!< 16位RGB图，每一像素占用2个字节，字节顺序：字节0高3位绿色，低5位为蓝色；字节1低3位为绿色，高5位为红色
    MAP_PIXEL_MODE_RGB_888,  //!< 24位RGB图，每一像素占用3个字节，字节顺序：字节0为红色，字节1为绿色，字节2为蓝色
    MAP_PIXEL_MODE_ARGB_8888,  //!< 32位RGB图，每一像素占用4个字节，字节顺序：字节0为Alpha,
                               //!< 字节1为红色，字节2为绿色，字节3为蓝色
    MAP_PIXEL_MODE_RGBA_8888,  //!< 32位RGB图，每一像素占用4个字节，字节顺序：字节0为红色，字节1为绿色，字节2为蓝色，字节3为Alpha
    MAP_PIXEL_MODE_BGR_233,  //!< 8位BGR图，每一像素占用1个字节，字节顺序：高2位为蓝色，中3位为绿色，低3位为红色
    MAP_PIXEL_MODE_BGR_565,  //!< 16位BGR图，每一像素占用2个字节，字节顺序：字节0低3位为绿色，高5位为蓝色；字节1高3位为绿色，低5位为红色
    MAP_PIXEL_MODE_BGR_888,  //!< 24位BGR图，每一像素占用3个字节，字节顺序：字节0为蓝色，字节1为绿色，字节2为红色
    MAP_PIXEL_MODE_BGRA_8888,  //!< 32位BGR图，每一像素占用4个字节，字节顺序：字节0为蓝色，字节1为绿色，字节2为红色，字节3为Alpha
    MAP_PIXEL_MODE_ABGR_8888,  //!< 32位BGR图，每一像素占用4个字节，字节顺序：字节0为Alpha，字节1为蓝色，字节2为绿色，字节3为红色
} map_pixel_mode;

/**
 * @brief 位图定义
 */
typedef struct {
    map_pixel_mode pixel_mode;  //!< 位图格式
    uint8_t *buffer;            //!< 位图buffer内容
    uint32_t buffer_size;       //!< 位图buffer大小
    uint32_t width;             //!< 位图宽度, 单位：像素
    uint32_t height;            //!< 位图高度, 单位：像素
    uint32_t stride;  //!< 每行的字节数，包含可能的填充，例如ARGB8888为(width * 4)对齐后的实际行字节数
    bool pre_multiplied;  //!< 是否已经预乘alpha值，只有ARGB8888才有效
} map_bitmap;

/**
 * @brief 图像格式类型
 */
typedef enum {
    MAP_IMAGE_FORMAT_JPEG = 0,  //!< 二进制为jpeg格式
} map_image_format;

/**
 * @brief 位图定义
 */
typedef struct {
    map_image_format image_format;  //!< 图像内容格式
    uint8_t *buffer;                //!< 图像buffer内容
    uint32_t buffer_size;           //!< 图像buffer大小
    uint32_t width;                 //!< 图像宽度, 单位：像素
    uint32_t height;                //!< 图像高度, 单位：像素
} map_image_buf_info;

/**
 * @brief 返回结果
 */

typedef enum {
    MAP_API_SUCCESS = 0,        // 成功
    MAP_API_NO_INIT = -1,       // 没有初始化
    MAP_API_NULL_PARAM = -2,    // 传入参数为NULL
    MAP_API_THREAD_ERROR = -3,  // 初始化的线程和当前调用不一致
} map_api_result;

/**
 * @brief 2d经纬度坐标结构
 */
typedef struct {
    double lon;  //!< 经度
    double lat;  //!< 纬度
} map_coord2d;

}  // namespace ACELite
}  // namespace OHOS
#endif  // MAP_COMMON_DEFINES_H
