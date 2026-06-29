/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: awk mal render adapter
 * Created: 2026-01-14
 */

//  高德地图渲染适配接口，反向回调接口
// 　双向接口接需要适配

#ifndef AWK_RENDER_ADAPTER_H
#define AWK_RENDER_ADAPTER_H
#include <awk_adapter.h>
#include "mal_render_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif
// 初始化接口
void awk_render_adapter_init(mal_render_adapter_t mal_render_adapter,
                             mal_map_render_callback_t mal_map_render_callback);
void awk_render_adapter_deinit(void);

// 渲染适配接口
void awk_render_begin_drawing_adapter(uint32_t map_id, awk_render_context_t status);
void awk_render_commit_drawing_adapter(uint32_t map_id);
void awk_render_point_adapter(uint32_t map_id, awk_point_t *point, uint32_t point_size, const awk_paint_style_t *style);
void awk_render_polyline_adapter(uint32_t map_id, awk_point_t *points, uint32_t point_size,
                                 const awk_paint_style_t *style);
void awk_render_polygon_adapter(uint32_t map_id, awk_point_t *points, uint32_t point_size,
                                const awk_paint_style_t *style);
void awk_render_bitmap_adapter(uint32_t map_id, awk_rect_area_t area, awk_bitmap_t bitmap,
                               const awk_paint_style_t *style);
void awk_render_draw_text_adapter(uint32_t map_id, awk_point_t center, const char *text,
                                  const awk_paint_style_t *style);
void awk_render_color_adapter(uint32_t map_id, awk_rect_area_t area, const awk_paint_style_t *style);
bool awk_render_measure_text_adapter(uint32_t map_id, const char *text, const awk_paint_style_t *style, int32_t *width,
                                     int32_t *ascender, int32_t *descender);
void awk_render_draw_image_buffer(uint32_t map_id, awk_rect_area_t area, const awk_image_buf_info_t *image_info,
                                  const awk_paint_style_t *style);
void awk_render_draw_svg_bitmap_adapter(
    uint32_t map_id, awk_rect_area_t area, awk_bitmap_t bitmap, const awk_paint_style_t *style);

// 渲染生命周期回调适配接口
bool awk_map_on_point_begin_draw_callback(uint32_t map_id, int32_t guid);
void awk_map_on_point_end_draw_callback(uint32_t map_id, int32_t guid, int32_t status);
bool awk_map_on_line_begin_draw_callback(uint32_t map_id, int32_t guid);
void awk_map_on_line_end_draw_callback(uint32_t map_id, int32_t guid, int32_t status);
bool awk_map_on_polygon_begin_draw_callback(uint32_t map_id, int32_t guid);
void awk_map_on_polygon_end_draw_callback(uint32_t map_id, int32_t guid, int32_t status);
bool awk_map_on_tile_begin_draw_callback(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom);
void awk_map_on_tile_end_draw_callback(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom,
                                       int32_t status);
bool awk_map_on_poi_begin_draw_callback(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom);
void awk_map_on_poi_end_draw_callback(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom, int32_t status);
bool awk_map_on_img_bin_begin_draw_callback(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom);
void awk_map_on_img_bin_end_draw_callback(uint32_t map_id, uint32_t tile_x, uint32_t tile_y, uint32_t zoom,
                                          int32_t status);
bool awk_map_on_img_bin_layer_begin_callback(uint32_t map_id);
void awk_map_on_img_bin_layer_end_callback(uint32_t map_id, int32_t status);

#ifdef __cplusplus
}
#endif

#endif
