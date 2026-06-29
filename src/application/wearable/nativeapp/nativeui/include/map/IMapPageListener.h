/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: Map page listener interfaces
 * Created: 2026-01-14
 */

#ifndef IMAP_PAGE_LISTENER_H
#define IMAP_PAGE_LISTENER_H

#include <cstdint>

namespace OHOS {

// POI 状态回调
class IPoiStateListener {
public:
    virtual ~IPoiStateListener() = default;
    virtual void OnPoiSearchSuccess() = 0;
    virtual void OnPoiSearchFailed() = 0;
};

// 路线规划状态回调
class IRouteStateListener {
public:
    virtual ~IRouteStateListener() = default;
    virtual void OnRoutePlanSuccess(int routeCount) = 0;
    virtual void OnRoutePlanFailed() = 0;
};

// 导航状态回调
class INaviStateListener {
public:
    virtual ~INaviStateListener() = default;
    virtual void OnLocationUpdated() = 0;
    virtual void OnNaviStepChanged(int32_t stepIndex) = 0;
    virtual void OnNaviArrived() = 0;
    virtual void OnTurnApproaching(const char *message, double distance) = 0;
    virtual void OnPassPoint(int32_t pointIndex) = 0;
};

}  // namespace OHOS

#endif  // IMAP_PAGE_LISTENER_H
