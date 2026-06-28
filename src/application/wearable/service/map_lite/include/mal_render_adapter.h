
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-11
 */

#ifndef _MAL_RENDER_API_H
#define _MAL_RENDER_API_H
#include <stdint.h>
#include "mal_common_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *    绘制相关适配层
 *********************/
/**
 * @brief 文字排布的对齐样式
 */
typedef enum {
    MAL_ALIGN_STYLE_NONE,
    MAL_ALIGN_STYLE_CENTER,
    MAL_ALIGN_STYLE_LEFT,
    MAL_ALIGN_STYLE_RIGHT
} mal_align_style_t;

/**
 * @brief 字体粗细样式
 */
typedef enum {
    MAL_FONT_WEIGHT_NORMAL,
    MAL_FONT_WEIGHT_THIN,
    MAL_FONT_WEIGHT_BOLD,
} mal_font_weight_t;

/**
 * @brief 文字样式
 */
typedef struct _mal_text_style_t {
    mal_align_style_t align;
    uint32_t font_size;
    mal_font_weight_t font_weight;
} mal_text_style_t;

/**
 * @brief 填充类型
 */
typedef enum {
    MAL_FILL_STYLE_DRAWING_ONLY,        // 仅填充几何图形
    MAL_FILL_STYLE_DRAWING_AND_STROKE,  // 填充几何图形和描边
    MAL_FILL_STYLE_STROKE_ONLY          // 仅填充描边
} mal_fill_style_t;

/**
 * @brief 虚线定义，单位：像素
 */
typedef struct _mal_dash_style_t {
    int32_t painted_length;    //!< 绘制长度
    int32_t unpainted_length;  //!< 空白长度
    int32_t offset;            //!< 起始位置
} mal_dash_style_t;

/**
 * @brief 笔刷样式
 */
typedef struct _mal_paint_style_t {
    uint32_t width;               //!< 画笔宽度, 单位：像素
    uint32_t color;               //!< 画笔颜色, ARGB
    float angle;                  //!< 旋转角度，[0-360)，正北为0，逆时针旋转
    mal_text_style_t text_style;  //!< 文字样式
    mal_fill_style_t fill_style;  // 填充样式
    mal_dash_style_t dash_style;  //!< 虚线样式
} mal_paint_style_t;

/**
 * @brief 绘制上下文
 */
typedef struct _mal_render_context_t {
    int32_t width;                  //!< 画布宽度, 单位：像素
    int32_t height;                 //!< 画布高度, 单位：像素
    mal_rect_area_t map_view_rect;  //!< 地图view的rect区域
} mal_render_context_t;

/**
 * @brief 绘制相关适配接口， 下层是可能是C/C++回调，所以此处只能定义C兼容接口
 */
typedef struct _mal_render_adapter_t mal_render_adapter_t;
struct _mal_render_adapter_t {
    /**
     * @brief 开始绘制时回调方法
     * @param {map_id} 地图实例id
     * @param {mal_map_status_t} status
     * @return {void}
     */
    void (*begin_drawing)(uint32_t map_id, mal_render_context_t status);

    /**
     * @brief 绘制结束时回调方方法
     * @param {map_id} 地图实例id
     * @return {void}
     */
    void (*commit_drawing)(uint32_t map_id);

    /**
     * @brief 需要绘制点时回调
     * @param {map_id} 地图实例id
     * @param {mal_point_t} *point 点的数据
     * @param {uint32_t} point_size 点的个数
     * @param {mal_paint_style_t} *style 绘制样式
     * @return {void}
     */
    void (*draw_point)(uint32_t map_id, mal_point_t *point, uint32_t point_size, const mal_paint_style_t *style);

    /**
     * @brief 需要绘制线时回调
     * @param {map_id} 地图实例id
     * @param {mal_point_t} *points 线上点的结合
     * @param {uint32_t} point_size 点的数量
     * @param {mal_paint_style_t} *style 绘制样式
     * @return {void}
     */
    void (*draw_polyline)(uint32_t map_id, mal_point_t *points, uint32_t point_size, const mal_paint_style_t *style);

