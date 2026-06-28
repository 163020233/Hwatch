/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: dfx image
 * Author: Hisi Team
 * Created: 2025-12
 */
#ifndef GRAPHIC_LITE_DFX_IMG_H
#define GRAPHIC_LITE_DFX_IMG_H
#include "components/ui_image_view.h"
#include <cstring>

namespace OHOS {
class DfxImg : public HeapBase {
public:
    static DfxImg* GetInstance();
    void Show(Rect rect, uint32_t resId, std::string file, bool isAutoSize, uint8_t mode);
    void Hide();

private:
    DfxImg() {}
    DfxImg(const DfxImg&) = delete;
    DfxImg& operator=(const DfxImg&) = delete;
    ~DfxImg()
    {
        Hide();
    }

    UIImageView* imageView_ = nullptr;
    std::string file_;
    uint32_t resId_ = 0;
    bool isLoadImgRes_ = false;
};
}
#endif