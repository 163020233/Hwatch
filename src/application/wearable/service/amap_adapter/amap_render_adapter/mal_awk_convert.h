/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: mal awk convert
 * Created: 2026-01-14
 */

#ifndef _MAL_AWK_CONVERT_H
#define _MAL_AWK_CONVERT_H
#include "awk_adapter.h"
#include "map/awk_map_defines.h"
#include "map/awk_map.h"

#include "mal_common_defines.h"
#include "mal_render_adapter.h"
#include "mal_map_defines.h"

#include <stdio.h>
#include <time.h>
#include <sys/time.h>  // for gettimeofday()
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

mal_render_context_t awk_to_mal_render_context_t(const awk_render_context_t src);

mal_point_t awk_to_mal_point_t(const awk_point_t src);

void awk_to_mal_point_list_t(const awk_point_t *src, uint32_t point_size, mal_point_t *dst);
void awk_to_mal_paint_style_t(const awk_paint_style_t *src, mal_paint_style_t *dst);

mal_rect_area_t awk_to_mal_rect_area_t(const awk_rect_area_t src);
mal_bitmap_t awk_to_mal_bitmap_t(const awk_bitmap_t src);

void awk_to_mal_image_buf_info_t(const awk_image_buf_info_t *src, mal_image_buf_info_t *dst);

// 地图部分数据转换(mal to awk & awk to mal)
awk_map_view_param_t mal_to_awk_map_view_param_t(mal_map_view_param_t src);
awk_map_view_port_t mal_to_awk_map_view_port_t(mal_map_view_port_t src);
awk_map_coord2d_t mal_to_awk_map_coord2d_t(mal_map_coord2d_t src);

void awk_to_mal_map_coord2d_t(awk_map_coord2d_t *src, mal_map_coord2d_t *dst);

void mal_to_awk_map_texture_data_t(const mal_map_texture_data_t *src, awk_map_texture_data_t *);

void mal_to_awk_map_point_overlay_t(mal_map_point_overlay_t *src, awk_map_point_overlay_t *dst);

void awk_to_mal_map_point_overlay_t(awk_map_point_overlay_t *src, mal_map_point_overlay_t *dst);

void mal_to_awk_map_polyline_overlay_t(mal_map_polyline_overlay_t *src, awk_map_polyline_overlay_t *dst);

void awk_to_mal_map_polyline_overlay_t(awk_map_polyline_overlay_t *src, mal_map_polyline_overlay_t *dst);

void mal_to_awk_map_polygon_overlay_t(mal_map_polygon_overlay_t *src, awk_map_polygon_overlay_t *dst);

void awk_to_mal_map_polygon_overlay_t(awk_map_polygon_overlay_t *src, mal_map_polygon_overlay_t *dst);

void mal_to_awk_map_base_overlay_t(const mal_map_base_overlay_t *src, awk_map_base_overlay_t *dst);

awk_map_render_callback_t mal_to_awk_map_render_callback_t(mal_map_render_callback_t src);

void awk_to_mal_map_posture_t(awk_map_posture_t *src, mal_map_posture_t *dst);

// 添加部分代码
// 1. track info 相关转换
void awk_to_mal_map_track_info_t(const awk_map_track_info_t *src, mal_map_track_info_t *dst);
void mal_to_awk_map_track_info_t(const mal_map_track_info_t *src, awk_map_track_info_t *dst);

// 2.turn point info 转换
void awk_to_mal_map_track_turn_point_info_t(const awk_map_track_turn_point_info_t *src,
                                            mal_map_track_turn_point_info_t *dst);
void mal_to_awk_map_track_turn_point_info_t(const mal_map_track_turn_point_info_t *src,
                                            awk_map_track_turn_point_info_t *dst);

// 3. navi event info 转换
void awk_to_mal_map_track_navi_event_info_t(const awk_map_track_navi_event_info_t *src,
                                            mal_map_track_navi_event_info_t *dst);
void mal_to_awk_map_track_navi_event_info_t(const mal_map_track_navi_event_info_t *src,
                                            awk_map_track_navi_event_info_t *dst);

// 4. track navi overlay 转换
void awk_to_mal_map_track_navi_overlay_t(awk_map_track_navi_overlay_t *src, mal_map_track_navi_overlay_t *dst);
void mal_to_awk_map_track_navi_overlay_t(mal_map_track_navi_overlay_t *src, awk_map_track_navi_overlay_t *dst);

// 5. options 结构体转换
void awk_to_mal_map_track_navi_options_t(const awk_map_track_navi_options_t *src, mal_map_track_navi_options_t *dst);
void mal_to_awk_map_track_navi_options_t(const mal_map_track_navi_options_t *src, awk_map_track_navi_options_t *dst);

void awk_to_mal_map_track_yaw_options_t(const awk_map_track_yaw_options_t *src, mal_map_track_yaw_options_t *dst);
void mal_to_awk_map_track_yaw_options_t(const mal_map_track_yaw_options_t *src, awk_map_track_yaw_options_t *dst);

void awk_to_mal_map_track_real_polyline_options_t(const awk_map_track_real_polyline_options_t *src,
                                                  mal_map_track_real_polyline_options_t *dst);
void mal_to_awk_map_track_real_polyline_options_t(const mal_map_track_real_polyline_options_t *src,
                                                  awk_map_track_real_polyline_options_t *dst);

void awk_to_mal_map_track_plan_polyline_options_t(const awk_map_track_plan_polyline_options_t *src,
                                                  mal_map_track_plan_polyline_options_t *dst);
void mal_to_awk_map_track_plan_polyline_options_t(const mal_map_track_plan_polyline_options_t *src,
                                                  awk_map_track_plan_polyline_options_t *dst);

void awk_to_mal_map_point_overlay_marker_t(const awk_map_point_overlay_marker_t *src,
                                           mal_map_point_overlay_marker_t *dst);
void mal_to_awk_map_point_overlay_marker_t(const mal_map_point_overlay_marker_t *src,
                                           awk_map_point_overlay_marker_t *dst);

void awk_to_mal_map_line_overlay_marker_t(const awk_map_line_overlay_marker_t *src, mal_map_line_overlay_marker_t *dst);
void mal_to_awk_map_line_overlay_marker_t(const mal_map_line_overlay_marker_t *src, awk_map_line_overlay_marker_t *dst);

void awk_to_mal_map_track_info_free(mal_map_track_info_t *info);

#ifdef __cplusplus
}
#endif

#endif