    /**
     * @brief 需要绘制面时回调
     * @param {map_id} 地图实例id
     * @param {mal_point_t} *points 面的点集合
     * @param {uint32_t} point_size 点的数量
     * @param {mal_paint_style_t} *style 绘制样式
     * @return {void}
     */
    void (*draw_polygon)(uint32_t map_id, mal_point_t *points, uint32_t point_size, const mal_paint_style_t *style);

    /**
     * @brief 需要绘制bitmap时回调
     * @param {map_id} 地图实例id
     * @param {mal_rect_area_t} area 区域
     * @param {mal_bitmap_t} bitmap bitmap信息
     * @param {mal_paint_style_t} *style 样式
     * @return {void}
     */
    void (*draw_bitmap)(uint32_t map_id, mal_rect_area_t area, mal_bitmap_t bitmap, const mal_paint_style_t *style);

    /**
     * @brief 需要绘制图像二进制时回调
     * @param {map_id} 地图实例id
     * @param {mal_rect_area_t} area 区域
     * @param {mal_image_buf_info_t} image_info 图像信息
     * @param {mal_paint_style_t} *style 样式
     * @return {void}
     */
    void (*draw_image_buffer)(uint32_t map_id, mal_rect_area_t area, const mal_image_buf_info_t *image_info,
                              const mal_paint_style_t *style);

    /**
     * @brief 需要绘制文字时回调
     * @param {map_id} 地图实例id
     * @param {mal_point_t} center 文字中心位置
     * @param {char} *text 文字内容
     * @param {mal_paint_style_t} *style 绘制样式
     * @return {void}
     */
    void (*draw_text)(uint32_t map_id, mal_point_t center, const char *text, const mal_paint_style_t *style);

    /**
     * @brief 需要绘制颜色时回调
     * @param {map_id} 地图实例id
     * @param {mal_rect_area_t} area 区域
     * @param {mal_paint_style_t} *style 样式
     * @return {void}
     */
    void (*draw_color)(uint32_t map_id, mal_rect_area_t area, const mal_paint_style_t *style);

    /**
     * @brief 测量文字宽度
     * @param {map_id} 地图实例id
     * @param text 字符串
     * @param style 文字绘制样式
     * @param width 文字的宽度
     * @param ascender 文字的上行高度
     * @param descender 文字的下行高度
     */
    bool (*measure_text)(uint32_t map_id, const char *text, const mal_paint_style_t *style, int32_t *width,
                         int32_t *ascender, int32_t *descender);

    /**
     * @brief 需要绘制svg bitmap时回调
     * @param {map_id} 地图实例id
     * @param {mal_rect_area_t} area 区域
     * @param {mal_bitmap_t} bitmap bitmap信息
     * @return {void}
     */
    void (*draw_svg_bitmap)(uint32_t map_id, mal_rect_area_t area, mal_bitmap_t bitmap, const mal_paint_style_t *style);
};

/**
 * @brief 地图渲染回调
 */
typedef struct _mal_map_render_callback_t mal_map_render_callback_t;
struct _mal_map_render_callback_t {
    /**
     * @brief 点覆盖物开始绘制回调
     * @param {uint32_t} map_id 地图实例id
     * @param {int32_t} guid 覆盖物id
     * @return {bool} 如果允许绘制，则返回true，否则返回false
     */
    bool (*on_point_begin_draw)(uint32_t map_id, int32_t guid);

    /**
     * @brief 点覆盖物结束绘制回调
     * @param {uint32_t} map_id 地图实例id
     * @param {int32_t} guid 覆盖物id
     * @param {int32_t} status 绘制成功返回0，否则返回错误码
     * @return {void}
     */
    void (*on_point_end_draw)(uint32_t map_id, int32_t guid, int32_t status);

    /**
     * @brief 线覆盖物开始绘制回调
     * @param {uint32_t} map_id 地图实例id
     * @param {int32_t} guid 覆盖物id
     * @return {bool} 如果允许绘制，则返回true，否则返回false
     */
    bool (*on_line_begin_draw)(uint32_t map_id, int32_t guid);

    /**
     * @brief 线覆盖物结束绘制回调
     * @param {uint32_t} map_id 地图实例id
     * @param {int32_t} guid 覆盖物id
     * @param {int32_t} status 绘制成功返回0，否则返回错误码
     * @return {void}
     */
    void (*on_line_end_draw)(uint32_t map_id, int32_t guid, int32_t status);

