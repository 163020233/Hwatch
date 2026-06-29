/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: map module.
 * Author: CompanyName
 * Create: 2026-03-09
 */

#ifndef OHOS_ACELITE_MAP_MODULE_H
#define OHOS_ACELITE_MAP_MODULE_H

#include "map_client.h"
#include "acelite_config.h"
#include <stdint.h>
#include "jsi.h"

namespace OHOS {
namespace ACELite {

class MapModule final : public MemoryHeap {
public:
    MapModule() = default;
    ~MapModule() = default;

    static JSIValue SearchPoi(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static void OnDestroy();

private:
    static MapClient *mapClient_;
    static const char *const POI_NAME;
    static const char *const API_KEY;
};

void InitMapModule(JSIValue exports);

}  // namespace ACELite
}  // namespace OHOS
#endif  // OHOS_ACELITE_MAP_MODULE_H
