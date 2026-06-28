/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: awk mal render adapter
 * Created: 2026-01-14
 */

#include "awk_mal_render_adapter.h"
#include <stdio.h>
#include "mal_awk_convert.h"
#include "amap_system_adapter.h"
#include "amap_log_adapter.h"

// 渲染适配
static mal_render_adapter_t render_adapter;
// 渲染元素执行周期回调
static mal_map_render_callback_t map_render_callback;

void awk_render_adapter_init(mal_render_adapter_t mal_render_adapter, mal_map_render_callback_t mal_map_render_callback)
{
    AMAP_LOGD("awk_render_adapter_init");
    // 赋值待确认是否行
    render_adapter = mal_render_adapter;
    map_render_callback = mal_map_render_callback;
}
void awk_render_adapter_deinit(void)
{
    // 清理待分析
}
void awk_render_draw_image_buffer(uint32_t map_id, awk_rect_area_t area, const awk_image_buf_info_t *image_info,
                                  const awk_paint_style_t *style)
{
    AMAP_LOGD("awk_render_draw_image_buffer called!");
    mal_image_buf_info_t mal_image_info = {0};
    awk_to_mal_image_buf_info_t(image_info, &mal_image_info);
    mal_paint_style_t mal_style;
    awk_to_mal_paint_style_t(style, &mal_style);
    render_adapter.draw_image_buffer(map_id, awk_to_mal_rect_area_t(area), &mal_image_info, &mal_style);
}
void awk_render_begin_drawing_adapter(uint32_t map_id, awk_render_context_t status)
{
    render_adapter.begin_drawing(map_id, awk_to_mal_render_context_t(status));
}
void awk_render_commit_drawing_adapter(uint32_t map_id)
{
    render_adapter.commit_drawing(map_id);
}
void awk_render_point_adapter(uint32_t map_id, awk_point_t *point, uint32_t point_size, const awk_paint_style_t *style)
{
    // 栈变量自动释放，但栈可能不足，使用malloc分配较好
    mal_point_t *mal_points = (mal_point_t *)amap_mem_malloc(sizeof(mal_point_t) * point_size);
    awk_to_mal_point_list_t(point, point_size, mal_points);
    // 数据量不大，直接使用栈
    mal_paint_style_t mal_style;
    awk_to_mal_paint_style_t(style, &mal_style);
    render_adapter.draw_point(map_id, mal_points, point_size, &mal_style);
    amap_mem_free(mal_points);
}
void awk_render_polyline_adapter(uint32_t map_id, awk_point_t *points, uint32_t point_size,
                                 const awk_paint_style_t *style)
{
    mal_point_t *mal_points = (mal_point_t *)amap_mem_malloc(sizeof(mal_point_t) * point_size);
    awk_to_mal_point_list_t(points, point_size, mal_points);
    // 数据量不大，直接使用栈
    mal_paint_style_t mal_style;
    awk_to_mal_paint_style_t(style, &mal_style);
    render_adapter.draw_polyline(map_id, mal_points, point_size, &mal_style);
    amap_mem_free(mal_points);
}
void awk_render_polygon_adapter(uint32_t map_id, awk_point_t *points, uint32_t point_size,
                                const awk_paint_style_t *style)
{
    mal_point_t *mal_points = (mal_point_t *)amap_mem_malloc(sizeof(mal_point_t) * point_size);
    awk_to_mal_point_list_t(points, point_size, mal_points);
    // 数据量不大，直接使用栈
    mal_paint_style_t mal_style;
    awk_to_mal_paint_style_t(style, &mal_style);
    render_adapter.draw_polygon(map_id, mal_points, point_size, &mal_style);
    amap_mem_free(mal_points);
}
void awk_render_bitmap_adapter(uint32_t map_id, awk_rect_area_t area, awk_bitmap_t bitmap,
                               const awk_paint_style_t *style)
{
    AMAP_LOGD("awk_render_bitmap_adapter called!");
    // 数据量不大，直接使用栈
    mal_paint_style_t mal_style;
    awk_to_mal_paint_style_t(style, &mal_style);
    render_adapter.draw_bitmap(map_id, awk_to_mal_rect_area_t(area), awk_to_mal_bitmap_t(bitmap), &mal_style);
}
void awk_render_draw_text_adapter(uint32_t map_id, awk_point_t center, const char *text, const awk_paint_style_t *style)
{
    // 数据量不大，直接使用栈
    mal_paint_style_t mal_style;
    awk_to_mal_paint_style_t(style, &mal_style);
    render_adapter.draw_text(map_id, awk_to_mal_point_t(center), text, &mal_style);
}
void awk_render_color_adapter(uint32_t map_id, awk_rect_area_t area, const awk_paint_style_t *style)
{
    // 数据量不大，直接使用栈
    mal_paint_style_t mal_style;
    awk_to_mal_paint_style_t(style, &mal_style);
    render_adapter.draw_color(map_id, awk_to_mal_rect_area_t(area), &mal_style);
}
bool awk_render_measure_text_adapter(uint32_t map_id, const char *text, const awk_paint_style_t *style, int32_t *width,
                                     int32_t *ascender, int32_t *descender)
{
    // 数据量不大，直接使用栈
    mal_paint_style_t mal_style;
    awk_to_mal_paint_style_t(style, &mal_style);
    return render_adapter.measure_text(map_id, text, &mal_style, width, ascender, descender);
}
void awk_render_draw_svg_bitmap_adapter(
    uint32_t map_id, awk_rect_area_t area, awk_bitmap_t bitmap, const awk_paint_style_t *style)
{
    // 数据量不大，直接使用栈
    mal_paint_style_t mal_style;
    awk_to_mal_paint_style_t(style, &mal_style);
    render_adapter.draw_svg_bitmap(map_id, awk_to_mal_rect_area_t(area), awk_to_mal_bitmap_t(bitmap), &mal_style);
}
bool awk_map_on_point_begin_draw_callback(uint32_t map_id, int32_t guid)
{
    return map_render_callback.on_point_begin_draw(map_id, guid);
}
void awk_map_on_point_end_draw_callback(uint32_t map_id, int32_t guid, int32_t status)
{
    map_render_callback.on_point_end_draw(map_id, guid, status);
}
bool awk_map_on_line_begin_draw_callback(uint32_t map_id, int32_t guid)
{
    return map_render_callback.on_line_begin_draw(map_id, guid);
}
void awk_map_on_line_end_draw_callback(uint32_t map_id, int32_t guid, int32_t status)
{
    map_render_callback.on_line_end_draw(map_id, guid, status);
}
bool awk_map_on_polygon_begin_draw_callback(uint32_t map_id, int32_t guid)
{
    return map_render_callback.on_polygon_begin_draw(map_id, guid);
}
void awk_map_on_polygon_end_draw_callback(uint32_t map_id, int32_t guid, int32_t status)
{
    map_render_callback.on_polygon_end_draw(map_id, guid, status);
}
bool awk_map_on_tile_begin_draw_callback(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom)
{
    return map_render_callback.on_tile_begin_draw(map_id, tile_x, tile_y, zoom);
}
void awk_map_on_tile_end_draw_callback(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom, int32_t status)
{
    map_render_callback.on_tile_end_draw(map_id, tile_x, tile_y, zoom, status);
}
bool awk_map_on_poi_begin_draw_callback(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom)
{
    return map_render_callback.on_poi_begin_draw(map_id, tile_x, tile_y, zoom);
}
void awk_map_on_poi_end_draw_callback(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom, int32_t status)
{
    map_render_callback.on_poi_end_draw(map_id, tile_x, tile_y, zoom, status);
}

bool awk_map_on_img_bin_begin_draw_callback(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom)
{
    return map_render_callback.on_img_bin_begin_draw(map_id, tile_x, tile_y, zoom);
}
void awk_map_on_img_bin_end_draw_callback(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom,
                                          int32_t status)
{
    map_render_callback.on_img_bin_end_draw(map_id, tile_x, tile_y, zoom, status);
}

bool awk_map_on_img_bin_layer_begin_callback(uint32_t map_id)
{
    return map_render_callback.on_img_bin_layer_begin(map_id);
}

void awk_map_on_img_bin_layer_end_callback(uint32_t map_id, int32_t status)
{
    map_render_callback.on_img_bin_layer_end(map_id, status);
}