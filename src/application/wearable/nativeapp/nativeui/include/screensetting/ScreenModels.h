/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ScreenModels
 * Created: 2025-12-15
 */

#ifndef SCREEN_MODEL_H
#define SCREEN_MODEL_H

#include "gfx_utils/list.h"
#include "components/ui_image_view.h"

namespace OHOS {
class ScreenModels {
public:
    ScreenModels();
    ~ScreenModels();
    static ScreenModels *GetInstance(void);
    bool LoadAppImages(std::string file, UIImageView *view, uint32_t resId);
    void PreLoadAppimages(void);
    void InitKvStorage(void);
private:
    void LoadAppMultiImages(std::string file, uint32_t startId, uint32_t lastId);
};
}
#endif
