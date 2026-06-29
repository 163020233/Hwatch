/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: map render defines.
 * Author: CompanyName
 * Create: 2026-02-27
 */
// 绘制层结构体
#ifndef MAP_RENDER_DEFINES_H
#define MAP_RENDER_DEFINES_H

namespace OHOS {
namespace ACELite {
/**
 * @brief 文字排布的对齐样式
 */
typedef enum {
    MAP_ALIGN_STYLE_NONE,
    MAP_ALIGN_STYLE_CENTER,
    MAP_ALIGN_STYLE_LEFT,
    MAP_ALIGN_STYLE_RIGHT
} map_align_style;

/**
 * @brief 字体粗细样式
 */
typedef enum {
    MAP_FONT_WEIGHT_NORMAL,
    MAP_FONT_WEIGHT_THIN,
    MAP_FONT_WEIGHT_BOLD,
} map_font_weight;

/**
 * @brief 文字样式
 */
typedef struct {
    map_align_style align;
    uint32_t font_size;
    map_font_weight font_weight;
} map_text_style;

/**
 * @brief 填充类型
 */
typedef enum {
    MAP_FILL_STYLE_DRAWING_ONLY,        // 仅填充几何图形
    MAP_FILL_STYLE_DRAWING_AND_STROKE,  // 填充几何图形和描边
    MAP_FILL_STYLE_STROKE_ONLY          // 仅填充描边
} map_fill_style;

/**
 * @brief 虚线定义，单位：像素
 */
typedef struct {
    int32_t painted_length;    //!< 绘制长度
    int32_t unpainted_length;  //!< 空白长度
    int32_t offset;            //!< 起始位置
} map_dash_style;

/**
 * @brief 笔刷样式
 */
typedef struct {
    uint32_t width;             //!< 画笔宽度, 单位：像素
    uint32_t color;             //!< 画笔颜色, ARGB
    float angle;                //!< 旋转角度，[0-360)，正东为0，逆时针为正
    map_text_style text_style;  //!< 文字样式
    map_fill_style fill_style;  //!< 填充样式
    map_dash_style dash_style;  //!< 虚线样式
} map_paint_style;

/**
 * @brief 绘制上下文
 */
typedef struct {
    int32_t width;   //!< 画布宽度, 单位：像素
    int32_t height;  //!< 画布高度, 单位：像素
    map_rect_area map_view_rect;
} map_render_context;

/**
 * @brief 绘制相关适配接口， 下层是可能是C/C++回调，所以此处只能定义C兼容接口
 */
typedef struct {
    /**
     * @brief 开始绘制时回调方法
     * @param {map_id} 地图实例id
     * @param {map_render_context} status
     * @return {*}
     */
    void (*begin_drawing)(uint32_t map_id, map_render_context status);

    /**
     * @brief 绘制结束时回调方方法
     * @param {map_id} 地图实例id
     * @return {*}
     */
    void (*commit_drawing)(uint32_t map_id);

    /**
     * @brief 需要绘制点时回调
     * @param {map_id} 地图实例id
     * @param {map_point} *point 点的数据
     * @param {uint32_t} point_size 点的个数
     * @param {map_paint_style} *style 绘制样式
     * @return {*}
     */
    void (*draw_point)(uint32_t map_id, map_point *point, uint32_t point_size, const map_paint_style *style);

    /**
     * @brief 需要绘制线时回调
     * @param {map_id} 地图实例id
     * @param {map_point} *points 线上点的结合
     * @param {uint32_t} point_size 点的数量
     * @param {map_paint_style} *style 绘制样式
     * @return {*}
     */
    void (*draw_polyline)(uint32_t map_id, map_point *points, uint32_t point_size, const map_paint_style *style);

    /**
     * @brief 需要绘制面时回调
     * @param {map_id} 地图实例id
     * @param {map_point} *points 面的点集合
     * @param {uint32_t} point_size 点的数量
     * @param {map_paint_style} *style 绘制样式
     * @return {*}
     */
    void (*draw_polygon)(uint32_t map_id, map_point *points, uint32_t point_size, const map_paint_style *style);

    /**
     * @brief 需要绘制bitmap时回调
     * @param {map_id} 地图实例id
     * @param {map_rect_area} area 区域
     * @param {map_bitmap} bitmap bitmap信息
     * @param {map_paint_style} *style 样式
     * @return {*}
     */
    void (*draw_bitmap)(uint32_t map_id, map_rect_area area, map_bitmap bitmap, const map_paint_style *style);

    /**
     * @brief 需要绘制图像二进制时回调
     * @param {map_id} 地图实例id
     * @param {map_rect_area} area 区域
     * @param {map_image_buf_info} image_info 图像信息
     * @param {map_paint_style} *style 样式
     * @return {*}
     */
    void (*draw_image_buffer)(
        uint32_t map_id, map_rect_area area, const map_image_buf_info *image_info, const map_paint_style *style);

    /**
     * @brief 需要绘制文字时回调
     * @param {map_id} 地图实例id
     * @param {map_point} center 文字中心位置
     * @param {char} *text 文字内容
     * @param {map_paint_style} *style 绘制样式
     * @return {*}
     */
    void (*draw_text)(uint32_t map_id, map_point center, const char *text, const map_paint_style *style);

    /**
     * @brief 需要绘制颜色时回调
     * @param {map_id} 地图实例id
     * @param {map_rect_area} area 区域
     * @param {map_paint_style} *style 样式
     * @return {*}
     */
    void (*draw_color)(uint32_t map_id, map_rect_area area, const map_paint_style *style);

    /**
     * @brief 测量文字宽度
     * @param {map_id} 地图实例id
     * @param text 字符串
     * @param style 文字绘制样式
     * @param width 文字的宽度
     * @param ascender 文字的上行高度
     * @param descender 文字的下行高度
     */
    bool (*measure_text)(uint32_t map_id, const char *text, const map_paint_style *style, int32_t *width,
        int32_t *ascender, int32_t *descender);

    /**
     * @brief 需要绘制svg bitmap时回调
     * @param {map_id} 地图实例id
     * @param {map_rect_area} area 区域
     * @param {map_bitmap} bitmap bitmap信息
     * @return {*}
     */
    void (*draw_svg_bitmap)(uint32_t map_id, map_rect_area area, map_bitmap bitmap);
} map_render_adapter;

/**
 * @brief 地图渲染回调
 */
typedef struct {
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
     * @return {*}
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
     * @return {*}
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
     * @return {*}
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
     * @return {*}
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
     * @return {*}
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
     * @return {*}
     */
    void (*on_img_bin_end_draw)(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom, int32_t status);

    /**
     * @brief 图片二进制图层开始绘制回调
     * @param {uint32_t} map_id 地图实例id
     * @return {*}
     */
    bool (*on_img_bin_layer_begin)(uint32_t map_id);

    /**
     * @brief 图片二进制图层结束绘制回调
     * @param {uint32_t} map_id 地图实例id
     * @param {int32_t} status 绘制成功返回0，否则返回错误码
     * @return {*}
     */
    void (*on_img_bin_layer_end)(uint32_t map_id, int32_t status);

    // 在线下载回调待需要再添加，当前先不添加
} map_render_callback;
}  // namespace ACELite
}  // namespace OHOS
#endif  // MAP_RENDER_DEFINES_H
