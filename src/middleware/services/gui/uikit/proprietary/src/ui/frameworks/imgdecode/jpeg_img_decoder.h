/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
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

#ifndef GRAPHIC_LITE_JPEG_IMG_DECODER_H
#define GRAPHIC_LITE_JPEG_IMG_DECODER_H

#include "file_img_decoder.h"
#if (ENABLE_JPEG_AND_PNG && CACHE_JPEG_AND_PNG && CAPABILITY_SW_JPEG_DECODE)
#include "draw/draw_image.h"
#include "draw/draw_utils.h"

namespace OHOS {
class JpegImgDecoder : public FileImgDecoder {
public:
    static JpegImgDecoder& GetInstance();

    RetCode ReadLine(ImgResDsc& dsc, const Point& start, int16_t len, uint8_t* buf) override
    {
        return RetCode::FAIL;
    }

protected:
    JpegImgDecoder() {};
    ~JpegImgDecoder() {};

    JpegImgDecoder(const JpegImgDecoder&) = delete;
    JpegImgDecoder& operator=(const JpegImgDecoder&) = delete;
    JpegImgDecoder(JpegImgDecoder&&) = delete;
    JpegImgDecoder& operator=(JpegImgDecoder&&) = delete;
};
}
#endif
#endif
