/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: map convert.
 * Author: CompanyName
 * Create: 2026-02-27
 */
#ifndef MAP_CONVERT_H
#define MAP_CONVERT_H

#include "map_defines.h"
#include "mal_map_defines.h"

namespace OHOS {
namespace ACELite {

class MapConverter {
public:
    static MapConverter &GetInstance();

    mal_point_t convert_point(const map_point *src);
    static map_point convert_point(const mal_point_t *src);

    mal_rect_area_t convert_rect_area(const map_rect_area *src);
    static map_rect_area convert_rect_area(const mal_rect_area_t *src);

    mal_render_context_t convert_render_context(const map_render_context *src);

    mal_text_style_t convert_text_style(const map_text_style *src);
    static map_text_style convert_text_style(const mal_text_style_t *src);

    mal_dash_style_t convert_dash_style(const map_dash_style *src);
    static map_dash_style convert_dash_style(const mal_dash_style_t *src);

    mal_paint_style_t convert_paint_style(const map_paint_style *src);
    static map_paint_style convert_paint_style(const mal_paint_style_t *src);

    mal_bitmap_t convert_bitmap(const map_bitmap *src);
    static map_bitmap convert_bitmap(const mal_bitmap_t *src);

    mal_image_buf_info_t convert_image_buf_info(const map_image_buf_info *src);
    static map_image_buf_info convert_image_buf_info(const mal_image_buf_info_t *src);

    void convert_render_adapter(const map_render_adapter *src);
    void convert_render_callback(const map_render_callback *src);
    void convert_track_listener(const map_track_navigation_listener *listener);

    static map_track_navi_event_type convert_track_navi_event_type_mode(mal_map_track_navi_event_type mode);
    static map_track_point_type convert_track_point_type_mode(mal_map_track_point_type mode);

    mal_pixel_mode_t convert_pixel_mode(map_pixel_mode mode);
    static map_pixel_mode convert_pixel_mode(mal_pixel_mode_t mode);

    mal_align_style_t convert_align_style(map_align_style align);
    static map_align_style convert_align_style(mal_align_style_t align);

    mal_font_weight_t convert_font_weight(map_font_weight weight);
    static map_font_weight convert_font_weight(mal_font_weight_t weight);

    mal_fill_style_t convert_fill_style(map_fill_style fill);
    static map_fill_style convert_fill_style(mal_fill_style_t fill);

    mal_image_format_t convert_image_format(map_image_format format);
    static map_image_format convert_image_format(mal_image_format_t format);

    mal_map_overlay_type convert_map_overlay_type(map_overlay_type type);
    static map_overlay_type convert_map_overlay_type(mal_map_overlay_type type);

    mal_map_point_texture_info_t convert_point_texture_info(map_point_texture_info info);
    mal_map_texture_data_t convert_texture(const map_texture_data *texture_data);

    mal_map_base_overlay_t convert_base_overlay(map_base_overlay point_overlay);

    mal_map_point_overlay_marker_t convert_point_overlay_marker(map_point_overlay_marker marker);
    mal_map_coord2d_t convert_coord2d(map_coord2d coord2d);
    mal_point_64_t convert_point_64(map_point_64 point);
    mal_map_line_overlay_marker_t convert_line_overlay(map_line_overlay_marker map_overlay);
    mal_map_view_port_t convert_view_port(map_view_port view);

    map_point_texture_info convert_map_point_texture_info(mal_map_point_texture_info_t info);
    map_base_overlay convert_map_base_overlay(mal_map_base_overlay_t point_overlay);
    static map_coord2d convert_map_coord2d(mal_map_coord2d_t coord2d);
    map_point_overlay_marker convert_map_point_overlay_marker(mal_map_point_overlay_marker_t marker);

    // map_track_navi_overlay转换
    bool ConvertTrackNaviOverlay(const map_track_navi_overlay *src, mal_map_track_navi_overlay_t *dst);
    void FreeMalTrackNaviOverlay(mal_map_track_navi_overlay_t *overlay);
    bool ConvertTrackInfo(const map_track_info *src, mal_map_track_info_t *dst, bool need_full = true);
    void FreeMalTrackInfo(mal_map_track_info_t *info);
    bool ConvertPlanPolylineOptions(
        const map_track_plan_polyline_options *src, mal_map_track_plan_polyline_options_t *dst);
    bool ConvertRealPolylineOptions(
        const map_track_real_polyline_options *src, mal_map_track_real_polyline_options_t *dst);
    bool ConvertYawOptions(const map_track_yaw_options *src, mal_map_track_yaw_options_t *dst);
    bool ConvertNaviOptions(const map_track_navi_options *src, mal_map_track_navi_options_t *dst);
    mal_map_coord2d_t *ConvertPointsArray(const map_coord2d *src, size_t size);
    int32_t *ConvertInt32Array(const int32_t *src, size_t size);
    uint8_t *ConvertUint8Array(const uint8_t *src, size_t size);
    double *ConvertDoubleArray(const double *src, size_t size);
    mal_map_track_turn_point_info_t *ConvertTurnPointsArray(const map_track_turn_point_info *src, size_t size);
    mal_map_point_overlay_marker_t *ConvertPointMarkerArray(const map_point_overlay_marker *src, size_t size);
    // end

    static void track_on_track_pass_point(int32_t index, mal_map_coord2d_t point);
    static void track_on_trajector_yaw(double distance);
    static void track_on_trajector_yaw_end();
    static void track_on_navi_event(mal_map_track_navi_event_info_t *event);

    static map_render_adapter map_render_adapter_;
    static map_render_callback map_render_callback_;
    static map_track_navigation_listener map_track_listener_;

private:
    MapConverter() = default;
    ~MapConverter() = default;
};
}  // namespace ACELite
}  // namespace OHOS
#endif  // MAP_CONVERT_H
