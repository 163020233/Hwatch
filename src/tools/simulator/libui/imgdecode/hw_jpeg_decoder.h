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

#ifndef GRAPHIC_LITE_HW_JPEG_DECODER_H
#define GRAPHIC_LITE_HW_JPEG_DECODER_H

#include "graphic_config.h"
#if CAPABILITY_HW_JPEG_DECODE
#include "imgdecode/file_img_decoder.h"
#include "draw/draw_image.h"
#include "draw/draw_utils.h"
#define UINT16 JPEG_UINT16 
#define boolean JPEG_BOOLEAN 
#include "jpeglib.h"
#undef UINT16
#undef boolean

namespace OHOS {
class HWJpegDecoder : public FileImgDecoder {
public:
    static HWJpegDecoder& GetInstance()
    {
        static HWJpegDecoder instance;
        return instance;
    }

    RetCode Open(ImgResDsc& dsc) override;

    RetCode GetHeader(ImgResDsc& dsc) override;

    RetCode ReadToCache(ImgResDsc& dsc) override;

    RetCode DecodeJPEG(ImageInfo &imageInfo, const uint8_t *fileData, uint32_t fileLength);

    RetCode ReadLine(ImgResDsc& dsc, const Point& start, int16_t len, uint8_t* buf) override
    {
        return RetCode::FAIL;
    }

private:
    FILE* fp_ {nullptr};
    struct jpeg_decompress_struct cinfo_ {0};
    struct jpeg_error_mgr jerr_ {0};

    HWJpegDecoder() {}
    ~HWJpegDecoder() override {}

    RetCode CreateDecoder(ImgResDsc& dsc);
    RetCode CreateDecoder(const uint8_t *fileData, uint32_t fileLength, struct jpeg_decompress_struct *cinfo);
    void DestroyDecoder(void);

    HWJpegDecoder(const HWJpegDecoder&) = delete;
    HWJpegDecoder& operator=(const HWJpegDecoder&) = delete;
    HWJpegDecoder(HWJpegDecoder&&) = delete;
    HWJpegDecoder& operator=(HWJpegDecoder&&) = delete;
};
}
#endif
#endif
