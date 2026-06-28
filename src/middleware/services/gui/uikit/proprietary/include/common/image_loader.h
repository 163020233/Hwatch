/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: image parser for resource tool
 * Author: Hisi Graphic Team
 */

#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H
#include "graphic_config.h"
#include "gfx_utils/image_info.h"
#include "gfx_utils/heap_base.h"

namespace OHOS {
class ImageLoader : public HeapBase {
public:
    static ImageLoader &GetInstance();

    /**
     * @brief Load image resources through resID. If info->data is nullptr, loading fails.
     *
     * @param resId Image Resource ID.
     * @param info Image Information.
     */
    void LoadImage(uint16_t resId, ImageInfo &info);
private:
    ImageLoader();
    ~ImageLoader() {}

    uint16_t imageNum_ = 0;
    uint32_t *imageOffset_ = nullptr;
    uint8_t* resAddr_ = reinterpret_cast<uint8_t*>(MEM_RESOURCE_START());
};
}
#endif // IMAGE_LOADER_H