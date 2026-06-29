/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: MapModel - Pure data container
 * Created: 2026-01-14
 */

#ifndef MAP_MODEL_H
#define MAP_MODEL_H

#include <string>
#include <vector>
#include <map>
#include "mal_map_defines.h"
#include "map_client.h"

namespace OHOS {

// POI 搜索结果项
struct MapResultItem {
    std::string name;      // POI 名称
    std::string location;  // "lon,lat"
    int16_t index;
};

struct RoutePointRemain {
    int32_t remainingDistance;  // meters
    int32_t remainingDuration;  // seconds
};

// 路线步骤信息
struct RouteStep {
    int32_t distance;                // 步骤距离（米）
    int32_t duration;                // 步骤时间（秒）
    int32_t remainingDistance;       // 从该步骤起点到终点的剩余距离
    int32_t remainingDuration;       // 从该步骤起点到终点的剩余时间
    std::string instruction;         // 导航指示 "沿XX路向北骑行100米"
    std::string road;                // 道路名称
    std::string action;              // 左转、右转、向左前方行驶、向右前方行驶
    std::string orientation;         // 方向 "北"、"东南" 等
    int32_t polylineStartIndex = 0;  // 该步骤在整体 polylinePoints 中的起始索引
};

// 单条路线信息
struct RouteInfo {
    int32_t distance;                                  // 总距离（米）
    int32_t duration;                                  // 总时间（秒）
    std::string description;                           // 描述 "路线1: 3.9km, 约22分钟"
    std::vector<RouteStep> steps;                      // 导航步骤列表
    std::vector<ACELite::map_coord2d> polylinePoints;  // 完整路线坐标点序列（GCJ02）
    std::vector<RoutePointRemain> remains;             // 与 polylinePoints 一一对应
};

// 导航状态
enum class NaviStatus {
    IDLE,        // 空闲
    NAVIGATING,  // 导航中
    PAUSED,      // 暂停
    ARRIVED      // 已到达
};

class MapModel {
public:
    static MapModel *GetInstance();
    MapModel(const MapModel &) = delete;
    MapModel &operator=(const MapModel &) = delete;
    MapModel(MapModel &&) = delete;
    MapModel &operator=(MapModel &&) = delete;

    // ===== 起终点 =====
    void SetOrigin(const std::string &name, double lon, double lat);
    void SetDestination(const std::string &name, double lon, double lat);

    const std::string &GetOriginName() const
    {
        return originName_;
    }
    const std::string &GetDestName() const
    {
        return destName_;
    }
    const ACELite::map_coord2d &GetOriginCoord() const
    {
        return originCoord_;
    }
    const ACELite::map_coord2d &GetDestCoord() const
    {
        return destCoord_;
    }

    std::string GetOriginString() const;
    std::string GetDestString() const;

    // ===== POI 搜索结果 =====
    void SetPoiResults(std::vector<MapResultItem> &&results);
    const std::vector<MapResultItem> &GetPoiResults() const
    {
        return poiResults_;
    }
    void ClearPoiResults();

    // ===== 路线规划结果 =====
    void SetRouteResults(std::vector<RouteInfo> &&routes);
    const std::vector<RouteInfo> &GetRouteResults() const
    {
        return routeResults_;
    }
    size_t GetRouteCount() const
    {
        return routeResults_.size();
    }
    void ClearRouteResults();

    void SetSelectedRouteIndex(int32_t index);
    int32_t GetSelectedRouteIndex() const
    {
        return selectedRouteIndex_;
    }
    const RouteInfo *GetSelectedRoute() const;

    // ===== 导航状态 =====
    void SetNaviStatus(NaviStatus status);
    NaviStatus GetNaviStatus() const
    {
        return naviStatus_;
    }

    void SetCurrentLocation(double lon, double lat, float bearing);
    const ACELite::map_coord2d &GetCurrentLocation() const
    {
        return currentLocation_;
    }
    float GetCurrentBearing() const
    {
        return currentBearing_;
    }

    // ===== 当前导航步骤 =====
    void SetCurrentStepIndex(int32_t index);
    int32_t GetCurrentStepIndex() const
    {
        return currentStepIndex_;
    }
    const RouteStep *GetCurrentStep() const;
    const RouteStep *GetNextStep() const;

    // ===== 剩余距离/时间 =====
    void UpdateRemainingInfo(int32_t distance, int32_t duration);
    int32_t GetRemainingDistance() const
    {
        return remainingDistance_;
    }
    int32_t GetRemainingDuration() const
    {
        return remainingDuration_;
    }

private:
    MapModel();
    ~MapModel() = default;

    // 起终点
    std::string originName_;
    std::string destName_;
    ACELite::map_coord2d originCoord_{};
    ACELite::map_coord2d destCoord_{};

    // POI 结果
    std::vector<MapResultItem> poiResults_;

    // 路线规划结果
    std::vector<RouteInfo> routeResults_;
    int32_t selectedRouteIndex_ = -1;

    // 导航状态
    NaviStatus naviStatus_ = NaviStatus::IDLE;
    ACELite::map_coord2d currentLocation_{};
    float currentBearing_ = 0.0f;
    int32_t currentStepIndex_ = 0;
    int32_t remainingDistance_ = 0;
    int32_t remainingDuration_ = 0;
};

}  // namespace OHOS

#endif  // MAP_MODEL_H
