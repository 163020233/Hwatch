/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: MapView
 * Created: 2026-01-14
 */

#ifndef MAP_VIEW_H
#define MAP_VIEW_H

#include "View.h"

namespace OHOS {

enum MAP_PAGES {
    MAP_MAIN_PAGE = 1,  // 0 is invalid
};

class MapPresenter;
class MapView : public View<MapPresenter> {};

}  // namespace OHOS

#endif  // MAP_VIEW_H
