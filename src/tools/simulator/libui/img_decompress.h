/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd..
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GRAPHIC_LITE_IMAGE_DECOMPRESS_H
#define GRAPHIC_LITE_IMAGE_DECOMPRESS_H

#include "gfx_utils/heap_base.h"
#include "securec.h"
#include "imgdecode/image_load.h"
#ifdef _WIN32
#include "ice_v5r1_bolck_htc_interface.h"
namespace OHOS {
class ImgDecompress : public HeapBase {
public:
    static ImgDecompress &GetInstance();
    int GetBufferandSize(ext_vau_cmp_header header, unsigned char* &imageBuffer,
                         uint32_t &dataSize, auto_dcmp_output* &outPut);
    void FreeCmpBuffer(auto_cmp_output* &info, auto_dcmp_output* &outPut);
    void Decompress(ImageInfo& img, bool isAssigned);
};
}
#endif
#endif