    /**
     * @brief 面覆盖物开始绘制回调
     * @param {uint32_t} map_id 地图实例id
     * @param {int32_t} guid 覆盖物id
     * @return {bool} 如果允许绘制，则返回true，否则返回false
     */
    bool (*on_polygon_begin_draw)(uint32_t map_id, int32_t guid);

    /**
     * @brief 面覆盖物结束绘制回调
     * @param {uint32_t} map_id 地图实例id
     * @param {int32_t} guid 覆盖物id
     * @param {int32_t} status 绘制成功返回0，否则返回错误码
     * @return {void}
     */
    void (*on_polygon_end_draw)(uint32_t map_id, int32_t guid, int32_t status);

    /**
     * @brief tile开始绘制回调
     * @param {uint32_t} map_id 地图实例id
     * @param {uint32_t} tile_x tile的x坐标(P20坐标)
     * @param {uint32_t} tile_y tile的y坐标(P20坐标)
     * @param {uint32_t} zoom 地图缩放级别
     * @return {bool} 如果允许绘制，则返回true，否则返回false
     */
    bool (*on_tile_begin_draw)(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom);

    /**
     * @brief tile结束绘制回调
     * @param {uint32_t} map_id 地图实例id
     * @param {uint32_t} tile_x tile的x坐标(P20坐标)
     * @param {uint32_t} tile_y tile的y坐标(P20坐标)
     * @param {uint32_t} zoom 地图缩放级别
     * @param {int32_t} status 绘制成功返回0，否则返回错误码
     * @return {void}
     */
    void (*on_tile_end_draw)(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom, int32_t status);

    /**
     * @brief tile开始绘制回调
     * @param {uint32_t} map_id 地图实例id
     * @param {uint32_t} tile_x tile的x坐标(P20坐标)
     * @param {uint32_t} tile_y tile的y坐标(P20坐标)
     * @param {uint32_t} zoom 地图缩放级别
     * @return {bool} 如果允许绘制，则返回true，否则返回false
     */
    bool (*on_poi_begin_draw)(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom);

    /**
     * @brief tile结束绘制回调
     * @param {uint32_t} map_id 地图实例id
     * @param {uint32_t} tile_x tile的x坐标(P20坐标)
     * @param {uint32_t} tile_y tile的y坐标(P20坐标)
     * @param {uint32_t} zoom 地图缩放级别
     * @param {int32_t} status 绘制成功返回0，否则返回错误码
     * @return {void}
     */
    void (*on_poi_end_draw)(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom, int32_t status);

    /**
     * @brief 图片二进制开始绘制回调
     * @param {uint32_t} map_id 地图实例id
     * @param {uint32_t} tile_x tile的x坐标(P20坐标)
     * @param {uint32_t} tile_y tile的y坐标(P20坐标)
     * @param {uint32_t} zoom 地图缩放级别
     * @return {bool} 如果允许绘制，则返回true，否则返回false
     */
    bool (*on_img_bin_begin_draw)(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom);

    /**
     * @brief 图片二进制结束绘制回调
     * @param {uint32_t} map_id 地图实例id
     * @param {uint32_t} tile_x tile的x坐标(P20坐标)
     * @param {uint32_t} tile_y tile的y坐标(P20坐标)
     * @param {uint32_t} zoom 地图缩放级别
     * @param {int32_t} status 绘制成功返回0，否则返回错误码
     * @return {void}
     */
    void (*on_img_bin_end_draw)(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom, int32_t status);

    /**
     * @brief 图片二进制图层开始绘制回调
     * @param {uint32_t} map_id 地图实例id
     * @return {bool}
     */
    bool (*on_img_bin_layer_begin)(uint32_t map_id);

    /**
     * @brief 图片二进制图层结束绘制回调
     * @param {uint32_t} map_id 地图实例id
     * @param {int32_t} status 绘制成功返回0，否则返回错误码
     * @return {void}
     */
    void (*on_img_bin_layer_end)(uint32_t map_id, int32_t status);

    // 在线下载回调待需要再添加，当前先不添加
};

#ifdef __cplusplus
}
#endif

#endif  // _MAL_RENDER_API_H