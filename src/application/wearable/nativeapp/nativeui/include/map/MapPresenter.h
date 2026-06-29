/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: MapPresenter - Business logic coordinator
 * Created: 2026-01-14
 */

#ifndef MAP_PRESENTER_H
#define MAP_PRESENTER_H

#include "MapModel.h"
#include "MapService.h"
#include "IMapPageListener.h"
#include "Presenter.h"
#include "map_controller_interface.h"
#include "map_client.h"
#include "MapView.h"

namespace OHOS {

class MapPresenter : public Presenter<MapView> {
public:
    static MapPresenter *GetInstance();
    MapPresenter(const MapPresenter &) = delete;
    MapPresenter &operator=(const MapPresenter &) = delete;
    MapPresenter(MapPresenter &&) = delete;
    MapPresenter &operator=(MapPresenter &&) = delete;
    MapPresenter() = default;

    void SetMapController(IMapController *controller)
    {
        mapController_ = controller;
    }

    // ===== Page 注册回调 =====
    void SetPoiPageListener(IPoiStateListener *listener)
    {
        poiListener_ = listener;
    }
    void SetRoutePageListener(IRouteStateListener *listener)
    {
        routeListener_ = listener;
    }
    void SetNaviPageListener(INaviStateListener *listener)
    {
        naviListener_ = listener;
    }
    // ===== POI 搜索 =====
    void SearchPOI(const std::string &keyword);
    const std::vector<MapResultItem> &GetPoiResults() const;

    /**
     * @brief 下载并保存离线地图
     * @param adcode 区/市/省 adcode
     */
    void DownloadOfflineMap(const std::string &adcode);

    /**
     * @brief 获取省级离线地图目录树
     * @param provinceAdcode 省 adcode,如 "310000"
     * @param outRoot 输出
     * @return true=成功
     */
    bool FetchOfflineMapList(const std::string &provinceAdcode, OfflineRegion &outRoot);

    // ===== 起终点 =====
    void SetOrigin(const std::string &name, double lon, double lat);
    void SetDestinationFromPoiResult(int16_t index);
    const std::string GetDestName();

    // ===== 路线规划 =====
    void PlanRoute();
    const std::vector<RouteInfo> &GetRouteResults() const;
    void SelectRoute(int index);
    const RouteInfo *GetSelectedRoute() const;
    void ClearRouteResults() const;

    // ===== 导航 =====
    void StartNavigation();
    void PauseNavigation();
    void ResumeNavigation();
    void StopNavigation();
    NaviStatus GetNaviStatus() const;

    int32_t GetRemainingDistance() const;
    int32_t GetRemainingDuration() const;
    const ACELite::map_coord2d &GetCurrentLocation() const;
    int32_t GetCurrentStepIndex() const;

    void UpdateLocation(double lon, double lat, float bearing);
    float GetCurrentBearing() const;

private:
    ~MapPresenter() = default;
    void CalcStepRemaining(RouteInfo &route);
    MapModel *model_ = MapModel::GetInstance();
    MapService *service_ = MapService::GetInstance();

private:
    IMapController *mapController_ = nullptr;
    IPoiStateListener *poiListener_ = nullptr;
    IRouteStateListener *routeListener_ = nullptr;
    INaviStateListener *naviListener_ = nullptr;
};

}  // namespace OHOS

#endif  // MAP_PRESENTER_H
