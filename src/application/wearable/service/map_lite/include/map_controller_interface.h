/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-11
 */

// 地图控制接口
#ifndef MAP_IMAP_CONTROLLER_H
#define MAP_IMAP_CONTROLLER_H

#include "mal_map_defines.h"

namespace OHOS {
class IMapController {
public:
    virtual ~IMapController() = default;

    virtual int32_t mal_map_init(mal_context_t mal_context) = 0;
    virtual int32_t mal_map_uninit(void) = 0;

    virtual int32_t mal_map_activate_device(void) = 0;

    virtual int32_t mal_map_create_view(mal_map_view_create_param_t param) = 0;

    virtual int32_t mal_map_destroy_view(uint32_t map_id) = 0;

    virtual int32_t mal_map_do_render(void) = 0;

    virtual int32_t mal_map_pause_render(uint32_t map_id) = 0;

    virtual int32_t mal_map_resume_render(uint32_t map_id) = 0;

    virtual int32_t mal_map_add_texture(const mal_map_texture_data_t *texture_data) = 0;

    virtual int32_t mal_map_update_texture(uint32_t texture_id, const mal_map_texture_data_t *texture_data) = 0;

    virtual int32_t mal_map_remove_texture(uint32_t texture_id) = 0;

    virtual int32_t mal_map_init_point_overlay(mal_map_point_overlay_t *point_overlay) = 0;

    virtual int32_t mal_map_init_line_overlay(mal_map_polyline_overlay_t *line_overlay) = 0;

    virtual int32_t mal_map_init_polygon_overlay(mal_map_polygon_overlay_t *polygon_overlay) = 0;

    virtual int32_t mal_map_add_overlay(uint32_t map_id, mal_map_base_overlay_t *overlay) = 0;

    virtual int32_t mal_map_update_overlay(uint32_t map_id, mal_map_base_overlay_t *overlay) = 0;

    virtual int32_t mal_map_remove_overlay(uint32_t map_id, uint32_t overlay_id) = 0;

    virtual const mal_map_base_overlay_t *mal_map_find_overlay(uint32_t map_id, uint32_t overlay_id) = 0;

    virtual uint32_t mal_map_get_overlay_count(uint32_t map_id) = 0;

    virtual const mal_map_base_overlay_t *mal_map_get_overlay(uint32_t map_id, uint32_t index) = 0;

    virtual int32_t mal_map_get_map_bounds(uint32_t map_id, mal_map_base_overlay_t *overlay,
                                           mal_map_coord2d_t *north_east, mal_map_coord2d_t *south_west,
                                           float *level) = 0;

    virtual int32_t mal_map_release_overlay(mal_map_base_overlay_t *overlay) = 0;

    virtual int32_t mal_map_set_view_port(uint32_t map_id, mal_map_view_port_t view_port) = 0;

    virtual int32_t mal_map_set_map_view_rect(uint32_t map_id, mal_rect_area_t map_view_rect) = 0;

    virtual int32_t mal_map_set_render_callback(mal_map_render_callback_t callback) = 0;
    virtual int32_t mal_map_set_center(uint32_t map_id, mal_map_coord2d_t coord2d) = 0;
    virtual int32_t mal_map_set_level(uint32_t map_id, float level) = 0;
    virtual float mal_map_get_level(uint32_t map_id) = 0;
    virtual int32_t mal_map_set_map_view_anchor(uint32_t map_id, mal_map_view_anchor_t anchor) = 0;
    virtual int32_t mal_map_set_roll_angle(uint32_t map_id, float roll_angle) = 0;
    virtual int32_t mal_map_get_posture(uint32_t map_id, mal_map_posture_t *posture) = 0;
    virtual int32_t mal_map_set_visible_bounds(uint32_t map_id, mal_navi_point_bounds_t bounds,
                                               mal_edge_insets_t insets) = 0;
    virtual int32_t mal_map_lonlat_to_xy(uint32_t map_id, mal_map_coord2d_t lonlat, int32_t *x, int32_t *y) = 0;
    virtual int32_t mal_map_xy_to_lonlat(uint32_t map_id, int32_t x, int32_t y, mal_map_coord2d_t *lonlat) = 0;
    virtual int32_t mal_map_touch_begin(uint32_t map_id, int32_t x, int32_t y) = 0;
    virtual int32_t mal_map_touch_update(uint32_t map_id, int32_t x, int32_t y) = 0;
    virtual int32_t mal_map_touch_end(uint32_t map_id, int32_t x, int32_t y) = 0;
    virtual mal_map_point_overlay_t **mal_map_click_points(uint32_t map_id, int32_t x, int32_t y,
                                                           int32_t *overlay_count) = 0;
    virtual int32_t mal_map_release_overlays(mal_map_base_overlay_t **overlays, int32_t overlay_count) = 0;
    virtual mal_map_track_info_t *mal_map_track_navi_parse_gpx(const char *gpx_data, size_t length) = 0;
    virtual mal_map_track_info_count_t *mal_map_track_parser_json(const char *json_data, int mode) = 0;
    virtual char *mal_poi_aos_json(const char *search_name) = 0;
    virtual char *mal_route_aos_json(char *origin_coord, char *dest_coord, int mode) = 0;
    virtual mal_map_poi_t *mal_poi_search_json(const char *json_str) = 0;
    virtual void mal_map_poi_mem_free(mal_map_poi_t *mal_map_poi_info) = 0;
    virtual void mal_map_track_navi_parse_track_info_count_free(mal_map_track_info_count_t *info) = 0;
    virtual void mal_map_track_navi_parse_track_info_free(mal_map_track_info_t *track_info) = 0;
    virtual int32_t mal_map_init_track_navi_overlay(mal_map_track_navi_overlay_t *overlay) = 0;
    virtual int32_t mal_map_track_navi_add_overlay(uint32_t map_id, mal_map_track_navi_overlay_t *overlay) = 0;
    virtual int32_t mal_map_track_navi_remove_overlay(uint32_t map_id, uint32_t overlay_id) = 0;
    virtual void mal_map_track_navi_update_real_point(uint32_t map_id, mal_map_coord2d_t point, float bear,
                                                      mal_map_track_navi_overlay_t *overlay) = 0;
    virtual double mal_map_track_navi_get_distance(uint32_t map_id, mal_map_coord2d_t point,
                                                   mal_map_track_point_type point_type,
                                                   mal_map_track_navi_overlay_t *overlay) = 0;

    /*********************
     *    在线地图缓存
     *********************/
    virtual int32_t mal_map_request_tiles(mal_rect_area_t req_rect, mal_map_coord2d_t map_center, float map_level,
                                          int32_t priority) = 0;
    virtual int32_t mal_map_cancel_request_tiles(int32_t req_id) = 0;

    /*********************
     *    工具函数
     *********************/
    virtual float mal_map_calc_points_distance(double lon1, double lat1, double lon2, double lat2) = 0;
    virtual void mal_map_wgs84_to_mgs(double lng_wgs, double lat_wgs, double *lng_mgs, double *lat_mgs) = 0;
};
}  // namespace OHOS

// 由平台适配实现具备的mal控制接口
extern "C" OHOS::IMapController *ProvideMapController();

#endif