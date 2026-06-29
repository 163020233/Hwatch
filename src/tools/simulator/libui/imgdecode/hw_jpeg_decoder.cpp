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

#include "imgdecode/hw_jpeg_decoder.h"
#if CAPABILITY_HW_JPEG_DECODE
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/mem_api.h"
#include "securec.h"

namespace OHOS {
RetCode HWJpegDecoder::Open(ImgResDsc& dsc)
{
    fp_ = fopen(dsc.path, "rb");
    if (fp_ == nullptr) {
        printf("open file:%s failed\n", dsc.path);
        return RetCode::FAIL;
    }

    dsc.imgInfo.data = nullptr;
    dsc.inCache_ = false;
    return RetCode::OK;
}

RetCode HWJpegDecoder::CreateDecoder(ImgResDsc& dsc)
{
    if (fp_ == nullptr) {
        printf("File is not opened, call Open first\n");
        return RetCode::FAIL;
    }
    cinfo_.err = jpeg_std_error(&jerr_);
    jpeg_create_decompress(&cinfo_);
    jpeg_stdio_src(&cinfo_, fp_);
    jpeg_read_header(&cinfo_, TRUE);
    jpeg_start_decompress(&cinfo_);

    return RetCode::OK;
}

RetCode HWJpegDecoder::CreateDecoder(const uint8_t *fileData, uint32_t fileLength, struct jpeg_decompress_struct *cinfo)
{
    cinfo->err = jpeg_std_error(&jerr_);
    jpeg_create_decompress(cinfo);
    jpeg_mem_src(cinfo, fileData, fileLength);
    jpeg_read_header(cinfo, TRUE);
    jpeg_start_decompress(cinfo);

    return RetCode::OK;
}

RetCode HWJpegDecoder::GetHeader(ImgResDsc& dsc)
{
    CreateDecoder(dsc);
    dsc.imgInfo.header.width = cinfo_.output_width;
    dsc.imgInfo.header.height = cinfo_.output_height;
    dsc.imgInfo.header.colorMode = ARGB8888;
    return RetCode::OK;
}

RetCode HWJpegDecoder::ReadToCache(ImgResDsc& dsc)
{
    uint8_t pixelByteSize = DrawUtils::GetPxSizeByColorMode(ARGB8888) >> 3; // 3: Shift right 3 bits
    uint16_t width = cinfo_.output_width;
    uint16_t height = cinfo_.output_height;
    uint32_t dataStride = ALIGN_BYTE(width, BYTE_ALIGNMENT) * pixelByteSize;
    uint32_t dataSize = dataStride * height;
    uint16_t rowStride = width * 3; // 3: RGB 3 bytes
    dsc.imgInfo.dataSize = dataSize;
    // 1: one-row-high array
    unsigned char *buffer = (unsigned char *)malloc(sizeof(unsigned char) * rowStride);
    uint8_t* srcData = static_cast<uint8_t*>(ImageCacheMalloc(dsc.imgInfo));
    if ((srcData == nullptr) || (buffer == nullptr)) {
        DestroyDecoder();
        return RetCode::FAIL;
    }

    for (uint16_t y = 0; y < height; y++) {
        jpeg_read_scanlines(&cinfo_, &buffer, 1);       // 1: read one line each time
        for (uint16_t x = 0; x < width; x++) {
            uint32_t  jpgIndex = x * 3;                       // 3: color components per pixel
            uint32_t  argbIndex = x * 4 + y * dataStride;     // 4: color ARGB
            srcData[argbIndex] = buffer[jpgIndex + 2];          // 2: JPEG B channel
            srcData[argbIndex + 1] = buffer[jpgIndex + 1];      // 1: G channel
            srcData[argbIndex + 2] = buffer[jpgIndex];          // 2: R channel
            srcData[argbIndex + 3] = 255;                          // 3: A channel  255: set alpha channel
        }
    }
    free(buffer);
    buffer = nullptr;
    DestroyDecoder();
    dsc.inCache_ = true;
    return RetCode::OK;
}

RetCode HWJpegDecoder::DecodeJPEG(ImageInfo &imageInfo, const uint8_t *fileData, uint32_t fileLength)
{
    struct jpeg_decompress_struct cinfo = {0};
    if (CreateDecoder(fileData, fileLength, &cinfo) != RetCode::OK) {
        printf("CreateDecoder failed\n");
        return RetCode::FAIL;
    }
    imageInfo.header.width = cinfo.output_width;
    imageInfo.header.height = cinfo.output_height;
    imageInfo.header.colorMode = ARGB8888;
 
    uint8_t pixelByteSize = DrawUtils::GetPxSizeByColorMode(ARGB8888) >> 3; // 3: Shift right 3 bits
    uint32_t dataStride = ALIGN_BYTE(imageInfo.header.width, BYTE_ALIGNMENT) * pixelByteSize;
    uint32_t dataSize = dataStride * imageInfo.header.height;
    uint16_t rowStride = imageInfo.header.width * 3; // 3: RGB 3 bytes
    imageInfo.dataSize = dataSize;
    // 1: one-row-high array
    unsigned char *buffer = (unsigned char *)malloc(sizeof(unsigned char) * rowStride);
    uint8_t* srcData = static_cast<uint8_t*>(ImageCacheMalloc(imageInfo));
    if ((srcData == nullptr) || (buffer == nullptr)) {
        printf("buffer(%p) or srcData(%p) malloc failed\n", buffer, srcData);
        if (buffer) {
            free(buffer);
        }
        if (srcData) {
            ImageCacheFree(imageInfo);
        }
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        return RetCode::FAIL;
    }
    for (uint16_t y = 0; y < imageInfo.header.height; y++) {
        jpeg_read_scanlines(&cinfo, &buffer, 1);       // 1: read one line each time
        for (uint16_t x = 0; x < imageInfo.header.width; x++) {
            uint32_t jpgIndex = x * 3;                       // 3: color components per pixel
            uint32_t argbIndex = x * 4 + y * dataStride;     // 4: color ARGB
            srcData[argbIndex] = buffer[jpgIndex + 2];          // 2: JPEG B channel
            srcData[argbIndex + 1] = buffer[jpgIndex + 1];      // 1: G channel
            srcData[argbIndex + 2] = buffer[jpgIndex];          // 2: R channel
            srcData[argbIndex + 3] = 255;                          // 3: A channel  255: set alpha channel
        }
    }
    free(buffer);
    buffer = nullptr;
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return RetCode::OK;
}

void HWJpegDecoder::DestroyDecoder(void)
{
    if (fp_ != nullptr) {
        fclose(fp_);
        fp_ = nullptr;
    }

    jpeg_finish_decompress(&cinfo_);
    jpeg_destroy_decompress(&cinfo_);
}
} // namespace OHOS
#endif
