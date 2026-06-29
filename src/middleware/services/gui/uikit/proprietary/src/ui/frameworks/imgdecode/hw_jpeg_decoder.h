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

#ifndef GRAPHIC_LITE_HW_JPEG_DECODER_H
#define GRAPHIC_LITE_HW_JPEG_DECODER_H

#include "graphic_config.h"
#if CAPABILITY_HW_JPEG_DECODE
#include "imgdecode/file_img_decoder.h"
#include "draw/draw_image.h"
#include "draw/draw_utils.h"
#include "soc_vau_api.h"
#include "soc_vau_type.h"

namespace OHOS {
class HWJpegDecoder : public FileImgDecoder {
public:
    static HWJpegDecoder& GetInstance();

    virtual RetCode DecodeJPEG(ImageInfo &imageInfo, const td_u8 *fileData, uint32_t fileLength) = 0;

    RetCode ReadLine(ImgResDsc& dsc, const Point& start, int16_t len, uint8_t* buf) override
    {
        return RetCode::FAIL;
    }

protected:
    HWJpegDecoder() {}
    ~HWJpegDecoder() override {}

    HWJpegDecoder(const HWJpegDecoder&) = delete;
    HWJpegDecoder& operator=(const HWJpegDecoder&) = delete;
    HWJpegDecoder(HWJpegDecoder&&) = delete;
    HWJpegDecoder& operator=(HWJpegDecoder&&) = delete;
};
}
#endif
#endif
